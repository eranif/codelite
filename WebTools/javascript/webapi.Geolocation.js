
/**
 * @brief The Geolocation interface represents an object able to programmatically obtain the position of the device. It gives Web content access to the location of the device. This allows a Web site or app to offer customized results based on the user's location.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation
 */
function Geolocation() {

	/**
	 * @brief The Geolocation.getCurrentPosition() method is used to get the current position of the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/getCurrentPosition
	 */
	this.getCurrentPosition() = '';

	/**
	 * @brief The Geolocation.watchPosition() method is used to register a handler function that will be called automatically each time the position of the device changes. You can also, optionally, specify an error handling callback function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/watchPosition
	 */
	this.watchPosition() = '';

	/**
	 * @brief The Geolocation.clearWatch() method is used to unregister location/error monitoring handlers previously installed using Geolocation.watchPosition().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/clearWatch
	 */
	this.clearWatch() = '';

	/**
	 * @brief The Geolocation.getCurrentPosition() method is used to get the current position of the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/getCurrentPosition
	 * @param success - A callback function that takes a Position object as its sole input parameter.
	 * @param error Optional - An optional callback function that takes a PositionError object as its sole input parameter.
	 * @param options Optional - An optional PositionOptions object.
	 */
	this.getCurrentPosition = function(success, error Optional, options Optional) {};

	/**
	 * @brief The Geolocation.watchPosition() method is used to register a handler function that will be called automatically each time the position of the device changes. You can also, optionally, specify an error handling callback function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/watchPosition
	 * @param success - A callback function that takes a Position object as an input parameter.
	 * @param error Optional - An optional callback function that takes a PositionError object as an input parameter.
	 * @param options Optional - An optional PositionOptions object.
	 */
	this.watchPosition = function(success, error Optional, options Optional) {};

	/**
	 * @brief The Geolocation.clearWatch() method is used to unregister location/error monitoring handlers previously installed using Geolocation.watchPosition().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Geolocation/clearWatch
	 * @param id - The ID number returned by the Geolocation.watchPosition() method when installing the handler you wish to remove.
	 */
	this.clearWatch = function(id) {};

}

