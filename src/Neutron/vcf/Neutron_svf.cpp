#include <cstdlib>
#include <cmath>
#include "Neutron_svf.h"
#include "fastmath.h"

// Maximum Newton-Raphson Iteration Steps
#define SVF_MAX_NEWTON_STEPS 8

// Check For Newton-Raphson Breaking Limit
#define SVF_NEWTON_BREAKING_LIMIT 1

// Constructor
NeutronSVF::NeutronSVF(double cutoffFrequency, double Resonance, double sampleRate)
{
    // Initialize Filter Parameters
    this->cutoffFrequency = cutoffFrequency;
    this->Resonance = Resonance;
    this->sampleRate = sampleRate;

    // Initialize Filter State
    SetFilterIntegrationRate();
    hp = bp = lp = out = u_t1 = 0.0;
}

// Default Constructor
NeutronSVF::NeutronSVF()
{
    // Initialize Filter Parameters
    this->cutoffFrequency = 0.25;
    this->Resonance = 0.5;
    this->sampleRate = 44100;

    SetFilterIntegrationRate();

    // Initialize Filter State
    hp = bp = lp = out = u_t1 = 0.0;
}

// Default Deconstructor
NeutronSVF::~NeutronSVF()
{
    //
}

void NeutronSVF::ResetFilterState()
{
    // Initialize Filter Parameters
    cutoffFrequency = 0.25;
    Resonance = 0.5;

    SetFilterIntegrationRate();

    // Initialize Filter State
    hp = bp = lp = out = u_t1 = 0.0;
}

void NeutronSVF::SetFilterCutoff(double cutoffFrequency)
{
    this->cutoffFrequency = cutoffFrequency;
    SetFilterIntegrationRate();
}

void NeutronSVF::SetFilterResonance(double Resonance)
{
    this->Resonance = Resonance;
}

void NeutronSVF::SetFilterSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
    SetFilterIntegrationRate();
}

void NeutronSVF::SetFilterIntegrationRate()
{
    // Normalize Cutoff Freq To Samplerate
    this->dt = 44100.0 / (this->sampleRate * this->cutoffFrequency);

    // Clamp Integration Rate
    if(this->dt < 0.0)
    {
        this->dt = 0.0;
    }
}

// GET
double NeutronSVF::GetFilterCutoff()
{
  return this->cutoffFrequency;
}

double NeutronSVF::GetFilterResonance()
{
  return this->Resonance;
}

double NeutronSVF::GetFilterOutput()
{
  return this->out;
}

double NeutronSVF::GetFilterSampleRate()
{
  return this->sampleRate;
}

// Tick

void NeutronSVF::filter(double input)
{
    // Noise Term
    double noise;

    // Feedback Amound Variables
    double fb = 1.0 - (3.5 * this->Resonance);

    // Integration Rate
    double dt2 = dt;

    // Update Noise Terms
    noise = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
    noise = 1.0e-6 * 2.0 * (noise - 0.5);

    input += noise;

    // Clamp Integration Rate
    if (dt2 > 0.65)
    {
        dt2 = 0.65;
    }

    // Trapezoidal Integration
    double alpha = dt2/2.0;
    double beta = 1.0 - (0.0075/1.0);
    double alpha2 = dt2 * dt2 / 4.0 + fb * alpha;
    double D_t = (1.0 - dt2 * dt2 / 4.0) * this->bp + alpha * (this->u_t1 + input - 2.0 * this->lp - fb * this->bp - SinhPade54(this->bp));
    double x_k, x_k2;
    
    // Starting Point Is Last Output
    x_k = this->bp;

    // Newton-Raphson
    for (int ii = 0; ii < SVF_MAX_NEWTON_STEPS; ii++)
    {
        x_k2 = x_k - (x_k + alpha * SinhPade54(x_k) + alpha2 * x_k - D_t) / (1.0 + alpha * CoshPade54(x_k) + alpha2);
            #ifdef SVF_NEWTON_BREAKING_LIMIT
                // Breaking Limit
                if (fabs(x_k2 - x_k) < 1.0e-9f)
                {
                    x_k = x_k2;
                    break;
                }
        #endif
        x_k = x_k2;
    }

    this->lp += alpha * this->bp;
    this->bp = beta * x_k;
    this->lp += alpha * this->bp;
    this->hp = input - this->lp - fb * this->bp;

    // Set Input at t-1
    this->u_t1 = input;
}

double NeutronSVF::GetFilterLowpass()
{
    return this->lp;
}
double NeutronSVF::GetFilterBandpass()
{
    return this->bp;
}
double NeutronSVF::GetFilterHighpass()
{
    return this->hp;
}