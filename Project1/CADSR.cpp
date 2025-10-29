#include "pch.h"
#include "CADSR.h"

template <typename T>
static inline T clamp_val(T v, T lo, T hi)
{
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

CADSR::CADSR() {}

void CADSR::SetADSR(double attack, double decay, double sustain, double release)
{
    m_attack = (attack > 0) ? attack : 1e-6;
    m_decay = (decay > 0) ? decay : 1e-6;
    m_sustain = clamp_val(sustain, 0.0, 1.0);
    m_release = (release > 0) ? release : 1e-6;
}

void CADSR::Start()
{
    m_time = 0.0;
    m_level = 0.0;
    m_releasing = false;
}

void CADSR::Release()
{
    m_releasing = true;
    m_time = 0.0;
}

double CADSR::Process(double input)
{
    const double dt = 1.0 / m_sampleRate;

    if (!m_releasing)
    {
        if (m_time < m_attack)
        {
            // linear attack to 1.0
            m_level = m_time / m_attack;
        }
        else if (m_time < m_attack + m_decay)
        {
            // linear decay to sustain
            double t = (m_time - m_attack) / m_decay;          // 0..1
            m_level = 1.0 + (m_sustain - 1.0) * t;
        }
        else
        {
            m_level = m_sustain;
        }
    }
    else
    {
        // smooth exponential release
        m_level *= std::exp(-dt / m_release);
        if (m_level < 0.0005) m_level = 0.0;
    }

    m_time += dt;
    return input * m_level;
}
