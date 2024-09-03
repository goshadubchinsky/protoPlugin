#include "plugin.hpp"
#include "Neutron/vco/LabSeven_3340_VCO.h"
#include <time.h>
#include "chowdsp/shared/VariableOversampling.hpp"
#include <fstream>
using namespace std;

template <typename T>
static T tanh_Pade(T x) {
	// return std::tanh(x);
	// Pade approximant of tanh
	x = simd::clamp(x, -3.f, 3.f);
	return x * (27 + x * x) / (27 + 9 * x * x);
}

struct CEM3340 : Module {
	enum ParamId {
		PARAM_MOD,
		PARAM_RANGE,
		PARAM_PULSEWIDTH,
		PARAM_PWMSOURCE,
		PARAM_VOLSQUARE,
		PARAM_VOLSAW,
		PARAM_VOLTRIANGLE,
		PARAM_VOLSUBOSC,
		PARAM_SUBOSCRATIO,
		PARAM_VOLNOISE,
		PARAM_VOLSINE,
		//PARAM12_PARAM,
		//PARAM13_PARAM,
		//PARAM14_PARAM,
		//PARAM15_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_PITCH,
		IN_MOD,
		IN_RANGE,
		IN_LFO,
		IN_ENV,
		IN_SUBOSCSELECT,
		//CV7_INPUT,
		//CV8_INPUT,
		//CV9_INPUT,
		//CV10_INPUT,
		//CV11_INPUT,
		//CV12_INPUT,
		//CV13_INPUT,
		//CV14_INPUT,
		//CV15_INPUT,
		//IN1_INPUT,
		//IN2_INPUT,
		//IN3_INPUT,
		//IN4_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_SIN,
		OUT_NOISE,
		OUT_SQUARE,
		OUT_SAW,
		OUT_SUB,
		OUT_TRIANGLE,
		OUT_MIX,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	//VCO instance and VCO output frame
    LabSeven::LS3340::TLS3340VCO vco;
    LabSeven::LS3340::TLS3340VCOFrame nextFrame;

	CEM3340() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM_MOD, 0.f, 1.f, 0.f, "MOD");
		configParam(PARAM_RANGE, 0.f, 3.f, 1.f, "RANGE");
			paramQuantities[PARAM_RANGE]->snapEnabled = true;
		configParam(PARAM_PULSEWIDTH, 0.f, 0.5f, 0.f, "PULSEWIDTH");
		configParam(PARAM_PWMSOURCE, 0.f, 2.f, 1.f, "PWMSOURCE");
		configParam(PARAM_VOLSQUARE, 0.f, 1.f, 0.f, "VOLSQUARE");
		configParam(PARAM_VOLSAW, 0.f, 1.f, 0.f, "VOLSAW");
		configParam(PARAM_VOLTRIANGLE, 0.f, 1.f, 0.f, "VOLTRIANGLE");
		configParam(PARAM_VOLSUBOSC, 0.f, 1.f, 0.f, "VOLSUBOSC");
		configParam(PARAM_SUBOSCRATIO, 0.f, 2.f, 2.f, "SUBOSCRATIO");
		configParam(PARAM_VOLNOISE, 0.f, 1.f, 0.f, "VOLNOISE");
		configParam(PARAM_VOLSINE, 0.f, 1.f, 0.f, "VOLSINE");
		//configParam(PARAM12_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM13_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM14_PARAM, 0.f, 1.f, 0.f, "");
		//configParam(PARAM15_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_PITCH, "PITCH cv");
		configInput(IN_MOD, "MOD cv");
		configInput(IN_RANGE, "RANGE cv");
		configInput(IN_LFO, "LFO cv");
		configInput(IN_ENV, "ENV cv");
		configInput(IN_SUBOSCSELECT, "SUBOSCSELECT");
		//configInput(CV7_INPUT, "");
		//configInput(CV8_INPUT, "");
		//configInput(CV9_INPUT, "");
		//configInput(CV10_INPUT, "");
		//configInput(CV11_INPUT, "");
		//configInput(CV12_INPUT, "");
		//configInput(CV13_INPUT, "");
		//configInput(CV14_INPUT, "");
		//configInput(CV15_INPUT, "");
		//configInput(IN1_INPUT, "");
		//configInput(IN2_INPUT, "");
		//configInput(IN3_INPUT, "");
		configOutput(OUT_SIN, "SIN");
		configOutput(OUT_NOISE, "NOISE");
		configOutput(OUT_SQUARE, "SQUARE");
		configOutput(OUT_SAW, "SAW");
		configOutput(OUT_SUB, "SUB");
		configOutput(OUT_TRIANGLE, "TRIANGLE");
		configOutput(OUT_MIX, "MIX");

