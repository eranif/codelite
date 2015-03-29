
/**
 * @brief The MozVoicemail API allows access to the information regarding the voicemail features available through the RIL of a Firefox OS device.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozVoicemail
 */
function MozVoicemail() {

	/**
	 * @brief Specifies an event listener to receive statuschange events. Those events occur when the voicemail status has changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozVoicemail/onstatuschange
	 */
	this.onstatuschange = '';

	/**
	 * @brief The getStaus method is used to access the current status of a specified service.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozVoicemail/getStatus
	 */
	this.getStatus = function() {};

	/**
	 * @brief The getNumber method is used to get the voicemail box dialing number of a specified service.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozVoicemail/getNumber
	 */
	this.getNumber = function() {};

	/**
	 * @brief The getDisplayName method is used to get the display name of the voicemail box dialing number of a specified service.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozVoicemail/getDisplayName
	 */
	this.getDisplayName = function() {};

}

