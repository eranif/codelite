
/**
 * @brief The AudioNode interface is a generic interface for representing an audio processing module like an audio source (e.g. an HTML &lt;audio> or &lt;video> element, an OscillatorNode, etc.), the audio destination, intermediate processing module (e.g. a filter like BiquadFilterNode or ConvolverNode), or volume control (like GainNode).
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioNode
 */
function AudioNode() {

	/**
	 * @brief Void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioNode/connect(AudioNode)
	 */
	this.udioNode.connect(AudioNode)

	/**
	 * @brief It is possible to connect an AudioNode output to more than one AudioParam, and more than one AudioNode output to a single AudioParam, with multiple calls to connect(). Fan-in and fan-out are therefore supported.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioNode/connect(AudioParam)
	 */
	this.udioNode.connect(AudioParam)

	/**
	 * @brief Void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioNode/disconnect
	 */
	this.disconnect = function() {};

}

