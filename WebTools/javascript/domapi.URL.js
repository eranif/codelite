
/**
 * @brief The URL interface represent an object providing static methods used for creating object URLs.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/URL
 */
function URL() {

	/**
	 * @brief The URLUtils.hash property returns a DOMString containing a '#' followed by the fragment identifier of the URL. The hash is not percent encoded.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/hash
	 */
	this.hash = '';

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
	 * @brief The URLUtils.href property is a DOMString containing the whole URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/href
	 */
	this.href = '';

	/**
	 * @brief The URLUtils.origin read-only property is a DOMString containing the Unicode serialization of the origin of the represented URL, that is, for http and https, the scheme followed by '://', followed by the domain, followed by ':', followed by the port (the default port, 80 and 443 respectively, if explicitely specified). For URL using file: scheme, the value is browser dependant.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/origin
	 */
	this.origin = '';

	/**
	 * @brief The URLUtils.password property is a DOMString containing the password specified before the domain name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/password
	 */
	this.password = '';

	/**
	 * @brief The URLUtils.pathname property is a DOMString containing an initial '/' followed by the path of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/pathname
	 */
	this.pathname = '';

	/**
	 * @brief The URLUtils.port property is a DOMString containing the port number of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/port
	 */
	this.port = '';

	/**
	 * @brief The URLUtils.protocol property is a DOMString representing the protocol scheme of the URL, including the final ':'.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/protocol
	 */
	this.protocol = '';

	/**
	 * @brief The URLUtils.search property is a DOMString containing a '?' followed by the parameters of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/search
	 */
	this.search = '';

	/**
	 * @brief The URLUtils.searchParams property is a URLSearchParams containing the query/search parameters of the URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/searchParams
	 */
	this.searchParams = '';

	/**
	 * @brief The URLUtils.username property is a DOMString containing the username specified before the domain name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/username
	 */
	this.username = '';

	/**
	 * @brief The URL.createObjectURL() static method creates a DOMString containing an URL representing the object given in parameter. The URL lifetime is tied to the document in the window on which it was created. The new object URL represents the specified File object or Blob object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URL/createObjectURL
	 */
	this.RL.createObjectURL = function() {};

	/**
	 * @brief The URL.revokeObjectURL() static method releases an existing object URL which was previously created by calling window.URL.createObjectURL().  Call this method when you've finished using a object URL, in order to let the browser know it doesn't need to keep the reference to the file any longer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URL/revokeObjectURL
	 */
	this.RL.revokeObjectURL = function() {};

	/**
	 * @brief The URLUtils.toString() stringifier method returns a DOMString containing the whole URL. It is a read-only version of URLUtilsReadOnly.href.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/toString
	 */
	this.RLUtils.toString = function() {};

}

