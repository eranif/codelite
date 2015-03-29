
/**
 * @brief The URL interface represent an object providing static methods used for creating object URLs.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/URL
 */
function URL() {

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
	 * @brief The URLUtils.toString() stringifier method returns a DOMString containing the whole URL. It is a read-only version of URLUtilsReadOnly.href.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/toString
	 */
	this.toString = function() {};

}

