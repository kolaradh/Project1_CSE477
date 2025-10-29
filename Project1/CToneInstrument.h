#pragma once
#include "CInstrument.h"
#include "CSineWave.h"
#include "CNote.h"
#include "CAR.h"

/// Instrument that generates a tone using a sine wave.
class CToneInstrument : public CInstrument
{
public:
    CToneInstrument();
    virtual ~CToneInstrument() {}

    //! Start the instrument
    virtual void Start() override;

    //! Generate one frame of audio
    virtual bool Generate() override;

    //! Set the frequency of the tone
    void SetFreq(double f) { m_sinewave.SetFreq(f); }

    //! Set the amplitude of the tone
    void SetAmplitude(double a) { m_sinewave.SetAmplitude(a); }

    //! Set the duration of the tone (in beats)
    void SetDuration(double d) { m_duration = d; }

    virtual void SetNote(CNote* note) override;

private:
    CSineWave m_sinewave;   ///< Sine wave generator
    double m_duration = 0.25; ///< Duration in beats
    double m_time = 0.0;      ///< Elapsed time

private:
    CAR m_ar;
};
