
/**
 * @brief The NodeIterator interface represents an iterator over the members of a list of the nodes in a subtree of the DOM. The nodes will be returned in document order.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator
 */
function NodeIterator() {

	/**
	 * @brief The NodeIterator.detach() method is a no-op, kept for backward compatibility only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/detach
	 */
	this.detach = function() {};

	/**
	 * @brief The NodeIterator.previousNode() method returns the previous node in the set represented by the NodeIterator and moves the position of the iterator backwards within the set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/previousNode
	 */
	this.previousNode = function() {};

	/**
	 * @brief The NodeIterator.nextNode() method returns the next node in the set represented by the NodeIterator and advances the position of the iterator within the set.  The first call to nextNode() returns the first node in the set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeIterator/nextNode
	 */
	this.nextNode = function() {};

}

