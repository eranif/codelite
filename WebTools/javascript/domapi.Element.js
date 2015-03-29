
/**
 * @brief The Element interface represents an object of a Document. This interface describes methods and properties common to all kinds of elements. Specific behaviors are described in interfaces which inherit from Element but add additional functionality.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element
 */
function Element() {

	/**
	 * @brief The Element.accessKey property sets the keystroke by which a user can press to jump to this element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/accessKey
	 */
	this.accessKey = '';

	/**
	 * @brief The Element.attributes property returns a live collection of all attribute nodes registered to the specified node. It is a NamedNodeMap, not an Array, so it has no Array methods and the Attr nodes' indexes may differ among browsers. To be more specific, attributes is a key/value pair of strings that represents any information regarding that attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/attributes
	 */
	this.attributes = '';

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
	 * @brief classList returns a token list of the class attribute of the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/classList
	 */
	this.classList = '';

	/**
	 * @brief className gets and sets the value of the class attribute of the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/className
	 */
	this.className = '';

	/**
	 * @brief The Element.clientHeight read-only property returns the inner height of an element in pixels, including padding but not the horizontal scrollbar height, border, or margin.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/clientHeight
	 */
	this.clientHeight = '';

	/**
	 * @brief The width of the left border of an element in pixels. It includes the width of the vertical scrollbar if the text direction of the element is right–to–left and if there is an overflow causing a left vertical scrollbar to be rendered. clientLeft does not include the left margin or the left padding. clientLeft is read-only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/clientLeft
	 */
	this.clientLeft = '';

	/**
	 * @brief The width of the top border of an element in pixels. It does not include the top margin or padding. clientTop is read-only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/clientTop
	 */
	this.clientTop = '';

	/**
	 * @brief The Element.clientWidth property is the inner width of an element in pixels. It includes padding but not the vertical scrollbar (if present, if rendered), border or margin.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/clientWidth
	 */
	this.clientWidth = '';

	/**
	 * @brief The ParentNode.firstElementChild read-only property returns the object's first child Element, or null if there are no child elements.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/firstElementChild
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
	this.innerHTML = '';

	/**
	 * @brief The ParentNode.lastElementChild read-only method returns the object's last child Element or null if there are no child elements.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ParentNode/lastElementChild
	 */
	this.lastElementChild = '';

	/**
	 * @brief name gets or sets the name property of a DOM object; it only applies to the following elements: &lt;a>, &lt;applet>, &lt;button>, &lt;form>, &lt;frame>, &lt;iframe>, &lt;img>, &lt;input>, &lt;map>, &lt;meta>, &lt;object>, &lt;param>, &lt;select>, and &lt;textarea>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/name
	 */
	this.name = '';

	/**
	 * @brief The NonDocumentTypeChildNode.nextElementSibling read-only property returns the element immediately following the specified one in its parent's children list, or null if the specified element is the last one in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NonDocumentTypeChildNode/nextElementSibling
	 */
	this.nextElementSibling = '';

	/**
	 * @brief The onwheel property returns the onwheel event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/onwheel
	 */
	this.onwheel = '';

	/**
	 * @brief The outerHTML attribute of the element DOM interface gets the serialized HTML fragment describing the element including its descendants. It can be set to replace the element with nodes parsed from the given string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/outerHTML
	 */
	this.outerHTML = '';

	/**
	 * @brief The NonDocumentTypeChildNode.previousElementSibling read-only property returns the Element immediately prior to the specified one in its parent's children list, or null if the specified element is the first one in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NonDocumentTypeChildNode/previousElementSibling
	 */
	this.previousElementSibling = '';

	/**
	 * @brief The Element.scrollHeight read-only attribute is a measurement of the height of an element's content, including content not visible on the screen due to overflow. The scrollHeight value is equal to the minimum clientHeight the element would require in order to fit all the content in the viewpoint without using a vertical scrollbar. It includes the element padding but not its margin.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollHeight
	 */
	this.scrollHeight = '';

	/**
	 * @brief The Element.scrollLeft property gets or sets the number of pixels that an element's content is scrolled to the left.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollLeft
	 */
	this.scrollLeft = '';

	/**
	 * @brief The Element.scrollLeftMax read-only property returns a Number representing the maximum left scroll offset possible for the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollLeftMax
	 */
	this.scrollLeftMax = '';

	/**
	 * @brief The Element.scrollTop property gets or sets the number of pixels that the content of an element is scrolled upward. An element's scrollTop is a measurement of the distance of an element's top to its topmost visible content. When an element content does not generate a vertical scrollbar, then its scrollTop value defaults to 0.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollTop
	 */
	this.scrollTop = '';

	/**
	 * @brief The Element.scrollTopMax read-only property returns a Number representing the maximum top scroll offset possible for the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollTopMax
	 */
	this.scrollTopMax = '';

	/**
	 * @brief The Element.scrollWidth read–only property returns either the width in pixels of the content of an element or the width of the element itself, whichever is greater. If the element is wider than its content area (for example, if there are scroll bars for scrolling through the content), the scrollWidth is larger than the clientWidth.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollWidth
	 */
	this.scrollWidth = '';

	/**
	 * @brief Returns the name of the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/tagName
	 */
	this.tagName = '';

	/**
	 * @brief The Element.closest() method returns the closest ancestor of the current element (or the current element itself) which matches the selectors given in parameter. If there isn't such an ancestor, it returns null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/closest
	 */
	this.lement.closest = function() {};

	/**
	 * @brief getAttribute() returns the value of a specified attribute on the element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttribute
	 */
	this.lement.getAttribute = function() {};

	/**
	 * @brief Returns the specified attribute of the specified element, as an Attr node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttributeNode
	 */
	this.lement.getAttributeNode = function() {};

	/**
	 * @brief Returns the Attr node for the attribute with the given namespace and name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttributeNodeNS
	 */
	this.lement.getAttributeNodeNS = function() {};

	/**
	 * @brief The Element.getBoundingClientRect() method returns the size of an element and its position relative to the viewport.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getAttributeNS
	 * @param namespace - The namespace in which to look for the specified attribute.
	 * @param name - The name of the attribute to look for.
	 */
	this.lement.getBoundingClientRect = function(namespace, name) {};

	/**
	 * @brief The Element.getClientRects() method returns a collection of rectangles that indicate the bounding rectangles for each box in a client.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getClientRects
	 */
	this.lement.getClientRects = function() {};

	/**
	 * @brief The Element.getElementsByClassName() method returns a live HTMLCollection containing all child elements which have all of the given class names. When called on the document object, the complete document is searched, including the root node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByClassName
	 */
	this.lement.getElementsByClassName = function() {};

	/**
	 * @brief The Element.getElementsByTagName() method returns a live HTMLCollection of elements with the given tag name. The subtree underneath the specified element is searched, excluding the element itself. The returned list is live, meaning that it updates itself with the DOM tree automatically. Consequently, there is no need to call several times Element.getElementsByTagName() with the same element and arguments.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByTagName
	 */
	this.lement.getElementsByTagName = function() {};

	/**
	 * @brief The Element.getElementsByTagNameNS() method returns a live HTMLCollection of elements with the given tag name belonging to the given namespace. It is similar to Document.getElementsByTagNameNS, except that its search is restricted to descendants of the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/getElementsByTagNameNS
	 */
	this.lement.getElementsByTagNameNS = function() {};

	/**
	 * @brief The Element.hasAttribute() method returns a Boolean value indicating whether the specified element has the specified attribute or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttribute
	 */
	this.lement.hasAttribute = function() {};

	/**
	 * @brief hasAttributeNS returns a boolean value indicating whether the current element has the specified attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttributeNS
	 */
	this.lement.hasAttributeNS = function() {};

	/**
	 * @brief The Element.hasAttributes() method returns a Boolean value, true or false, indicating if the current element has any attributes or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/hasAttributes
	 */
	this.lement.hasAttributes = function() {};

	/**
	 * @brief insertAdjacentHTML() parses the specified text as HTML or XML and inserts the resulting nodes into the DOM tree at a specified position. It does not reparse the element it is being used on and thus it does not corrupt the existing elements inside the element. This, and avoiding the extra step of serialization make it much faster than direct innerHTML manipulation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/insertAdjacentHTML
	 */
	this.lement.insertAdjacentHTML = function() {};

	/**
	 * @brief The Element.matches() method returns true if the element would be selected by the specified selector string; otherwise, returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/matches
	 */
	this.lement.matches = function() {};

	/**
	 * @brief Returns the first element that is a descendant of the element on which it is invoked that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/querySelector
	 */
	this.lement.querySelector = function() {};

	/**
	 * @brief Returns a non-live NodeList of all elements descended from the element on which it is invoked that match the specified group of CSS selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/querySelectorAll
	 */
	this.lement.querySelectorAll = function() {};

	/**
	 * @brief The ChildNode.remove() method removes the object from the tree it belongs to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ChildNode/remove
	 */
	this.hildNode.remove = function() {};

	/**
	 * @brief removeAttribute removes an attribute from the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttribute
	 */
	this.lement.removeAttribute = function() {};

	/**
	 * @brief removeAttributeNode removes the specified attribute from the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttributeNode
	 */
	this.lement.removeAttributeNode = function() {};

	/**
	 * @brief removeAttributeNS removes the specified attribute from an element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeAttributeNS
	 */
	this.lement.removeAttributeNS = function() {};

	/**
	 * @brief Asynchronously requests that the element be made full-screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/requestFullScreen
	 */
	this.lement.requestFullscreen = function() {};

	/**
	 * @brief The Element.requestPointerLock() method allows to asynchronously ask for the pointer to be locked on the given element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/requestPointerLock
	 */
	this.lement.requestPointerLock = function() {};

	/**
	 * @brief The Element.scrollIntoView() method scrolls the current element into the visible area of the browser window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/scrollIntoView
	 * @param alignToTop - Is a Boolean value:   If true, the top of the element will be aligned to the top of the visible area of the scrollable ancestor.  If false, the bottom of the element will be aligned to the bottom of the visible area of the scrollable ancestor.
	 * @param scrollIntoViewOptions - A boolean or an object with the following options:
	 */
	this.lement.scrollIntoView = function(alignToTop, scrollIntoViewOptions) {};

	/**
	 * @brief Adds a new attribute or changes the value of an existing attribute on the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/setAttribute
	 */
	this.lement.setAttribute = function() {};

	/**
	 * @brief setAttributeNode() adds a new Attr node to the specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/setAttributeNode
	 */
	this.lement.setAttributeNode = function() {};

	/**
	 * @brief setAttributeNodeNS adds a new namespaced attribute node to an element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/setAttributeNodeNS
	 */
	this.lement.setAttributeNodeNS = function() {};

	/**
	 * @brief setAttributeNS adds a new attribute or changes the value of an attribute with the given namespace and name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/setAttributeNS
	 */
	this.lement.setAttributeNS = function() {};

	/**
	 * @brief Call this method during the handling of a mousedown event to retarget all mouse events to this element until the mouse button is released or document.releaseCapture() is called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/setCapture
	 */
	this.lement.setCapture = function() {};

}

