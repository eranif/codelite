
/**
 * @brief The Location interface represents the location of the object it is linked to. Changes done on it are reflected on the object it relates to. Both the Document and Window interface have such a linked Location, accessible via Document.location and Window.location respectively.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Location
 */
function Location() {

	/**
	 * @brief The URLUtils.href property is a DOMString containing the whole URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/href
	 */
	this.href = '';

	/**
	 * @brief The URLUtils.protocol property is a DOMString representing the protocol scheme of the URL, including the final ':'.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/protocol
	 */
	this.protocol = '';

	/**
	 * @brief The URLUtils.host property is a DOMString containing the host, that is the hostname, and then, if the port of the URL is nonempty, a ':', and the port of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/host
	 */
	this.host = '';

	/**
	 * @brief The URLUtils.hostname property is a DOMString containing the domain of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/hostname
	 */
	this.hostname = '';

	/**
	 * @brief The URLUtils.port property is a DOMString containing the port number of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/port
	 */
	this.port = '';

	/**
	 * @brief The URLUtils.pathname property is a DOMString containing an initial '/' followed by the path of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/pathname
	 */
	this.pathname = '';

	/**
	 * @brief The URLUtils.search property is a DOMString containing a '?' followed by the parameters of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/search
	 */
	this.search = '';

	/**
	 * @brief The URLUtils.hash property returns a DOMString containing a '#' followed by the fragment identifier of the URL. The hash is not percent encoded.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/hash
	 */
	this.hash = '';

	/**
	 * @brief The URLUtils.username property is a DOMString containing the username specified before the domain name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/username
	 */
	this.username = '';

	/**
	 * @brief The URLUtils.password property is a DOMString containing the password specified before the domain name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/password
	 */
	this.password = '';

	/**
	 * @brief The Location.assign()method causes the window to load and display the document at the URL specified.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Location/assign
	 * @param url - Is a DOMString containing the URL of the page to navigate to.
	 */
	this.assign = function(url) {};

	/**
	 * @brief The Location.reload()method Reloads the resource from the current URL. Its optional unique parameter is a Boolean, which, when it is true, causes the page to always be reloaded from the server. If it is false or not specified, the browser may reload the page from its cache.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Location/reload
	 * @param forcedReload Optional - Is a Boolean flag, which, when it is true, causes the page to always be reloaded from the server. If it is false or not specified, the browser may reload the page from its cache.
	 */
	this.reload = function(forcedReload Optional) {};

	/**
	 * @brief The Location.replace()method replaces the current resource with the one at the provided URL. The difference from the assign() method is that after using replace() the current page will not be saved in session History, meaning the user won't be able to use the back button to navigate to it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Location/replace
	 * @param url - Is a DOMString containing the URL of the page to navigate to.
	 */
	this.replace = function(url) {};

	/**
	 * @brief The URLUtils.toString() stringifier method returns a DOMString containing the whole URL. It is a read-only version of URLUtilsReadOnly.href.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/toString
	 */
	this.toString = function() {};

}

