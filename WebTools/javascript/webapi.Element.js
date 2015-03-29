
/**
 * @brief The Element interface represents an object of a Document. This interface describes methods and properties common to all kinds of elements. Specific behaviors are described in interfaces which inherit from Element but add additional functionality.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element
 */
function Element() {

	/**
	 * @brief The Element.attributes property returns a live collection of all attribute nodes registered to the specified node. It is a NamedNodeMap, not an Array, so it has no Array methods and the Attr nodes' indexes may differ among browsers. To be more specific, attributes is a key/value pair of strings that represents any information regarding that attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/attributes
	 */
	this.childElementCount = '';

	/**
	 * @brief The ParentNode.children read-only property returns a live HTMLCollection of child elements of the given object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/children
	 */
	this.children = '';

	/**
	 * @brief classList returns a token list of the class attribute of the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/classList
	 */
	this.className = '';

	/**
	 * @brief The Element.clientHeight read-only property returns the inner height of an element in pixels, including padding but not the horizontal scrollbar height, border, or margin.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/clientHeight
	 */
	this.firstElementChild = '';

	/**
	 * @brief The Element.id property represents the element's identifier, reflecting the id global attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/id
	 */
	this.id = '';

	/**
	 * @brief The Element.innerHTML property sets or gets the HTML syntax describing the element's descendants.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML
	 */
	this.lastElementChild = '';

	/**
	 * @brief The NonDocumentTypeChildNode.nextElementSibling read-only property returns the element immediately following the specified one in its parent's children list, or null if the specified element is the last one in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NonDocumentTypeChildNode/nextElementSibling
	 */
	this.nextElementSibling = '';

	/**
	 * @brief The outerHTML attribute of the element DOM interface gets the serialized HTML fragment describing the element including its descendants. It can be set to replace the element with nodes parsed from the given string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/outerHTML
	 */
	this.previousElementSibling = '';

	/**
	 * @brief The EventTarget.addEventListener() method registers the specified listener on the EventTarget it's called on. The event target may be an Element in a document, the Document itself, a Window, or any other object that supports events (such as XMLHttpRequest).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
	 */
	this.addEventListener = function() {};

	/**
	 * @brief The Element.closest() method returns the closest ancestor of the current element (or the current element itself) which matches the selectors given in parameter. If there isn't such an ancestor, it returns null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/closest
	 */
	this.closest = function() {};

	/**
	 * @brief Testing to save this page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/createShadowRoot
	 */
	this.createShadowRoot = function() {};

	/**
	 * @brief Dispatches an Event at the specified EventTarget, invoking the affected EventListeners in the appropriate order. The normal event processing rules (including the capturing and optional bubbling phase) apply to events dispatched manually with dispatchEvent().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/dispatchEvent
	 */
	this.dispatchEvent = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/find
	 */
	this.find = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/findAll
	 */
	this.findAll = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Animatable/getAnimationPlayers
	 */
	this.getAnimationPlayers = function() {};

	/**
	 * @brief getAttribute() returns the value of a specified attribute on the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttribute
	 */
	this.getAttribute = function() {};

	/**
	 * @brief The Object constructor creates an object wrapper.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttributeNS" title='getAttributeNS returns the string value of the attribute with the specified namespace and name. If the named attribute does not exist, the value returned will either be null or "" (the empty string); see Notes for details.'><code>Element.getAttributeNS()</code></a></dt>
 <dd>Retrieves the value of the attribute with the specified name and namespace, from the current node and returns it as an <a href=
	 */
	this.bject

	/**
	 * @brief Returns the Attr node for the attribute with the given namespace and name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttributeNodeNS
	 */
	this.getAttributeNodeNS = function() {};

	/**
	 * @brief The Element.getBoundingClientRect() method returns the size of an element and its position relative to the viewport.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getBoundingClientRect
	 */
	this.getBoundingClientRect = function() {};

	/**
	 * @brief The Element.getClientRects() method returns a collection of rectangles that indicate the bounding rectangles for each box in a client.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getClientRects
	 */
	this.getClientRects = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getDestinationInsertionPoints
	 */
	this.getDestinationInsertionPoints = function() {};

	/**
	 * @brief The Element.getElementsByClassName() method returns a live HTMLCollection containing all child elements which have all of the given class names. When called on the document object, the complete document is searched, including the root node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByClassName
	 */
	this.getElementsByClassName = function() {};

	/**
	 * @brief The Element.getElementsByTagName() method returns a live HTMLCollection of elements with the given tag name. The subtree underneath the specified element is searched, excluding the element itself. The returned list is live, meaning that it updates itself with the DOM tree automatically. Consequently, there is no need to call several times Element.getElementsByTagName() with the same element and arguments.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByTagName
	 */
	this.getElementsByTagName = function() {};

	/**
	 * @brief The Element.getElementsByTagNameNS() method returns a live HTMLCollection of elements with the given tag name belonging to the given namespace. It is similar to Document.getElementsByTagNameNS, except that its search is restricted to descendants of the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByTagNameNS
	 */
	this.getElementsByTagNameNS = function() {};

	/**
	 * @brief The Element.hasAttribute() method returns a Boolean value indicating whether the specified element has the specified attribute or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttribute
	 */
	this.hasAttribute = function() {};

	/**
	 * @brief hasAttributeNS returns a boolean value indicating whether the current element has the specified attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttributeNS
	 */
	this.hasAttributeNS = function() {};

	/**
	 * @brief insertAdjacentHTML() parses the specified text as HTML or XML and inserts the resulting nodes into the DOM tree at a specified position. It does not reparse the element it is being used on and thus it does not corrupt the existing elements inside the element. This, and avoiding the extra step of serialization make it much faster than direct innerHTML manipulation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/insertAdjacentHTML
	 */
	this.lement.insertAdjacentHTML

	/**
	 * @brief The Element.matches() method returns true if the element would be selected by the specified selector string; otherwise, returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/matches
	 */
	this.matches = function() {};

	/**
	 * @brief Returns the first element that is a descendant of the element on which it is invoked that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/querySelector
	 */
	this.querySelector = function() {};

	/**
	 * @brief Returns a non-live NodeList of all elements descended from the element on which it is invoked that match the specified group of CSS selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/querySelectorAll
	 */
	this.lement.querySelectorAll

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/releasePointerCapture
	 */
	this.lement.releasePointerCapture

	/**
	 * @brief The ChildNode.remove() method removes the object from the tree it belongs to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ChildNode/remove
	 */
	this.remove = function() {};

	/**
	 * @brief removeAttribute removes an attribute from the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttribute
	 */
	this.removeAttribute = function() {};

	/**
	 * @brief removeAttributeNS removes the specified attribute from an element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttributeNS
	 */
	this.removeAttributeNS = function() {};

	/**
	 * @brief removeAttributeNode removes the specified attribute from the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttributeNode
	 */
	this.removeAttributeNode = function() {};

	/**
	 * @brief Removes the event listener previously registered with EventTarget.addEventListener().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/removeEventListener
	 */
	this.removeEventListener = function() {};

	/**
	 * @brief Asynchronously requests that the element be made full-screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/requestFullscreen
	 */
	this.requestFullscreen = function() {};

	/**
	 * @brief The Element.requestPointerLock() method allows to asynchronously ask for the pointer to be locked on the given element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/requestPointerLock
	 */
	this.requestPointerLock = function() {};

}

