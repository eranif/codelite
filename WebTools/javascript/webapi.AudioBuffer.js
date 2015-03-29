
/**
 * @brief Objects of these types are designed to hold small audio snippets, typically less than 45 s. For longer sounds, objects implementing the MediaElementAudioSourceNode are more suitable. The buffer contains data in the following format:  non-interleaved IEEE754 32-bit linear PCM with a nominal range between -1 and +1, that is, 32bits floating point buffer, with each samples between -1.0 and 1.0. If the AudioBuffer has multiple channels, they are stored in separate buffer.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBuffer
 */
function AudioBuffer() {

	/**
	 * @brief A Float32Array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBuffer/getChannelData
	 */
	this.getChannelData = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBuffer/copyFromChannel
	 * @param destination - A Float32Array to copy the channel data to.
	 * @param channelNumber - The channel number of the current AudioBuffer to copy the channel data from. If channelNumber is greater than or equal to AudioBuffer.numberOfChannels, an INDEX_SIZE_ERR will be thrown.
	 * @param startInChannel Optional - An optional offset to copy the data from. If startInChannel is greater than AudioBuffer.length, an INDEX_SIZE_ERR will be thrown.
	 */
	this.copyFromChannel = function(destination, channelNumber, startInChannel Optional) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioBuffer/copyToChannel
	 * @param source - A Float32Array that the channel data will be copied from.
	 * @param channelNumber - The channel number of the current AudioBuffer to copy the channel data to. If channelNumber is greater than or equal to AudioBuffer.numberOfChannels, an INDEX_SIZE_ERR will be thrown.
	 * @param startInChannel Optional - An optional offset to copy the data to. If startInChannel is greater than AudioBuffer.length, an INDEX_SIZE_ERR will be thrown.
	 */
	this.copyToChannel = function(source, channelNumber, startInChannel Optional) {};

}

