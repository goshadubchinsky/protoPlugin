#include "plugin.hpp"
#include "chowdsp/diode_clipper_wdf_class.h"

struct PROTO6x2 : Module {
	enum ParamId {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAM3_PARAM,
		PARAM4_PARAM,
		//PARAM5_PARAM,
		//PARAM6_PARAM,
		PARAM7_PARAM,
		//PARAM8_PARAM,
		//PARAM9_PARAM,
		//PARAM10_PARAM,
		//PARAM11_PARAM,
		//PARAM12_PARAM,
		//PARAM13_PARAM,
		//PARAM14_PARAM,
		//PARAM15_PARAM,
		PUSH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		//CV2_INPUT,
		//CV3_INPUT,
		//CV4_INPUT,
		//CV5_INPUT,
		//CV6_INPUT,
		//CV7_INPUT,
		//CV8_INPUT,
		//CV9_INPUT,
		//CV10_INPUT,
		//CV11_INPUT,
		//CV12_INPUT,
		//CV13_INPUT,
		//CV14_INPUT,
		//CV15_INPUT,
		IN1_INPUT,
		//IN2_INPUT,
		//IN3_INPUT,
		//IN4_INPUT,
		//IN5_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		//OUT2_OUTPUT,
		//OUT3_OUTPUT,
		//OUT4_OUTPUT,
		//OUT5_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_LIGHT,
		PUSH_LIGHT,
		LIGHTS_LEN
	};

	PROTO6x2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		const float minFreqHz = 16.0f;
		const float maxFreqHz = 22000.0f;
		const float defaultFreqHz = 1000.0f;

