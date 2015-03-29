
/**
 * @brief The AnalyserNode interface represents a node able to provide real-time frequency and time-domain analysis information. It is an AudioNode
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode
 */
function AnalyserNode() {

	/**
	 * @brief The fftSize property of the AnalyserNode interface is an unsigned long value representing the size of the FFT (Fast Fourier Transform) to be used to determine the frequency domain.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/fftSize
	 */
	this.fftSize = '';

	/**
	 * @brief The frequencyBinCount property of the AnalyserNode interface is an unsigned long value half that of the FFT size. This generally equates to the number of data values you will have to play with for the visualization.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/frequencyBinCount
	 */
	this.frequencyBinCount = '';

	/**
	 * @brief The maxDecibels property of the AnalyserNode interface Is a double value representing the maximum power value in the scaling range for the FFT analysis data, for conversion to unsigned byte/float values — basically, this specifies the maximum value for the range of results when using getFloatFrequencyData() or getByteFrequencyData().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/maxDecibels
	 */
	this.maxDecibels = '';

	/**
	 * @brief The minDecibels property of the AnalyserNode interface Is a double value representing the minimum power value in the scaling range for the FFT analysis data, for conversion to unsigned byte/float values — basically, this specifies the minimum value for the range of results when using getFloatFrequencyData() or getByteFrequencyData().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/minDecibels
	 */
	this.minDecibels = '';

	/**
	 * @brief The smoothingTimeConstant property of the AnalyserNode interface is a double value representing the averaging constant with the last analysis frame. It's basically an average
 between the current buffer and the last buffer the AnalyserNode processed, and results in a much smoother set of value changes over time.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/smoothingTimeConstant
	 */
	this.smoothingTimeConstant = '';

	/**
	 * @brief A Uint8Array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/getByteFrequencyData
	 */
	this.getByteFrequencyData = function() {};

	/**
	 * @brief A Uint8Array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/getByteTimeDomainData
	 */
	this.getByteTimeDomainData = function() {};

	/**
	 * @brief A Float32Array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/getFloatFrequencyData
	 */
	this.getFloatFrequencyData = function() {};

	/**
	 * @brief A Float32Array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnalyserNode/getFloatTimeDomainData
	 */
	this.getFloatTimeDomainData = function() {};

}

