#pragma once
#include <rack.hpp>
#include <xsimd/xsimd.hpp>

namespace chowdsp {
	// code taken from https://github.com/jatinchowdhury18/ChowDSP-VCV/blob/master/src/shared/, commit 21701fb 
	// * AAFilter_XSIMD.hpp
	// * VariableOversampling_XSIMD.hpp
	// * oversampling_XSIMD.hpp
	// * iir.hpp

template <int ORDER, typename T = xsimd::batch<float>>
struct IIRFilter_XSIMD {
	/** transfer function numerator coefficients: b_0, b_1, etc.*/
	xsimd::batch<float> b[ORDER] = {};

	/** transfer function denominator coefficients: a_0, a_1, etc.*/
	xsimd::batch<float> a[ORDER] = {};

	/** filter state */
	xsimd::batch<float> z[ORDER];

	IIRFilter_XSIMD() {
		reset();
	}

	void reset() {
		std::fill(z, &z[ORDER], xsimd::batch<float>(0.0f));
	}

	void setCoefficients(const T* b, const T* a) {
		for (int i = 0; i < ORDER; i++) {
			this->b[i] = b[i];
		}
		for (int i = 1; i < ORDER; i++) {
			this->a[i] = a[i];
		}
	}

	template <int N = ORDER>
	inline typename std::enable_if <N == 2, T>::type process(xsimd::batch<float> x) noexcept {
		xsimd::batch<float> y = z[1] + x * b[0];
		z[1] = x * b[1] - y * a[1];
		return y;
	}

	template <int N = ORDER>
	inline typename std::enable_if <N == 3, T>::type process(xsimd::batch<float> x) noexcept {
		xsimd::batch<float> y = z[1] + x * b[0];
		z[1] = z[2] + x * b[1] - y * a[1];
		z[2] = x * b[2] - y * a[2];
		return y;
	}

	template <int N = ORDER>
	inline typename std::enable_if < (N > 3), T >::type process(xsimd::batch<float> x) noexcept {
		xsimd::batch<float> y = z[1] + x * b[0];

		for (int i = 1; i < ORDER - 1; ++i)
			z[i] = z[i + 1] + x * b[i] - y * a[i];

		z[ORDER - 1] = x * b[ORDER - 1] - y * a[ORDER - 1];

		return y;
	}
};

template <typename T = xsimd::batch<float>>
struct TBiquadFilter_XSIMD : IIRFilter_XSIMD<3, xsimd::batch<float>> {
	enum Type {
		LOWPASS,
		HIGHPASS,
		LOWSHELF,
		HIGHSHELF,
		BANDPASS,
		PEAK,
		NOTCH,
		NUM_TYPES
	};

	TBiquadFilter_XSIMD() {
		setParameters(LOWPASS, xsimd::batch<float>(0.f), xsimd::batch<float>(0.f), xsimd::batch<float>(1.f));
	}

	/** Calculates and sets the biquad transfer function coefficients.
	f: normalized frequency (cutoff frequency / sample rate), must be less than 0.5
	Q: quality factor
	V: gain
	*/
	void setParameters(Type type, xsimd::batch<float> f, xsimd::batch<float> Q, xsimd::batch<float> V) {
		xsimd::batch<float> K = xsimd::tan(xsimd::batch<float>(M_PI) * f);
		switch (type) {
			case LOWPASS: {
				xsimd::batch<float> norm = xsimd::batch<float>(1.f) / (xsimd::batch<float>(1.f) + K / Q + K * K);
				this->b[0] = K * K * norm;
				this->b[1] = xsimd::batch<float>(2.f) * this->b[0];
				this->b[2] = this->b[0];
				this->a[1] = xsimd::batch<float>(2.f) * (K * K - xsimd::batch<float>(1.f)) * norm;
				this->a[2] = (xsimd::batch<float>(1.f) - K / Q + K * K) * norm;
			} break;
			default: break;
		}
	}
};

typedef TBiquadFilter_XSIMD<> BiquadFilter_XSIMD;


/**
    High-order filter to be used for anti-aliasing or anti-imaging.
    The template parameter N should be 1/2 the desired filter order.

    Currently uses an 2*N-th order Butterworth filter.
    source: https://github.com/jatinchowdhury18/ChowDSP-VCV/blob/master/src/shared/AAFilter_XSIMD.hpp
*/
template<int N, typename T = xsimd::batch<float>>
class AAFilter_XSIMD {
public:
	AAFilter_XSIMD() = default;

	/** Calculate Q values for a Butterworth filter of a given order */
	static std::vector<float> calculateButterQs(int order) {
		const int lim = int (order / 2);
		std::vector<float> Qs;

		for (int k = 1; k <= lim; ++k) {
			auto b = -2.0f * simd::cos((2.0f * k + order - 1) * 3.14159 / (2.0f * order));
			Qs.push_back(1.0f / b);
		}

		std::reverse(Qs.begin(), Qs.end());
		return Qs;
	}

	/**
	 * Resets the filter to process at a new sample rate.
	 *
	 * @param sampleRate: The base (i.e. pre-oversampling_XSIMD) sample rate of the audio being processed
	 * @param osRatio: The oversampling_XSIMD ratio at which the filter is being used
	 */
	void reset(float sampleRate, int osRatio) {
		float fc = 0.85f * (sampleRate / 2.0f);
		auto Qs = calculateButterQs(2 * N);

		for (int i = 0; i < N; ++i)
			filters[i].setParameters(TBiquadFilter_XSIMD<T>::Type::LOWPASS, fc / (osRatio * sampleRate), Qs[i], 1.0f);
	}

