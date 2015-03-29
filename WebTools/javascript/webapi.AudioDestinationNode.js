
/**
 * @brief AudioDestinationNode has no output (as it is the output, no more AudioNode can be linked after it in the audio graph) and one input. The amount of channels in the input must be between 0 and the maxChannelCount value or an exception is raised.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioDestinationNode
 */
function AudioDestinationNode() {

	/**
	 * @brief An unsigned long.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioDestinationNode/maxChannelCount
	 */
	this.maxChannelCount = '';

}

