// SynthieView.cpp : implementation of the CSynthieView class
//

#include "pch.h"
#include "Synthie.h"
#include "SynthieView.h"
#include "CSynthesizer.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Helper to clamp values to 16-bit range
static inline short RangeBound(double d)
{
    if (d < -32768.0) return -32768;
    if (d > 32767.0) return 32767;
    return static_cast<short>(d);
}

// Constructor
CSynthieView::CSynthieView()
{
    // Initialize synthesizer with default audio configuration
    m_synthesizer.SetNumChannels(NumChannels());
    m_synthesizer.SetSampleRate(SampleRate());

    m_audiooutput = true;
    m_fileoutput = false;
}

CSynthieView::~CSynthieView()
{
}

// Message map
BEGIN_MESSAGE_MAP(CSynthieView, CWnd)
    ON_WM_PAINT()
    ON_COMMAND(ID_GENERATE_FILEOUTPUT, &CSynthieView::OnGenerateFileoutput)
    ON_UPDATE_COMMAND_UI(ID_GENERATE_FILEOUTPUT, &CSynthieView::OnUpdateGenerateFileoutput)
    ON_COMMAND(ID_GENERATE_AUDIOOUTPUT, &CSynthieView::OnGenerateAudiooutput)
    ON_UPDATE_COMMAND_UI(ID_GENERATE_AUDIOOUTPUT, &CSynthieView::OnUpdateGenerateAudiooutput)
    ON_COMMAND(ID_GENERATE_1000HZTONE, &CSynthieView::OnGenerate1000hztone)
    ON_COMMAND(ID_GENERATE_SYNTHESIZER, &CSynthieView::OnGenerateSynthesizer)
    ON_COMMAND(ID_FILE_OPENSCORE, &CSynthieView::OnFileOpenscore)
END_MESSAGE_MAP()

// Paint handler
void CSynthieView::OnPaint()
{
    CPaintDC dc(this); // required for painting
    // You can add custom drawing here if needed
}

// 1000Hz Tone Generator
void CSynthieView::OnGenerate1000hztone()
{
    if (!GenerateBegin())
        return;

    short audio[2];
    double freq = 1000.0;
    double duration = 5.0;

    for (double time = 0.0; time < duration; time += 1.0 / SampleRate())
    {
        audio[0] = static_cast<short>(32000.0 * sin(time * 2.0 * PI * freq));
        audio[1] = audio[0];
        GenerateWriteFrame(audio);

        if (ProgressAbortCheck())
        {
            TRACE("Generation aborted by user.\n");
            break;
        }
    }

    GenerateEnd();
}

// === SYNTHESIZER TEST HANDLER ===
void CSynthieView::OnGenerateSynthesizer()
{
    TRACE("CSynthieView::OnGenerateSynthesizer() called.\n");
    if (!GenerateBegin())
        return;

    m_synthesizer.Start();
    TRACE("Synthesizer started successfully.\n");
    short  audio[2] = { 0, 0 };
    double frame[2] = { 0.0, 0.0 };

    while (m_synthesizer.Generate(frame))
    {
        audio[0] = RangeBound(frame[0] * 32767.0);
        audio[1] = RangeBound(frame[1] * 32767.0);

        GenerateWriteFrame(audio);

        if (ProgressAbortCheck())
        {
            TRACE("Generation aborted by user.\n");
            break;
        }
    }
    TRACE("OnGenerateSynthesizer: end — t=%.3f s\n", m_synthesizer.GetTime());
    GenerateEnd();
}

// === File & Audio Output Toggles ===
void CSynthieView::OnGenerateFileoutput()
{
    m_fileoutput = !m_fileoutput;
}

void CSynthieView::OnUpdateGenerateFileoutput(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_fileoutput);
}

void CSynthieView::OnGenerateAudiooutput()
{
    m_audiooutput = !m_audiooutput;
}

void CSynthieView::OnUpdateGenerateAudiooutput(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_audiooutput);
}

// === Audio Generation Backend ===
BOOL CSynthieView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW),
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
        NULL);

    return TRUE;
}

// Opens audio stream for generation
bool CSynthieView::GenerateBegin()
{
    m_waveformBuffer.Start(NumChannels(), SampleRate());

    if (m_fileoutput)
    {
        if (!OpenGenerateFile(m_wave))
            return false;
    }

    ProgressBegin(this);

    if (m_audiooutput)
    {
        m_soundstream.SetChannels(NumChannels());
        m_soundstream.SetSampleRate(int(SampleRate()));
        m_soundstream.Open(((CSynthieApp*)AfxGetApp())->DirSound());
    }

    return true;
}

// Writes one frame of audio to outputs
void CSynthieView::GenerateWriteFrame(short* p_frame)
{
    m_waveformBuffer.Frame(p_frame);

    if (m_fileoutput)
        m_wave.WriteFrame(p_frame);

    if (m_audiooutput)
        m_soundstream.WriteFrame(p_frame);
}

// Ends audio generation
void CSynthieView::GenerateEnd()
{
    m_waveformBuffer.End();

    if (m_fileoutput)
        m_wave.close();

    if (m_audiooutput)
        m_soundstream.Close();

    ProgressEnd(this);
}

// Opens file for wave output
bool CSynthieView::OpenGenerateFile(CWaveOut& p_wave)
{
    p_wave.NumChannels(NumChannels());
    p_wave.SampleRate(SampleRate());

    static WCHAR BASED_CODE szFilter[] = L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||";

    CFileDialog dlg(FALSE, L".wav", NULL, 0, szFilter, NULL);
    if (dlg.DoModal() != IDOK)
        return false;

    p_wave.open(dlg.GetPathName());
    if (p_wave.fail())
        return false;

    return true;
}

void CSynthieView::OnFileOpenscore()
{
    static WCHAR BASED_CODE szFilter[] = L"Score files (*.score)|*.score|All Files (*.*)|*.*||";

    // Display file open dialog filtered for .score files
    CFileDialog dlg(TRUE, L".score", NULL, 0, szFilter, NULL);
    if (dlg.DoModal() != IDOK)
        return;

    // Pass filename to the synthesizer
    m_synthesizer.OpenScore(dlg.GetPathName());
}
