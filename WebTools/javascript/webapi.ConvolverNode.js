
/**
 * @brief The ConvolverNode interface is an AudioNode that performs a Linear Convolution on a given AudioBuffer, often used to achieve a reverb effect. A ConvolverNode always has exactly one input and one output.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ConvolverNode
 */
function ConvolverNode() {

	/**
	 * @brief An AudioBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ConvolverNode/buffer
	 */
	this.buffer = '';

	/**
	 * @brief A boolean.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ConvolverNode/normalize
	 */
	this.normalize = '';

}

