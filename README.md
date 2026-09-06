# SkaroLIngo

Card 202 for the Music Thing Modular Workshop Computer.

SkaroLIngo is an MF-102-inspired voice ring modulator: patch speech or another
programme signal into Audio In 1, tune the internal carrier until the sidebands
start talking back, then use drive, mix, LFO and character controls to move
between rounded diode-ring clang and harder digital multiplication.

The name nods to two famous metal-voice traditions from Doctor Who. The firmware
is original and intentionally generic: a playable Workshop Computer ring
modulator rather than an attempt to reproduce any protected recording or circuit
exactly.

## Build

```sh
cmake -S . -B build
cmake --build build
```

The build should produce `build/skarolingo.uf2`.

## Patch

| Jack | Function |
| --- | --- |
| Audio In 1 | Programme input: voice, drum, synth, radio |
| Audio In 2 | Optional external carrier, summed with the internal carrier |
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
| Main | Carrier frequency |
| X | Dry/ring mix |
| Y | Drive |

### Switch Up: Modulation / Character

| Control | Function |
| --- | --- |
| Main | LFO rate |
| X | LFO depth into carrier frequency |
| Y | Carrier shape and analogue-to-digital ring character |

### Switch Down: Voice Character

Tap the spring-loaded switch down to cycle three characters:

| Character | Sound |
| --- | --- |
| Skaro | Lower range, rounder carrier, analogue diode-ring emphasis |
| Mondas | Mid range, squarer carrier, digital multiply emphasis |
| Hybrid | Wide range, full character sweep |

LED 5 flashes dim, medium, or bright after a tap to show the selected character.

## Notes

- `ComputerCard.h` was copied from the newest visible release copy in
  `Workshop_Computer/releases` on 2026-09-06.
- Normalisation probing is not enabled in this first pass. Unpatched inputs are
  handled with DC blocking and a small dead zone, avoiding probe noise in the
  audio path.
- The code runs from RAM and uses `PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64`, per
  the Workshop Computer AI directive.
- This is alpha firmware and has not yet been tested on hardware.
