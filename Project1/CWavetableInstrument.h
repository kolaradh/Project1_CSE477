#pragma once
#include "CInstrument.h"
#include "CNote.h"
#include "CAudioNode.h"
#include "CADSR.h"
#include <vector>

class CWavetableInstrument : public CInstrument
{
public:
    CWavetableInstrument();
    virtual ~CWavetableInstrument() {}

    void Start() override;
    bool Generate() override;
    void SetNote(CNote* note) override;

private:
    void BuildTables();
    double ReadInterpolated(const std::vector<double>& tbl, double index) const;

    static inline double Clamp01(double x)
    {
        return (x < 0.0) ? 0.0 : ((x > 1.0) ? 1.0 : x);
    }
    double MidiToHz(int midi) const;

private:
    // ---- musical params (from score) ----
    double m_freq = 440.0;   // Hz (base)
    double m_duration = 0.25;    // beats
    double m_gain = 1.0;     // 0..2
    double m_pan = 0.5;     // 0..1 (L..R)
    double m_crossfade = 0.0;     // 0..1 (A..B)

    // ---- wavetable ----
    std::vector<double> m_tableA;
    std::vector<double> m_tableB;
    double m_index = 0.0;         // table phase index
    double m_time = 0.0;         // seconds since Start
    double m_last = 0.0;         // de-zipper state
    double m_smooth = 0.12;        // 0..1 smoothing amount

    // ---- envelope ----
    CADSR  m_env;
    bool   m_released = false;

    // ---- glissando ----
    bool   m_enableGliss = false;
    bool   m_glissExpo = false;
    double m_startFreq = 440.0;
    double m_targetFreq = 440.0;
    double m_glissTime = 0.0;    // seconds

    // ---- table morph in attack ----
    bool   m_useAttackSustain = true;
    double m_attackTableTime = 0.02; // seconds of A->B morph during attack
};
