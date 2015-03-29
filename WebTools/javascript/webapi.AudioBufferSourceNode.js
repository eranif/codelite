
/**
 * @brief The AudioBufferSourceNode interface represents an audio source consisting of in-memory audio data, stored in an AudioBuffer. It is an AudioNode that acts as an audio source.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode
 */
function AudioBufferSourceNode() {

	/**
	 * @brief If the buffer property is set to the value NULL, it defines a single channel of silence.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/buffer
	 */
	this.loop = '';

	/**
	 * @brief The loopStart property's default value is 0.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/loopStart
	 */
	this.loopStart = '';

	/**
	 * @brief The loopEnd property's default value is 0.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/loopEnd
	 */
	this.loopEnd = '';

	/**
	 * @brief The most simple example just starts the audio buffer playing from the beginning — you don't need to specify any parameters in this case:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/start
	 */
	this.start = function() {};

	/**
	 * @brief The most simple example just stops the audio buffer playing immediately — you don't need to specify any parameters in this case:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBufferSourceNode/stop
	 */
	this.stop = function() {};

}

