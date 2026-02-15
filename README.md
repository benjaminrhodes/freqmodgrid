# FreqmodGrid - 6-Operator FM Synthesizer

A 6-operator FM synthesizer VST3/AU plugin built with iPlug2.

## Features

- 6 FM operators with configurable ratio, level, and feedback
- 8 algorithm presets for different modulation routings
- Multi-mode biquad filter (LP/HP, 12dB/oct) with resonance
- ADSR envelope with proper exponential curves
- 2 LFOs (vibrato on pitch, modulation on filter cutoff)
- Built-in chorus and delay effects
- MIDI note input with velocity sensitivity
- 16-voice polyphony with oldest-note voice stealing
- 35 automatable parameters

## Building

### Prerequisites

- macOS: Xcode 15+, CMake 3.20+
- Windows: Visual Studio 2022, CMake 3.20+
- iPlug2: Included as submodule

### Build Instructions

```bash
git clone https://github.com/benjaminrhodes/freqmodgrid.git
cd freqmodgrid
git submodule update --init --recursive

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Or use the build script:
```bash
./scripts/build.sh
```

### Output

- VST3: `build/FreqmodGrid.vst3`
- AU: `build/FreqmodGrid.component` (macOS only)

## Installation

### macOS

```bash
# VST3
cp -r build/FreqmodGrid.vst3 ~/Library/Audio/Plug-Ins/VST3/

# AU (optional)
cp -r build/FreqmodGrid.component ~/Library/Audio/Plug-Ins/Components/
```

## Architecture

```
src/
├── iPlug/                # iPlug2 plugin wrapper
│   ├── FreqmodGrid.h/cpp # Plugin entry, parameter registration
│   └── FreqmodGrid_DSP.h # MIDI routing + FMEngine bridge
└── DSP/                  # FM synthesis engine
    ├── FMEngine.h        # 6-op FM core with voice management
    ├── Operator.h        # Single FM operator with fast sine
    ├── Envelope.h        # ADSR envelope (exponential decay)
    ├── Filter.h          # Biquad filter (12dB/oct LP/HP)
    ├── LFO.h             # Low frequency oscillator
    └── Effects.h         # Chorus and delay
```

### Algorithm Routings

| # | Name | Description |
|---|------|-------------|
| 1 | Serial | 1>2>3>4>5>6 |
| 2 | (1+2)>Chain | (1+2)>3>4>5>6 |
| 3 | 1>All | 1>(2+3+4+5+6) |
| 4 | Pairs>Chain | ((1+2)+(3+4))>5>6 |
| 5 | 3 Pairs | 1>2, 3>4, 5>6 |
| 6 | Groups | (1+2+3)>(4+5+6) |
| 7 | 2 Chains | 1>2>3, 4>5>6 |
| 8 | All Parallel | All summed (no modulation) |

## Development

The project uses iPlug2 for cross-platform plugin support:
- VST3, AU, AUv3 formats supported
- FMEngine handles voice allocation and DSP internally
- iPlug2 provides MIDI routing and parameter automation

## License

MIT License - See LICENSE file for details.
