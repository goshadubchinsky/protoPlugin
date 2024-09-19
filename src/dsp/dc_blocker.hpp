#pragma once

template <typename T>
class DC_Blocker
{
public:
    DC_Blocker() : sample_rate(44100), input_1(0), output_1(0) {}

    void setSampleRate(float sample_rate)
    {
        this->sample_rate = sample_rate;
    }

    T process(T input, float cutoff)
    {
        float R = 1 - (M_PI * 2 * cutoff / sample_rate);

        T output = input - input_1 + R * output_1;

        // Update the previous state
        input_1 = input;
        output_1 = output;

        return output;
    }

private:
    float sample_rate;  // Declare first
    T input_1;      // Declare second
    T output_1;     // Declare third
};
