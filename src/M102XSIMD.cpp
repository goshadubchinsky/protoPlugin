#include "plugin.hpp"
#include "chowdsp/ChowDSP.hpp"
#include "dsp/dc_blocker.hpp"
#include "chowdsp/diode_clipper_wdf_class.hpp"

using b_float = xsimd::batch<float>;

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

	chowdsp::VariableOversampling<6, b_float> oversample[4];	// uses a 2*6=12th order Butterworth filter
	int oversamplingIndex = 1;

	M102XSIMD() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(INPUT_PARAM, 0.f, 10.f, 1.f, "Input Gain Multiplier");
		configParam(OFFSET_PARAM, -10.f, 10.f, 0.f, "Voltage Offset", " V");
		configParam(OUTPUT_PARAM, 0.f, 2.f, 1.f, "Output Gain Multiplier");
			paramQuantities[OUTPUT_PARAM]->randomizeEnabled = false;
		configParam(CV1_PARAM, -1.f, 1.f, 0.f, "CV1 Attenuator", "", 0.f, 5.f);
		configParam(CV2_PARAM, -1.f, 1.f, 0.f, "CV2 Attenuverter", "", 0.f, 5.f);

		const float minFreqHz = 16.0f;
		const float maxFreqHz = 22000.0f;
		//const float defaultFreqHz = 1000.0f;
		const float minFreq = (std::log2(minFreqHz / dsp::FREQ_C4) + 5) / 10;
		const float maxFreq = (std::log2(maxFreqHz / dsp::FREQ_C4) + 5) / 10;
		//const float defaultFreq = (std::log2(defaultFreqHz / dsp::FREQ_C4) + 5) / 10;
		configParam(CUTOFF_PARAM, minFreq, maxFreq, maxFreq, "Cutoff frequency", " ", 0.f, 10.f/(maxFreq-minFreq), -minFreq * (10.f/(maxFreq-minFreq)) );

		configInput(INPUT_INPUT, "Input");
		configSwitch(RANGE_PARAM, 1.f, 3.f, 1.f, "Gain Control Multiplier", {"x1", "x2", "x3"});
			paramQuantities[RANGE_PARAM]->snapEnabled = true;


		configInput(CV1_INPUT, "");
		configInput(CV2_INPUT, "");

		configOutput(OUTPUT_OUTPUT, "Output");
		configBypass(INPUT_INPUT, OUTPUT_OUTPUT);

		onSampleRateChange();
	}

	xsimd::batch<float> input_batch[4] = {0.f};
	xsimd::batch<float> input_param_batch[4] = {0.f};
	xsimd::batch<float> cv_batch[4] = {0.f};
	xsimd::batch<float> cv_2_batch[4] = {0.f};
	float output_array[16] = {0.f};

	// INPUTS+p
	//float input[channels] = {0.f};
	//float input_param{1.f};
	float range_param{1.f};
	float input_cv{0.f}; float input_cv_param{0.f};
	float input_cv_2{0.f}; float input_cv_param_2{0.f};

	// OUTPUTS+p
	//float output[channels] = {0.f};
	float output_param{1.f};

	// PARAMS
	float offset_param{0.f};
	float cutoff = {0.f};

	//CLASSES
	DiodeClipper<b_float> diode_clipper[4];
	int diode_type = 1;
	int capacitor_type = 1;
	DC_Blocker<b_float> dc_blocker[4];
	bool dc_blocker_active = false;

	//OTHER
	float gate_length{0.5f};
	rack::dsp::PulseGenerator gateGenerator;

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

	float maxAbsValue = 0.0f;

    void process(const ProcessArgs& args) override
    {
		static const int batch_size = xsimd::batch<float>::size;
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

		output_param = params[OUTPUT_PARAM].getValue() * 2.f;
		offset_param = params[OFFSET_PARAM].getValue() * 0.2f;

		cutoff = params[CUTOFF_PARAM].getValue();
		cutoff = cutoff * 10.f - 5.f;
		cutoff = dsp::FREQ_C4 * dsp::exp2_taylor5(cutoff);
		cutoff = clamp(cutoff, 1.f, 22000.f);

		
		const int oversamplingRatio = oversample[0].getOversamplingRatio();
		
        const int channels = inputs[INPUT_INPUT].getChannels();

		float* input_voltage_ptr = inputs[INPUT_INPUT].getVoltages();
		float* cv_voltage_ptr = inputs[CV1_INPUT].getVoltages();
		float* cv_2_voltage_ptr = inputs[CV2_INPUT].getVoltages();

		for (int x = 0; x < channels; x += batch_size)
		{
			int batch_index = x / batch_size;
       		input_batch[batch_index] = xsimd::load_unaligned(&voltage_ptr[x]);
       		input_batch[batch_index] *= 0.2f;

			diode_clipper[batch_index].setCircuitParams(input_param_batch[batch_index], offset_param, cutoff);
			oversample[batch_index].upsample(input_batch[batch_index]);
			//OVERSAMPLING LOOP
			b_float* osBuffer = oversample[batch_index].getOSBuffer();
			for (int i = 0; i < oversamplingRatio; ++i)
			{
				osBuffer[i] = diode_clipper[batch_index].processSample(input_batch[batch_index]);
				osBuffer[i] += offset_param * 0.5f;
			}
			//END OVERSAMPLING LOOP
			//DOWNSAMPLE
			input_batch[batch_index] = 5.f * output_param * oversample[batch_index].downsample();
			if (dc_blocker_active)
			{
				input_batch[batch_index] = dc_blocker[batch_index].process(input_batch[batch_index], (float)1.f);
			}
			//input_batch[batch_index].store_aligned(&output_array[x]);
			input_batch[batch_index].store_aligned(&output_array[x]);

			float batch_max = xsimd::reduce_max(xsimd::abs(input_batch[batch_index]));
			maxAbsValue = std::max(maxAbsValue, batch_max);
			
		}

		for (int o = 0; o < channels; ++o)
        {
            outputs[OUTPUT_OUTPUT].setVoltage(-output_array[o], o);
			//maxAbsValue = std::max(maxAbsValue, std::fabs(output_array[o]));
        }

        outputs[OUTPUT_OUTPUT].setChannels(channels);

		
		gate_length = 0.1f;
		if (maxAbsValue >= 10.f)
		{gateGenerator.trigger(gate_length);}
		maxAbsValue = 0.0f;
		lights[LIGHT_LIGHT].setBrightness(gateGenerator.process(args.sampleTime));
		
    }

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "dc_blocker_active", json_boolean(dc_blocker_active));
		json_object_set_new(rootJ, "diode_type", json_integer(diode_type));
		json_object_set_new(rootJ, "capacitor_type", json_integer(capacitor_type));
		json_object_set_new(rootJ, "oversamplingIndex", json_integer(oversample[0].getOversamplingIndex()));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {

		json_t* dc_blocker_active_J = json_object_get(rootJ, "dc_blocker_active");
		if (dc_blocker_active_J) {
			dc_blocker_active = json_boolean_value(dc_blocker_active_J);
		}

		json_t* diode_type_J = json_object_get(rootJ, "diode_type");
		if (diode_type_J) {
			diode_type = json_integer_value(diode_type_J);
			onSampleRateChange();
		}

		json_t* capacitor_type_J = json_object_get(rootJ, "capacitor_type");
		if (capacitor_type_J) {
			capacitor_type = json_integer_value(capacitor_type_J);
			onSampleRateChange();
		}

		json_t* oversamplingIndexJ = json_object_get(rootJ, "oversamplingIndex");
		if (oversamplingIndexJ) {
			oversamplingIndex = json_integer_value(oversamplingIndexJ);
			onSampleRateChange();
		}
	}

