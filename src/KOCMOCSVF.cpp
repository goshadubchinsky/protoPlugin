#include "plugin.hpp"
#include "Neutron/vcf/Neutron_svf.h"



struct KOCMOCSVF : Module {
	enum ParamId {
		
		FREQ_PARAM,
		RES_PARAM,
		LIN_PARAM,
		EXP_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		
		LIN_INPUT,
		EXP_INPUT,
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		
		LP_OUTPUT,
		BP_OUTPUT,
		HP_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	NeutronSVF svf;

	KOCMOCSVF() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Cutoff");
		configParam(RES_PARAM, 0.f, 1.f, 0.f, "Resonance");
		configParam(LIN_PARAM, -1.f, 1.f, 0.f, "Linear");
		configParam(EXP_PARAM, -1.f, 1.f, 0.f, "Exponential");

		configInput(LIN_INPUT, "Linear");
		configInput(EXP_INPUT, "Exponential");
		configInput(IN1_INPUT, "Input");

		configOutput(LP_OUTPUT, "LP");
		configOutput(BP_OUTPUT, "BP");
		configOutput(HP_OUTPUT, "HP");
		
		configBypass(IN1_INPUT, LP_OUTPUT);
		configBypass(IN1_INPUT, BP_OUTPUT);
		configBypass(IN1_INPUT, HP_OUTPUT);
		
	}

	float input = {0.f};
	float cutoff = {0.f};
    float lincv_atten = {0.f};
    float expcv_atten = {0.f};
    float reso = {0.f};

	void process(const ProcessArgs& args) override {

		if (outputs[LP_OUTPUT].isConnected() || outputs[BP_OUTPUT].isConnected() || outputs[HP_OUTPUT].isConnected())
		{
			// Inputs
			input = inputs[IN1_INPUT].getVoltageSum();
	
			// Params
			cutoff = params[FREQ_PARAM].getValue();
			lincv_atten = params[LIN_PARAM].getValue();
			expcv_atten = params[EXP_PARAM].getValue();
			reso = params[RES_PARAM].getValue();
	
			// Shape Panel Input For A Pseudoexponential Response
			cutoff = 0.001 + 2.25 * (cutoff * cutoff * cutoff * cutoff);
			lincv_atten *= lincv_atten * lincv_atten;
			expcv_atten *= expcv_atten * expcv_atten;
	
			// Linear CV
			//channelCutoff += lincv_atten*inputs[LINCV_INPUT].getVoltage(ii)/10.f;
			cutoff += lincv_atten * inputs[LIN_INPUT].getVoltage() / 10.f;
	
			// Exponential CV
			//channelCutoff = channelCutoff * std::pow(2.f, expcv_atten*inputs[EXPCV_INPUT].getVoltage(ii));
			cutoff = cutoff * std::pow(2.f, expcv_atten * inputs[EXP_INPUT].getVoltage());
	
			// Set Filter Parameters
			svf.SetFilterCutoff((double)(cutoff));
			svf.SetFilterResonance((double)(reso));

			// Tick Filter State
			svf.filter((double)(input));

			outputs[LP_OUTPUT].setVoltage((float)svf.GetFilterLowpass());
			outputs[BP_OUTPUT].setVoltage((float)svf.GetFilterBandpass());
			outputs[HP_OUTPUT].setVoltage((float)svf.GetFilterHighpass());
		}
	}

	void onSampleRateChange() override
	{
		float sr = APP->engine->getSampleRate();
		svf.SetFilterSampleRate(sr);
	}

	void onReset() override
	{
		float sr = APP->engine->getSampleRate();
		svf.ResetFilterState();
		svf.SetFilterSampleRate(sr);
		svf.SetFilterCutoff((double)(0.25f));
		svf.SetFilterResonance((double)(0.f));
	}

	void onAdd() override
	{
		float sr = APP->engine->getSampleRate();
		svf.ResetFilterState();
		svf.SetFilterSampleRate(sr);
		svf.SetFilterCutoff((double)(0.25f));
		svf.SetFilterResonance((double)(0.f));
	}
	
};


struct KOCMOCSVFWidget : ModuleWidget {
	KOCMOCSVFWidget(KOCMOCSVF* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/KOCMOCSVF.svg")));


		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 12.85)), module, KOCMOCSVF::FREQ_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 38.55)), module, KOCMOCSVF::RES_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 64.25)), module, KOCMOCSVF::LIN_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 64.25)), module, KOCMOCSVF::EXP_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 77.1)), module, KOCMOCSVF::LIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 77.1)), module, KOCMOCSVF::EXP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 107.562)), module, KOCMOCSVF::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.208, 118.296)), module, KOCMOCSVF::LP_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.624, 118.296)), module, KOCMOCSVF::BP_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 118.296)), module, KOCMOCSVF::HP_OUTPUT));
	}
};


Model* modelKOCMOCSVF = createModel<KOCMOCSVF, KOCMOCSVFWidget>("KOCMOCSVF");