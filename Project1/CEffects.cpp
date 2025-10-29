// CEffects.cpp  —  Stereo delay (no C++17 required)
#include "pch.h"
#include "CEffects.h"
#include <cmath>

// --- Local clamp (C++17-free) ---
template<typename T>
static inline T clamp_val(T v, T lo, T hi)
{
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

CEffects::CEffects() {}

// ------------------------------------------------------------------
// Configuration
// ------------------------------------------------------------------
void CEffects::SetSampleRate(double rate)
{
    m_sampleRate = (rate > 1000.0) ? rate : 44100.0;
    ReallocBuffer();
}

void CEffects::SetDelayMs(double ms)
{
    m_delayMs = (ms >= 1.0) ? ms : 1.0;   // at least 1 ms
    ReallocBuffer();
}

void CEffects::SetFeedback(double fb)
{
    // keep a safety margin below 1.0 to avoid runaway
    m_feedback = clamp_val(fb, 0.0, 0.95);
}

void CEffects::SetMix(double mix)
{
    m_mix = clamp_val(mix, 0.0, 1.0);
}

void CEffects::SetPostGain(double gain)
{
    m_postGain = clamp_val(gain, 0.0, 2.0);
}

// ------------------------------------------------------------------
// Internal helpers
// ------------------------------------------------------------------
void CEffects::ReallocBuffer()
{
    // Convert ms -> samples (rounded to nearest)
    int delaySamples = (int)(m_delayMs * 0.001 * m_sampleRate + 0.5);
    if (delaySamples < 1) delaySamples = 1;

    m_bufL.assign(delaySamples, 0.0);
    m_bufR.assign(delaySamples, 0.0);
    m_idx = 0;

    TRACE(L"[Effects] Initialized stereo delay: %d samples (%.2f ms)\n",
        delaySamples, 1000.0 * (double)delaySamples / m_sampleRate);
}

// ------------------------------------------------------------------
// Processing (in-place stereo)
// frame[0] = left, frame[1] = right
// ------------------------------------------------------------------
void CEffects::Process(double* frame)
{
    if (!frame || m_bufL.empty()) return;

    const double inL = frame[0];
    const double inR = frame[1];

    // Read delayed taps
    const double dL = m_bufL[m_idx];
    const double dR = m_bufR[m_idx];

    // Write new values with feedback
    m_bufL[m_idx] = inL + m_feedback * dL;
    m_bufR[m_idx] = inR + m_feedback * dR;

    // Advance circular index
    ++m_idx;
    if (m_idx >= (int)m_bufL.size()) m_idx = 0;

    // Wet/dry mix per channel + output gain
    const double outL = (1.0 - m_mix) * inL + m_mix * dL;
    const double outR = (1.0 - m_mix) * inR + m_mix * dR;

    frame[0] = outL * m_postGain;
    frame[1] = outR * m_postGain;
}
