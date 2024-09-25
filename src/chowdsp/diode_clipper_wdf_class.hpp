#ifndef DIODECLIPPER_H_INCLUDED
#define DIODECLIPPER_H_INCLUDED

#include "cmath"
#include <xsimd/xsimd.hpp>
#include "chowdsp_wdf.h"

using namespace chowdsp::wdft;

template <typename T>
class DiodeClipper
{
public:

    DiodeClipper() = default;

    void setSampleRate(float sampleRate)
    {
        this->sampleRate = sampleRate;
        C1.prepare((T)this->sampleRate);
    }

    void reset()
    {
        C1.reset();
    }

    void setCircuitParams(T input_gain, float offset, T cutoff)  //input_gain 0, 30, 1
    {
        //constexpr auto Cap = 47.0e-9f; // 47 nF - Ceramic
        auto Cap = this->Cap; // 100 nF - Ceramic
        //constexpr auto Cap = 10.0e-6f; // 10 µF - Film
        //constexpr auto Cap = { 22.0e-6f; // 22 µF - Tantalum
        
        
        //const auto Res = 1.f / (2.f * 3.141592653589793238L * (this->sampleRate * 0.5f) * Cap);
        const auto Res = 1.f / (2.f * 3.141592653589793238L * cutoff * Cap);
        C1.setCapacitanceValue(Cap);
        R1.setResistanceValue(Res);
        //this->input_gain = input_gain / 10.f;
        this->input_gain = input_gain;
        this->offset = offset;
    }

    inline T processSample(T x)
    {
        x = applyInputGain(x+ this->offset);

        Vs.setVoltage(x);
        dp.incident(P1.reflected());
        auto y = voltage<T>(C1);
        P1.incident(dp.reflected());
        return  y;

    }

    void setDiodeType(int diode_type)
    {
        switch (diode_type)
        {
            case 0: // GZ34
                dp.setDiodeParameters(47.0e-9, 25.85e-3, 1); // Example values
                break;
            case 1: // 1N4148
                dp.setDiodeParameters(4.0e-12, 25.85e-3, 1); // Example values
                break;
            case 2: // 1N4007
                dp.setDiodeParameters(15.0e-12, 25.85e-3, 1); // Example values
                break;
            case 3: // BAT46
                dp.setDiodeParameters(2.0e-12, 25.85e-3, 1); // Example values
                break;
            case 4: // Zener
                dp.setDiodeParameters(50.0e-12, 25.85e-3, 1); // Example values
                break;
            default:
                // Handle unknown diode type
                dp.setDiodeParameters(4.0e-12, 25.85e-3, 1); // Example values
                break;
        }
    }

    void setCapacitorType(int capacitor_type)
    {
        switch (capacitor_type)
        {
            case 0: // 47 nF - Ceramic
                this->Cap = 47.0e-9;
                break;
            case 1: // 100 nF - Ceramic
                this->Cap = 100.0e-9; 
                break;
            case 2: // 10 µF - Film
                this->Cap = 10.0e-6;
                break;
            case 3: // 22 µF - Tantalum
                this->Cap = 22.0e-6;
                break;
            default: // 100 nF - Ceramic
                this->Cap = 100.0e-9;
                break;
        }
    }

private:

    T Cap{100.0e-9};
    T applyInputGain(T input)
    {
        //input *= this->input_gain * 1.5f * 1.104f;
        input *= this->input_gain;
        return input;
    }

    float sampleRate{44100.f};
    T input_gain{1.f};
    float offset{0.f};
    
    ResistorT<T> R1 { 4700.f };
    ResistiveVoltageSourceT<T> Vs;
    WDFSeriesT<T, decltype (Vs), decltype (R1)> S1 { Vs, R1 };
    //CapacitorT<T> C1 { 47.0e-9f }; // 47 nF - Ceramic
    CapacitorT<T> C1 { 100.0e-9f }; // 100 nF - Ceramic
    //CapacitorT<T> C1 { 10.0e-6f }; // 10 µF - Film
    //CapacitorT<T> C1 { 22.0e-6f }; // 22 µF - Tantalum




    WDFParallelT<T, decltype (S1), decltype (C1)> P1 { S1, C1 };
    //DiodePairT<T, decltype (P1)> dp { P1, 2.52e-9f };           // GZ34         diode pair
    DiodePairT<T, decltype (P1)> dp { P1, 4.0e-12 };           // 1N4148       diode pair
    //DiodePairT<T, decltype (P1)> dp { P1, 15.0e-12f };          // 1N4007       diode pair
    //DiodePairT<T, decltype (P1)> dp { P1, 2.0e-12f };           // BAT46        diode pair
    //DiodePairT<T, decltype (P1)> dp { P1, 50.0e-12f };          // Zener        diode pair

};

#endif // DIODECLIPPER_H_INCLUDED