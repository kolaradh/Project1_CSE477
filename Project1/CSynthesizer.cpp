#include "pch.h"
#include "CSynthesizer.h"
#include "CWavetableInstrument.h"
#include "CToneInstrument.h"
#include "xmlhelp.h"
#include <algorithm>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif

using namespace std;

CSynthesizer::CSynthesizer()
{
    CoInitialize(NULL);
    m_channels = 2;
    m_sampleRate = 44100.0;
    m_samplePeriod = 1.0 / m_sampleRate;
    m_time = 0.0;
    m_bpm = 120.0;
    m_beatspermeasure = 4;
    m_secperbeat = 60.0 / m_bpm;
}

CSynthesizer::~CSynthesizer()
{
    for (auto instrument : m_instruments)
        delete instrument;
    m_instruments.clear();
    CoUninitialize();
}

void CSynthesizer::Clear()
{
    for (auto instrument : m_instruments)
        delete instrument;
    m_instruments.clear();
    m_notes.clear();
}

void CSynthesizer::Start()
{
    m_currentNote = 0;
    m_measure = 0;
    m_beat = 0.0;
    m_time = 0.0;

    // Important: reset delay buffer and sample rate
    m_effects.SetSampleRate(m_sampleRate);

    TRACE(L"[Synth] Start() initialized. BPM=%.2f  SR=%.1f\n", m_bpm, m_sampleRate);
}

bool CSynthesizer::Generate(double* frame)
{
    if (!frame)
        return false;

    // --- Phase 1: Start new notes ---
    while (m_currentNote < (int)m_notes.size())
    {
        CNote* note = &m_notes[m_currentNote];
        if (note->Measure() > m_measure) break;
        if (note->Measure() == m_measure && note->Beat() > m_beat) break;

        CInstrument* instrument = nullptr;

        if (note->Instrument() == L"WavetableInstrument")
            instrument = new CWavetableInstrument();
        else if (note->Instrument() == L"ToneInstrument")
            instrument = new CToneInstrument();

        if (instrument)
        {
            instrument->SetSampleRate(m_sampleRate);
            instrument->SetBPM(m_bpm);
            instrument->SetNote(note);
            instrument->Start();
            m_instruments.push_back(instrument);
        }

        m_currentNote++;
    }

    // --- Phase 2: Clear output frame ---
    for (int c = 0; c < m_channels; c++)
        frame[c] = 0.0;

    // --- Phase 3: Mix instruments ---
    for (auto node = m_instruments.begin(); node != m_instruments.end(); )
    {
        auto next = node; ++next;
        CInstrument* instrument = *node;

        if (instrument->Generate())
        {
            for (int c = 0; c < m_channels; c++)
                frame[c] += instrument->Frame(c);
        }
        else
        {
            delete instrument;
            m_instruments.erase(node);
        }
        node = next;
    }

    // --- Phase 4: Effects (safe) ---
    m_effects.Process(frame);

    // --- Phase 5: Advance time ---
    m_time += m_samplePeriod;
    m_beat += m_samplePeriod / m_secperbeat;

    if (m_beat > m_beatspermeasure)
    {
        m_beat -= m_beatspermeasure;
        m_measure++;
    }

    // --- Phase 6: Stop check ---
    bool active = !m_instruments.empty() || m_currentNote < (int)m_notes.size();
    return active;
}

void CSynthesizer::OpenScore(CString& filename)
{
    Clear();

    CComPtr<IXMLDOMDocument> pXMLDoc;
    bool succeeded = SUCCEEDED(CoCreateInstance(
        CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,
        IID_IXMLDOMDocument, (void**)&pXMLDoc));

    if (!succeeded)
    {
        AfxMessageBox(L"XML document creation failed.");
        return;
    }

    VARIANT_BOOL ok;
    succeeded = SUCCEEDED(pXMLDoc->load(CComVariant(filename), &ok));
    if (!succeeded || ok == VARIANT_FALSE)
    {
        AfxMessageBox(L"Unable to open score file.");
        return;
    }

    CComPtr<IXMLDOMNode> node;
    pXMLDoc->get_firstChild(&node);
    for (; node != NULL; NextNode(node))
    {
        CComBSTR nodeName;
        node->get_nodeName(&nodeName);
        if (nodeName == L"score") XmlLoadScore(node);
    }

    sort(m_notes.begin(), m_notes.end());
    TRACE(L"[Synth] Score loaded with %d notes.\n", (int)m_notes.size());
}

void CSynthesizer::XmlLoadScore(IXMLDOMNode* xml)
{
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    xml->get_attributes(&attributes);

    if (attributes)
    {
        long len = 0;
        attributes->get_length(&len);

        for (int i = 0; i < len; i++)
        {
            CComPtr<IXMLDOMNode> attrib;
            attributes->get_item(i, &attrib);
            if (!attrib) continue;

            CComBSTR name;
            attrib->get_nodeName(&name);
            CComVariant value;
            attrib->get_nodeValue(&value);

            if (name == L"bpm")
            {
                value.ChangeType(VT_BSTR);
                m_bpm = _wtof(value.bstrVal);
                m_secperbeat = 60.0 / m_bpm;
            }
            else if (name == L"beatspermeasure")
            {
                value.ChangeType(VT_BSTR);
                m_beatspermeasure = _wtoi(value.bstrVal);
            }
        }
    }

    CComPtr<IXMLDOMNode> node;
    xml->get_firstChild(&node);
    for (; node != NULL; NextNode(node))
    {
        CComBSTR name;
        node->get_nodeName(&name);
        if (name == L"instrument") XmlLoadInstrument(node);
    }
}

void CSynthesizer::XmlLoadInstrument(IXMLDOMNode* xml)
{
    wstring instrument = L"";
    CComPtr<IXMLDOMNamedNodeMap> attributes;
    xml->get_attributes(&attributes);
    if (attributes)
    {
        long len = 0;
        attributes->get_length(&len);

        for (int i = 0; i < len; i++)
        {
            CComPtr<IXMLDOMNode> attrib;
            attributes->get_item(i, &attrib);
            CComBSTR name;
            attrib->get_nodeName(&name);
            CComVariant value;
            attrib->get_nodeValue(&value);

            if (name == L"instrument")
                instrument = value.bstrVal;
        }
    }

    CComPtr<IXMLDOMNode> node;
    xml->get_firstChild(&node);
    for (; node != NULL; NextNode(node))
    {
        CComBSTR name;
        node->get_nodeName(&name);
        if (name == L"note") XmlLoadNote(node, instrument);
    }
}

void CSynthesizer::XmlLoadNote(IXMLDOMNode* xml, std::wstring& instrument)
{
    m_notes.push_back(CNote());
    m_notes.back().XmlLoad(xml, instrument);
}
