#ifndef _Neutron_svf_
#define _Neutron_svf_

class NeutronSVF
{
public:
    // Constructor / Deconstructor
    NeutronSVF(double Cutoff, double Resonance, double SampleRate);
    NeutronSVF();
   ~NeutronSVF();

    // Set Filter Parameters
    void SetFilterCutoff(double Cutoff);
    void SetFilterResonance(double Resonance);
    void SetFilterSampleRate(double SampleRate);

    // Get Filter Parameters
    double GetFilterCutoff();
    double GetFilterResonance();
    double GetFilterSampleRate();

    // Tick Filter State
    void filter(double input);

    // get filter responses
    double GetFilterLowpass();
    double GetFilterBandpass();
    double GetFilterHighpass();

    // get filter output
    //double GetFilterOutput();

    // reset state
    void ResetFilterState();

private:
    // set integration rate
    void SetFilterIntegrationRate();

    // Pade Approximant Functions For Hyperbolic Functions
    // Filter Parameters
    double cutoffFrequency;
    double Resonance;
    double dt;
    double sampleRate;

    // Filter State
    double lp;
    double bp;
    double hp;
    double u_t1;

    // Filter Output
    // double out;

};

#endif