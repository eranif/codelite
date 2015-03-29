
/**
 * @brief The Response interface of the Fetch API represents the response to a request.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Response
 */
function Response() {

	/**
	 * @brief The type read-only property of the Response interface contains the type of the response. It can be one of the following:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Response/type
	 */
	this.useFinalURL = '';

	/**
	 * @brief The clone() method of the Response interface creates a clone of a response object, identical in every way, but stored in a different variable.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Response/clone
	 */
	this.clone = function() {};

	/**
	 * @brief The error() method of the Response interface returns a new Response object associated with a network error.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Response/error
	 */
	this.error = function() {};

	/**
	 * @brief The redirect() method of the Response interface returns a Response resulting in a redirect to the specified URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Response/redirect
	 * @param url - The URL that the new response is to originate from.
	 * @param status Optional - An optional status code for the response (e.g., 302.)
	 */
	this.redirect = function(url, status Optional) {};

}

