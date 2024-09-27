#include "plugin.hpp"
#include "shared/shared.cpp"
#include "chowdsp/ChowDSP.hpp"
#include "dsp/dc_blocker.hpp"
#include "chowdsp/diode_clipper_wdf_class.hpp"

using b_float = xsimd::batch<float>;

struct M102XSIMD2 : Module {
	enum ParamId {
		INPUT_PARAM,
		RANGE_PARAM,
		OUTPUT_PARAM,
		CV2_PARAM,
		BIAS_PARAM,
		TONE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		CVOUT_INPUT,
		CV2_INPUT,
		CVBIAS_INPUT,
		CVTONE_INPUT,
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_LIGHT,
		LIGHTS_LEN
	};

	M102XSIMD2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(INPUT_PARAM, 0.f, 10.f, 1.f, "Input Gain");
		configSwitch(RANGE_PARAM, 1.f, 3.f, 1.f, "Input Gain Multiplier", {"x1", "x2", "x3"});
			paramQuantities[RANGE_PARAM]->snapEnabled = true;
		configParam(OUTPUT_PARAM, 0.f, 4.f, 1.f, "Output Gain", "", 0.f, 2.5f);
			paramQuantities[OUTPUT_PARAM]->randomizeEnabled = false;
		configParam(CV2_PARAM, -1.f, 1.f, 0.f, "CV2 Attenuverter", "%", 0.f, 100.f, 0.f);
		configParam(BIAS_PARAM, -10.f, 10.f, 0.f, "Input Offset", " V");

