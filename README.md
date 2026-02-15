# FreqmodGrid - FM Synthesizer

A work-in-progress FM synthesizer plugin built with iPlug2.

## Current Status

**WORK IN PROGRESS** - Currently implementing:
- Basic 2-operator FM synthesis (for rapid prototyping)
- Full 6-operator FM engine ready to integrate

## Features (Planned)

- 6 FM operators with configurable ratio, level, and feedback
- 8 algorithm presets for different modulation routings
- Multi-mode filter (LP/HP)
- ADSR envelope
- 2 LFOs with multiple waveforms
- Built-in chorus and delay effects
- MIDI note input
- Polyphonic (up to 16 voices)
- Visual grid UI for modulation routing

## Building

### Prerequisites

- macOS: Xcode 15+, CMake 3.20+
- VST3 SDK: Included as submodule
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

### Output

- VST3: `build/FreqmodGrid.vst3/Contents/MacOS/FreqmodGrid`
- AU: `build/FreqmodGrid.component/Contents/MacOS/FreqmodGrid`

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
├── iPlug/           # iPlug2 plugin wrapper
│   ├── FreqmodGrid.h/cpp
│   └── FreqmodGrid_DSP.h
├── DSP/             # FM synthesis engine
│   ├── FMEngine.h   # 6-op FM core
│   ├── Operator.h
│   ├── Envelope.h
│   ├── Filter.h
│   ├── LFO.h
│   └── Effects.h
└── plugin/          # VST3/AU wrappers (legacy)
```

## Development

The project uses iPlug2 for cross-platform plugin support:
- VST3, AU, AUv3 formats supported
- iGraphics for UI (NanoVG/Metal on macOS)
- MidiSynth for voice management

## License

MIT License - See LICENSE file for details.

## Credits

- FM Engine: Custom DSP implementation
- Plugin Framework: iPlug2
- VST3 SDK: Steinberg Media Technologies
