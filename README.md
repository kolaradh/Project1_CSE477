# 🎵 Project 1 — Wavetable Synthesizer  
**Author:** Adhvik Kunal Kolar  
**Course:** CSE 471 

**Institution:** Michigan State University  

---

## 🎧 Overview  

This project implements a **Wavetable Synthesizer System** capable of generating high-quality audio using short waveform tables, envelopes, and dynamic effects.  
The synthesizer reads musical score files in XML format (`.score`), streams audio output sample-by-sample, and exports stereo `.wav` files at **44.1 kHz / 16-bit** quality.  

The project produces:
- A **1–1.5 minute musical selection** that demonstrates the full system  
- Two **10–15 second component demo pieces** for grading:  
  - **Wavetable Demo:** core synthesis (pitch, glissando, ADSR, crossfade)  
  - **Effects Demo:** delay and feedback effects applied to synthesized tones  

---

## ⚙️ System Description  

| Module | Description |
|:--|:--|
| **CWavetableInstrument** | Implements wavetable synthesis with two tables (bright A and warm B), glissando, ADSR envelopes, and table crossfading. |
| **CADSR** | Controls the Attack–Decay–Sustain–Release envelope to shape sound and remove clicks. |
| **CEffects** | Adds stereo delay and feedback effects for spatial and echo-based sounds. |
| **CSynthesizer** | Core driver that loads and plays `.score` files while managing beats and timing. |
| **CInstrument / CNote** | Defines basic instrument and note properties and links with the XML parser. |

---

## 🧱 Key Features  

✅ **Pitch Control** — Converts musical notes or MIDI numbers to frequency  
✅ **Glissando** — Smooth pitch slides between notes  
✅ **Table Crossfading** — Blends between bright and warm tone tables  
✅ **Envelope Generation** — ADSR dynamics for natural instrument feel  
✅ **Stereo Panning** — Left-right balance control per note  
✅ **Delay/Feedback Effects** — Adds spatial depth and realistic echo  
✅ **Real-Time Streaming** — Generates sound per-sample (meets streaming requirement)  

---

## ▶️ How to Build & Launch the Application  

### 🧩 Requirements  
- **Windows + Visual Studio 2022**  
- **MSXML 2.0** library (for XML score parsing)  
- CSE 477 Synthie Base Code (framework provided in class)

---

### 🛠️ Build Instructions  
1. Open the Visual Studio Solution file:  
2. Set the project configuration to **Debug** or **Release** (x64).  
3. Build the solution (`Ctrl + Shift + B`).  
4. After a successful build, launch the executable from within Visual Studio (press `F5`) or from the output directory.

---

### 🎼 Running the Synthesizer  

Once the Synthie application window opens:

1. **Open a Score File**
- Click **File → Open Score File**.  
- Navigate to the folder `score files/`.  
- Select either:
  - `wavetable_demo.score` → shows wavetable synthesis  
  - `effects_demo.score` → shows delay/feedback effects  

2. **View and Edit Parameters (Optional)**
- You can inspect each note’s attributes such as `duration`, `note`, `crossfade`, and `glissando`.  
- Edits can be saved using **File → Save As...**.

3. **Synthesize Audio**
- Click **Render** or **Synthesize** to generate sound.  
- The output will be saved as a `.wav` file (e.g., `wavetable_demo.wav`) in the `wav/` folder.  

4. **Playback**
- Play the rendered `.wav` file in any media player to verify the output.  
- Each demo lasts approximately **15 seconds**, while the final musical piece is **1–1.5 minutes** long.

---

## 🧪 Demo Details  

### 🎶 Wavetable Demo (≈15 seconds)
Demonstrates:
- Table A → Table B crossfade morph  
- Smooth glissando transitions  
- ADSR-based envelope shaping  
- Stereo panning control  

### 🎵 Effects Demo (≈15 seconds)
Demonstrates:
- Delay and feedback with adjustable wet/dry mix  
- Echo and reflection behavior  
- Enhanced depth and realism  

### 🎼 Full Composition (≈1 minute)
- Built on the **“Sa Re Ga Ma Pa Da Ni Sa”** melodic sequence  
- Uses layered wavetables, panning, and envelope shaping  
- Recognizable musical phrasing with rhythmic consistency  

---

## 📊 Output Specifications  

| Property | Value |
|:--|:--|
| Sampling Rate | 44,100 Hz |
| Bit Depth | 16-bit |
| Channels | Stereo |
| Demo Duration | 15 seconds each |
| Full Composition Duration | 1–1.5 minutes |
| Output Format | `.wav` |

---

## 👨‍💻 Author  

**Adhvik Kunal Kolar**  
Department of Computer Science  
Michigan State University  
📍 East Lansing, Michigan  
🔗 [GitHub: kolaradh](https://github.com/kolaradh)

---

## 🧠 Acknowledgements  
- CSE 471 course staff and Professor Yiying Tong for the framework  
- MSXML2 library for XML parsing  
- Audacity and Visual Studio for waveform and playback testing  

---

> *“Music is the arithmetic of sounds as optics is the geometry of light.”*  
> — **Claude Debussy**
