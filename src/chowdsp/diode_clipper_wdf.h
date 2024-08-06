#ifndef DIODECLIPPER_H_INCLUDED
#define DIODECLIPPER_H_INCLUDED

#include "cmath"
#include "chowdsp_wdf.h"

using namespace chowdsp::wdft;

class DiodeClipper
{
public:

    DiodeClipper() = default;

    void prepare(double sampleRate)
    {
        C1.prepare((float) sampleRate);
    }

    void reset()
    {
        C1.reset();
    }

    void setCircuitParams(float cutoff)
    {
        dp.setDiodeParameters(2.52e-9f, 0.02585f, 16);
        constexpr auto Cap = 47.0e-9f;
        const auto Res = 1.0f / (2 * 3.141592653589793238L * cutoff * Cap);
        C1.setCapacitanceValue(Cap);
        R1.setResistanceValue(Res);
    }

    void setCircuitElements(float res, float cap)
    {
        C1.setCapacitanceValue(cap);
        R1.setResistanceValue(res);
    }

    inline float processSample(float x)
    {
        Vs.setVoltage(x);

        dp.incident(P1.reflected());
        auto y = voltage<float>(C1);
        P1.incident(dp.reflected());

        return y;
    }

    //void setDiodeType(float diodeTypeNumber)
    //{
    //    float newIs = 0.0f;
    //    float newVt = 0.02585f; // Typical thermal voltage at room temperature
    //    float nDiodes = 16.f; // Number of diodes in series, adjust as needed
    //
    //    if (diodeTypeNumber == 0.f)
    //    {
    //        newIs = 2.52e-9f; // GZ34
    //    }
    //    else if (diodeTypeNumber == 1.f)
    //    {
    //        newIs = 200.0e-12f; // 1N34
    //    }
    //    else if (diodeTypeNumber == 2.f)
    //    {
    //        newIs = 2.64e-9f; // 1N4148
    //    }
    //
    //    // Set the new diode parameters
    //    dp.setDiodeParameters(newIs, newVt, nDiodes);
    //}

private:
    //float diodeType;
    ResistorT<float> R1 { 4700.0f };
    ResistiveVoltageSourceT<float> Vs;
    WDFSeriesT<float, decltype (Vs), decltype (R1)> S1 { Vs, R1 };

    CapacitorT<float> C1 { 47.0e-9f };
    WDFParallelT<float, decltype (S1), decltype (C1)> P1 { S1, C1 };

    // GZ34 diode pair
    DiodePairT<float, decltype (P1)> dp { P1, 2.52e-9f };
};

#endif // DIODECLIPPER_H_INCLUDED
