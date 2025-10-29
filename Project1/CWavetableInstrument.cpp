#include "pch.h"
#include "CWavetableInstrument.h"
#include "Notes.h"               // NoteToFrequency helpers if needed
#include <atlbase.h>
#include <atlcomcli.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// ---------- ctor ----------
CWavetableInstrument::CWavetableInstrument()
{
    BuildTables();
}

// ---------- table build: bright A, warm B ----------
void CWavetableInstrument::BuildTables()
{
    const int N = 2048;
    m_tableA.assign(N, 0.0);
    m_tableB.assign(N, 0.0);

    for (int i = 0; i < N; ++i)
    {
        const double t = (double)i / N;          // 0..1
        const double ph = 2.0 * PI * t;

        // A: bright (saw+sine)
        const double saw = 2.0 * t - 1.0;
        const double sine = sin(ph);
        m_tableA[i] = 0.70 * saw + 0.30 * sine;

        // B: warm (triangle+sine)
        const double tri = 2.0 * fabs(2.0 * (t - floor(t + 0.5))) - 1.0;
        m_tableB[i] = 0.50 * tri + 0.50 * sine;
    }

    TRACE(L"[Wavetable] Built A/B tables, N=%d\n", N);
}

// ---------- helpers ----------
double CWavetableInstrument::ReadInterpolated(const std::vector<double>& tbl, double index) const
{
    const int N = (int)tbl.size();
    int i0 = (int)index;
    if (i0 >= N) i0 -= N;
    int i1 = (i0 + 1) % N;
    const double frac = index - (double)i0;
    return (1.0 - frac) * tbl[i0] + frac * tbl[i1];
}

double CWavetableInstrument::MidiToHz(int midi) const
{
    return 440.0 * pow(2.0, (midi - 69) / 12.0);
}

// ---------- SetNote: parse XML via COM so it matches any CNote impl ----------
void CWavetableInstrument::SetNote(CNote* note)
{
    if (!note) return;

    // Defaults
    double freq = 440.0, durBeats = 0.25, gain = 1.0, pan = 0.5;
    double crossfade = 0.0;
    double glideToHz = 440.0, glideTime = 0.0;
    bool   glideExpo = false;

    // Parse generic XML attributes from the <note ...> node
    CComPtr<IXMLDOMNode> node = note->Node();
    if (node)
    {
        CComPtr<IXMLDOMNamedNodeMap> attrs;
        node->get_attributes(&attrs);
        if (attrs)
        {
            long len = 0; attrs->get_length(&len);
            for (long i = 0; i < len; ++i)
            {
                CComPtr<IXMLDOMNode> attrib;
                attrs->get_item(i, &attrib);
                if (!attrib) continue;

                CComBSTR name;
                attrib->get_nodeName(&name);
                CComVariant value;
                attrib->get_nodeValue(&value);
                value.ChangeType(VT_BSTR);

                if (name == L"duration")        durBeats = _wtof(value.bstrVal);
                else if (name == L"note")       freq = NoteToFrequency(value.bstrVal);
                else if (name == L"midi")       freq = MidiToHz(_wtoi(value.bstrVal));
                else if (name == L"vel")        gain = _wtof(value.bstrVal);
                else if (name == L"pan")        pan = _wtof(value.bstrVal);
                else if (name == L"crossfade")  crossfade = _wtof(value.bstrVal);
                else if (name == L"gliss")      glideToHz = MidiToHz(_wtoi(value.bstrVal));   // e.g., gliss="76"
                else if (name == L"glisshz")    glideToHz = _wtof(value.bstrVal);             // direct Hz
                else if (name == L"glisstime")  glideTime = _wtof(value.bstrVal);             // seconds
                else if (name == L"glissexpo")  glideExpo = (_wtoi(value.bstrVal) != 0);      // 0/1
            }
        }
    }

    // Commit params (with safe ranges)
    m_freq = freq;
    m_duration = durBeats;
    m_gain = (gain < 0.0) ? 0.0 : ((gain > 2.0) ? 2.0 : gain);
    m_pan = Clamp01(pan);
    m_crossfade = Clamp01(crossfade);

    m_targetFreq = (glideTime > 0.0) ? glideToHz : freq;
    m_glissTime = (glideTime > 0.0) ? glideTime : 0.0;
    m_glissExpo = glideExpo;
    m_enableGliss = (m_glissTime > 0.0);

    TRACE(L"[Wavetable::SetNote] f=%.2fHz dur=%.2f vel=%.2f pan=%.2f cf=%.2f gliss->%.2fHz t=%.2fs expo=%d\n",
        m_freq, m_duration, m_gain, m_pan, m_crossfade, m_targetFreq, m_glissTime, m_glissExpo ? 1 : 0);
}

