#pragma once

#include "msxml2.h"
#pragma comment(lib, "msxml2.lib")
#include <list>
#include <vector>
#include <string>
#include "CInstrument.h"
#include "CNote.h"
#include "CEffects.h"

class CSynthesizer
{
public:
    CSynthesizer();
    virtual ~CSynthesizer();

    // === Configuration ===
    int    GetNumChannels() const { return m_channels; }
    double GetSampleRate()  const { return m_sampleRate; }
    double GetSamplePeriod()const { return m_samplePeriod; }
    void   SetNumChannels(int n) { m_channels = n; }
    void   SetSampleRate(double s) { m_sampleRate = s; m_samplePeriod = 1.0 / s; }

    void Clear();
    void OpenScore(CString& filename);

    void Start();
    bool Generate(double* frame);
    double GetTime() const { return m_time; }

private:
    // === Audio setup ===
    int    m_channels = 2;
    double m_sampleRate = 44100.0;
    double m_samplePeriod = 1.0 / 44100.0;
    double m_time = 0.0;

    // === Instruments & Notes ===
    std::list<CInstrument*> m_instruments;
    std::vector<CNote>      m_notes;

    // === Score timing ===
    double  m_bpm = 120.0;              //!< Beats per minute
    int     m_beatspermeasure = 4;      //!< Beats per measure
    double  m_secperbeat = 0.5;         //!< Seconds per beat

    // === XML helpers ===
    void XmlLoadScore(IXMLDOMNode* xml);
    void XmlLoadInstrument(IXMLDOMNode* xml);
    void XmlLoadNote(IXMLDOMNode* xml, std::wstring& instrument);

    // === Playback tracking ===
    int m_currentNote = 0;      //!< The current note we are playing
    int m_measure = 0;          //!< The current measure
    double m_beat = 0.0;        //!< The current beat within the measure

    CEffects m_effects;         //!< Global effects processor
};
