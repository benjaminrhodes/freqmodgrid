# FrequadMod - FM Synthesizer

A 6-operator FM synthesizer plugin for VST3-compatible DAWs.

## Features

- 6 FM operators with configurable ratio, level, and feedback
- 8 algorithm presets for different modulation routings
- Multi-mode filter (LP/HP)
- ADSR envelope
- 2 LFOs with multiple waveforms
- Built-in chorus and delay effects
- MIDI note input
- Polyphonic (up to 16 voices)
- 30+ factory presets

## Building

### Prerequisites

- **Windows**: Visual Studio 2022, CMake 3.20+
- **macOS**: Xcode 15+, CMake 3.20+
- **VST3 SDK**: Download from https://github.com/steinbergmedia/vst3sdk

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/freqmod/freqmod-grid.git
cd freqmod-grid
```

2. Initialize VST3 SDK submodule:
```bash
git submodule update --init --recursive
```

3. Build:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

4. The plugin will be at `build/FreqmodGrid.vst3`

### macOS Specific

For a universal binary (Intel + Apple Silicon):
```bash
cmake .. -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
```

## Installation

### Windows
Copy `FreqmodGrid.vst3` to:
- User: `%USERPROFILE%\Documents\VST3\`
- System: `%PROGRAMFILES%\VST3\`

### macOS
Copy `FreqmodGrid.vst3` to:
- User: `~/Library/Audio/Plug-Ins/VST3/`
- System: `/Library/Audio/Plug-Ins/VST3/`

## Parameter Reference

| ID | Parameter | Range | Default |
|----|-----------|-------|---------|
| 0 | Master Volume | 0.0-1.0 | 0.7 |
| 1-3 | Operator 1 | Ratio: 0.25-32, Level: 0-1, Feedback: 0-1 |
| 4-6 | Operator 2 | Same as Op1 |
| 7-9 | Operator 3 | Same as Op1 |
| 10-12 | Operator 4 | Same as Op1 |
| 13-15 | Operator 5 | Same as Op1 |
| 16-18 | Operator 6 | Same as Op1 |
| 19 | Algorithm | 1-8 | 1 |
| 20 | Filter Type | 0=LP, 1=HP | 0 |
| 21 | Filter Cutoff | 20-20000 Hz | 12000 |
| 22 | Filter Resonance | 0.0-1.0 | 0.0 |
| 23 | Env Attack | 0.001-5.0s | 0.01 |
| 24 | Env Decay | 0.001-5.0s | 0.1 |
| 25 | Env Sustain | 0.0-1.0 | 0.7 |
| 26 | Env Release | 0.01-10s | 0.3 |
| 27-28 | LFO1 | Rate: 0.01-20Hz, Depth: 0-1 |
| 29-30 | LFO2 | Same as LFO1 |
| 31-32 | Chorus | Rate: 0.1-10Hz, Depth: 0-1 |
| 33-34 | Delay | Time: 0.001-2s, Feedback: 0-0.9 |

## Algorithm Reference

1. **Serial**: 1→2→3→4→5→6
2. **Parallel 2**: (1+2)→3→4→5→6
3. **Stack**: 1→(2+3+4+5+6)
4. **Dual Parallel**: ((1+2)+(3+4))→5→6
5. **3 Pairs**: 1→2, 3→4, 5→6
6. **2 Groups**: (1+2+3)→(4+5+6)
7. **2 Chains**: 1→2→3, 4→5→6
8. **Sum**: All summed

## License

MIT License - See LICENSE file for details.

## Credits

- FM Engine: Custom DSP implementation
- VST3 SDK: Steinberg Media Technologies
