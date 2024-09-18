#include "plugin.hpp"
#include "chowdsp/ChowDSP.hpp"
#include "dsp/dc_blocker.hpp"
#include "chowdsp/diode_clipper_wdf_class.hpp"

using b_float = xsimd::batch<float>;

template <typename T>
xsimd::batch<T> tanh_Pade_XSIMD(xsimd::batch<T> x) {
    using batch_type = xsimd::batch<T>;

    // Pade approximant of tanh
    x = xsimd::max(xsimd::min(x, batch_type(3.f)), batch_type(-3.f)); // Clamp within [-3.f, 3.f]

    batch_type numerator = x * (batch_type(27) + x * x);
    batch_type denominator = batch_type(27) + batch_type(9) * x * x;

    return numerator / denominator;
}

template <typename T>
static T plushalf_normal(T x) {

	x += 0.5;
	return x;
}

struct M102XSIMD : Module {
	enum ParamId {
		INPUT_PARAM,
		OFFSET_PARAM,
		OUTPUT_PARAM,
		CV1_PARAM,
		CV2_PARAM,
		CUTOFF_PARAM,
		RANGE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_INPUT,
		CV1_INPUT,
		CV2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_LIGHT,
		LIGHTS_LEN
	};

	M102XSIMD() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(INPUT_PARAM, 0.f, 10.f, 0.5f, "Input Gain Multiplier");
		configParam(OFFSET_PARAM, -10.f, 10.f, 0.f, "Voltage Offset", " V");
		configParam(OUTPUT_PARAM, 0.f, 4.f, 2.f, "Output Gain Multiplier");
			paramQuantities[OUTPUT_PARAM]->randomizeEnabled = false;
		configParam(CV1_PARAM, 0.f, 1.f, 1.f, "CV1 Attenuator", "", 0.f, 10.f);
		configParam(CV2_PARAM, -1.f, 1.f, 0.f, "CV2 Attenuverter", "", 0.f, 5.f);

		const float minFreqHz = 16.0f;
		const float maxFreqHz = 22000.0f;
		//const float defaultFreqHz = 1000.0f;
		const float minFreq = (std::log2(minFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float maxFreq = (std::log2(maxFreqHz / dsp::FREQ_C4) + 5) / 10;
		//const float defaultFreq = (std::log2(defaultFreqHz / dsp::FREQ_C4) + 5) / 10;
		configParam(CUTOFF_PARAM, minFreq, maxFreq, maxFreq, "Cutoff frequency", " ", 0.f, 10.f/(maxFreq-minFreq), -minFreq * (10.f/(maxFreq-minFreq)) );

		configInput(INPUT_INPUT, "Input");
		configSwitch(RANGE_PARAM,1.f, 3.f, 1.f, "Gain Control Multiplier", {"x1", "x2", "x3"});
			paramQuantities[RANGE_PARAM]->snapEnabled = true;


		configInput(CV1_INPUT, "");
		configInput(CV2_INPUT, "");

		configOutput(OUTPUT_OUTPUT, "Output");

		onSampleRateChange();
	}

	xsimd::batch<float> input_batch[4] = {0.f};
	
    void process(const ProcessArgs& args) override
    {
        const int channels = inputs[INPUT_INPUT].getChannels();
		if (channels > 1)
        {
			float* voltage_ptr = inputs[INPUT_INPUT].getVoltages();
			int batch_size = xsimd::batch<float>::size;
			for (int x = 0; x < channels; x += batch_size)
			{
				int batch_index = x / batch_size;
       			input_batch[batch_index] = xsimd::load_aligned(&voltage_ptr[x]);
				input_batch[batch_index] = xsimd::sin(xsimd::tanh(input_batch[batch_index])); //TEST MATH
				input_batch[batch_index].store_aligned(&voltage_ptr[x]);
			}

			for (int o = 0; o < channels; ++o)
            {
                outputs[OUTPUT_OUTPUT].setVoltage(voltage_ptr[o], o);
            }
			
            outputs[OUTPUT_OUTPUT].setChannels(channels);
        }
        else if (channels == 1)
        {
            // Monophonic case
            float input = inputs[INPUT_INPUT].getVoltage();
            input = sin(tanh(input));
            outputs[OUTPUT_OUTPUT].setVoltage(input);
			outputs[OUTPUT_OUTPUT].setChannels(channels);
        }
		else if (channels == 0)
		{
			outputs[OUTPUT_OUTPUT].setVoltage(0.f);
		}
    }

	
};


struct M102XSIMDWidget : ModuleWidget {
	M102XSIMDWidget(M102XSIMD* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/M102XSIMD.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 21.8)), module, M102XSIMD::INPUT_PARAM));
		addParam(createParamCentered<ScrewKnobBlue>(mm2px(Vec(6.716, 42.75)), module, M102XSIMD::OFFSET_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 42.75)), module, M102XSIMD::OUTPUT_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 63.7)), module, M102XSIMD::CV1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 84.65)), module, M102XSIMD::CV2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 105.6)), module, M102XSIMD::CUTOFF_PARAM));

		addParam(createParamCentered<BefacoSwitchVertical>(mm2px(Vec(6.716, 32.782)), module, M102XSIMD::RANGE_PARAM));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.7, 42.75)), module, M102XSIMD::LIGHT_LIGHT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 21.8)), module, M102XSIMD::INPUT_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 32.782)), module, M102XSIMD::RANGE_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 42.75)), module, M102XSIMD::LIGHT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 63.7)), module, M102XSIMD::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 84.65)), module, M102XSIMD::CV2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.716, 105.6)), module, M102XSIMD::OUTPUT_OUTPUT));
	}
};


Model* modelM102XSIMD = createModel<M102XSIMD, M102XSIMDWidget>("M102XSIMD");