
/**
 * @brief The BiquadFilterNode interface represents a simple low-order filter, and is created using the AudioContext.createBiquadFilter() method. It is an AudioNode that can represent different kinds of filters, tone control devices, and graphic equalizers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/BiquadFilterNode
 */
function BiquadFilterNode() {

	/**
	 * @brief An AudioParam.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BiquadFilterNode/frequency
	 */
	this.type = '';

	/**
	 * @brief A BiquadFilterNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BiquadFilterNode/getFrequencyResponse
	 */
	this.getFrequencyResponse = function() {};

}

