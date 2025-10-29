#pragma once
#include <cmath>

// Simple ADSR envelope (seconds; sustain in [0,1])
class CADSR
{
public:
    CADSR();

    void SetADSR(double attack, double decay, double sustain, double release);
    void SetSampleRate(double rate) { m_sampleRate = (rate > 1000.0) ? rate : 44100.0; }

    void Start();     // begin attack
    void Release();   // begin release

    // Process one sample through the current envelope state
    double Process(double input);

    // Envelope still audible/active?
    bool IsActive() const { return m_level > 0.001 || !m_releasing; }

private:
    double m_sampleRate = 44100.0;

    // params
    double m_attack = 0.05;
    double m_decay = 0.10;
    double m_sustain = 0.80;  // 0..1
    double m_release = 0.20;

    // state
    double m_level = 0.0;
    double m_time = 0.0;
    bool   m_releasing = false;
};
