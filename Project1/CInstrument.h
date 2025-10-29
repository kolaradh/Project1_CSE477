#pragma once
#include "CAudioNode.h"
#include "CNote.h"

// Base class for all musical instruments
class CInstrument : public CAudioNode
{
public:
    CInstrument();
    virtual ~CInstrument();

    // Core interface
    virtual void Start() = 0;
    virtual bool Generate() = 0;
    virtual void SetNote(CNote* note) = 0;

    // Audio/time helpers
    double Frame(int c) const { return m_frame[c]; }
    void   SetSampleRate(double s) { m_sampleRate = s; }
    double GetSampleRate() const { return m_sampleRate; }

    void   SetBPM(double bpm) { m_bpm = bpm; }
    double GetSecPerBeat() const { return 60.0 / m_bpm; }
    double GetSamplePeriod() const { return 1.0 / m_sampleRate; }

protected:
    double m_frame[2] = { 0.0, 0.0 };  // stereo frame
    double m_sampleRate = 44100.0;
    double m_bpm = 120.0;
};
