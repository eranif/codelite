
/**
 * @brief The PositionOptions interface describes an object containing option properties to pass as a parameter of Geolocation.getCurrentPosition() and Geolocation.watchPosition(). 
 * @link https://developer.mozilla.org/en-US/docs/Web/API/PositionOptions
 */
function PositionOptions() {

	/**
	 * @brief The PositionOptions.enableHighAccuracy property is a Boolean that indicates the application would like to receive the best possible results. If true and if the device is able to provide a more accurate position, it will do so. Note that this can result in slower response times or increased power consumption (with a GPS chip on a mobile device for example). On the other hand, if false (the default value), the device can take the liberty to save resources by responding more quickly and/or using less power.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PositionOptions/enableHighAccuracy
	 */
	this.enableHighAccuracy = '';

	/**
	 * @brief The PositionOptions.timeout property is a positive long value representing the maximum length of time (in milliseconds) the device is allowed to take in order to return a position. The default value is Infinity, meaning that getCurrentPosition() won't return until the position is available.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PositionOptions/timeout
	 */
	this.timeout = '';

	/**
	 * @brief The PositionOptions.maximumAge property is a positive long value indicating the maximum age in milliseconds of a possible cached position that is acceptable to return. If set to 0, it means that the device cannot use a cached position and must attempt to retrieve the real current position. If set to Infinity the device must return a cached position regardless of its age.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PositionOptions/maximumAge
	 */
	this.maximumAge = '';

}

