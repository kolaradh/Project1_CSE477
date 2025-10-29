#include "pch.h"
#include "CToneInstrument.h"
#include "CNote.h"
#include "Notes.h"

CToneInstrument::CToneInstrument()
{
    m_duration = 0.25; // quarter beat default
    m_time = 0.0;
}

void CToneInstrument::Start()
{
    m_sinewave.SetSampleRate(GetSampleRate());
    m_sinewave.Start();
    m_time = 0.0;

    // Configure Attack/Release
    m_ar.SetSource(&m_sinewave);
    m_ar.SetSampleRate(GetSampleRate());
    m_ar.SetDuration(m_duration);   // from your note length
    m_ar.Start();
}



bool CToneInstrument::Generate()
{
    // Generate next frame from sinewave
    m_sinewave.Generate();

    // Copy stereo samples
    m_frame[0] = m_sinewave.Frame(0);
    m_frame[1] = m_sinewave.Frame(1);

    // Advance time
    m_time += GetSamplePeriod();

    // Convert beats to seconds using BPM
    double durationSeconds = m_duration * GetSecPerBeat();
    bool playing = (m_time < durationSeconds);

    if (!playing)
    {
        TRACE(L"[ToneInstrument] Done note freq=%.2f Hz, duration=%.2f beats (%.3f s total)\n",
            m_sinewave.GetFreq(), m_duration, durationSeconds);
    }

    return playing;
}

void CToneInstrument::SetNote(CNote* note)
{
    if (!note) return;

    CComPtr<IXMLDOMNamedNodeMap> attributes;
    note->Node()->get_attributes(&attributes);
    if (!attributes) return;

    long len = 0;
    attributes->get_length(&len);

    double freq = 440.0;    // Default A4
    double duration = 0.25; // Default quarter beat

    for (int i = 0; i < len; i++)
    {
        CComPtr<IXMLDOMNode> attrib;
        attributes->get_item(i, &attrib);
        if (!attrib) continue;

        CComBSTR name;
        attrib->get_nodeName(&name);

        CComVariant value;
        attrib->get_nodeValue(&value);
        value.ChangeType(VT_BSTR);

        if (name == L"duration")
        {
            duration = _wtof(value.bstrVal);
        }
        else if (name == L"note")
        {
            freq = NoteToFrequency(value.bstrVal);
        }
    }

    SetDuration(duration);
    SetFreq(freq);

    TRACE(L"[ToneInstrument::SetNote] Parsed note=%ls  freq=%.2f Hz  duration=%.2f beats  bpm=%.2f\n",
        note->Instrument().c_str(), freq, duration, GetBPM());
}
