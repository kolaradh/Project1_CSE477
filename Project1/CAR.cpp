#include "pch.h"
#include "CAR.h"
#include <algorithm>

CAR::CAR()
{
}

void CAR::Start()
{
    m_time = 0.0;
    m_active = true;
}

bool CAR::Generate()
{
    if (!m_source)
        return false;

    // Generate source sample first
    m_source->Generate();

    // Copy its frame
    m_frame[0] = m_source->Frame(0);
    m_frame[1] = m_source->Frame(1);

    // Compute envelope multiplier
    double amplitude = 1.0;
    if (m_time < m_attack)
    {
        amplitude = m_time / m_attack; // linear fade-in
    }
    else if (m_time > m_duration - m_release)
    {
        double t = (m_duration - m_time) / m_release;
        amplitude = (std::max)(0.0, t);
    }

    // Apply envelope
    m_frame[0] *= amplitude;
    m_frame[1] *= amplitude;

    // Advance time
    m_time += GetSamplePeriod();   // ✅ FIXED

    // End note if we pass total duration
    if (m_time > m_duration)
    {
        m_active = false;
        return false;
    }

    return m_active;
}
