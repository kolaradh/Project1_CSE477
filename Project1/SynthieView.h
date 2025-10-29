// SynthieView.h : interface of the CSynthieView class
//

#pragma once

#include "Progress.h"
#include "audio/wave.h"
#include "audio/DirSoundStream.h"
#include "audio/WaveformBuffer.h"
#include "CSynthesizer.h"   // include synthesizer

// CSynthieView window
class CSynthieView : public CWnd, private CProgress
{
    // Construction
public:
    CSynthieView();
    virtual ~CSynthieView();

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()

private:
    bool  m_fileoutput;
    bool  m_audiooutput;

    void  GenerateWriteFrame(short* p_frame);
    bool  OpenGenerateFile(CWaveOut& p_wave);
    void  GenerateEnd();
    bool  GenerateBegin();

    CSynthesizer  m_synthesizer;  // our synthesizer object

    // Audio destinations
    CWaveOut        m_wave;
    CDirSoundStream m_soundstream;
    CWaveformBuffer m_waveformBuffer;

    int    NumChannels() { return 2; }
    double SampleRate() { return 44100; }

public:
    afx_msg void OnGenerateFileoutput();
    afx_msg void OnUpdateGenerateFileoutput(CCmdUI* pCmdUI);
    afx_msg void OnGenerateAudiooutput();
    afx_msg void OnUpdateGenerateAudiooutput(CCmdUI* pCmdUI);
    afx_msg void OnGenerate1000hztone();
    afx_msg void OnGenerateSynthesizer();
    afx_msg void OnFileOpenscore();

};
