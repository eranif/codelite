
/**
 * @brief The Navigator interface represents the state and the identity of the user agent. It allows scripts to query it and to register themselves to carry on some activities.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Navigator
 */
function Navigator() {

	/**
	 * @brief The NavigatorID.appCodeName always returns 'Mozilla', on any browser. This property is kept only for compatibility purposes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NavigatorID/appCodeName
	 */
	this.oscpu = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NavigatorUserMedia/getUserMedia
	 */
	this.getUserMedia = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/navigator/registerContentHandler
	 */
	this.avigator.registerContentHandler

	/**
	 * @brief Allows web sites to register themselves as possible handlers for particular protocols.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Navigator/registerProtocolHandler
	 */
	this.avigator.registerProtocolHandler

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Navigator/requestMediaKeySystemAccess
	 */
	this.requestMediaKeySystemAccess = function() {};

	/**
	 * @brief The NavigatorID.taintEnabled() method always returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NavigatorID/taintEnabled
	 */
	this.taintEnabled = function() {};

	/**
	 * @brief The Navigator.vibrate() method pulses the vibration hardware on the device, if such hardware exists. If the device doesn't support vibration, this method has no effect. If a vibration pattern is already in progress when this method is called, the previous pattern is halted and the new one begins instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Navigator/vibrate
	 */
	this.vibrate = function() {};

}

