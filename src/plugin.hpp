#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelM102;
//extern Model* modelM102XSIMD;
extern Model* modelPROTO6;
extern Model* modelCEM3340;
extern Model* modelNeutron;
extern Model* modelKOCMOCSVF;
extern Model* modelM102XSIMD2;


inline float getSampleRate() noexcept {
// in benchmarking app, sample rate is not defined
    return APP->engine->getSampleRate();

}


////

struct ScrewBlue : SvgScrew {
	ScrewBlue() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewBlue.svg")));
	}
};

struct Neutron_Knob_Large : RoundKnob {
    Neutron_Knob_Large() {
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Large.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Large_bg.svg")));
        /// shadow->opacity = 0;
    }
};

struct Neutron_Knob_Medium : RoundKnob {
    Neutron_Knob_Medium() {
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Medium.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Medium_bg.svg")));
        /// shadow->opacity = 0;
    }
};

struct Neutron_Knob_Small : RoundKnob {
    Neutron_Knob_Small() {
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Small.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Neutron/Neutron_Knob_Small_bg.svg")));
        /// shadow->opacity = 0;
    }
};


struct WhiteKnobL : RoundKnob {
    WhiteKnobL() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnobL.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnobL_bg.svg")));
        /// shadow->opacity = 0;
    }
};

struct WhiteKnob10 : RoundKnob {
    WhiteKnob10() {
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob10.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob10_bg.svg")));
        /// shadow->opacity = 0;
    }
};

/// min angle half Pi max angle half Pi
struct WhiteKnob10a : RoundKnob {
    WhiteKnob10a() {
        minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob10.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob10_bg.svg")));
        /// shadow->opacity = 0;
    }
};

struct WhiteKnob15 : RoundKnob {
    WhiteKnob15() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob15.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/WhiteKnob15_bg.svg")));
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        /// shadow->opacity = 0;
    }
};

struct ScrewKnobBlue : RoundKnob {
    ScrewKnobBlue() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewKnobBlue.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/ScrewKnobBlue_bg.svg")));
        //minAngle = -0.75*M_PI;
		//maxAngle = 0.75*M_PI;
        minAngle = -12.f*M_PI;
		maxAngle = 12.f*M_PI;
        shadow->opacity = 0;
    }
};

struct BefacoSwitchHorizontal : rack::app::SvgSwitch {
	BefacoSwitchHorizontal() {
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchHoriz_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchHoriz_1.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchHoriz_2.svg")));
        shadow->opacity = 0;
	}
};

struct BefacoSwitchVertical : rack::app::SvgSwitch {
	BefacoSwitchVertical() {
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchVertical_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchVertical_1.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/BefacoSwitchVertical_2.svg")));
        //shadow->opacity = 1;
	}
};