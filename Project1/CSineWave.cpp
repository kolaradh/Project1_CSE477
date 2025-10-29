#include "pch.h"
#include "CSineWave.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

CSineWave::CSineWave()
{
    m_phase = 0;
    m_amp = 0.1;
    m_freq = 440;   // Default: standard A4 pitch
}

void CSineWave::Start()
{
    m_phase = 0;
}

bool CSineWave::Generate()
{
    // Compute left and right channel values (identical for mono-to-stereo)
    m_frame[0] = m_amp * sin(m_phase * 2 * PI);
    m_frame[1] = m_frame[0];

    // Advance phase by one sample
    m_phase += m_freq * GetSamplePeriod();

    // Wrap phase back into [0, 1) range
    if (m_phase > 1.0)
    {
        m_phase -= 1.0;
    }

    return true;
}
