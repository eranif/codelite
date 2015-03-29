
/**
 * @brief It is important to note that there is only one listener per context and that it isn't an AudioNode.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioListener
 */
function AudioListener() {

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioListener/dopplerFactor
	 */
	this.dopplerFactor = '';

	/**
	 * @brief The speedOfSound property of the AudioListener interface is a double value representing the speed of sound, in meters per second.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioListener/speedOfSound
	 */
	this.speedOfSound = '';

	/**
	 * @brief Void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioListener/setPosition
	 */
	this.setPosition = function() {};

}

