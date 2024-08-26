#include "plugin.hpp"


struct Neutron : Module {
	enum ParamId {
		FREQ_PARAM,
		LFO_RATE_PARAM,
		TIME_PARAM,
		REPEATS_PARAM,
		MIX_PARAM,
		VOLUME_PARAM,
		OSC_MIX_PARAM,
		TUNE_1_PARAM,
		TUNE_2_PARAM,
		LFO_SHAPE_PARAM,
		RESO_PARAM,
		DRIVE_PARAM,
		TONE_PARAM,
		LEVEL_PARAM,
		SH_RATE_PARAM,
		SH_GLIDE_PARAM,
		SHAPE_1_PARAM,
		SHAPE_2_PARAM,
		MOD_DEPTH_PARAM,
		NOISE_PARAM,
		ATTACK_1_PARAM,
		DECAY_1_PARAM,
		SUSTAIN_1_PARAM,
		RELEASE_1_PARAM,
		SLEW_PARAM,
		PORTA_TIME_PARAM,
		WIDTH_1_PARAM,
		WIDTH_2_PARAM,
		ENV_DEPTH_PARAM,
		VCA_BIAS_PARAM,
		ATTACK_2_PARAM,
		DECAY_2_PARAM,
		SUSTAIN_2_PARAM,
		RELEASE_2_PARAM,
		ATT_1_PARAM,
		ATT_2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		OSC_1_INPUT,
		OSC_2_INPUT,
		OSC_BOTH_INPUT,
		INVERT_IN_INPUT,
		SHAPE_1_INPUT,
		SHAPE_2_INPUT,
		PW_1_INPUT,
		PW_2_INPUT,
		VCF_IN_INPUT,
		FREQ_MOD_INPUT,
		RES_INPUT,
		OD_IN_INPUT,
		VCA_IN_INPUT,
		VCA_CV_INPUT,
		DELAY_IN_INPUT,
		DELAY_TIME_INPUT,
		E_GATE_1_INPUT,
		E_GATE_2_INPUT,
		SH_IN_INPUT,
		SH_CLOCK_INPUT,
		LFO_RATE_INPUT,
		LFO_SHAPE_INPUT,
		LFO_TRIG_INPUT,
		MULT_INPUT,
		ATT_1_IN_INPUT,
		ATT_1_CV_INPUT,
		ATT_2_IN_INPUT,
		SLEW_IN_INPUT,
		SUM_1_A_INPUT,
		SUM_1_B_INPUT,
		SUM_2_A_INPUT,
		SUM_2_B_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OSC_1_OUTPUT,
		OSC_2_OUTPUT,
		OSC_MIX_OUTPUT,
		VCF_1_OUTPUT,
		VCF_2_OUTPUT,
		OVERDRIVE_OUTPUT,
		VCA_OUTPUT,
		OUTPUT_OUTPUT,
		NOISE_OUTPUT,
		ENV_1_OUTPUT,
		ENV_2_OUTPUT,
		INVERT_OUTPUT,
		LFO_OUTPUT,
		LFO_UNI_OUTPUT,
		SH_OUTPUT,
		MULT_1_OUTPUT,
		MULT_2_OUTPUT,
		MIDI_GATE_OUTPUT,
		ATT_1_OUTPUT,
		ATT_2_OUTPUT,
		SLEW_OUTPUT,
		SUM_1_OUTPUT,
		SUM_2_OUTPUT,
		ASSIGN_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		FILTER_MODE_HP_LIGHT,
		LFO_SHAPE_SAW_DOWN_LIGHT,
		FILTER_MODE_BP_LIGHT,
		LFO_SHAPE_TRI_LIGHT,
		LFO_SHAPE_SQUARE_LIGHT,
		FILTER_MODE_LP_LIGHT,
		OSC_1_RANGE_8_LIGHT,
		OSC_2_RANGE_8_LIGHT,
		LFO_SHAPE_SINE_LIGHT,
		LFO_SHAPE_SAW_UP_LIGHT,
		OSC_1_PULSE_A_LIGHT,
		OSC_1_RANGE_16_LIGHT,
		OSC_2_RANGE_16_LIGHT,
		OSC_2_PULSE_A_LIGHT,
		OSC_1_PULSE_B_LIGHT,
		OSC_1_RANGE_32_LIGHT,
		OSC_2_RANGE_32_LIGHT,
		OSC_2_PULSE_B_LIGHT,
		OSC_1_SAW_LIGHT,
		OSC_2_SAW_LIGHT,
		OSC_1_TRI_LIGHT,
		OSC_2_TRI_LIGHT,
		OSC_1_SINE_LIGHT,
		OSC_2_SINE_LIGHT,
		LIGHTS_LEN
	};

