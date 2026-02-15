# FreqmodGrid

A free, open-source 6-operator FM synthesizer plugin. Built with [iPlug2](https://github.com/iPlug2/iPlug2) and C++20.

FreqmodGrid aims to make FM synthesis accessible. Instead of requiring deep theory knowledge to patch operator routings, it provides 8 ready-made algorithms with a visual grid interface (planned), letting you focus on sound design rather than signal flow.

## Status

**Functional.** The DSP engine is complete and builds successfully. No GUI yet - parameters are automatable but not visible.

What works today:
- Full 6-operator FM synthesis engine
- 8 algorithm routings
- 12dB/oct biquad filter with resonance
- ADSR envelope, 2 LFOs, chorus, delay
- 16-voice polyphony
- 35 automatable parameters
- 10 factory presets
- VST3 and AU output via iPlug2

What's not done:
- GUI (parameters work but no visual interface)
- Preset browser in GUI
- MPE support
- Stereo effects chain
- Oversampling for offline render

## Building

### Build & Install

```bash
# Clone and setup
git clone https://github.com/benjaminrhodes/freqmodgrid.git
cd freqmodgrid
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target FreqmodGrid-vst3

# Install (macOS)
cp -r out/FreqmodGrid.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

### Usage in DAW

1. Rescan plugins in your DAW
2. Add FreqmodGrid as an instrument
3. Send MIDI notes to play
4. Automate parameters via MIDI CC or host automation

### Requirements

| Platform | Compiler | Build Tool |
|----------|----------|------------|
| macOS 12+ | Xcode 15+ | CMake 3.20+ |
| Windows 10/11 | Visual Studio 2022 | CMake 3.20+ |

### Steps

```bash
git clone https://github.com/benjaminrhodes/freqmodgrid.git
cd freqmodgrid
git submodule update --init --recursive

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

There is also a convenience script:

```bash
./scripts/build.sh
```

### Build Output

| Format | Path |
|--------|------|
| VST3 | `build/FreqmodGrid.vst3` |
| AU | `build/FreqmodGrid.component` (macOS only) |

## Installation

### macOS

```bash
cp -r build/FreqmodGrid.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r build/FreqmodGrid.component ~/Library/Audio/Plug-Ins/Components/
```

### Windows

Copy `FreqmodGrid.vst3` to:
```
C:\Program Files\Common Files\VST3\
```

Then rescan plugins in your DAW.

## How It Works

FreqmodGrid is a phase modulation (FM) synthesizer. Six sine-wave **operators** can modulate each other's phase to produce complex timbres from simple building blocks.

Each operator has three controls:
- **Ratio** (0.25x - 32x) -- frequency multiplier relative to the played note
- **Level** (0 - 100%) -- output amplitude
- **Feedback** (0 - 100%) -- self-modulation amount

The **algorithm** determines how operators connect. In each algorithm, some operators act as **modulators** (shaping the timbre) and others as **carriers** (producing the audible output).

### Algorithms

```
1: Serial           1 -> 2 -> 3 -> 4 -> 5 -> [6]
2: (1+2) > Chain    1 -\
                    2 -/-> 3 -> 4 -> 5 -> [6]
3: 1 > All          1 -> [2]
                    1 -> [3]
                    1 -> [4]
                    1 -> [5]
                    1 -> [6]
4: Pairs > Chain    1 -\
                    2 -|
                    3 -|-> 5 -> [6]
                    4 -/
5: 3 Pairs          1 -> [2]    3 -> [4]    5 -> [6]
6: Groups           1 -\    /-> [4]
                    2 -+--> +-> [5]
                    3 -/    \-> [6]
7: 2 Chains         1 -> 2 -> [3]    4 -> 5 -> [6]
8: All Parallel     [1]  [2]  [3]  [4]  [5]  [6]
```

Operators in `[brackets]` are carriers (heard in the output). All others are modulators only.

### Signal Flow

```
MIDI Note -> Operators (per algorithm) -> Filter (LP/HP) -> Envelope -> Voice Mix
                  ^                            ^
                  |                            |
                LFO 1 (pitch)            LFO 2 (cutoff)

Voice Mix -> Chorus -> Delay -> Stereo Out
```

## Parameters

### Operators (x6)

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Ratio | 0.25 - 32.0 | 1.0 (Op1) | multiplier |
| Level | 0 - 100 | 50 (Ops 1-3), 0 (Ops 4-6) | % |
| Feedback | 0 - 100 | 0 | % |

### Algorithm

| Parameter | Range | Default |
|-----------|-------|---------|
| Algorithm | 1 - 8 | 1 (Serial) |

### Filter

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Type | Low Pass, High Pass | Low Pass | - |
| Cutoff | 20 - 20,000 | 12,000 | Hz |
| Resonance | 0 - 100 | 0 | % |

### Envelope (ADSR)

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Attack | 1 - 5,000 | 10 | ms |
| Decay | 1 - 5,000 | 100 | ms |
| Sustain | 0 - 100 | 70 | % |
| Release | 10 - 10,000 | 300 | ms |

### LFOs

| Parameter | Range | Default | Unit | Target |
|-----------|-------|---------|------|--------|
| LFO1 Rate | 0.01 - 20 | 1.0 | Hz | Pitch (vibrato) |
| LFO1 Depth | 0 - 100 | 0 | % | |
| LFO2 Rate | 0.01 - 20 | 2.0 | Hz | Filter cutoff |
| LFO2 Depth | 0 - 100 | 0 | % | |

### Effects

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Chorus Rate | 0.1 - 10 | 1.0 | Hz |
| Chorus Depth | 0 - 100 | 30 | % |
| Delay Time | 1 - 2,000 | 250 | ms |
| Delay Feedback | 0 - 90 | 30 | % |

### Master

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Master Volume | 0 - 100 | 70 | % |

## Factory Presets

| Name | Category | Algorithm | Character |
|------|----------|-----------|-----------|
| Init | Init | 1 (Serial) | Clean starting point |
| Simple Lead | Lead | 2 | Bright, cutting lead |
| Warm Pad | Pad | 3 | Evolving, wide pad |
| Bell Tone | Bell | 1 | Classic FM bell |
| Pluck | Keys | 2 | Short, percussive |
| Fat Bass | Bass | 5 (3 Pairs) | Thick sub bass |
| Sci-Fi FX | FX | 4 | Metallic, evolving |
| Glassy Keys | Keys | 3 | Bright electric piano |
| Dark Drone | Pad | 6 (Groups) | Low, atmospheric |
| Brass Hit | Lead | 2 | Punchy brass stab |

Presets are stored in `resources/presets/factory_presets.json`.

## Project Structure

```
freqmodgrid/
├── src/
│   ├── DSP/                  # Synthesis engine (framework-independent)
│   │   ├── FMEngine.h        # Voice management, algorithm routing, mixing
│   │   ├── FMEngine.cpp
│   │   ├── Operator.h        # Single FM operator (Bhaskara sine approx)
│   │   ├── Envelope.h        # ADSR with exponential decay
│   │   ├── Filter.h          # Biquad LP/HP (12dB/oct, Direct Form II)
│   │   ├── LFO.h             # Sine/saw/square/triangle LFO
│   │   └── Effects.h         # Chorus + delay (heap-allocated buffers)
│   └── iPlug/                # iPlug2 plugin wrapper
│       ├── FreqmodGrid.h     # Plugin class declaration
│       ├── FreqmodGrid.cpp   # Parameter registration
│       └── FreqmodGrid_DSP.h # MIDI -> FMEngine bridge, param scaling
├── resources/
│   ├── config.h              # iPlug2 plugin config
│   ├── resource.h
│   └── presets/
│       └── factory_presets.json
├── scripts/
│   └── build.sh
├── ci/
│   └── build.yml             # GitHub Actions (Windows + macOS)
├── CMakeLists.txt
├── VST3-Plugin-Research-Build.md  # Design research document
└── LICENSE                        # MIT
```

The DSP engine (`src/DSP/`) has no framework dependencies and could be reused with JUCE, the raw VST3 SDK, or any other plugin framework.

## Technical Notes

- **Sine approximation**: Operators use the Bhaskara I formula instead of `std::sin()` for performance. Max error is ~0.15% across the full cycle.
- **Filter**: Standard biquad (Robert Bristow-Johnson cookbook formulas). Resonance maps Q from 0.707 (Butterworth) to 12.
- **Envelope**: Linear attack ramp, exponential decay/release (~60dB over the specified time).
- **Voice stealing**: Oldest-note-first, tracked by a monotonic age counter.
- **Buffer sizes**: Chorus uses 8,192 samples (safe to 96kHz+). Delay uses 192,000 samples (2s at 96kHz). Both heap-allocated.
- **C++ standard**: C++20. No external dependencies beyond iPlug2.

## Contributing

Contributions are welcome. Areas that need work:

1. **GUI** -- The visual grid interface described in `VST3-Plugin-Research-Build.md`
2. **Stereo effects** -- Chorus and delay currently output mono
3. **More presets** -- Target is 50+ factory presets
4. **Testing** -- No unit tests exist yet
5. **MPE support** -- Per-note expression (pitch bend, pressure, slide)

## License

MIT License. See [LICENSE](LICENSE) for details.
