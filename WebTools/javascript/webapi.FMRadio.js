
/**
 * @brief The WebFM API provides access to the device FM radio. This interface lets you turn the FM radio on and off and tune it to different stations. It is accessible through the navigator.mozFMRadio property.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio
 */
function FMRadio() {

	/**
	 * @brief The enabled property value indicates whether the radio is playing (true) or not (false)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/enabled
	 */
	this.enabled = '';

	/**
	 * @brief The antennaAvailable property value indicates if an antenna is plugged and available.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/antennaAvailable
	 */
	this.antennaAvailable = '';

	/**
	 * @brief The frequency property value returns the current radio frequency.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/frequency
	 */
	this.frequency = '';

	/**
	 * @brief The frequencyUpperBound property value indicates the maximum frequency up to which the seek method searches for radio stations.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/frequencyUpperBound
	 */
	this.frequencyUpperBound = '';

	/**
	 * @brief The frequencyLowerBound property value indicates the minimum frequency down to which the seek method searches for radio stations.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/frequencyLowerBound
	 */
	this.frequencyLowerBound = '';

	/**
	 * @brief The channelWidth property value indicates the channel width of the ranges of frequency, in MHz.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/channelWidth
	 */
	this.channelWidth = '';

	/**
	 * @brief The enable method is used to turns on the radio on the given frequency.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/enable
	 * @param frenquency - The value of the frenquency the radio should listen to.
	 */
	this.enable = function(frenquency) {};

	/**
	 * @brief The disable method is used to turn off the radio.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/disable
	 */
	this.disable = function() {};

	/**
	 * @brief The setFrequency method is used to change the frequency listened to by the radio.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/setFrequency
	 * @param frequency - The value of the new frequency to set, It's a number representing a value in MHz (MegaHertz).
	 */
	this.setFrequency = function(frequency) {};

	/**
	 * @brief The seekUp method is used to tell the radio to seek up to the next radio channel. The search circles back to lower frequencies when the highest frequency has been reached.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/seekUp
	 */
	this.seekUp = function() {};

	/**
	 * @brief The seekDown method is used to tell the radio to seek down to the next radio channel. The search circles back to upper frequencies when the lowest frequency has been reached.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/seekDown
	 */
	this.seekDown = function() {};

	/**
	 * @brief The cancelSeek method is used to tell the radio to stop seeking for frequencies.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FMRadio/cancelSeek
	 */
	this.cancelSeek = function() {};

}