		const float minFreqHz = 1.0f;	// ??????? 16
		const float maxFreqHz = 22000.0f;	// ???????
		const float minFreq = (std::log2(minFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float maxFreq = (std::log2(maxFreqHz / dsp::FREQ_C4) + 5) / 10;
		configParam(TONE_PARAM, minFreq, maxFreq, maxFreq, "Cutoff frequency", " ", 0.f, 10.f/(maxFreq-minFreq), -minFreq * (10.f/(maxFreq-minFreq)) );

		configInput(CV1_INPUT, "CV1");
		configInput(CVOUT_INPUT, "CV Out");
		configInput(CV2_INPUT, "CV2");
		configInput(CVBIAS_INPUT, "CV Bias");
		configInput(CVTONE_INPUT, "CV Tone");
		configInput(AUDIO_INPUT, "Signal");
		configOutput(AUDIO_OUTPUT, "Signal");

		configBypass(AUDIO_INPUT, AUDIO_OUTPUT);

		onSampleRateChange();
	}

	// params
	float input_param = {0.f};
	float range_param = {0.f};
	float output_param = {0.f};
	float cv2_param = {0.f};
	float offset_param = {0.f};
	float tone_param = {0.f};

	// inputs XSIMD
	b_float audio_input_batch[4] = {0.f};
	b_float cv_1_input_batch[4] = {0.f};
	b_float cv_out_input_batch[4] = {0.f};
	b_float cv_2_input_batch[4] = {0.f};
	b_float cv_bias_input_batch[4] = {0.f};
	b_float cv_tone_input_batch[4] = {0.f};
	float output_array[16] = {0.f};

	// OVERSAMPLING
	chowdsp::VariableOversampling<6, b_float> oversample[4];	// uses a 2*6=12th order Butterworth filter
	int oversamplingIndex = 1;	// default oversampling ratio x2

	// Diode_WDF
	DiodeClipper<b_float> diode_clipper[4];
	int diode_type = {1};
	int capacitor_type = {1};

	// DC Blocker
	DC_Blocker<b_float> dc_blocker[4];
	bool dc_blocker_active = false;

	// Safety Clipper Light
	static constexpr float gate_length = 0.5f;
	rack::dsp::PulseGenerator gateGenerator;
	float maxAbsValue = 0.0f;

	void process(const ProcessArgs& args) override {
		
		// Get XSIMD batch size
			// Prepare the dispatcher to target AVX2 and SSE2 architectures
		shared<float> detector;

			// Define a functor (lambda) that calls the batch_size_detector
		auto batch_size_functor = [&detector](auto arch) {
			return detector.batch_size_detector(arch);
		};

			// Create the dispatching function, specifying the architecture list (AVX2 and SSE2)
		auto dispatched = xsimd::dispatch<xsimd::arch_list<xsimd::avx2, xsimd::sse2>>(batch_size_functor);

			// Dispatch based on runtime information (will choose AVX2 or SSE2 based on the CPU)
		int batch_size = dispatched();

		//static const int batch_size = get_batch_size();
		outputs[AUDIO_OUTPUT].setVoltage((float)batch_size);

		// Get oversampling ratio
		//const int oversamplingRatio = oversample[0].getOversamplingRatio();
		//
		//// Get number of polyphony
		//const int channels = inputs[AUDIO_INPUT].getChannels();
		//
		//
		//// Range Switch
		//range_param = params[RANGE_PARAM].getValue();
		//
		//// Params (place these in different process to call only once in channels loop)
		//input_param =	params[INPUT_PARAM].getValue();
		//range_param =	params[RANGE_PARAM].getValue();
		//output_param =	params[OUTPUT_PARAM].getValue();
		//cv2_param =		params[CV2_PARAM].getValue();
		//offset_param =	params[BIAS_PARAM].getValue();
		//tone_param =	params[TONE_PARAM].getValue();
		//
		//// Inputs Pointers
		//float* audio_voltage_ptr = 		inputs[AUDIO_INPUT].getVoltages();
		//float* cv_1_voltage_ptr = 		inputs[CV1_INPUT].getVoltages();
		//float* cv_out_voltage_ptr = 	inputs[CVOUT_INPUT].getVoltages();
		//float* cv_2_voltage_ptr = 		inputs[CV2_INPUT].getVoltages();
		//float* cv_bias_voltage_ptr = 	inputs[CVBIAS_INPUT].getVoltages();
		//float* cv_tone_voltage_ptr = 	inputs[CVTONE_INPUT].getVoltages();
		//
		//// Cutoff calculation (MONO)
		//if (channels == 1 || inputs[CVTONE_INPUT].getChannels() == 1)
		//{
		//	tone_param += &cv_tone_voltage_ptr[0]; // ???????
		//	tone_param = dsp::FREQ_C4 * dsp::exp2_taylor5(tone_param); // ???????
		//	tone_param = clamp(tone_param, 1.f, getSampleRate() * oversamplingRatio * 0.5f); // ??????? // getSampleRate() * oversample[0].getOversamplingRatio()
		//}
		//
		//// Polyphony loop
		//for (int c = 0; c < channels; c += batch_size)
		//{
		//	// Calculate batch_index for oversampler
		//	const int batch_index = c / batch_size;
		//
		//	// Cutoff calculation (POLY)
		//	if (channels > 1 || inputs[CVTONE_INPUT].getChannels() > 1)
		//	{
		//		cv_tone_input_batch[batch_index] = xsimd::load_unaligned(&cv_tone_voltage_ptr[c]);
		//		cv_tone_input_batch[batch_index] += tone_param;
		//		// TODO XSIMD cv_tone_input_batch[batch_index] = dsp::FREQ_C4 * dsp::exp2_taylor5(cv_tone_input_batch[batch_index]);
		//		// TODO XSIMD cv_tone_input_batch[batch_index] = clamp(cv_tone_input_batch[batch_index], 1.f, getSampleRate() * oversamplingRatio * 0.5f);
		//
		//	}
		//
		//	audio_input_batch[batch_index] = xsimd::load_unaligned(&input_voltage_ptr[c]);
       	//	audio_input_batch[batch_index] *= 0.2f;
		//}

	}

	void onSampleRateChange() override {
		float newSampleRate = getSampleRate();
		static const int batch_size = xsimd::batch<float>::size;

		for (int c = 0; c < batch_size; c++) {
			oversample[c].setOversamplingIndex(oversamplingIndex);
			oversample[c].reset(newSampleRate);

			diode_clipper[c].setDiodeType(diode_type);
			diode_clipper[c].setCapacitorType(capacitor_type);
			diode_clipper[c].setSampleRate(newSampleRate * oversample[0].getOversamplingRatio());
			diode_clipper[c].reset();

			if (dc_blocker_active)
			{
				dc_blocker[c].setSampleRate(newSampleRate * oversample[0].getOversamplingRatio());
			}
		}
	}

	void onReset() override {
        Module::onReset();

		float newSampleRate = getSampleRate();
		static const int batch_size = xsimd::batch<float>::size;

		for (int c = 0; c < batch_size; ++c)
		{
			oversample[c].setOversamplingIndex(oversamplingIndex);
			oversample[c].reset(newSampleRate);

			diode_clipper[c].setDiodeType(diode_type);
			diode_clipper[c].setCapacitorType(capacitor_type);
			diode_clipper[c].setSampleRate(newSampleRate * oversample[0].getOversamplingRatio());
			diode_clipper[c].reset();

			if (dc_blocker_active)
			{
				dc_blocker[c].setSampleRate(newSampleRate * oversample[0].getOversamplingRatio());
			}
		}
    }
};


struct M102XSIMD2Widget : ModuleWidget {
	M102XSIMD2Widget(M102XSIMD2* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/M102XSIMD2.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Neutron_Knob_Small>(mm2px(Vec(23.35, 19.0)), module, M102XSIMD2::INPUT_PARAM));
		addParam(createParamCentered<BefacoSwitchVertical>(mm2px(Vec(7.0, 29.875)), module, M102XSIMD2::RANGE_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 40.75)), module, M102XSIMD2::OUTPUT_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 58.75)), module, M102XSIMD2::CV2_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 76.75)), module, M102XSIMD2::BIAS_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 94.75)), module, M102XSIMD2::TONE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 19.0)), module, M102XSIMD2::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 40.75)), module, M102XSIMD2::CVOUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 58.75)), module, M102XSIMD2::CV2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 76.75)), module, M102XSIMD2::CVBIAS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 94.75)), module, M102XSIMD2::CVTONE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 114.0)), module, M102XSIMD2::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.56, 114.0)), module, M102XSIMD2::AUDIO_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(14.75, 40.75)), module, M102XSIMD2::LIGHT_LIGHT));
	}
};


Model* modelM102XSIMD2 = createModel<M102XSIMD2, M102XSIMD2Widget>("M102XSIMD2");