		oversample.setOversamplingIndex(2); // default 4x oversampling
		onSampleRateChange();
	}

	float sampleTimeCurrent = 0.0;
    float sampleRateCurrent = 0.0;
    double pitch,maxPitch,rangeFactor;
	float square;
	float saw;
	float triangle;
	float sine;
	float subosc;
	float noise;
	float mix;

	void onSampleRateChange() override {
        float newSampleRate = getSampleRate();
        oversample.reset(newSampleRate);
    }

	void onReset() override {
        Module::onReset();
        oversample.reset(getSampleRate());
    }

	void process(const ProcessArgs& args) override {

		//update external sample rate if neccessary
    	if (sampleTimeCurrent != args.sampleTime)
    	{
    	    sampleTimeCurrent = args.sampleTime;
    	    sampleRateCurrent = 1.0/sampleTimeCurrent;
    	    vco.setSamplerateExternal(sampleRateCurrent);

			maxPitch = sampleRateCurrent*0.45;
    		if (maxPitch > 40000) maxPitch = 40000; //high value so that suboscillator can go up to 10kHz
    	}

		//get pitch and pitch mod input
    	pitch = inputs[IN_PITCH].getVoltage();
    	pitch +=  pow(2,2.25*0.2*inputs[IN_MOD].getVoltage() * params[PARAM_MOD].getValue());

    	//set rangeFactor
		rangeFactor = params[PARAM_RANGE].getValue();
		switch ((int)rangeFactor)
		{
			case 0: rangeFactor = 0.5; break;
			case 1: rangeFactor = 1.0; break;
			case 2: rangeFactor = 2.0; break;
			case 3: rangeFactor = 4.0; break;
			default: rangeFactor = 1.0;
		}
		
		//range modulation
    	if (inputs[IN_RANGE].active)
    	{
    	    int rangeSelect = (int)round(inputs[IN_RANGE].getVoltage()*0.6);
    	    switch (rangeSelect)
    	    {
    	        case -3: rangeFactor /= 8.0; break;
    	        case -2: rangeFactor /= 4.0; break;
    	        case -1: rangeFactor /= 2.0; break;
    	        case  0: break; //no change
    	        case  1: rangeFactor *= 2.0; break;
    	        case  2: rangeFactor *= 4.0; break;
    	        case  3: rangeFactor *= 8.0; break;
    	    }
    	    if (rangeFactor > 16.0) rangeFactor = 16.0;
    	}

		//set pitch
    	//TODO: Clean up this paragraph!!!
		pitch = 261.626f * pow(2.0, pitch) * rangeFactor;
    	pitch = clamp(pitch, 0.01f, maxPitch);
    	//simulate the jitter observed in the hardware synth
    	//use values > 0.02 for dirtier sound
    	pitch *= 1.0+0.02*((double) rand() / (RAND_MAX)-0.5);
    	vco.setFrequency(pitch);

		//update suboscillator
    	switch((int)inputs[IN_SUBOSCSELECT].getVoltage())
    	{
    	    case 1: vco.setSuboscillatorMode(0); break;
    	    case 2: vco.setSuboscillatorMode(1); break;
    	    case 3: vco.setSuboscillatorMode(2); break;
    	    default: vco.setSuboscillatorMode(2 - (unsigned short)params[PARAM_SUBOSCRATIO].getValue());
    	}

		//pulse width modulation
    	switch ((int)params[PARAM_PWMSOURCE].getValue())
		{
    	    //LFO PWM requires values between -0.4 and 0.4; SH does PWM between 10% and 90% pulse width
    	    case 2:  vco.setPwmCoefficient(-2.0*params[PARAM_PULSEWIDTH].getValue()*0.4*0.2*inputs[IN_LFO].getVoltage()); break; //bipolar, -5V - +5V
    	    case 1:  vco.setPwmCoefficient(-0.8*params[PARAM_PULSEWIDTH].getValue()); break;
    	    case 0:  vco.setPwmCoefficient(-2.0*params[PARAM_PULSEWIDTH].getValue()*0.4*0.1*inputs[IN_ENV].getVoltage()); break; //unipolar, 0V - +10v
    	    default: vco.setPwmCoefficient(-0.8*params[PARAM_PULSEWIDTH].getValue());
		}

		//get next frame and put it out
	    double scaling = 8.0;

	    //TODO: PROPER (FREQUENCY DEPENDENT) AMPLITUDE SCALING FOR SAW AND TRIANGLE
	    //TODO: PWM FOR TRIANGLE

	    //calculate next frame

	    if (this->sampleRateCurrent != 192000)
	    {
	        //TODO: Add a 'standard/high' quality switch to GUI
	        //and choose interpolation method accordingly
	        if (true)
	        {
	            vco.getNextFrameAtExternalSampleRateSinc(&nextFrame);
	        }
	        else
	        {
	            //currently next neighbour interpolation, not used!
	            //TODO: Add quality switch (low/medium/high) to select
	            //nn (has its own sound), dsp::cubic or sinc interpolation
	            vco.getNextFrameAtExternalSampleRateCubic(&nextFrame);
	        }
	    }
	    else //no interpolation required if internal sample rate == external sample rate == 192kHz
	    {
	        vco.getNextBlock(&nextFrame,1);
	    }


		square		= scaling * nextFrame.square;
		saw			= scaling * nextFrame.sawtooth;
		triangle	= scaling * nextFrame.triangle;

		sine = 0.2 * triangle;
		oversample.upsample(sine);
		float* osBuffer = oversample.getOSBuffer();
		float gain = 2.f;
		gain = clamp(gain, 0.01f, 10.f);

		for(int k = 0; k < oversample.getOversamplingRatio(); k++)
            {osBuffer[k] = (float) tanh_Pade(osBuffer[k] * gain);}
        sine = 5.f * oversample.downsample();

		subosc		= scaling * nextFrame.subosc;
		noise		= scaling * 6.0 * nextFrame.noise;
		mix			= 0.4 * (	(square * params[PARAM_VOLSQUARE].getValue()) +
								(saw * params[PARAM_VOLSAW].getValue()) +
								(triangle * params[PARAM_VOLTRIANGLE].getValue()) +
								(subosc * params[PARAM_VOLSUBOSC].getValue()) +
								(sine * params[PARAM_VOLSINE].getValue()) +
								(noise * params[PARAM_VOLNOISE].getValue())
							);
		//TODO: Activate/deactivate interpolation if outs are not active
    	outputs[OUT_SQUARE].setVoltage(square);
    	outputs[OUT_SAW].setVoltage(saw);
    	outputs[OUT_SUB].setVoltage(subosc);
    	outputs[OUT_TRIANGLE].setVoltage(triangle);
    	outputs[OUT_SIN].setVoltage(sine);
    	outputs[OUT_NOISE].setVoltage(noise);
    	outputs[OUT_MIX].setVoltage(mix);

	}

	VariableOversampling<> oversample;

