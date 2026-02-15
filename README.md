# FreqmodGrid

A free, open-source 6-operator FM synthesizer plugin. Built with [iPlug2](https://github.com/iPlug2/iPlug2) and C++20.

FreqmodGrid makes FM synthesis accessible with 8 ready-made algorithms and a clean visual interface, letting you focus on sound design rather than signal flow.

## Status

**Functional.** The DSP engine and GUI are complete and build successfully.

What works today:
- Full 6-operator FM synthesis engine
- 8 algorithm routings
- GUI with knobs and buttons for all parameters
- 12dB/oct biquad filter with resonance
- ADSR envelope, 2 LFOs, chorus, delay
- 16-voice polyphony
- 35 automatable parameters
- 10 factory presets
- VST3 and AU output via iPlug2

What's not done:
- Preset browser in GUI
- MPE support
- Stereo effects chain
- Oversampling for offline render

## Quick Start

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
```

The plugin will be installed automatically to `~/Library/Audio/Plug-Ins/VST3/`

### Usage in DAW

1. Restart your DAW to detect the new plugin
2. Add FreqmodGrid as an instrument
3. Send MIDI notes to play
4. Use the GUI to adjust parameters:
   - **Algorithm buttons (1-8)** - Select FM routing
   - **Operator knobs** - Adjust ratio and level for each operator
   - **Filter** - Set cutoff frequency and resonance
   - **ADSR** - Shape the amplitude envelope
   - **LFOs** - Add pitch and filter modulation
   - **Effects** - Add chorus and delay
   - **RND** - Randomize all parameters

## Building

### Requirements

| Platform | Compiler | Build Tool |
|----------|----------|------------|
| macOS 12+ | Xcode 15+ | CMake 3.20+ |
| Windows 10/11 | Visual Studio 2022 | CMake 3.20+ |

### Build Commands

```bash
# Clone
git clone https://github.com/benjaminrhodes/freqmodgrid.git
cd freqmodgrid
git submodule update --init --recursive

# Build VST3 only
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target FreqmodGrid-vst3

# Or build all formats (VST3, AU)
cmake --build . --config Release
```

### Build Output

| Format | Path |
|--------|------|
| VST3 | `build/out/FreqmodGrid.vst3` |
| AU | `build/out/FreqmodGrid.component` (macOS only) |

### Manual Installation

**macOS:**
```bash
cp -r build/out/FreqmodGrid.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

**Windows:**
Copy `FreqmodGrid.vst3` to `C:\Program Files\Common Files\VST3\`

## How It Works

FreqmodGrid is a phase modulation (FM) synthesizer. Six sine-wave **operators** can modulate each other's phase to produce complex timbres from simple building blocks.

Each operator has two controls:
- **Ratio** (0.25x - 32x) -- frequency multiplier relative to the played note
- **Level** (0 - 100%) -- output amplitude

The **algorithm** determines how operators connect. In each algorithm, some operators act as **modulators** (shaping the timbre) and others as **carriers** (producing the audible output).

### Algorithms

```
1: Serial           1 -> 2 -> 3 -> 4 -> 5 -> [6]
2: (1+2) > Chain   1 -\ 
                    2 -/-> 3 -> 4 -> 5 -> [6]
3: 1 > All         1 -> [2]
                    1 -> [3]
                    1 -> [4]
                    1 -> [5]
                    1 -> [6]
4: Pairs > Chain   1 -\ 
                    2 -|
                    3 -|-> 5 -> [6]
                    4 -/
5: 3 Pairs         1 -> [2]    3 -> [4]    5 -> [6]
6: Groups          1 -\    /-> [4]
                    2 -+--> +-> [5]
                    3 -/    \-> [6]
7: 2 Chains        1 -> 2 -> [3]    4 -> 5 -> [6]
8: All Parallel    [1]  [2]  [3]  [4]  [5]  [6]
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

## GUI Controls

The GUI is organized into sections:

### Algorithm Section (top row)
- 8 buttons to select the FM routing algorithm

### Operators Section
- 6 operator strips, each with:
  - **Ratio** knob - Frequency multiplier (0.25x to 32x)
  - **Lev** knob - Output level (0-100%)

### Filter Section
- **LP/HP** switch - Low-pass or High-pass filter
- **Cutoff** knob - Filter frequency (20Hz - 20kHz)
- **Res** knob - Filter resonance (0-100%)

### Envelope Section (ADSR)
- **A** - Attack time (1-5000ms)
- **D** - Decay time (1-5000ms)  
- **S** - Sustain level (0-100%)
- **R** - Release time (10-10000ms)

### LFO Sections
- **LFO 1** - Pitch modulation (vibrato)
  - **Rate** - LFO speed (0.01-20Hz)
  - **Depth** - Modulation amount (0-100%)
- **LFO 2** - Filter modulation
  - **Rate** - LFO speed (0.01-20Hz)
  - **Depth** - Modulation amount (0-100%)

### Effects Section
- **Ch Rt** - Chorus rate (0.1-10Hz)
- **Ch Dp** - Chorus depth (0-100%)
- **Del T** - Delay time (1-2000ms)
- **Del F** - Delay feedback (0-90%)

### Master Section
- **Vol** - Master volume (0-100%)
- **RND** - Randomize all parameters

## Parameters

### Operators (x6)

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Ratio | 0.25 - 32.0 | varies | multiplier |
| Level | 0 - 100 | 50/0 | % |

### Filter

| Parameter | Range | Default | Unit |
|-----------|-------|---------|------|
| Type | LP, HP | LP | - |
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
| LFO1 Rate | 0.01 - 20 | 1.0 | Hz | Pitch |
| LFO1 Depth | 0 - 100 | 0 | % | |
| LFO2 Rate | 0.01 - 20 | 2.0 | Hz | Filter |
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
│   │   └── Effects.h          # Chorus + delay (heap-allocated buffers)
│   └── iPlug/                # iPlug2 plugin wrapper
│       ├── FreqmodGrid.h     # Plugin class declaration
│       ├── FreqmodGrid.cpp   # Parameter registration & GUI
│       └── FreqmodGrid_DSP.h # MIDI -> FMEngine bridge, param scaling
├── resources/
│   ├── config.h              # iPlug2 plugin config
│   └── presets/
│       └── factory_presets.json
├── scripts/
│   └── build.sh
├── ci/
│   └── build.yml             # GitHub Actions (Windows + macOS)
├── CMakeLists.txt
├── VST3-Plugin-Research-Build.md
└── LICENSE
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

1. **Preset browser** -- GUI-based preset selection
2. **Stereo effects** -- Chorus and delay currently output mono
3. **More presets** -- Target is 50+ factory presets
4. **Testing** -- No unit tests exist yet
5. **MPE support** -- Per-note expression (pitch bend, pressure, slide)

## License

MIT License. See [LICENSE](LICENSE) for details.
