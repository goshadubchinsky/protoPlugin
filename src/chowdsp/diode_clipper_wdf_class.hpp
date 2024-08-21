#ifndef DIODECLIPPER_H_INCLUDED
#define DIODECLIPPER_H_INCLUDED

#include "cmath"
//#include <xsimd/xsimd.hpp>
#include "chowdsp_wdf.h"

using namespace chowdsp::wdft;

class DiodeClipper
{
public:

    DiodeClipper() = default;

    void setSampleRate(float sampleRate, int UPSAMPLE)
    {
        this->UPSAMPLE = UPSAMPLE;
        this->sampleRate = sampleRate * this->UPSAMPLE;
        C1.prepare(this->sampleRate);
    }

    void reset()
    {
        C1.reset();
    }

    void setCircuitParams(float input_gain, float offset, float cutoff)  //input_gain 0, 30, 1
    {
        dp.setDiodeParameters(2.52e-9f, 0.02585f, 16);
        constexpr auto Cap = 47.0e-9f;
        
        //const auto Res = 1.f / (2.f * 3.141592653589793238L * (this->sampleRate * 0.5f) * Cap);
        const auto Res = 1.f / (2.f * 3.141592653589793238L * cutoff * Cap);
        C1.setCapacitanceValue(Cap);
        R1.setResistanceValue(Res);
        this->input_gain = input_gain;
        this->offset = offset;
    }

    inline float processSample(float x)
    {
        x = applyInputGain(x) + this->offset;

        Vs.setVoltage(x);
        dp.incident(P1.reflected());
        auto y = voltage<float>(C1);
        P1.incident(dp.reflected());

        return y;
    }

private:

    float applyInputGain(float input)
    {
        if      (this->UPSAMPLE == 8) {input *= this->input_gain * 1.5f * 0.668f;}
        else if (this->UPSAMPLE == 4) {input *= this->input_gain * 1.5f * 0.733f;}
        else if (this->UPSAMPLE == 2) {input *= this->input_gain * 1.5f * 0.820f;}
        else if (this->UPSAMPLE == 1) {input *= this->input_gain * 1.5f * 1.104f;}
        else                          {input = 0.f;}
        return input;
    }

    int UPSAMPLE{1};
    float sampleRate{44100.f};
    float input_gain{1.f};
    float offset{0.f};
    
    ResistorT<float> R1 { 4700.f };
    ResistiveVoltageSourceT<float> Vs;
    WDFSeriesT<float, decltype (Vs), decltype (R1)> S1 { Vs, R1 };
    CapacitorT<float> C1 { 47.0e-9f };
    WDFParallelT<float, decltype (S1), decltype (C1)> P1 { S1, C1 };
    DiodePairT<float, decltype (P1)> dp { P1, 2.52e-9f };   // GZ34 diode pair
};

#endif // DIODECLIPPER_H_INCLUDED