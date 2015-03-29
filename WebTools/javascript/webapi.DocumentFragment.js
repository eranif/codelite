
/**
 * @brief The DocumentFragment interface represents a minimal document object that has no parent. It is used as a light-weight version of Document to store well-formed or potentially non-well-formed fragments of XML.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment
 */
function DocumentFragment() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/find
	 */
	this.find = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/findAll
	 */
	this.findAll = function() {};

	/**
	 * @brief The DocumentFragment.querySelector() method returns the first element, or null if no matches are found, within the DocumentFragment (using depth-first pre-order traversal of the document's nodes) that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/querySelector
	 * @param selectors - Is a DOMString containing one or more CSS selectors separated by commas.
	 */
	this.querySelector = function(selectors) {};

	/**
	 * @brief The DocumentFragment.querySelectorAll() method returns a NodeList of elements within the DocumentFragment (using depth-first pre-order traversal of the document's nodes) that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/querySelectorAll
	 * @param selectors - Is a DOMString containing one or more CSS selectors separated by commas.
	 */
	this.querySelectorAll = function(selectors) {};

}

