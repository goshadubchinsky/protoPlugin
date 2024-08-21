#include "plugin.hpp"
#include "chowdsp/diode_clipper_wdf_class.hpp"
#include "chowdsp/ChowDSP.hpp"


struct M102 : Module {
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
		ENUMS(IN_INPUT, 2),
		CV1_INPUT,
		CV2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(OUT_OUTPUT, 2),
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_LIGHT,
		LIGHTS_LEN
	};

	M102() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(INPUT_PARAM, 0.f, 10.f, 1.f, "Input Gain Multiplier");
		configParam(OFFSET_PARAM, -20.f, 20.f, 0.f, "Voltage Offset", " V");
		configParam(OUTPUT_PARAM, 0.f, 2.f, 1.f, "Output Gain Multiplier");
		configParam(CV1_PARAM, 0.f, 1.f, 1.f, "CV1 Attenuator");
		configParam(CV2_PARAM, -1.f, 1.f, 0.f, "CV2 Attenuverter");

		const float minFreqHz = 16.0f;
		const float maxFreqHz = 22000.0f;
		//const float defaultFreqHz = 1000.0f;
		const float minFreq = (std::log2(minFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float maxFreq = (std::log2(maxFreqHz / dsp::FREQ_C4) + 5) / 10;
		//const float defaultFreq = (std::log2(defaultFreqHz / dsp::FREQ_C4) + 5) / 10;
		configParam(CUTOFF_PARAM, minFreq, maxFreq, maxFreq, "Cutoff frequency", " Hz", std::pow(2, 10.f), dsp::FREQ_C4 / std::pow(2, 5.f));

		configInput(IN_INPUT+0, "Left Input");
		configInput(IN_INPUT+1, "Right Input");

		configSwitch(RANGE_PARAM,1.f, 3.f, 1.f, "Gain Control Multiplier", {"x1", "x2", "x3"});
			paramQuantities[RANGE_PARAM]->snapEnabled = true;
		configInput(CV1_INPUT, "");
		configInput(CV2_INPUT, "");
		configOutput(OUT_OUTPUT+0, "Left Output");
		configOutput(OUT_OUTPUT+1, "Right Output");

		configBypass(IN_INPUT + 0, OUT_OUTPUT + 0);
		configBypass(IN_INPUT + 1, OUT_OUTPUT + 1);

		oversample.setOversamplingIndex(2); // default 4x oversampling

		onSampleRateChange();
	}

	// channels
	static const int channels = 2;
	// INPUTS+p
	float input[channels] = {0.f};
	float input_param{1.f}; float range_param{1.f};
	float input_cv{0.f}; float input_cv_param{0.f};
	float input_cv_2{0.f}; float input_cv_param_2{0.f};
	
	// OUTPUTS+p
	float output[channels] = {0.f};
	float output_param{1.f};

	// PARAMS
	float offset_param{0.f};
	float stereo_offset_param[channels] = {0.f};
	float cutoff = {0.f};

	// OVERSAMPLE
	static const int UPSAMPLE = 8;
	//static const int QUALITY = 8;
	//dsp::Upsampler<UPSAMPLE, QUALITY> upsampler[channels];
    //dsp::Decimator<UPSAMPLE, QUALITY> decimator[channels];
	//float upsampled[channels][UPSAMPLE];
	//float processed[channels][UPSAMPLE];

	// CHOWDSP OVERSAMPLING
	chowdsp::VariableOversampling oversample;
	int oversamplingIndex = {1};

	//CLASSES
	DiodeClipper diode_clipper[channels];
	
	//OTHER
	float gate_length{0.5f};
	rack::dsp::PulseGenerator gateGenerator;


	void onAdd (const AddEvent &e) override
	{
		for (int c = 0; c < channels; ++c)
		{
			UPSAMPLE = 2^oversamplingIndex;
			diode_clipper[c].setSampleRate(APP->engine->getSampleRate(), UPSAMPLE);
			diode_clipper[c].reset();
		}
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override
	{
		float newSampleRate = getSampleRate();
		
		for (int c = 0; c < channels; ++c)
		{
			UPSAMPLE = 2^oversamplingIndex;
			diode_clipper[c].setSampleRate(newSampleRate * oversample.getOversamplingRatio(), UPSAMPLE);
			diode_clipper[c].reset();
		}
	}

	//void onReset(const ResetEvent &e) override
	//{
	//	for (int c = 0; c < channels; ++c)
	//	{diode_clipper[c].reset();}
	//}

	void process(const ProcessArgs& args) override {

		if ((inputs[IN_INPUT + 0].isConnected() && outputs[OUT_OUTPUT + 0].isConnected()) || (inputs[IN_INPUT + 1].isConnected() && outputs[OUT_OUTPUT + 1].isConnected()))
		{
			range_param = params[RANGE_PARAM].getValue();
			input_param = params[INPUT_PARAM].getValue();
			input_param *= range_param;
			
			if (inputs[CV2_INPUT].isConnected())
			{
				input_cv_param_2 = params[CV2_PARAM].getValue();
				input_cv_2 = input_cv_param_2 * inputs[CV2_INPUT].getVoltage();
				input_param += input_cv_2;
			}

			if (inputs[CV1_INPUT].isConnected())
			{
				input_cv_param = params[CV1_PARAM].getValue() * 0.1f;
				input_cv = input_cv_param * (inputs[CV1_INPUT].getVoltage());
				input_param *= input_cv;
			}

			output_param = params[OUTPUT_PARAM].getValue();
			offset_param = params[OFFSET_PARAM].getValue();

			cutoff = params[CUTOFF_PARAM].getValue();
			cutoff *= 10.f - 5.f;
			cutoff = dsp::FREQ_C4 * dsp::exp2_taylor5(cutoff);
			cutoff = clamp(cutoff, 16.f, APP->engine->getSampleRate() / 2.f );
		}

		for (int c = 0; c < channels; ++c)
		{
			if (inputs[IN_INPUT + c].isConnected() && outputs[OUT_OUTPUT + c].isConnected())
			{
				diode_clipper[c].setCircuitParams(input_param, offset_param, cutoff);
				input[c] = inputs[IN_INPUT + c].getVoltageSum();
				upsampler[c].process(input[c], upsampled[c]);

				for (int u = 0; u < UPSAMPLE; ++u)
				{
					processed[c][u] = diode_clipper[c].processSample(upsampled[c][u]);
				}

				output[c] = output_param * decimator[c].process(processed[c]);
				outputs[OUT_OUTPUT + c].setVoltage(clamp(-output[c], -10.f, 10.f));
			}
		}

		gate_length = 0.1f;
		float sample_time = args.sampleRate;
		if (std::max(std::fabs(output[0]),std::fabs(output[1])) >= 10.f)
		{gateGenerator.trigger(gate_length);}
		lights[LIGHT_LIGHT].setBrightness(gateGenerator.process(1.f/sample_time));
		
	}
};


struct M102Widget : ModuleWidget {
	M102Widget(M102* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/M102.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 21.8)), module, M102::INPUT_PARAM));
		addParam(createParamCentered<ScrewKnobBlue>(mm2px(Vec(6.716, 42.75)), module, M102::OFFSET_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 42.75)), module, M102::OUTPUT_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 63.7)), module, M102::CV1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 84.65)), module, M102::CV2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(25.4, 105.6)), module, M102::CUTOFF_PARAM));

		addParam(createParamCentered<BefacoSwitchVertical>(mm2px(Vec(6.716, 32.782)), module, M102::RANGE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.729, 13.152)), module, M102::IN_INPUT+0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 21.8)), module, M102::IN_INPUT+1));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.7, 42.75)), module, M102::LIGHT_LIGHT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 63.7)), module, M102::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.716, 84.65)), module, M102::CV2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.716, 105.6)), module, M102::OUT_OUTPUT+0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.716, 114.255)), module, M102::OUT_OUTPUT+1));
	}

	void appendContextMenu(Menu* menu) override {
		M102* module = dynamic_cast<M102*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator());
		
		menu->addChild(createIndexSubmenuItem("Oversampling",
		{"Off", "x2", "x4", "x8"},
		[ = ]() {
		//	return module->oversamplingIndex;
			return 0;
		},
		[ = ](int mode) {
			module->oversamplingIndex = mode;
			module->onSampleRateChange();
			return 0;
		}
		                                     ));

	}
};


Model* modelM102 = createModel<M102, M102Widget>("M102");