		const float minFreq = (std::log2(minFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float maxFreq = (std::log2(maxFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float defaultFreq = (std::log2(defaultFreqHz / dsp::FREQ_C4) + 5) / 10;

		configParam(PARAM1_PARAM,  minFreq, maxFreq, defaultFreq, "Cutoff frequency", " Hz", std::pow(2, 10.f), dsp::FREQ_C4 / std::pow(2, 5.f));
		configParam(PARAM2_PARAM, 0.f, 30.f, 1.f, "in");
		configParam(PARAM3_PARAM, 0.f, 2.f, 1.f, "out");
		configParam(PARAM4_PARAM, -10.f, 10.f, 0.f, "offset");
		//configParam(PARAM5_PARAM, 0.f, 2.f, 0.f, "Diode Type"); getParamQuantity(PARAM5_PARAM)->snapEnabled = true;
		//configParam(PARAM6_PARAM, 0.f, 10.f, 0.f, "soft");
		configParam(PARAM7_PARAM, 0.f, 1.f, 1.f, "CV");
		configButton(PUSH_PARAM, "Push");
		//configParam(PARAM8_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM9_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM10_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM11_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM12_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM13_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM14_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM15_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CV1_INPUT, "CV INPUT");
		//configInput(CV2_INPUT, "");
		//configInput(CV3_INPUT, "");
		//configInput(CV4_INPUT, "");
		//configInput(CV5_INPUT, "");
		//configInput(CV6_INPUT, "");
		//configInput(CV7_INPUT, "");
		//configInput(CV8_INPUT, "");
		//configInput(CV9_INPUT, "");
		//configInput(CV10_INPUT, "");
		//configInput(CV11_INPUT, "");
		//configInput(CV12_INPUT, "");
		//configInput(CV13_INPUT, "");
		//configInput(CV14_INPUT, "");
		//configInput(CV15_INPUT, "");
		configInput(IN1_INPUT, "in");
		//configInput(IN2_INPUT, "");
		//configInput(IN3_INPUT, "");
		//configInput(IN4_INPUT, "");
		//configInput(IN5_INPUT, "");
		configOutput(OUT1_OUTPUT, "out");
		//configOutput(OUT2_OUTPUT, "");
		//configOutput(OUT3_OUTPUT, "");
		//configOutput(OUT4_OUTPUT, "");
		//configOutput(OUT5_OUTPUT, "");

		configBypass(IN1_INPUT, OUT1_OUTPUT);
	}

	// INPUTS+p
	float input{0.f}; float input_param{1.f};
	float input_cv{0.f}; float input_cv_param{0.f};
	
	// OUTPUTS+p
	float output{0.f}; float output_param{1.f};

	// PARAMS
	float offset_param{0.f};

	// OVERSAMPLE
	static const int UPSAMPLE = 8;
	static const int QUALITY = 8;
	dsp::Upsampler<UPSAMPLE, QUALITY> upsampler;
    dsp::Decimator<UPSAMPLE, QUALITY> decimator;
	float upsampled[UPSAMPLE];
	float processed[UPSAMPLE];

	//CLASSES
	DiodeClipper diode_clipper;

	//OTHER
	//int loopCounter = 0;

	void onAdd (const AddEvent &e) override
	{
		diode_clipper.setSampleRate(APP->engine->getSampleRate(), UPSAMPLE);
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override
	{
		diode_clipper.setSampleRate(APP->engine->getSampleRate(), UPSAMPLE);
	}

	void onReset(const ResetEvent &e) override
	{
		diode_clipper.reset();
	}

	void process(const ProcessArgs& args) override {

		if (inputs[IN1_INPUT].isConnected())
		{
			input_param = params[PARAM2_PARAM].getValue();
			
			
			if (inputs[CV1_INPUT].isConnected())
			{
			input_cv_param = params[PARAM7_PARAM].getValue() * 0.1f;
			input_cv = input_cv_param * (inputs[CV1_INPUT].getVoltage());
			input_param *= input_cv;
			}

			output_param = params[PARAM3_PARAM].getValue();
			offset_param = params[PARAM4_PARAM].getValue();

			float cutoff = params[PARAM1_PARAM].getValue();
			cutoff *= 10.f - 5.f;
			cutoff = dsp::FREQ_C4 * dsp::exp2_taylor5(cutoff);
			cutoff = clamp(cutoff, 16.f, args.sampleRate / 2.f );

			diode_clipper.setCircuitParams(input_param, offset_param, cutoff);

			input = inputs[IN1_INPUT].getVoltageSum();
			upsampler.process(input, upsampled);

			for (int i = 0; i < UPSAMPLE; ++i)
			{
				processed[i] = diode_clipper.processSample(upsampled[i]);
			}

			output = output_param * decimator.process(processed);
			outputs[OUT1_OUTPUT].setVoltage(clamp(-output, -10.f, 10.f));

			bool push = (params[PUSH_PARAM].getValue() > 0.f);
			lights[PUSH_LIGHT].setBrightness(push);

			if (std::fabs(output) >= 10.f && !push)
			{lights[LIGHT_LIGHT].setBrightness(1.f);}
			else if (push)
			{lights[LIGHT_LIGHT].setBrightness(0.f);}

			
		}	
	}
};


struct PROTO6x2Widget : ModuleWidget {
	PROTO6x2Widget(PROTO6x2* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PROTO6.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 12.85)), module, PROTO6x2::PARAM1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 12.85)), module, PROTO6x2::PARAM2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 12.85)), module, PROTO6x2::PARAM3_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 12.85)), module, PROTO6x2::PARAM4_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 12.85)), module, PROTO6x2::PARAM5_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 38.55)), module, PROTO6x2::PARAM6_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 38.55)), module, PROTO6x2::PARAM7_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 38.55)), module, PROTO6x2::PARAM8_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 38.55)), module, PROTO6x2::PARAM9_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 38.55)), module, PROTO6x2::PARAM10_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 64.25)), module, PROTO6x2::PARAM11_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 64.25)), module, PROTO6x2::PARAM12_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 64.25)), module, PROTO6x2::PARAM13_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 64.25)), module, PROTO6x2::PARAM14_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 64.25)), module, PROTO6x2::PARAM15_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 77.1)), module, PROTO6x2::CV1_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 77.1)), module, PROTO6x2::CV2_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 77.1)), module, PROTO6x2::CV3_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 77.1)), module, PROTO6x2::CV4_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 77.1)), module, PROTO6x2::CV5_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 86.171)), module, PROTO6x2::CV6_INPUT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.208, 86.171)), module, PROTO6x2::LIGHT_LIGHT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 86.171)), module, PROTO6x2::CV7_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 86.171)), module, PROTO6x2::CV8_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 86.171)), module, PROTO6x2::CV9_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 86.171)), module, PROTO6x2::CV10_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 95.242)), module, PROTO6x2::CV11_INPUT));
		addParam(createLightParamCentered<VCVLightBezel<WhiteLight>>(mm2px(Vec(13.208, 95.242)), module, PROTO6x2::PUSH_PARAM, PROTO6x2::PUSH_LIGHT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 95.242)), module, PROTO6x2::CV12_INPUT));
		
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 95.242)), module, PROTO6x2::CV13_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 95.242)), module, PROTO6x2::CV14_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 95.242)), module, PROTO6x2::CV15_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 107.562)), module, PROTO6x2::IN1_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 107.562)), module, PROTO6x2::IN2_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 107.562)), module, PROTO6x2::IN3_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 107.562)), module, PROTO6x2::IN4_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 107.562)), module, PROTO6x2::IN5_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.208, 118.296)), module, PROTO6x2::OUT1_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.624, 118.296)), module, PROTO6x2::OUT2_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 118.296)), module, PROTO6x2::OUT3_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.456, 118.296)), module, PROTO6x2::OUT4_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118.872, 118.296)), module, PROTO6x2::OUT5_OUTPUT));
	}
};


Model* modelPROTO6x2 = createModel<PROTO6x2, PROTO6x2Widget>("PROTO6x2");