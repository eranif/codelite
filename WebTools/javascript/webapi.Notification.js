
/**
 * @brief The Notification object is used to configure and display desktop notifications to the user.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/notification
 */
function Notification() {

	/**
	 * @brief The requestPermission static method is used to ask the user for his permission to display a Notification to him.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Notification/requestPermission
	 * @param callback Optional - An optional callback function that is called when the user chooses a permission. The permission picked by the user is a string passed as the first argument to the callback function. Possible values for this string are granted, denied, or default.
	 */
	this.requestPermission = function(callback Optional) {};

}

