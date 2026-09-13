// SkaroLIngo - MF-102-inspired voice ring modulator for the Music Thing
// Modular Workshop Computer.
//
// Copyright 2026 Adrian Vos. MIT licensed.
//
// Attribution:
// - Built on ComputerCard by Chris Johnson, copied here as ComputerCard.h.
// - The fixed-point analogue/digital ring-modulation approach is adapted from
//   Alloy (`Workshop_Computer/releases/97_alloy/dsp/xmod_algorithms.h`), which
//   itself documents its Mutable Instruments Warps/Parasites DSP lineage.
// - The Pico SDK import helper is Raspberry Pi (Trading) Ltd. BSD-3-Clause
//   code, included unchanged as pico_sdk_import.cmake.
// See ATTRIBUTION.md for the fuller repository attribution notes.
//
// Audio In 1 is the programme input: voice, drum machine, radio, oscillator,
// whatever you want to send into the modulator. Audio In 2 is an optional
// external carrier which replaces the internal carrier when patched.
//
// Switch middle is the performance page:
//   MAIN: carrier frequency
//   X: dry/ring mix
//   Y: input/ring drive
//
// Switch up is the modulation/character page:
//   MAIN: LFO rate
//   X: LFO depth into carrier frequency
//   Y: character, from round analogue diode ring to harder digital multiply
//
// Tap switch down to cycle three voice characters:
//   0 Skaro: lower range, rounder carrier, analogue ring
//   1 Mondas: mid range, squarer carrier, digital ring
//   2 Hybrid: wide range, character knob scans the whole machine

#include <cstdint>

#include "ComputerCard.h"
#include "hardware/clocks.h"

namespace skarolingo
{
constexpr int32_t kSampleMax = 2047;
constexpr int32_t kSampleMin = -2048;
constexpr int32_t kParamMax = 4095;

inline int32_t Clip(int32_t x)
{
    if (x > kSampleMax) return kSampleMax;
    if (x < kSampleMin) return kSampleMin;
    return x;
}

inline int32_t Abs(int32_t x)
{
    return x < 0 ? -x : x;
}

inline int32_t Clamp(int32_t x, int32_t lo, int32_t hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

inline int32_t Triangle(uint32_t phase)
{
    uint32_t x = phase >> 20; // 0..4095
    if (x >= 2048) x = 4095 - x;
    return static_cast<int32_t>(x) * 2 - 2048;
}

inline int32_t Sineish(uint32_t phase)
{
    int32_t tri = Triangle(phase);
    int32_t bend = (tri * Abs(tri)) >> 11;
    return Clip((tri * 3 - bend) >> 1);
}

inline int32_t Square(uint32_t phase)
{
    return (phase & 0x80000000u) ? 2047 : -2048;
}

inline int32_t Crossfade(int32_t a, int32_t b, int32_t amount)
{
    return Clip((a * (kParamMax - amount) + b * amount) >> 12);
}

inline int32_t SoftLimit(int32_t x)
{
    if (x > 4095) x = 4095;
    if (x < -4096) x = -4096;

    int32_t x2 = (x * x) >> 12;
    int32_t x3 = (x2 * x) >> 12;
    return Clip((x - x3 / 3) >> 1);
}

inline int32_t Diode(int32_t x)
{
    int32_t sign = x < 0 ? -1 : 1;
    int32_t d = Abs(x) - 205;
    if (d < 0) return 0;
    return sign * ((d * d) >> 13);
}

inline int32_t AnalogRing(int32_t input, int32_t carrier, int32_t gain)
{
    // A clean four-quadrant multiply is the stable core of the effect. At
    // sub-audio carrier rates it becomes the expected tremolo; higher rates
    // create conventional ring modulation. Drive then introduces the diode
    // character used by Alloy's attributed ring-mod implementation.
    int32_t clean = (input * carrier) >> 11;
    int32_t c2 = carrier << 1;
    int32_t ring = Diode(input + c2) + Diode(input - c2);
    int32_t stageGain = 4096 + gain * 5;
    int32_t scaled = static_cast<int32_t>((static_cast<int64_t>(ring) * stageGain) >> 12);
    int32_t diode = SoftLimit(scaled << 1);
    return Crossfade(clean, diode, Clamp(gain, 0, kParamMax));
}

inline int32_t DigitalRing(int32_t input, int32_t carrier, int32_t gain)
{
    int32_t ring = (input * carrier) >> 9;
    int64_t g = (static_cast<int64_t>(ring) * (4096 + gain * 8)) >> 12;
    int32_t r = Clamp(static_cast<int32_t>(g), -(1 << 24), (1 << 24));
    int32_t mag = Abs(r);
    return Clip((r * 2048) / (2048 + mag));
}

class DcBlock
{
public:
    int32_t Process(int32_t x)
    {
        int32_t y = x - lastIn_ + ((lastOut_ * 4088) >> 12);
        lastIn_ = x;
        lastOut_ = Clip(y);

        if (Abs(lastOut_) < 10) return 0;
        return lastOut_;
    }

private:
    int32_t lastIn_ = 0;
    int32_t lastOut_ = 0;
};

inline int32_t LevelToLed(int32_t x)
{
    int32_t b = Abs(x) << 1;
    return b > 4095 ? 4095 : b;
}

} // namespace skarolingo

class SkaroLIngo : public ComputerCard
{
public:
    SkaroLIngo()
    {
        EnableNormalisationProbe();
    }

