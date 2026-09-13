# Attribution

SkaroLIngo is original card firmware by Adrian Vos, generated with AI assistance
in Codex on 2026-09-06.

## Included Code

- `ComputerCard.h`: copied from
  `/Users/adrianvos/coding/GitHub/Workshop_Computer/releases/107_scintillator/ComputerCard.h`.
  The header identifies itself as **ComputerCard by Chris Johnson**, version
  0.3.0, 12 May 2026. It is the hardware framework for the Music Thing Modular
  Workshop Computer.

- `pico_sdk_import.cmake`: copied from the Raspberry Pi Pico SDK import helper.
  It carries the original Raspberry Pi (Trading) Ltd. BSD-3-Clause copyright and
  license notice in the file.

## Adapted Patterns

- Ring-modulation DSP: `main.cpp` follows Alloy's Q12 sample conventions and
  fixed-point cross-modulation approach from
  `/Users/adrianvos/coding/GitHub/Workshop_Computer/releases/97_alloy/dsp/xmod_algorithms.h`.
  Alloy is MIT licensed in its `info.yaml`. Its comments state that the
  algorithms are modelled on Mutable Instruments Warps/Parasites behavior and
  rewritten in integer arithmetic for the RP2040. SkaroLIngo's saturated ring
  stage and sine-to-square carrier morph are original implementations.

- Build/release structure: `CMakeLists.txt`, `README.md`, and `info.yaml` follow
  the style of Adrian Vos Workshop Computer releases, especially
  `999_Dub_Warning` and `107_scintillator`, and the Workshop Computer AI
  directive at
  `/Users/adrianvos/coding/GitHub/Workshop_Computer/Demonstrations+HelloWorlds/AI/WORKSHOP_COMPUTER_AI_DIRECTIVE.md`.

## Inspiration

SkaroLIngo is inspired by the Moog Moogerfooger MF-102 ring modulator and by the
historic use of ring modulation for science-fiction voice treatments. It is not
an official Moog, BBC, Doctor Who, Dalek, or Cyberman product, and it does not
include any code, samples, recordings, logos, or artwork from those sources.
