
/**
 * @brief The DocumentFragment interface represents a minimal document object that has no parent. It is used as a light-weight version of Document to store well-formed or potentially non-well-formed fragments of XML.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment
 */
function DocumentFragment() {

	/**
	 * @brief The ParentNode.childElementCount read-only property returns an unsigned long representing the number of child elements of the given element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/childElementCount
	 */
	this.childElementCount = '';

	/**
	 * @brief The ParentNode.children read-only property returns a live HTMLCollection of child elements of the given object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/children
	 */
	this.children = '';

	/**
	 * @brief The ParentNode.firstElementChild read-only property returns the object's first child Element, or null if there are no child elements.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/firstElementChild
	 */
	this.firstElementChild = '';

	/**
	 * @brief The ParentNode.lastElementChild read-only method returns the object's last child Element or null if there are no child elements.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/lastElementChild
	 */
	this.lastElementChild = '';

	/**
	 * @brief The DocumentFragment.querySelector() method returns the first element, or null if no matches are found, within the DocumentFragment (using depth-first pre-order traversal of the document's nodes) that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/querySelector
	 * @param selectors - Is a DOMString containing one or more CSS selectors separated by commas.
	 */
	this.ocumentFragment.querySelector = function(selectors) {};

	/**
	 * @brief The DocumentFragment.querySelectorAll() method returns a NodeList of elements within the DocumentFragment (using depth-first pre-order traversal of the document's nodes) that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentFragment/querySelectorAll
	 * @param selectors - Is a DOMString containing one or more CSS selectors separated by commas.
	 */
	this.ocumentFragment.querySelectorAll = function(selectors) {};

}