    virtual void ProcessSample() override
    {
        if (startupSamples_ > 0)
        {
            ShowStartup();
            return;
        }

        if (SwitchVal() == Switch::Down && SwitchChanged())
        {
            voice_ = (voice_ + 1) % 3;
            voiceFlash_ = 24000;
        }

        if (PulseIn1RisingEdge())
        {
            lfoPhase_ = 0;
            lfoResetFlash_ = 12000;
        }

        const bool characterPage = SwitchVal() == Switch::Up;
        const int32_t main = KnobVal(Knob::Main);
        const int32_t x = KnobVal(Knob::X);
        const int32_t y = KnobVal(Knob::Y);

        if (characterPage)
        {
            lfoRate_ += (main - lfoRate_) >> 8;
            lfoDepth_ += (x - lfoDepth_) >> 8;
            character_ += (y - character_) >> 8;
        }
        else
        {
            freq_ += (main - freq_) >> 8;
            mix_ += (x - mix_) >> 8;
            drive_ += (y - drive_) >> 8;
        }

        int32_t input = inputBlock_.Process(AudioIn1());
        int32_t externalCarrier = carrierBlock_.Process(AudioIn2());

        int32_t driveGain = 4096 + ((drive_ * 7) >> 2);
        input = skarolingo::SoftLimit((input * driveGain) >> 12);

        // 0.05 Hz to about 20 Hz, with useful resolution at slow rates.
        lfoPhase_ += 4500u + static_cast<uint32_t>(
            (static_cast<int64_t>(lfoRate_) * lfoRate_ * 1800000) >> 24);
        int32_t lfo = skarolingo::Triangle(lfoPhase_);
        int32_t lfoBend = (lfo * lfoDepth_) >> 11;

        int32_t rangeShift = voice_ == 0 ? -1 : (voice_ == 1 ? 0 : 1);
        int32_t freqControl = skarolingo::Clamp(freq_ + (CVIn1() << 1) + lfoBend, 0, 4095);
        // The original alpha's floor was around 10 Hz in Skaro mode, which
        // made its lowest setting an audible whine. This maps the bottom of
        // the control to sub-audio rates, preserving the MF-102-like tremolo
        // range before it rises into metallic ring modulation.
        int64_t step = 22000 + ((static_cast<int64_t>(freqControl) * freqControl * 350000000) >> 24);
        if (rangeShift < 0) step >>= 1;
        if (rangeShift > 0) step <<= 1;
        carrierPhase_ += static_cast<uint32_t>(skarolingo::Clamp(static_cast<int32_t>(step), 400000, 900000000));

        int32_t sine = skarolingo::Sineish(carrierPhase_);
        int32_t square = skarolingo::Square(carrierPhase_);

        int32_t shape = character_;
        if (voice_ == 0) shape = shape >> 2;
        if (voice_ == 1) shape = 2600 + (shape >> 3);

        int32_t internalCarrier = skarolingo::Crossfade(sine, square, skarolingo::Clamp(shape, 0, 4095));

        // A patched Audio In 2 is the carrier, full stop. This avoids the
        // internal oscillator leaking through external-carrier patches.
        int32_t carrier = Connected(Input::Audio2) ? externalCarrier : internalCarrier;

        int32_t analog = skarolingo::AnalogRing(input, carrier, drive_);
        int32_t digital = skarolingo::DigitalRing(input, carrier, drive_);
        int32_t ringMorph = character_;
        if (voice_ == 0) ringMorph = ringMorph >> 2;
        if (voice_ == 1) ringMorph = 3200 + (ringMorph >> 3);
        int32_t ring = skarolingo::Crossfade(analog, digital, skarolingo::Clamp(ringMorph, 0, 4095));

        int32_t mixControl = skarolingo::Clamp(mix_ + CVIn2(), 0, 4095);
        if (PulseIn2())
        {
            int32_t trem = (lfo + 2048) >> 1;
            mixControl = (mixControl * trem) >> 11;
        }

        int32_t out = skarolingo::Crossfade(input, ring, mixControl);
        AudioOut1(out);
        AudioOut2(ring);

        CVOut1(lfo);
        CVOut2(carrier >> 1);
        PulseOut1(lfo > 0);
        PulseOut2(skarolingo::Abs(input) > 768);

        LedBrightness(0, skarolingo::LevelToLed(input));
        LedBrightness(1, skarolingo::LevelToLed(ring));
        LedBrightness(2, freq_);
        LedBrightness(3, mixControl);
        LedBrightness(4, skarolingo::Clamp(lfo + 2048, 0, 4095));

        if (voiceFlash_ > 0)
        {
            voiceFlash_--;
            const int32_t levels[3] = {1000, 2450, 4095};
            LedBrightness(5, levels[voice_]);
        }
        else if (lfoResetFlash_ > 0)
        {
            lfoResetFlash_--;
            LedBrightness(5, 4095);
        }
        else
        {
            LedBrightness(5, characterPage ? character_ : drive_);
        }
    }

private:
    void ShowStartup()
    {
        constexpr int32_t kSamplesPerLed = 4800;
        int32_t active = (28800 - startupSamples_) / kSamplesPerLed;
        for (int32_t i = 0; i < 6; i++)
        {
            LedOn(i, i == active);
        }
        startupSamples_--;
    }

    skarolingo::DcBlock inputBlock_;
    skarolingo::DcBlock carrierBlock_;

    uint32_t carrierPhase_ = 0;
    uint32_t lfoPhase_ = 0;

    int32_t startupSamples_ = 28800;
    int32_t voiceFlash_ = 0;
    int32_t lfoResetFlash_ = 0;
    int32_t voice_ = 0;

    int32_t freq_ = 1700;
    int32_t mix_ = 3000;
    int32_t drive_ = 1300;
    int32_t lfoRate_ = 900;
    int32_t lfoDepth_ = 0;
    int32_t character_ = 800;
};

int main()
{
    set_sys_clock_khz(144000, true);

    SkaroLIngo card;
    card.Run();
}
