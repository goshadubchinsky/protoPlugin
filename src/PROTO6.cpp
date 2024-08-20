#include "plugin.hpp"
#include <cmath>

float Transistor(float in, float gain) {
    const float log2e = std::log2(std::exp(1.0f));
    float x = in * gain;
    float z = std::exp2(x * log2e);
    float result = z / (z + 1.0f);
    return result;
}


struct PROTO6 : Module {
	enum ParamId {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAM3_PARAM,
		PARAM4_PARAM,
		PARAM5_PARAM,
		PARAM6_PARAM,
		PARAM7_PARAM,
		PARAM8_PARAM,
		PARAM9_PARAM,
		PARAM10_PARAM,
		PARAM11_PARAM,
		PARAM12_PARAM,
		PARAM13_PARAM,
		PARAM14_PARAM,
		PARAM15_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		CV2_INPUT,
		CV3_INPUT,
		CV4_INPUT,
		CV5_INPUT,
		CV6_INPUT,
		CV7_INPUT,
		CV8_INPUT,
		CV9_INPUT,
		CV10_INPUT,
		CV11_INPUT,
		CV12_INPUT,
		CV13_INPUT,
		CV14_INPUT,
		CV15_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PROTO6() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM1_PARAM, 1.f, 1000.f, 1.f, "HPF");
		configParam(PARAM2_PARAM, 0.f, 10.f, 1.f, "GAIN");
		configParam(PARAM3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM8_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM9_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM10_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM11_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM12_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM13_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM14_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM15_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CV1_INPUT, "");
		configInput(CV2_INPUT, "");
		configInput(CV3_INPUT, "");
		configInput(CV4_INPUT, "");
		configInput(CV5_INPUT, "");
		configInput(CV6_INPUT, "");
		configInput(CV7_INPUT, "");
		configInput(CV8_INPUT, "");
		configInput(CV9_INPUT, "");
		configInput(CV10_INPUT, "");
		configInput(CV11_INPUT, "");
		configInput(CV12_INPUT, "");
		configInput(CV13_INPUT, "");
		configInput(CV14_INPUT, "");
		configInput(CV15_INPUT, "");
		configInput(IN1_INPUT, "DC");
		configInput(IN2_INPUT, "Transistor");
		configInput(IN3_INPUT, "");
		configInput(IN4_INPUT, "");
		configInput(IN5_INPUT, "");
		configOutput(OUT1_OUTPUT, "DC");
		configOutput(OUT2_OUTPUT, "Transistor");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");

		configBypass(IN1_INPUT, OUT1_OUTPUT);
		configBypass(IN2_INPUT, OUT2_OUTPUT);
	}

	float input_0 = {0.f};
	float input_1 = {0.f};
	float output_0 = {0.f};
	float output_1 = {0.f};
	float R = {0.f};
	float cutoff = {0.f};
	float sample_rate = {0.f};
	
	float input_0_B = {0.f};
	float gain = {1.f};

	void process(const ProcessArgs& args) override {
		
		sample_rate = args.sampleRate;
		cutoff = params[PARAM1_PARAM].getValue();
		R = 1.f - (M_PI * 2.f * cutoff / sample_rate);

		input_0 = inputs[IN1_INPUT].getVoltageSum();
		output_0 = input_0 - input_1 + R * output_1;

		
		outputs[OUT1_OUTPUT].setVoltage(output_0);

		output_1 = output_0;
		input_1 = input_0;
		input_0_B = inputs[IN2_INPUT].getVoltageSum() * 0.2f;
		gain = params[PARAM2_PARAM].getValue();
		input_0_B = Transistor(input_0_B, gain);
		float output = input_0_B;
		outputs[OUT2_OUTPUT].setVoltage(output);
		

	}
};


struct PROTO6Widget : ModuleWidget {
	PROTO6Widget(PROTO6* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PROTO6.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 12.85)), module, PROTO6::PARAM1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 12.85)), module, PROTO6::PARAM2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 12.85)), module, PROTO6::PARAM3_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 12.85)), module, PROTO6::PARAM4_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 12.85)), module, PROTO6::PARAM5_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 38.55)), module, PROTO6::PARAM6_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 38.55)), module, PROTO6::PARAM7_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 38.55)), module, PROTO6::PARAM8_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 38.55)), module, PROTO6::PARAM9_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 38.55)), module, PROTO6::PARAM10_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 64.25)), module, PROTO6::PARAM11_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 64.25)), module, PROTO6::PARAM12_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 64.25)), module, PROTO6::PARAM13_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 64.25)), module, PROTO6::PARAM14_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 64.25)), module, PROTO6::PARAM15_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 77.1)), module, PROTO6::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 77.1)), module, PROTO6::CV2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 77.1)), module, PROTO6::CV3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 77.1)), module, PROTO6::CV4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 77.1)), module, PROTO6::CV5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 86.171)), module, PROTO6::CV6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 86.171)), module, PROTO6::CV7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 86.171)), module, PROTO6::CV8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 86.171)), module, PROTO6::CV9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 86.171)), module, PROTO6::CV10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 95.242)), module, PROTO6::CV11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 95.242)), module, PROTO6::CV12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 95.242)), module, PROTO6::CV13_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 95.242)), module, PROTO6::CV14_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 95.242)), module, PROTO6::CV15_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 107.562)), module, PROTO6::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 107.562)), module, PROTO6::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 107.562)), module, PROTO6::IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 107.562)), module, PROTO6::IN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 107.562)), module, PROTO6::IN5_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.208, 118.296)), module, PROTO6::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.624, 118.296)), module, PROTO6::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 118.296)), module, PROTO6::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.456, 118.296)), module, PROTO6::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118.872, 118.296)), module, PROTO6::OUT5_OUTPUT));
	}
};


Model* modelPROTO6 = createModel<PROTO6, PROTO6Widget>("PROTO6");