private:
};


struct M102XSIMDWidget : ModuleWidget {
	M102XSIMDWidget(M102XSIMD* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/M102XSIMD.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Neutron_Knob_Small>(mm2px(Vec(25.4, 21.8)), module, M102XSIMD::INPUT_PARAM));
		addParam(createParamCentered<ScrewKnobBlue>(mm2px(Vec(6.716, 42.75)), module, M102XSIMD::OFFSET_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 42.75)), module, M102XSIMD::OUTPUT_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 63.7)), module, M102XSIMD::CV1_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 84.65)), module, M102XSIMD::CV2_PARAM));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(25.4, 105.6)), module, M102XSIMD::CUTOFF_PARAM));

		addParam(createParamCentered<BefacoSwitchVertical>(mm2px(Vec(6.716, 32.782)), module, M102XSIMD::RANGE_PARAM));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.7, 42.75)), module, M102XSIMD::LIGHT_LIGHT));

		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(6.716, 21.8)), module, M102XSIMD::INPUT_INPUT));
		//addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(6.716, 32.782)), module, M102XSIMD::RANGE_INPUT));
		//addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(12.7, 42.75)), module, M102XSIMD::LIGHT_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(6.716, 63.7)), module, M102XSIMD::CV1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(6.716, 84.65)), module, M102XSIMD::CV2_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(6.716, 105.6)), module, M102XSIMD::OUTPUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		M102XSIMD* module = dynamic_cast<M102XSIMD*>(this->module);
		assert(module);

		menu->addChild(new MenuSeparator());

		menu->addChild(createIndexSubmenuItem("Diode Type",{"GZ34", "1N4148", "1N4007", "BAT46", "Zener"},
		[ = ]()
		{
			return module->diode_type;
		},
		[ = ](int mode)
		{
			module->diode_type = mode;
			module->onSampleRateChange();
		}));
	
		menu->addChild(createIndexSubmenuItem("Capacitor Type",{"47 nF - Ceramic", "100 nF - Ceramic", "10 µF - Film", "22 µF - Tantalum"},
		[ = ]()
		{
			return module->capacitor_type;
		},
		[ = ](int mode)
		{
			module->capacitor_type = mode;
			module->onSampleRateChange();
		}));
	
		menu->addChild(new MenuSeparator());

		menu->addChild(createIndexSubmenuItem("Oversampling",{"Off", "x2", "x4", "x8"},
		[ = ]()
		{
			return module->oversamplingIndex;
		},
		[ = ](int mode)
		{
			module->oversamplingIndex = mode;
			module->onSampleRateChange();
		}));

		menu->addChild(createBoolPtrMenuItem("DC Blocker", "", &module->dc_blocker_active));
	}
};


Model* modelM102XSIMD = createModel<M102XSIMD, M102XSIMDWidget>("M102XSIMD");