	Neutron() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LFO_RATE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TIME_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_PARAM, 0.f, 1.f, 0.f, "");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.f, "");
		configParam(OSC_MIX_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TUNE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TUNE_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LFO_SHAPE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RESO_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DRIVE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TONE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LEVEL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SH_RATE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SH_GLIDE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SHAPE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SHAPE_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MOD_DEPTH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NOISE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATTACK_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DECAY_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SUSTAIN_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RELEASE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLEW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PORTA_TIME_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WIDTH_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WIDTH_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ENV_DEPTH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(VCA_BIAS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATTACK_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DECAY_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SUSTAIN_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RELEASE_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATT_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATT_2_PARAM, 0.f, 1.f, 0.f, "");
		configInput(OSC_1_INPUT, "");
		configInput(OSC_2_INPUT, "");
		configInput(OSC_BOTH_INPUT, "");
		configInput(INVERT_IN_INPUT, "");
		configInput(SHAPE_1_INPUT, "");
		configInput(SHAPE_2_INPUT, "");
		configInput(PW_1_INPUT, "");
		configInput(PW_2_INPUT, "");
		configInput(VCF_IN_INPUT, "");
		configInput(FREQ_MOD_INPUT, "");
		configInput(RES_INPUT, "");
		configInput(OD_IN_INPUT, "");
		configInput(VCA_IN_INPUT, "");
		configInput(VCA_CV_INPUT, "");
		configInput(DELAY_IN_INPUT, "");
		configInput(DELAY_TIME_INPUT, "");
		configInput(E_GATE_1_INPUT, "");
		configInput(E_GATE_2_INPUT, "");
		configInput(SH_IN_INPUT, "");
		configInput(SH_CLOCK_INPUT, "");
		configInput(LFO_RATE_INPUT, "");
		configInput(LFO_SHAPE_INPUT, "");
		configInput(LFO_TRIG_INPUT, "");
		configInput(MULT_INPUT, "");
		configInput(ATT_1_IN_INPUT, "");
		configInput(ATT_1_CV_INPUT, "");
		configInput(ATT_2_IN_INPUT, "");
		configInput(SLEW_IN_INPUT, "");
		configInput(SUM_1_A_INPUT, "");
		configInput(SUM_1_B_INPUT, "");
		configInput(SUM_2_A_INPUT, "");
		configInput(SUM_2_B_INPUT, "");
		configOutput(OSC_1_OUTPUT, "");
		configOutput(OSC_2_OUTPUT, "");
		configOutput(OSC_MIX_OUTPUT, "");
		configOutput(VCF_1_OUTPUT, "");
		configOutput(VCF_2_OUTPUT, "");
		configOutput(OVERDRIVE_OUTPUT, "");
		configOutput(VCA_OUTPUT, "");
		configOutput(OUTPUT_OUTPUT, "");
		configOutput(NOISE_OUTPUT, "");
		configOutput(ENV_1_OUTPUT, "");
		configOutput(ENV_2_OUTPUT, "");
		configOutput(INVERT_OUTPUT, "");
		configOutput(LFO_OUTPUT, "");
		configOutput(LFO_UNI_OUTPUT, "");
		configOutput(SH_OUTPUT, "");
		configOutput(MULT_1_OUTPUT, "");
		configOutput(MULT_2_OUTPUT, "");
		configOutput(MIDI_GATE_OUTPUT, "");
		configOutput(ATT_1_OUTPUT, "");
		configOutput(ATT_2_OUTPUT, "");
		configOutput(SLEW_OUTPUT, "");
		configOutput(SUM_1_OUTPUT, "");
		configOutput(SUM_2_OUTPUT, "");
		configOutput(ASSIGN_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct NeutronWidget : ModuleWidget {
	NeutronWidget(Neutron* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Neutron/Neutron_panel.svg")));

		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH * 31, 0)));
		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH * 48, 0)));
		addChild(createWidget<ScrewBlue>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH * 31, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlue>(Vec(RACK_GRID_WIDTH * 48, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlue>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(114.5, 19.0)), module, Neutron::FREQ_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(165.0, 19.0)), module, Neutron::LFO_RATE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(190.5, 19.0)), module, Neutron::TIME_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(216.0, 19.0)), module, Neutron::REPEATS_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(241.5, 19.0)), module, Neutron::MIX_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(267.0, 19.0)), module, Neutron::VOLUME_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(51.5, 30.0)), module, Neutron::OSC_MIX_PARAM));
		addParam(createParamCentered<Neutron_Knob_Large>(mm2px(Vec(27.051, 44.051)), module, Neutron::TUNE_1_PARAM));
		addParam(createParamCentered<Neutron_Knob_Large>(mm2px(Vec(76.0, 44.0)), module, Neutron::TUNE_2_PARAM));
		addParam(createParamCentered<Neutron_Knob_Medium>(mm2px(Vec(152.487, 45.513)), module, Neutron::LFO_SHAPE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(114.5, 49.0)), module, Neutron::RESO_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(190.5, 49.0)), module, Neutron::DRIVE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(216.0, 49.0)), module, Neutron::TONE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(241.5, 49.0)), module, Neutron::LEVEL_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(267.0, 49.0)), module, Neutron::SH_RATE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(292.5, 49.0)), module, Neutron::SH_GLIDE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(27.0, 79.0)), module, Neutron::SHAPE_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(76.0, 79.0)), module, Neutron::SHAPE_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(114.5, 79.0)), module, Neutron::MOD_DEPTH_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(140.0, 79.0)), module, Neutron::NOISE_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(165.0, 79.0)), module, Neutron::ATTACK_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(190.5, 79.0)), module, Neutron::DECAY_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(216.0, 79.0)), module, Neutron::SUSTAIN_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(241.5, 79.0)), module, Neutron::RELEASE_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(267.0, 79.0)), module, Neutron::SLEW_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(292.5, 79.0)), module, Neutron::PORTA_TIME_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(27.0, 109.0)), module, Neutron::WIDTH_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(76.0, 109.0)), module, Neutron::WIDTH_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(114.5, 109.0)), module, Neutron::ENV_DEPTH_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(140.0, 109.0)), module, Neutron::VCA_BIAS_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(165.0, 109.0)), module, Neutron::ATTACK_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(190.5, 109.0)), module, Neutron::DECAY_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(216.0, 109.0)), module, Neutron::SUSTAIN_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(241.5, 109.0)), module, Neutron::RELEASE_2_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(267.0, 109.0)), module, Neutron::ATT_1_PARAM));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(292.5, 109.0)), module, Neutron::ATT_2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 17.9)), module, Neutron::OSC_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 17.9)), module, Neutron::OSC_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 17.9)), module, Neutron::OSC_BOTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 17.9)), module, Neutron::INVERT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 31.229)), module, Neutron::SHAPE_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 31.229)), module, Neutron::SHAPE_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 31.229)), module, Neutron::PW_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 31.229)), module, Neutron::PW_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 44.557)), module, Neutron::VCF_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 44.557)), module, Neutron::FREQ_MOD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 44.557)), module, Neutron::RES_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 44.557)), module, Neutron::OD_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 57.886)), module, Neutron::VCA_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 57.886)), module, Neutron::VCA_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 57.886)), module, Neutron::DELAY_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 57.886)), module, Neutron::DELAY_TIME_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 71.214)), module, Neutron::E_GATE_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 71.214)), module, Neutron::E_GATE_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 71.214)), module, Neutron::SH_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 71.214)), module, Neutron::SH_CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 84.543)), module, Neutron::LFO_RATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 84.543)), module, Neutron::LFO_SHAPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 84.543)), module, Neutron::LFO_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 84.543)), module, Neutron::MULT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 97.871)), module, Neutron::ATT_1_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 97.871)), module, Neutron::ATT_1_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 97.871)), module, Neutron::ATT_2_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 97.871)), module, Neutron::SLEW_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(313.3, 111.2)), module, Neutron::SUM_1_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(327.25, 111.2)), module, Neutron::SUM_1_B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(341.2, 111.2)), module, Neutron::SUM_2_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(355.15, 111.2)), module, Neutron::SUM_2_B_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 17.9)), module, Neutron::OSC_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 17.9)), module, Neutron::OSC_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 17.9)), module, Neutron::OSC_MIX_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 31.229)), module, Neutron::VCF_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 31.229)), module, Neutron::VCF_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 31.229)), module, Neutron::OVERDRIVE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 44.557)), module, Neutron::VCA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 44.557)), module, Neutron::OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 44.557)), module, Neutron::NOISE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 57.886)), module, Neutron::ENV_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 57.886)), module, Neutron::ENV_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 57.886)), module, Neutron::INVERT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 71.214)), module, Neutron::LFO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 71.214)), module, Neutron::LFO_UNI_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 71.214)), module, Neutron::SH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 84.543)), module, Neutron::MULT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 84.543)), module, Neutron::MULT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 84.543)), module, Neutron::MIDI_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 97.871)), module, Neutron::ATT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 97.871)), module, Neutron::ATT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 97.871)), module, Neutron::SLEW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(369.1, 111.2)), module, Neutron::SUM_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(383.05, 111.2)), module, Neutron::SUM_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(397.0, 111.2)), module, Neutron::ASSIGN_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(98.3, 30.5)), module, Neutron::FILTER_MODE_HP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(152.5, 32.0)), module, Neutron::LFO_SHAPE_SAW_DOWN_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(98.3, 36.8)), module, Neutron::FILTER_MODE_BP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(139.46, 42.006)), module, Neutron::LFO_SHAPE_TRI_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(165.54, 42.006)), module, Neutron::LFO_SHAPE_SQUARE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(98.3, 43.1)), module, Neutron::FILTER_MODE_LP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.3, 56.4)), module, Neutron::OSC_1_RANGE_8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.7, 56.4)), module, Neutron::OSC_2_RANGE_8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(145.75, 57.192)), module, Neutron::LFO_SHAPE_SINE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(159.25, 57.192)), module, Neutron::LFO_SHAPE_SAW_UP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.0, 64.0)), module, Neutron::OSC_1_PULSE_A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.3, 64.0)), module, Neutron::OSC_1_RANGE_16_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.7, 64.0)), module, Neutron::OSC_2_RANGE_16_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(90.0, 64.0)), module, Neutron::OSC_2_PULSE_A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.0, 71.6)), module, Neutron::OSC_1_PULSE_B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.3, 71.6)), module, Neutron::OSC_1_RANGE_32_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.7, 71.6)), module, Neutron::OSC_2_RANGE_32_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(90.0, 71.6)), module, Neutron::OSC_2_PULSE_B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.0, 79.2)), module, Neutron::OSC_1_SAW_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(90.0, 79.2)), module, Neutron::OSC_2_SAW_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.0, 86.8)), module, Neutron::OSC_1_TRI_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(90.0, 86.8)), module, Neutron::OSC_2_TRI_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.0, 94.4)), module, Neutron::OSC_1_SINE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(90.0, 94.4)), module, Neutron::OSC_2_SINE_LIGHT));

		addChild(createWidgetCentered<Widget>(mm2px(Vec(292.5, 19.0))));
	}
};


Model* modelNeutron = createModel<Neutron, NeutronWidget>("Neutron");