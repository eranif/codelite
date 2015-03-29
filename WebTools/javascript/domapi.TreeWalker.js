
/**
 * @brief The TreeWalker object represents the nodes of a document subtree and a position within them.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker
 */
function TreeWalker() {

	/**
	 * @brief The TreeWalker.currentNode property represents the Node on which the TreeWalker is currently pointing at.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/currentNode
	 */
	this.currentNode = '';

	/**
	 * @brief The TreeWalker.expandEntityReferences read-only property returns a Boolean flag indicating whether or not the children of entity reference nodes are visible to the TreeWalker.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/expandEntityReferences
	 */
	this.expandEntityReferences = '';

	/**
	 * @brief The TreeWalker.filter read-only property returns a NodeFilter that is the filtering object associated with the TreeWalker.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/filter
	 */
	this.filter = '';

	/**
	 * @brief The TreeWalker.root read-only property returns the node that is the root of what the TreeWalker traverses.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/root
	 */
	this.root = '';

	/**
	 * @brief The TreeWalker.whatToShow read-only property returns an unsigned long being a bitmask made of constants describing the types of Node that must to be presented. Non-matching nodes are skipped, but their children may be included, if relevant. The possible values are:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/whatToShow
	 */
	this.whatToShow = '';

	/**
	 * @brief The TreeWalker.firstChild() method moves the current Node to the first visible child of the current node, and returns the found child. It also moves the current node to this child. If no such child exists, returns null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/firstChild
	 */
	this.reeWalker.firstChild = function() {};

	/**
	 * @brief The TreeWalker.lastChild() method moves the current Node to the last visible child of the current node, and returns the found child. It also moves the current node to this child. If no such child exists, returns null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/lastChild
	 */
	this.reeWalker.lastChild = function() {};

	/**
	 * @brief The TreeWalker.nextNode() method moves the current Node to the next visible node in the document order, and returns the found node. It also moves the current node to this one. If no such node exists, returns null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/nextNode
	 */
	this.reeWalker.nextNode = function() {};

	/**
	 * @brief The TreeWalker.nextSibling() method moves the current Node to its next sibling, if any, and returns the found sibling. I there is no such node, return null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/nextSibling
	 */
	this.reeWalker.nextSibling = function() {};

	/**
	 * @brief The TreeWalker.parentNode() method moves the current Node to the first visible ancestor node in the document order, and returns the found node. If no such node exists, or if it is above the TreeWalker's root node, returns null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/parentNode
	 */
	this.reeWalker.parentNode = function() {};

	/**
	 * @brief The TreeWalker.previousNode() method moves the current Node to the previous visible node in the document order, and returns the found node. It also moves the current node to this one. If no such node exists,or if it is before that the root node defined at the object construction, returns null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/previousNode
	 */
	this.reeWalker.previousNode = function() {};

	/**
	 * @brief The TreeWalker.previousSibling() method moves the current Node to its previous sibling, if any, and returns the found sibling. I there is no such node, return null and the current node is not changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TreeWalker/previousSibling
	 */
	this.reeWalker.previousSibling = function() {};

}