private:

};


struct CEM3340Widget : ModuleWidget {
	CEM3340Widget(CEM3340* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/CEM3340.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 12.85)), module, CEM3340::PARAM_MOD));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 12.85)), module, CEM3340::PARAM_RANGE));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 12.85)), module, CEM3340::PARAM_PULSEWIDTH));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 12.85)), module, CEM3340::PARAM_PWMSOURCE));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 12.85)), module, CEM3340::PARAM_VOLSQUARE));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 38.55)), module, CEM3340::PARAM_VOLSAW));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 38.55)), module, CEM3340::PARAM_VOLSUBOSC));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 38.55)), module, CEM3340::PARAM_VOLTRIANGLE));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 38.55)), module, CEM3340::PARAM_SUBOSCRATIO));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 38.55)), module, CEM3340::PARAM_VOLNOISE));
		addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(13.208, 64.25)), module, CEM3340::PARAM_VOLSINE));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(39.624, 64.25)), module, CEM3340::PARAM12_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(66.04, 64.25)), module, CEM3340::PARAM13_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(92.456, 64.25)), module, CEM3340::PARAM14_PARAM));
		//addParam(createParamCentered<WhiteKnob15>(mm2px(Vec(118.872, 64.25)), module, CEM3340::PARAM15_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 77.1)), module, CEM3340::IN_PITCH));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 77.1)), module, CEM3340::IN_MOD));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 77.1)), module, CEM3340::IN_RANGE));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 77.1)), module, CEM3340::IN_LFO));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 77.1)), module, CEM3340::IN_ENV));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 86.171)), module, CEM3340::IN_SUBOSCSELECT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 86.171)), module, CEM3340::CV7_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 86.171)), module, CEM3340::CV8_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 86.171)), module, CEM3340::CV9_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 86.171)), module, CEM3340::CV10_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 95.242)), module, CEM3340::CV11_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 95.242)), module, CEM3340::CV12_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 95.242)), module, CEM3340::CV13_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.456, 95.242)), module, CEM3340::CV14_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.872, 95.242)), module, CEM3340::CV15_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.208, 107.562)), module, CEM3340::IN1_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.624, 107.562)), module, CEM3340::IN2_INPUT));
		//addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 107.562)), module, CEM3340::IN3_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.456, 107.562)), module, CEM3340::OUT_SIN));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118.872, 107.562)), module, CEM3340::OUT_NOISE));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.208, 118.296)), module, CEM3340::OUT_SQUARE));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.624, 118.296)), module, CEM3340::OUT_SAW));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 118.296)), module, CEM3340::OUT_SUB));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.456, 118.296)), module, CEM3340::OUT_TRIANGLE));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118.872, 118.296)), module, CEM3340::OUT_MIX));
	}

	void appendContextMenu(Menu *menu) override {
        menu->addChild(new MenuSeparator());
        dynamic_cast<CEM3340*> (module)->oversample.addContextMenu(menu, module);
    }

};


Model* modelCEM3340 = createModel<CEM3340, CEM3340Widget>("CEM3340");