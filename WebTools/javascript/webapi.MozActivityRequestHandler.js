
/**
 * @brief The MozActivityRequestHandler interface allows apps that handle activities to access and interact with the request made by a third party app that tries to delegate an activity.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozActivityRequestHandler
 */
function MozActivityRequestHandler() {

	/**
	 * @brief The postResult method is used to send back an answer to the app that made the activity request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozActivityRequestHandler/postResult
	 */
	this.postResult = function() {};

	/**
	 * @brief The postError method is used to send back an error message to the app that made the activity request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozActivityRequestHandler/postError
	 */
	this.postError = function() {};

}

