
/**
 * @brief A WaveShaperNode always has exactly one input and one output.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/WaveShaperNode
 */
function WaveShaperNode() {

	/**
	 * @brief The curve property of the WaveShaperNode interface is a Float32Array of numbers describing the distortion to apply.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WaveShaperNode/curve
	 */
	this.curve = '';

	/**
	 * @brief The oversample property of the WaveShaperNode interface is an enumerated value indicating if oversampling must be used. Oversampling is a technique for creating more samples (up-sampling) before applying a distortion effect to the audio signal.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WaveShaperNode/oversample
	 */
	this.oversample = '';

}

