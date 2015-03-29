
/**
 * @brief The NodeIterator interface represents an iterator over the members of a list of the nodes in a subtree of the DOM. The nodes will be returned in document order.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator
 */
function NodeIterator() {

	/**
	 * @brief The NodeIterator.expandEntityReferences read-only property returns a Boolean flag indicating whether or not the children of entity reference nodes are visible to the NodeIterator.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/expandEntityReferences
	 */
	this.expandEntityReferences = '';

	/**
	 * @brief The NodeIterator.filter read-only method returns a NodeFilter object, that is an object implement an acceptNode(node) method, used to screen nodes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/filter
	 */
	this.filter = '';

	/**
	 * @brief The NodeIterator.pointerBeforeReferenceNode read-only property returns a Boolean flag that indicates whether the NodeFilter is anchored before (if this value is true) or after (if this value is false) the anchor node indicated by the NodeIterator.referenceNode property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/pointerBeforeReferenceNode
	 */
	this.pointerBeforeReferenceNode = '';

	/**
	 * @brief The NodeIterator.referenceNode read-only returns the Node to which the iterator is anchored; as new nodes are inserted, the iterator remains anchored to the reference node as specified by this property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/referenceNode
	 */
	this.referenceNode = '';

	/**
	 * @brief The NodeIterator.root read-only property represents the Node that is the root of what the NodeIterator traverses.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/root
	 */
	this.root = '';

	/**
	 * @brief The NodeIterator.whatToShow read-only property represents an unsigned integer representing a bitmask signifying what types of nodes should be returned by the NodeIterator.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/whatToShow
	 */
	this.whatToShow = '';

	/**
	 * @brief The NodeIterator.detach() method is a no-op, kept for backward compatibility only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/detach
	 */
	this.odeIterator.detach = function() {};

	/**
	 * @brief The NodeIterator.nextNode() method returns the next node in the set represented by the NodeIterator and advances the position of the iterator within the set.  The first call to nextNode() returns the first node in the set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/nextNode
	 */
	this.odeIterator.nextNode = function() {};

	/**
	 * @brief The NodeIterator.previousNode() method returns the previous node in the set represented by the NodeIterator and moves the position of the iterator backwards within the set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/previousNode
	 */
	this.odeIterator.previousNode = function() {};

}

