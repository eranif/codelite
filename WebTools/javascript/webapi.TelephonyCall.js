
/**
 * @brief Represents one telephone call, providing information about the call and offering mechanisms for controlling it and detecting changes to its status.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TelephonyCall
 */
function TelephonyCall() {

	/**
	 * @brief This method answers an incoming call.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TelephonyCall/answer
	 */
	this.answer = function() {};

	/**
	 * @brief This method hangs up an ongoing call.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TelephonyCall/hangUp
	 */
	this.hangUp = function() {};

	/**
	 * @brief The hold method is used to put a call on hold.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TelephonyCall/hold
	 */
	this.hold = function() {};

	/**
	 * @brief The resume method is used to get back a call that have been put on hold.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TelephonyCall/resume
	 */
	this.resume = function() {};

}

