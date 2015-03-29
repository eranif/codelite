
/**
 * @brief The HTMLAnchorElement interface represents hyperlink elements and provides special properties and methods (beyond those of the regular HTMLElement object interface they also have available to them by inheritance) for manipulating the layout and presentation of such elements.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement
 */
function HTMLAnchorElement() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/accessKey
	 */
	this.accessKey = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/charset
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
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/hreflang
	 */
	this.hreflang = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/media
	 */
	this.media = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/name
	 */
	this.password = '';

	/**
	 * @brief The URLUtils.origin read-only property is a DOMString containing the Unicode serialization of the origin of the represented URL, that is, for http and https, the scheme followed by '://', followed by the domain, followed by ':', followed by the port (the default port, 80 and 443 respectively, if explicitely specified). For URL using file: scheme, the value is browser dependant.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/origin
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
	 * @brief The HTMLAnchorElement.rel property reflects the rel attribute. It is a DOMString containing a space-separated list of link types indicating the relationship between the resource represented by the &lt;a> element and the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/rel
	 */
	this.rel = '';

	/**
	 * @brief The HTMLAnchorElement.relList read-only property reflects the rel attribute. It is a live DOMTokenList containing the set of link types indicating the relationship between the resource represented by the &lt;a> element and the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/relList
	 */
	this.search = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/shape
	 */
	this.tabindex = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/target
	 */
	this.target = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/text
	 */
	this.text = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLAnchorElement/type
	 */
	this.type = '';

	/**
	 * @brief The URLUtils.username property is a DOMString containing the username specified before the domain name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/username
	 */
	this.username = '';

	/**
	 * @brief The HTMLElement.blur() method removes keyboard focus from the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/blur
	 */
	this.blur = function() {};

	/**
	 * @brief The HTMLElement.focus() method sets focus on the specified element, if it can be focused.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/focus
	 */
	this.focus = function() {};

	/**
	 * @brief The URLUtils.toString() stringifier method returns a DOMString containing the whole URL. It is a read-only version of URLUtilsReadOnly.href.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtils/toString
	 */
	this.toString = function() {};

}

