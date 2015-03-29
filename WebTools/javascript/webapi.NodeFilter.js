
/**
 * @brief A NodeFilter interface represents an object used to filter the nodes in a NodeIterator or TreeWalker. They don't know anything about the DOM or how to traverse nodes; they just know how to evaluate a single node against the provided filter.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeFilter
 */
function NodeFilter() {

	/**
	 * @brief The NodeFilter.acceptNode() method returns an unsigned short that will be used to tell if a given Node must be accepted or not by the NodeIterator or TreeWalker iteration algorithm. This method is expected to be written by the user of a NodeFilter. Possible return values are:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeFilter/acceptNode
	 */
	this.acceptNode() = '';

	/**
	 * @brief The NodeFilter.acceptNode() method returns an unsigned short that will be used to tell if a given Node must be accepted or not by the NodeIterator or TreeWalker iteration algorithm. This method is expected to be written by the user of a NodeFilter. Possible return values are:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NodeFilter/acceptNode
	 * @param node - Is a Node being the object to check against the filter.
	 */
	this.acceptNode = function(node) {};

}

