# FreqmodGrid Feature Completion Design

**Date:** 2026-02-19

## Overview

Complete 4 remaining features for the FreqmodGrid FM synthesizer plugin:
1. Full preset manager with GUI
2. MPE (MIDI Polyphonic Expression) support
3. Stereo effects chain (chorus + delay)
4. Toggleable oversampling (2x/4x)

---

## 1. Full Preset Manager

### UI Components

- **Dropdown button** in top-right of GUI showing current preset name
- **Panel** (280px wide, ~300px tall) with:
  - Search input at top
  - Category tabs: All | Lead | Pad | Bass | Keys | FX | User
  - Preset list (scrollable, max ~200px)
  - Star icon for favorites
  - Save/Delete buttons for user presets

### Data Storage

| Type | Location | Format |
|------|----------|--------|
| Factory | `resources/presets/factory_presets.json` | Read-only JSON |
| User | `~/Library/Application Support/FreqmodGrid/user_presets.json` | JSON |
| Favorites | Stored in user presets file | Boolean flag |

### JSON Schema

```json
{
  "name": "Preset Name",
  "category": "Lead",
  "isFavorite": false,
  "isUserPreset": false,
  "parameters": {
    "algorithm": 1,
    "op1Ratio": 1.0,
    "op1Level": 50,
    ...
  }
}
```

### Categories

Enum: `Lead`, `Pad`, `Bass`, `Keys`, `FX`, `Init`

---

## 2. MPE Support

### MIDI Handling

- Detect MPE from incoming MIDI messages (MPE zone: channels 1-16)
- Per-note pitch bend: Store bend value per voice, apply to operator frequencies
- Per-note pressure (aftertouch): Store pressure per voice, apply as level modulator
- Per-note slide: Glide rate in semitones/second per voice

### Implementation

| Feature | MIDI Message | Implementation |
|---------|--------------|----------------|
| Pitch Bend | Pitch Bend (14-bit) | Store `bendCents` per voice |
| Pressure | Channel Pressure | Store `pressure` per voice, modulate level |
| Slide | MPE Slide (MIDI 1.0 CC 74) | Store `slideRate` per voice, glide frequency |

### Always enabled when DAW sends MPE data

No toggle - automatic detection from MIDI channel usage.

---

## 3. Stereo Effects Chain

### Chorus (Mid/Side Processing)

1. Input: Sum to mono
2. Mid/Side matrix conversion
3. Mid: Chorus with LFO (phase 0°)
4. Side: Chorus with LFO (phase 90°)
5. Output: Mid/Side → Stereo

### Delay (Mid/Side Processing)

1. Input: Sum to mono
2. Mid/Side matrix conversion
3. Mid: Delay line with feedback
4. Side: Separate delay line with feedback
5. Cross-feedback: Small mid↔side leakage
6. Output: Mid/Side → Stereo

### Buffer Requirements

| Effect | Samples (96kHz) |
|--------|-----------------|
| Chorus | 8,192 (existing) |
| Mid Delay | 192,000 (2s) |
| Side Delay | 192,000 (2s) |

Heap-allocated per effect instance.

---

## 4. Toggleable Oversampling

### UI

- Dropdown in master section: "OS" 
- Options: Off | 2x | 4x
- Default: Off

### Implementation

- Oversample FM engine + filter + effects together
- Use iPlug2's oversampling helper or 8x FIR polyphase
- Upsample → Process → Downsample

### Performance Impact

| Setting | CPU Increase |
|---------|--------------|
| Off | baseline |
| 2x | ~2x |
| 4x | ~4x |

---

## Parameters Modified

### New Parameters

| Parameter | Type | Range | Default |
|-----------|------|-------|---------|
| presetBrowser | - | - | - |
| osMode | Enum | Off/2x/4x | Off |
| mpeEnabled | Bool | - | Auto |

### Modified Effects

- Chorus: Stereo (Mid/Side)
- Delay: Stereo (Mid/Side with cross-feedback)

---

## Files Modified

### New Files

- `src/DSP/StereoChorus.h`
- `src/DSP/StereoDelay.h`
- `src/DSP/Oversampler.h`
- `src/iPlug/PresetManager.h`
- `src/iPlug/PresetManager.cpp`
- `resources/presets/factory_presets.json`

### Modified Files

- `src/DSP/FMEngine.h/cpp` - Add MPE, oversampling
- `src/DSP/Effects.h/cpp` - Remove, replace with stereo versions
- `src/iPlug/FreqmodGrid.h/cpp` - Add preset browser, oversampling UI
- `src/iPlug/FreqmodGrid_DSP.h` - Add MPE handling