	inline T process(T x) noexcept {
		for (int i = 0; i < N; ++i)
			x = filters[i].process(x);

		return x;
	}

private:
	TBiquadFilter_XSIMD<xsimd::batch<float>> filters[N];
};



/**
 * Base class for oversampling_XSIMD of any order
 * source: https://github.com/jatinchowdhury18/ChowDSP-VCV/blob/master/src/shared/oversampling_XSIMD.hpp
 */
template<typename T = xsimd::batch<float>>
class BaseOversampling_XSIMD {
public:
	BaseOversampling_XSIMD() = default;
	virtual ~BaseOversampling_XSIMD() {}

	/** Resets the oversampler for processing at some base sample rate */
	virtual void reset(float /*baseSampleRate*/) = 0;

	/** Upsample a single input sample and update the oversampled buffer */
	virtual void upsample(T) noexcept = 0;

	/** Output a downsampled output sample from the current oversampled buffer */
	virtual T downsample() noexcept = 0;

	/** Returns a pointer to the oversampled buffer */
	virtual T* getOSBuffer() noexcept = 0;
};


/**
    Class to implement an oversampled process.
    To use, create an object and prepare using `reset()`.

    Then use the following code to process samples:
    @code
    oversample.upsample(x);
    for(int k = 0; k < ratio; k++)
        oversample.osBuffer[k] = processSample(oversample.osBuffer[k]);
    float y = oversample.downsample();
    @endcode
*/
template<int ratio, int filtN = 4, typename T = xsimd::batch<float>>
class Oversampling_XSIMD : public BaseOversampling_XSIMD<T> {
public:
	Oversampling_XSIMD() = default;
	virtual ~Oversampling_XSIMD() {}

	void reset(float baseSampleRate) override {
		aaFilter_XSIMD.reset(baseSampleRate, ratio);
		aiFilter.reset(baseSampleRate, ratio);
		std::fill(osBuffer, &osBuffer[ratio], xsimd::batch<float>(0.0f));
	}

	inline void upsample(T x) noexcept override {
		osBuffer[0] = ratio * x;
		std::fill(&osBuffer[1], &osBuffer[ratio], xsimd::batch<float>(0.0f));

		for (int k = 0; k < ratio; k++)
			osBuffer[k] = aiFilter.process(osBuffer[k]);
	}

	inline T downsample() noexcept override {
		xsimd::batch<float> y = 0.0f;
		for (int k = 0; k < ratio; k++)
			y = aaFilter_XSIMD.process(osBuffer[k]);

		return y;
	}

	inline T* getOSBuffer() noexcept override {
		return osBuffer;
	}

	xsimd::batch<float> osBuffer[ratio];

private:
	AAFilter_XSIMD<filtN, xsimd::batch<float>> aaFilter_XSIMD; // anti-aliasing filter
	AAFilter_XSIMD<filtN, xsimd::batch<float>> aiFilter; // anti-imaging filter
};

//typedef Oversampling_XSIMD<1, 4, simd::float_4> Oversampling_XSIMDSIMD;


/**
    Class to implement an oversampled process, with variable
    oversampling_XSIMD factor. To use, create an object, set the oversampling_XSIMD
    factor using `setOversampling_XSIMDindex()` and prepare using `reset()`.

    Then use the following code to process samples:
    @code
    oversample.upsample(x);
    float* osBuffer = oversample.getOSBuffer();
    for(int k = 0; k < ratio; k++)
        osBuffer[k] = processSample(osBuffer[k]);
    float y = oversample.downsample();
    @endcode

	source (modified): https://github.com/jatinchowdhury18/ChowDSP-VCV/blob/master/src/shared/VariableOversampling_XSIMD.hpp
*/
template<int filtN = 4, typename T = xsimd::batch<float>>
class VariableOversampling_XSIMD {
public:
	VariableOversampling_XSIMD() = default;

	/** Prepare the oversampler to process audio at a given sample rate */
	void reset(float sampleRate) {
		for (auto* os : oss)
			os->reset(sampleRate);
	}

	/** Sets the oversampling_XSIMD factor as 2^idx */
	void setOversamplingIndex(int newIdx) {
		osIdx = newIdx;
	}

	/** Returns the oversampling_XSIMD index */
	int getOversamplingIndex() const noexcept {
		return osIdx;
	}

	/** Upsample a single input sample and update the oversampled buffer */
	inline void upsample(T x) noexcept {
		oss[osIdx]->upsample(x);
	}

	/** Output a downsampled output sample from the current oversampled buffer */
	inline T downsample() noexcept {
		return oss[osIdx]->downsample();
	}

	/** Returns a pointer to the oversampled buffer */
	inline T* getOSBuffer() noexcept {
		return oss[osIdx]->getOSBuffer();
	}

	/** Returns the current oversampling_XSIMD factor */
	int getOversamplingRatio() const noexcept {
		return 1 << osIdx;
	}


private:
	enum {
		NumOS = 5, // number of oversampling_XSIMD options
	};

	int osIdx = 0;

	Oversampling_XSIMD < 1 << 0, filtN, T > os0; // 1x
	Oversampling_XSIMD < 1 << 1, filtN, T > os1; // 2x
	Oversampling_XSIMD < 1 << 2, filtN, T > os2; // 4x
	Oversampling_XSIMD < 1 << 3, filtN, T > os3; // 8x
	Oversampling_XSIMD < 1 << 4, filtN, T > os4; // 16x
	BaseOversampling_XSIMD<T>* oss[NumOS] = { &os0, &os1, &os2, &os3, &os4 };
};

} // namespace chowdsp
