
/**
 * @brief A Node is an interface from which a number of DOM types inherit, and allows these various types to be treated (or tested) similarly.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/Node
 */
function Node() {

	/**
	 * @brief The Node.baseURI read-only property returns the absolute base URL of a node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/baseURI
	 */
	this.baseURI = '';

	/**
	 * @brief The Node.baseURIObject property returns the nsIURI representing the node's (typically a document or an element) base URL. It's similar to Node.baseURI, except it returns an nsIURI instead of a string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/baseURIObject
	 */
	this.baseURIObject = '';

	/**
	 * @brief The Node.childNodes read-only property returns a live collection of child nodes of the given element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/childNodes
	 */
	this.childNodes = '';

	/**
	 * @brief The Node.firstChild read-only property returns the node's first child in the tree, or null if the node is childless. If the node is a Document, it returns the first node in the list of its direct children.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/firstChild
	 */
	this.firstChild = '';

	/**
	 * @brief The Node.lastChild read-only property returns the last child of the node. If its parent is an element, then the child is generally an element node, a text node, or a comment node. It returns null if there are no child elements.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/lastChild
	 */
	this.lastChild = '';

	/**
	 * @brief The Node.localName read-only property returns the local part of the qualified name of this node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/localName
	 */
	this.localName = '';

	/**
	 * @brief The Node.namespaceURI read-only property returns the namespace URI of the node, or null if the node is not in a namespace (read-only). When the node is a document, it returns the XML namespace for the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/namespaceURI
	 */
	this.namespaceURI = '';

	/**
	 * @brief The Node.nextSibling read-only property returns the node immediately following the specified one in its parent's childNodes list, or null if the specified node is the last node in that list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/nextSibling
	 */
	this.nextSibling = '';

	/**
	 * @brief The Node.nodeName read-only property returns the name of the current node as a string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/nodeName
	 */
	this.nodeName = '';

	/**
	 * @brief 
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/nodePrincipal
	 */
	this.nodePrincipal = '';

	/**
	 * @brief The read-only Node.nodeType property returns an unsigned short integer representing the type of the node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/nodeType
	 */
	this.nodeType = '';

	/**
	 * @brief The Node.nodeValue property returns or sets the value of the current node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/nodeValue
	 */
	this.nodeValue = '';

	/**
	 * @brief The Node.ownerDocument read-only property returns the top-level document object for this node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/ownerDocument
	 */
	this.ownerDocument = '';

	/**
	 * @brief The Node.parentElement read-only property returns the DOM node's parent Element, or null if the node either has no parent, or its parent isn't a DOM Element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/parentElement
	 */
	this.parentElement = '';

	/**
	 * @brief The Node.parentNode read-only property returns the parent of the specified node in the DOM tree.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/parentNode
	 */
	this.parentNode = '';

	/**
	 * @brief The Node.prefix read-only property returns the namespace prefix of the specified node, or null if no prefix is specified. This property is read only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/prefix
	 */
	this.prefix = '';

	/**
	 * @brief The Node.previousSibling read-only property returns the node immediately preceding the specified one in its parent's childNodes list, or null if the specified node is the first in that list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/previousSibling
	 */
	this.previousSibling = '';

	/**
	 * @brief The Node.textContent property represents the text content of a node and its descendants.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/textContent
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
	 * @brief The Node.getUserData() method returns any user DOMUserData set previously on the given node by Node.setUserData().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/getUserData
	 */
	this.getUserData = function() {};

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
	 * @brief The Node.lookupNamespaceURI() method takes a prefix and returns the namespace URI associated with it on the given node if found (and null if not). Supplying null for the prefix will return the default namespace.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/lookupNamespaceURI
	 */
	this.lookupNamespaceURI = function() {};

	/**
	 * @brief The Node.lookupPrefix() method returns a DOMString containing the prefix for a given namespace URI, if present, and null if not. When multiple prefixes are possible, the result is implementation-dependent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/lookupPrefix
	 */
	this.lookupPrefix = function() {};

	/**
	 * @brief The Node.removeChild() method removes a child node from the DOM. Returns removed node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Node/normalize
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

