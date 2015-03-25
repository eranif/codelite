
/**
 * @brief The ParentNode interface contains methods that are particular to Node objects that can have children.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/ParentNode
 */
function ParentNode() {

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

}

