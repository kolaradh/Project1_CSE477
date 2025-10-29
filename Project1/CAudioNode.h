#pragma once

//! Base class for all audio-generating and processing nodes
class CAudioNode
{
public:
    CAudioNode(void);

    //! Start the node generation
    virtual void Start() = 0;

    //! Cause one sample to be generated
    virtual bool Generate() = 0;

    //! Get the sample rate in samples per second
    double GetSampleRate() const { return m_sampleRate; }

    //! Get the sample period in seconds
    double GetSamplePeriod() const { return m_samplePeriod; }

    //! Set the sample rate
    void SetSampleRate(double s)
    {
        m_sampleRate = s;
        m_samplePeriod = (s != 0.0) ? 1.0 / s : 0.0;
    }

    //! Access the generated audio frame
    const double* Frame() const { return m_frame; }

    //! Access one channel of a generated audio frame safely
    double Frame(int c) const
    {
        if (c < 0 || c >= 2)
            return 0.0;
        return m_frame[c];
    }

    //! Set the beats per minute
    void SetBPM(double bpm)
    {
        m_bpm = bpm;
        m_secPerBeat = (bpm != 0.0) ? 60.0 / bpm : 0.5;
    }

    //! Get the beats per minute
    double GetBPM() const { return m_bpm; }

    //! Get seconds per beat
    double GetSecPerBeat() const { return m_secPerBeat; }

protected:
    double m_sampleRate;      //!< Samples per second
    double m_samplePeriod;    //!< Seconds per sample
    double m_frame[2];        //!< Stereo audio frame (L,R)
    double m_bpm;             //!< Beats per minute
    double m_secPerBeat;      //!< Seconds per beat (60/bpm)
};
