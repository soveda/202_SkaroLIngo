# SkaroLIngo

Card 202 for the Music Thing Modular Workshop Computer.

SkaroLIngo is an MF-102-inspired voice ring modulator: patch speech or another
programme signal into Audio In 1, tune the internal carrier until the sidebands
start talking back, then use drive, mix, LFO and character controls to move
between a sine-like and square-like internal carrier waveform.

The name nods to two famous metal-voice traditions from Doctor Who. The firmware
is original and intentionally generic: a playable Workshop Computer ring
modulator rather than an attempt to reproduce any protected recording or circuit
exactly.

## Build

```sh
cmake -S . -B build
cmake --build build
```

If your shell has an old SDK path cached, point CMake at the local SDK:

```sh
PICO_SDK_PATH=/Users/adrianvos/coding/GitHub/pico-sdk cmake -S . -B build
cmake --build build
```

The build should produce `build/skarolingo.uf2`. A built alpha binary is also
included at `UF2/SkaroLIngo.uf2`.

## Patch

| Jack | Function |
| --- | --- |
| Audio In 1 | Programme input: voice, drum, synth, radio |
| Audio In 2 | Optional external carrier; replaces the internal carrier when patched |
| CV In 1 | Carrier pitch CV |
| CV In 2 | Dry/ring mix CV |
| Pulse In 1 | LFO reset |
| Pulse In 2 | Tremolo gate over the wet mix |
| Audio Out 1 | Main dry/wet output |
| Audio Out 2 | Ring-only output |
| CV Out 1 | Internal LFO |
| CV Out 2 | Half-level carrier monitor |
| Pulse Out 1 | LFO square |
| Pulse Out 2 | Input presence gate |

## Controls

### Switch Middle: Performance

| Control | Function |
| --- | --- |
| Main | Carrier frequency; soft pickup |
| X | Dry/ring mix; soft pickup |
| Y | Drive; soft pickup |

### Switch Up: Modulation / Character

Switch Up temporarily applies its LFO and character settings. Returning to
middle restores the stable performance sound.

| Control | Function |
| --- | --- |
| Main | LFO rate, 0.1-25 Hz; soft pickup |
| X | LFO depth into carrier frequency; soft pickup, fully CCW is off |
| Y | Internal carrier waveform, sine-like to square-like; soft pickup |

### Switch Down: Voice Character

Tap the spring-loaded switch down to cycle three characters:

| Character | Sound |
| --- | --- |
| Skaro | Sine-like carrier |
| Mondas | Square-like carrier |
| Hybrid | Halfway carrier waveform |

The internal carrier follows the MF-102 range: `0.6-80 Hz` over the lower half
of Main and `80 Hz-4 kHz` over its upper half.

LED 5 flashes dim, medium, or bright after a tap to show the selected character.

## Notes

- `ComputerCard.h` was copied from the newest visible release copy in
  `Workshop_Computer/releases` on 2026-09-06.
- Code attribution is tracked in [ATTRIBUTION.md](ATTRIBUTION.md). In short:
  ComputerCard is by Chris Johnson; `pico_sdk_import.cmake` is the Raspberry Pi
  Pico SDK helper; and the ring-mod DSP approach adapts ideas from the MIT
  licensed Alloy card in `Workshop_Computer/releases/97_alloy`.
- Normalisation probing is enabled so that an unpatched Audio In 2 reliably
  selects the internal carrier, while a patched Audio In 2 replaces it.
- The code runs from RAM and uses `PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64`, per
  the Workshop Computer AI directive.
- This is alpha firmware undergoing hardware tests.
