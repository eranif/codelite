
/**
 * @brief Each web page loaded in the browser has its own document object. The Document interface serves as an entry point into the web page's content (the DOM tree, including elements such as &lt;body> and &lt;table>) and provides functionality which is global to the document (such as obtaining the page's URL and creating new elements in the document).
 * @link https://developer.mozilla.org/en-US/docs/Web/API/document
 */
function Document() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/all
	 */
	this.doctype = '';

	/**
	 * @brief The Document.documentElement read-only property returns the Element that is the root element of the document (for example, the &lt;html> element for HTML documents).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/documentElement
	 */
	this.documentElement = '';

	/**
	 * @brief Returns the document location as string. It is read-only per DOM4 specification.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/documentURI
	 */
	this.documentURI = '';

	/**
	 * @brief This should return the DOMConfiguration for the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/domConfig
	 */
	this.implementation = '';

	/**
	 * @brief Returns a string representing the encoding under which the document was parsed (e.g. ISO-8859-1).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/inputEncoding
	 */
	this.lastStyleSheetSet = '';

	/**
	 * @brief Indicates whether or not the document is a synthetic one; that is, a document representing a standalone image, video, audio, or the like.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozSyntheticDocument
	 */
	this.preferredStyleSheetSet = '';

	/**
	 * @brief Indicates the name of the style sheet set that's currently in use.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/selectedStyleSheetSet
	 */
	this.selectedStyleSheetSet = '';

	/**
	 * @brief The Document.styleSheets read-only property returns a StyleSheetList of CSSStyleSheet objects for stylesheets explicitly linked into or embedded in a document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/styleSheets
	 */
	this.styleSheets = '';

	/**
	 * @brief Returns a live list of all of the currently-available style sheet sets.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/styleSheetSets
	 */
	this.styleSheetSets = '';

	/**
	 * @brief Adopts a node from an external document. The node and its subtree is removed from the document it's in (if any), and its ownerDocument is changed to the current document. The node can then be inserted into the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/adoptNode
	 */
	this.ocument.adoptNode(Node node)

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/captureEvents
	 */
	this.ocument.captureEvents(String eventName)

	/**
	 * @brief This method is used to retrieve the caret position in a document based on two coordinates. A CaretPosition is returned, containing the found DOM node and the character offset in that node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/caretPositionFromPoint
	 * @param x - Horizontal point on the current viewport where to determine the caret position.
	 * @param y - Vertical point on the current viewport at where to determine the caret position.
	 */
	this.ocument.caretPositionFromPoint(Number x, Number y)

	/**
	 * @brief createAttribute creates a new attribute node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createAttribute
	 */
	this.ocument.createAttribute(String name)

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createAttributeNS
	 */
	this.ocument.createAttributeNS(String namespace, String name)

	/**
	 * @brief createCDATASection() creates a new CDATA section node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createCDATASection
	 */
	this.ocument.createCDATASection(String data)

	/**
	 * @brief createComment() creates a new comment node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createComment
	 * @param data - A string containing the data to be added to the Comment.
	 */
	this.ocument.createComment(String comment)

	/**
	 * @brief Creates a new empty DocumentFragment.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createDocumentFragment
	 */
	this.createDocumentFragment = function() {};

	/**
	 * @brief In an HTML document, the Document.createElement() method creates the specified HTML element or an HTMLUnknownElement if the given element name isn't a known one.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createElement
	 */
	this.ocument.createElement(String name)

	/**
	 * @brief Creates an element with the specified namespace URI and qualified name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createElementNS
	 */
	this.ocument.createElementNS(String namespace, String name)

	/**
	 * @brief Prior to Gecko 7.0 this method showed up as present, due to bug bug 9850, it always only returned null. The only workaround is to create a text node, CDATA section, attribute node value, etc. which has the value referred to by the entity, using Unicode escape sequences or fromCharCode() as necessary.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createEntityReference
	 */
	this.ocument.createEntityReference(String name)

	/**
	 * @brief Creates an event of the type specified. The returned object should be first initialized and can then be passed to element.dispatchEvent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createEvent
	 */
	this.ocument.createEvent(String interface)

	/**
	 * @brief Returns a new NodeIterator object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createNodeIterator
	 */
	this.ocument.createNodeIterator(Node root[', Number whatToShow[', NodeFilter filter']'])

	/**
	 * @brief createProcessingInstruction() creates a new processing instruction node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createProcessingInstruction
	 */
	this.ocument.createProcessingInstruction(String target, String data)

	/**
	 * @brief Returns a new Range object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createRange
	 */
	this.createRange = function() {};

	/**
	 * @brief Creates a new Text node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTextNode
	 */
	this.ocument.createTextNode(String text)

	/**
	 * @brief The Document.createTreeWalker() creator method returns a newly created TreeWalker object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTreeWalker
	 * @param root - Is the root Node of this TreeWalker traversal. Typically this will be an element owned by the document.
	 * @param whatToShow Optional - Is an optional unsigned long representing a bitmask created by combining the constant properties of NodeFilter. It is a convenient way of filtering for certain types of node. It defaults to 0xFFFFFFFF representing the SHOW_ALL constant.
  
   
    
     Constant
     Numerical value
     Description
    
    
     NodeFilter.SHOW_ALL
     -1 (that is the max value of unsigned long)
     Shows all nodes.
    
    
     NodeFilter.SHOW_ATTRIBUTE  
     2
     Shows attribute Attr nodes. This is meaningful only when creating a TreeWalker with an Attr node as its root; in this case, it means that the attribute node will appear in the first position of the iteration or traversal. Since attributes are never children of other nodes, they do not appear when traversing over the document tree.
    
    
     NodeFilter.SHOW_CDATA_SECTION  
     8
     Shows CDATASection nodes.
    
    
     NodeFilter.SHOW_COMMENT
     128
     Shows Comment nodes.
    
    
     NodeFilter.SHOW_DOCUMENT
     256
     Shows Document nodes.
    
    
     NodeFilter.SHOW_DOCUMENT_FRAGMENT
     1024
     Shows DocumentFragment nodes.
    
    
     NodeFilter.SHOW_DOCUMENT_TYPE
     512
     Shows DocumentType nodes.
    
    
     NodeFilter.SHOW_ELEMENT
     1
     Shows Element nodes.
    
    
     NodeFilter.SHOW_ENTITY  
     32
     Shows Entity nodes. This is meaningful only when creating a TreeWalker with an Entity node as its root; in this case, it means that the Entity node will appear in the first position of the traversal. Since entities are not part of the document tree, they do not appear when traversing over the document tree.
    
    
     NodeFilter.SHOW_ENTITY_REFERENCE  
     16
     Shows EntityReference nodes.
    
    
     NodeFilter.SHOW_NOTATION  
     2048
     Shows Notation nodes. This is meaningful only when creating a TreeWalker with a Notation node as its root; in this case, it means that the Notation node will appear in the first position of the traversal. Since entities are not part of the document tree, they do not appear when traversing over the document tree.
    
    
     NodeFilter.SHOW_PROCESSING_INSTRUCTION
     64
     Shows ProcessingInstruction nodes.
    
    
     NodeFilter.SHOW_TEXT
     4
     Shows Text nodes.
	 * @param filter Optional - Is an optional NodeFilter, that is an object with a method acceptNode, which is called by the TreeWalker to determine whether or not to accept a node that has passed the whatToShow check.
	 * @param entityReferenceExpansion Optional - Is a Boolean flag indicating if when discarding an EntityReference its whole sub-tree must be discarded at the same time.
	 */
	this.ocument.createTreeWalker(Node root[', Number whatToShow[', NodeFilter filter']'])

	/**
	 * @brief Returns the element from the document whose elementFromPoint method is being called which is the topmost element which lies under the given point.  To get an element, specify the point via coordinates, in CSS pixels, relative to the upper-left-most point in the window or frame containing the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/elementFromPoint
	 */
	this.ocument.elementFromPoint(Number x, Number y)

	/**
	 * @brief Enables the style sheets matching the specified name in the current style sheet set, and disables all other style sheets (except those without a title, which are always enabled).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/enableStyleSheetsForSet
	 * @param name - The name of the style sheets to enable. All style sheets with a title that match this name will be enabled, while all others that have a title will be disabled. Specify an empty string for the name parameter to disable all alternate and preferred style sheets (but not the persistent style sheets; that is, those with no title attribute).
	 */
	this.ocument.enableStyleSheetsForSet(String name)

	/**
	 * @brief The exitPointerLock asynchronously releases a pointer lock previously requested through Element.requestPointerLock.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/exitPointerLock
	 */
	this.exitPointerLock = function() {};

	/**
	 * @brief Returns an array-like object of all child elements which have all of the given class names. When called on the document object, the complete document is searched, including the root node. You may also call getElementsByClassName() on any element; it will return only elements which are descendants of the specified root element with the given class names.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByClassName
	 */
	this.ocument.getElementsByClassName(String className)

	/**
	 * @brief Returns an HTMLCollection of elements with the given tag name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByTagName
	 */
	this.ocument.getElementsByTagName(String tagName)

	/**
	 * @brief Returns a list of elements with the given tag name belonging to the given namespace. The complete document is searched, including the root node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByTagNameNS
	 */
	this.ocument.getElementsByTagNameNS(String namespace, String tagName)

	/**
	 * @brief Creates a copy of a node from an external document that can be inserted into the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/importNode
	 */
	this.ocument.importNode(Node node, Boolean deep)

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/normalizeDocument
	 */
	this.normalizeDocument = function() {};

	/**
	 * @brief Releases mouse capture if it's currently enabled on an element within this document. Enabling mouse capture on an element is done by calling element.setCapture().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/releaseCapture
	 */
	this.releaseCapture = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/releaseEvents
	 */
	this.ocument.releaseEvents

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/routeEvent
	 */
	this.ocument.routeEvent

	/**
	 * @brief Changes the element being used as the CSS background for a background with a given background element ID.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozSetImageElement
	 */
	this.ocument.mozSetImageElement

}

