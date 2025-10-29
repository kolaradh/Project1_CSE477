#pragma once
#include <vector>
#include <algorithm>

class CEffects
{
public:
    CEffects();

    void SetSampleRate(double rate);
    void SetDelayMs(double ms);
    void SetFeedback(double fb);
    void SetMix(double mix);
    void SetPostGain(double gain);

    // In-place stereo processing
    void Process(double* frame);

private:
    void ReallocBuffer();

    double m_sampleRate = 44100.0;
    double m_delayMs = 250.0;   // ms
    double m_feedback = 0.35;    // 0..1
    double m_mix = 0.25;    // wet
    double m_postGain = 0.8;

    std::vector<double> m_bufL;
    std::vector<double> m_bufR;
    int m_idx = 0;
};
