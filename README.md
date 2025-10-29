# 🎵 Project 1 — Wavetable Synthesizer  
**Author:** Adhvik Kunal Kolar  
**Course:** CSE 477 — Web and Audio Application Development  
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

## ⚙️ System Architecture  

| Module | Description |
|:--|:--|
| **CWavetableInstrument** | Implements wavetable synthesis with two tables (bright A and warm B), glissando, ADSR envelopes, and table crossfading. |
| **CADSR** | Controls the Attack–Decay–Sustain–Release envelope to shape sound and remove clicks. |
| **CEffects** | Adds stereo delay and feedback effects for richer, spatial sound. |
| **CSynthesizer** | Core driver that parses XML `.score` files, manages beats, and streams output to audio. |
| **CInstrument / CNote** | Defines base instrument behavior and XML note handling. |

---

## 🧱 Wavetable Synthesis Features  

✅ **Pitch Control** — Converts note names or MIDI numbers to frequency  
✅ **Glissando** — Smooth pitch transitions (linear or exponential)  
✅ **Table Crossfading** — Blend between bright and warm tone tables  
✅ **Envelope Generation** — ADSR shaping for realistic dynamics  
✅ **Stereo Panning** — Independent left-right balance for each note  
✅ **Streaming Audio** — Real-time frame-by-frame synthesis (no pre-rendering)  

---

## 🎚️ Effects Processor  

The **CEffects** class adds a **Delay/Feedback** processor with adjustable parameters:

| Parameter | Description | Range |
|:--|:--|:--|
| `<wet>` | Wet signal level | 0.0–1.0 |
| `<feedback>` | Echo persistence | 0.0–1.0 |
| `<mix>` | Wet/dry output mix | 0.0–1.0 |

---

## 📂 File Structure  

