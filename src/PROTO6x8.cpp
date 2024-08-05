#include "plugin.hpp"
#include "chowdsp/diode_clipper_wdf.h"


struct PROTO6x8 : Module {
	enum ParamId {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAM3_PARAM,
		PARAM4_PARAM,
		PARAM5_PARAM,
		PARAM6_PARAM,
		//PARAM7_PARAM,
		//PARAM8_PARAM,
		//PARAM9_PARAM,
		//PARAM10_PARAM,
		//PARAM11_PARAM,
		//PARAM12_PARAM,
		//PARAM13_PARAM,
		//PARAM14_PARAM,
		//PARAM15_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		//CV1_INPUT,
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
		LIGHTS_LEN
	};

	PROTO6x8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM1_PARAM, 20.f, 20000.f, 1000.f, "freq");
		configParam(PARAM2_PARAM, 0.f, 30.f, 3.5f, "in");
		configParam(PARAM3_PARAM, 0.f, 5.f, 1.f, "out");
		configParam(PARAM4_PARAM, -5.f, 5.f, 0.f, "offset");
		configParam(PARAM5_PARAM, 0.f, 2.f, 0.f, "Diode Type"); getParamQuantity(PARAM5_PARAM)->snapEnabled = true;
		configParam(PARAM6_PARAM, 0.f, 10.f, 0.f, "soft");
		//configParam(PARAM7_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM8_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM9_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM10_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM11_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM12_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM13_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM14_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM15_PARAM, 0.f, 1.f, 0.f, "");
		//configInput(CV1_INPUT, "");
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
	}

	DiodeClipper DiodeClipper;

	void onSampleRateChange() override
	{
		DiodeClipper.reset();
	}

	void onReset() override
	{
		DiodeClipper.reset();
	}

	dsp::Upsampler<2, 2> upsampler;
    dsp::Decimator<2, 2> decimator;

	float upsampled[2];
	float processed[2];

	void process(const ProcessArgs& args) override {
		
		float input = inputs[IN1_INPUT].getVoltage();
        upsampler.process(input, upsampled);

		DiodeClipper.prepare(2*args.sampleRate);

		//float freqParam = (float)args.sampleRate/2.f;
		float freqParam = (float)args.sampleRate*2.f/2.f;
    	float gainParam = params[PARAM2_PARAM].getValue();
    	float outParam = params[PARAM3_PARAM].getValue();
		float offset = params[PARAM4_PARAM].getValue();
		float diodeTypeNumber = params[PARAM5_PARAM].getValue();
		DiodeClipper.setDiodeType((int)diodeTypeNumber);
		DiodeClipper.setCircuitParams (freqParam);
		//x = offset + (inputs[IN1_INPUT].getVoltage() * gainParam);
		//float output = outParam * DiodeClipper.processSample (x);

		for (int i = 0; i < 2; ++i)
		{
			float x1 = upsampled[i] * gainParam + offset;
			processed[i] = DiodeClipper.processSample(x1);
		}

		float output = outParam * decimator.process(processed);
		
		//outputs[OUT1_OUTPUT].setVoltage(-output*22.3f); //10.f*2.47524752475f*0.9f
		outputs[OUT1_OUTPUT].setVoltage(clamp(-output, -10.f, 10.f)); //10.f*2.47524752475f*0.9f

	}
};


struct PROTO6x8Widget : ModuleWidget {
	PROTO6x8Widget(PROTO6x8* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PROTO6.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 12.85)), module, PROTO6x8::PARAM1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 12.85)), module, PROTO6x8::PARAM2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 12.85)), module, PROTO6x8::PARAM3_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 12.85)), module, PROTO6x8::PARAM4_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 12.85)), module, PROTO6x8::PARAM5_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 38.55)), module, PROTO6x8::PARAM6_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 38.55)), module, PROTO6x8::PARAM7_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 38.55)), module, PROTO6x8::PARAM8_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 38.55)), module, PROTO6x8::PARAM9_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 38.55)), module, PROTO6x8::PARAM10_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 64.25)), module, PROTO6x8::PARAM11_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 64.25)), module, PROTO6x8::PARAM12_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 64.25)), module, PROTO6x8::PARAM13_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 64.25)), module, PROTO6x8::PARAM14_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 64.25)), module, PROTO6x8::PARAM15_PARAM));

		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 77.1)), module, PROTO6x8::CV1_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 77.1)), module, PROTO6x8::CV2_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 77.1)), module, PROTO6x8::CV3_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 77.1)), module, PROTO6x8::CV4_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 77.1)), module, PROTO6x8::CV5_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 86.171)), module, PROTO6x8::CV6_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 86.171)), module, PROTO6x8::CV7_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 86.171)), module, PROTO6x8::CV8_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 86.171)), module, PROTO6x8::CV9_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 86.171)), module, PROTO6x8::CV10_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 95.242)), module, PROTO6x8::CV11_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 95.242)), module, PROTO6x8::CV12_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 95.242)), module, PROTO6x8::CV13_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 95.242)), module, PROTO6x8::CV14_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 95.242)), module, PROTO6x8::CV15_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 107.562)), module, PROTO6x8::IN1_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 107.562)), module, PROTO6x8::IN2_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 107.562)), module, PROTO6x8::IN3_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 107.562)), module, PROTO6x8::IN4_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 107.562)), module, PROTO6x8::IN5_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.208, 118.296)), module, PROTO6x8::OUT1_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.624, 118.296)), module, PROTO6x8::OUT2_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 118.296)), module, PROTO6x8::OUT3_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.456, 118.296)), module, PROTO6x8::OUT4_OUTPUT));
		//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118.872, 118.296)), module, PROTO6x8::OUT5_OUTPUT));
	}
};


Model* modelPROTO6x8 = createModel<PROTO6x8, PROTO6x8Widget>("PROTO6x8");