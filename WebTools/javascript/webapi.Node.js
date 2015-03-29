
/**
 * @brief A Node is an interface from which a number of DOM types inherit, and allows these various types to be treated (or tested) similarly.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node
 */
function Node() {

	/**
	 * @brief The Node.baseURI read-only property returns the absolute base URL of a node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/baseURI
	 */
	this.nodeValue = '';

	/**
	 * @brief The Node.ownerDocument read-only property returns the top-level document object for this node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/ownerDocument
	 */
	this.textContent = '';

	/**
	 * @brief The Node.appendChild() method adds a node to the end of the list of children of a specified parent node. If the given child is a reference to an existing node in the document, appendChild() moves it from its current position to the new position (i.e. there is no requirement to remove the node from its parent node before appending it to some other node).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/appendChild
	 */
	this.appendChild = function() {};

	/**
	 * @brief The Node.cloneNode() method returns a duplicate of the node on which this method was called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/cloneNode
	 */
	this.cloneNode = function() {};

	/**
	 * @brief The Node.compareDocumentPosition() method compares the position of the current node against another node in any other document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/compareDocumentPosition
	 */
	this.compareDocumentPosition = function() {};

	/**
	 * @brief The Node.contains() method returns a Boolean value indicating whether a node is a descendant of a given node. or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/contains
	 */
	this.contains = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/getFeature
	 */
	this.getFeature = function() {};

	/**
	 * @brief The Node.getUserData() method returns any user DOMUserData set previously on the given node by Node.setUserData().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/getUserData
	 */
	this.getUserData = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/hasAttributes
	 */
	this.hasAttributes = function() {};

	/**
	 * @brief The Node.hasChildNodes() method returns a Boolean value indicating whether the current Node has child nodes or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/hasChildNodes
	 */
	this.hasChildNodes = function() {};

	/**
	 * @brief The Node.insertBefore() method inserts the specified node before a reference element as a child of the current node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/insertBefore
	 */
	this.insertBefore = function() {};

	/**
	 * @brief The Node.isDefaultNamespace()method accepts a namespace URI as an argument and returns a Boolean with a value of true if the namespace is the default namespace on the given node or false if not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/isDefaultNamespace
	 */
	this.isDefaultNamespace = function() {};

	/**
	 * @brief The Node.isEqualNode() tests whether two nodes are equal.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/isEqualNode
	 */
	this.isEqualNode = function() {};

	/**
	 * @brief The Node.isSameNode() tests whether two nodes are the same, that is if they reference the same object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/isSameNode
	 */
	this.isSameNode = function() {};

	/**
	 * @brief The Node.isSupported()returns a Boolean flag containing the result of a test whether the DOM implementation implements a specific feature and this feature is supported by the specific node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/isSupported
	 * @param feature - Is a DOMString containing the name of the feature to test. This is the same name which can be passed to the method hasFeature on DOMImplementation. Possible values defined within the core DOM specification are listed on the DOM Level 2 Conformance Section.
	 * @param version - Is a DOMString containing the version number of the feature to test. In DOM Level 2, version 1, this is the string 2.0. If the version is not specified, supporting any version of the feature will cause the method to return true.
	 */
	this.isSupported = function(feature, version) {};

	/**
	 * @brief The Node.lookupPrefix() method returns a DOMString containing the prefix for a given namespace URI, if present, and null if not. When multiple prefixes are possible, the result is implementation-dependent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/lookupPrefix
	 */
	this.lookupPrefix = function() {};

	/**
	 * @brief The Node.lookupNamespaceURI() method takes a prefix and returns the namespace URI associated with it on the given node if found (and null if not). Supplying null for the prefix will return the default namespace.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/lookupNamespaceURI
	 */
	this.lookupNamespaceURI = function() {};

	/**
	 * @brief The Node.removeChild() method removes a child node from the DOM. Returns removed node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/normalize" title='The Node.normalize() method puts the specified node and all of its sub-tree into a "normalized" form. In a normalized sub-tree, no text nodes in the sub-tree are empty and there are no adjacent text nodes.'><code>Node.normalize()</code></a></dt>
 <dd>Clean up all the text nodes under this element (merge adjacent, remove empty).</dd>
 <dt><a href=
	 */
	this.removeChild = function() {};

	/**
	 * @brief The Node.replaceChild() method replaces one child node of the specified element with another.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/replaceChild
	 */
	this.replaceChild = function() {};

	/**
	 * @brief The Node.setUserData() method allows a user to attach (or remove) data to an element, without needing to modify the DOM. Note that such data will not be preserved when imported via Node.importNode, as with Node.cloneNode() and Node.renameNode() operations (though Node.adoptNode does preserve the information), and equality tests in Node.isEqualNode() do not consider user data in making the assessment.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/setUserData
	 */
	this.setUserData = function() {};

}

