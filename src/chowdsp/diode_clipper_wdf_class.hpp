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

    void setSampleRate(float sampleRate)
    {
        this->sampleRate = sampleRate;
        C1.prepare(this->sampleRate);
    }

    void reset()
    {
        C1.reset();
    }

    void setCircuitParams(float input_gain, float offset, float cutoff)  //input_gain 0, 30, 1
    {
        //constexpr auto Cap = 47.0e-9f; // Basic
        //constexpr auto Cap = 47.0e-6f; // Electrolytic Capacitor - 47 µF
        constexpr auto Cap = 100.0e-9f; // Ceramic Capacitor - 100 nF
        //constexpr auto Cap = 10.0e-6f; // Film Capacitor - 10 µF
        //constexpr auto Cap = { 22.0e-6f; // Tantalum Capacitor - 22 µF
        
        
        //const auto Res = 1.f / (2.f * 3.141592653589793238L * (this->sampleRate * 0.5f) * Cap);
        const auto Res = 1.f / (2.f * 3.141592653589793238L * cutoff * Cap);
        C1.setCapacitanceValue(Cap);
        R1.setResistanceValue(Res);
        //this->input_gain = input_gain / 10.f;
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
        return  y;

    }

private:

    float applyInputGain(float input)
    {
        //input *= this->input_gain * 1.5f * 1.104f;
        input *= this->input_gain;
        return input;
    }

    float sampleRate{44100.f};
    float input_gain{1.f};
    float offset{0.f};
    
    ResistorT<float> R1 { 4700.f };
    ResistiveVoltageSourceT<float> Vs;
    WDFSeriesT<float, decltype (Vs), decltype (R1)> S1 { Vs, R1 };
    //CapacitorT<float> C1 { 47.0e-9f }; // Basic
    //CapacitorT<float> C1 { 47.0e-6f }; // Electrolytic Capacitor - 47 µF
    CapacitorT<float> C1 { 100.0e-9f }; // Ceramic Capacitor - 100 nF
    //CapacitorT<float> C1 { 10.0e-6f }; // Film Capacitor - 10 µF
    //CapacitorT<float> C1 { 22.0e-6f }; // Tantalum Capacitor - 22 µF




    WDFParallelT<float, decltype (S1), decltype (C1)> P1 { S1, C1 };
    //DiodePairT<float, decltype (P1)> dp { P1, 2.52e-9f };           // GZ34         diode pair
    DiodePairT<float, decltype (P1)> dp { P1, 4.0e-12f };           // 1N4148       diode pair
    //DiodePairT<float, decltype (P1)> dp { P1, 15.0e-12f };          // 1N4007       diode pair
    //DiodePairT<float, decltype (P1)> dp { P1, 2.0e-12f };           // BAT46        diode pair
    //DiodePairT<float, decltype (P1)> dp { P1, 50.0e-12f };          // Zener        diode pair

};

#endif // DIODECLIPPER_H_INCLUDED