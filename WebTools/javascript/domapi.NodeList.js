
/**
 * @brief NodeList objects are collections of nodes such as those returned by Node.childNodes and the document.querySelectorAll method.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/NodeList
 */
function NodeList() {

	/**
	 * @brief length returns the number of items in a NodeList.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeList/length
	 */
	this.length = '';

	/**
	 * @brief Returns a node from a NodeList by index. This method doesn't throw exceptions, a value of null is returned if the index is out of range.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeList/item
	 */
	this.item = function() {};

}