// ---------- Start ----------
void CWavetableInstrument::Start()
{
    m_index = 0.0;
    m_time = 0.0;
    m_last = 0.0;
    m_released = false;

    m_startFreq = m_freq;

    m_env.SetSampleRate(GetSampleRate());
    m_env.SetADSR(0.02, 0.10, 0.85, 0.25);
    m_env.Start();

    TRACE(L"[Wavetable] Start f=%.2fHz dur=%.2f beats gliss=%d tgt=%.2fHz t=%.2fs cf=%.2f\n",
        m_freq, m_duration, m_enableGliss ? 1 : 0, m_targetFreq, m_glissTime, m_crossfade);
}

// ---------- Generate ----------
bool CWavetableInstrument::Generate()
{
    const double sr = GetSampleRate();
    const double dt = 1.0 / sr;
    const double noteSeconds = m_duration * GetSecPerBeat();

    // trigger release when the scheduled duration elapses
    if (!m_released && m_time >= noteSeconds)
    {
        m_env.Release();
        m_released = true;
        TRACE(L"[Wavetable] Release @ t=%.3fs\n", m_time);
    }

    // current frequency with optional gliss
    double fNow;
    if (m_enableGliss && m_glissTime > 0.0 && m_time < m_glissTime)
    {
        const double a = Clamp01(m_time / m_glissTime);
        if (m_glissExpo && m_startFreq > 0.0 && m_targetFreq > 0.0)
            fNow = m_startFreq * pow(m_targetFreq / m_startFreq, a);  // exponential glide
        else
            fNow = (1.0 - a) * m_startFreq + a * m_targetFreq;        // linear glide
    }
    else
    {
        fNow = m_targetFreq; // steady
    }

    // advance wavetable phase
    const double N = (double)m_tableA.size();
    const double step = fNow * N / sr;
    m_index += step;
    while (m_index >= N) m_index -= N;

    // attack morph A->B, then sustain crossfade
    double blended;
    if (m_useAttackSustain && m_time < m_attackTableTime)
    {
        const double a = Clamp01(m_time / m_attackTableTime);
        const double aSamp = ReadInterpolated(m_tableA, m_index);
        const double bSamp = ReadInterpolated(m_tableB, m_index);
        blended = (1.0 - a) * aSamp + a * bSamp;
    }
    else
    {
        const double x = Clamp01(m_crossfade);
        const double aSamp = ReadInterpolated(m_tableA, m_index);
        const double bSamp = ReadInterpolated(m_tableB, m_index);
        blended = (1.0 - x) * aSamp + x * bSamp;
    }

    // tiny de-zipper before ADSR
    m_last = (1.0 - m_smooth) * m_last + m_smooth * blended;

    // ADSR
    double s = m_env.Process(m_last);

    // stop when envelope ends
    if (!m_env.IsActive())
        return false;

    // pan + soft clip
    s *= m_gain * 0.7;
    const double pan = Clamp01(m_pan);
    const double gL = cos(0.5 * PI * pan);
    const double gR = sin(0.5 * PI * pan);
    auto softclip = [](double x) { return x / (1.0 + 0.5 * fabs(x)); };

    m_frame[0] = softclip(s * gL);
    m_frame[1] = softclip(s * gR);

    m_time += dt;
    return true;
}
