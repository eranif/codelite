
/**
 * @brief XDomainRequest is an implementation of HTTP access control (CORS) that worked in Internet Explorer 8 and 9. It was removed in Internet Explorer 10 in favor of using XMLHttpRequest with proper CORS; if you are targeting Internet Explorer 10 or later, or wish to support any other browser, you need to use standard HTTP access control.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest
 */
function XDomainRequest() {

	/**
	 * @brief Gets or sets the amount of time in milliseconds until an XDomainRequest times out.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest/timeout
	 */
	this.timeout = '';

	/**
	 * @brief Returns the response body of an XDomainRequest as a string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest/responseText
	 */
	this.responseText = '';

	/**
	 * @brief Opens an XDomainRequest which is configured to use a given method (GET/POST) and URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest/open
	 * @param method - The HTTP method to use for the request. (GET or POST)
	 * @param url - The URL to which to send the request.
	 */
	this.open = function(method, url) {};

	/**
	 * @brief Sends an XDomainRequest which has previously been opened calling XDomainRequest.open().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest/send
	 */
	this.send = function() {};

	/**
	 * @brief Aborts an XDomainRequest.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XDomainRequest/abort
	 */
	this.abort = function() {};

}

