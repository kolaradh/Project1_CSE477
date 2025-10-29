#pragma once
#include "CAudioNode.h"
#include <cmath>

/// Class that implements a sine wave generator.
class CSineWave : public CAudioNode
{
public:
    CSineWave();

    //! Start audio generation
    virtual void Start() override;

    //! Generate one frame of audio
    virtual bool Generate() override;

    //! Set the sine wave frequency
    void SetFreq(double f) { m_freq = f; }

    //! Set the sine wave amplitude
    void SetAmplitude(double a) { m_amp = a; }

    //! Get the sine wave frequency
    double GetFreq() const { return m_freq; }


private:
    double m_freq;   ///< Frequency of the sine wave in Hz
    double m_amp;    ///< Amplitude of the sine wave
    double m_phase;  ///< Phase of the sine wave in cycles (0–1)
};

