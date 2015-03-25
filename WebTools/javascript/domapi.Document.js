
/**
 * @brief Each web page loaded in the browser has its own document object. The Document interface serves as an entry point into the web page's content (the DOM tree, including elements such as &lt;body> and &lt;table>) and provides functionality which is global to the document (such as obtaining the page's URL and creating new elements in the document).
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/Document
 */
function Document() {

	/**
	 * @brief Returns the currently focused element, that is, the element that will get keystroke events if the user types any. This attribute is read only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/activeElement
	 */
	this.activeElement = '';

	/**
	 * @brief Returns or sets the color of an active link in the document body. A link is active during the time between mousedown and mouseup events.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/alinkColor
	 */
	this.alinkColor = '';

	/**
	 * @brief anchors returns a list of all of the anchors in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/anchors
	 */
	this.anchors = '';

	/**
	 * @brief applets returns an ordered list of the applets within a document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/applets
	 */
	this.applets = '';

	/**
	 * @brief document.async can be set to indicate whether a document.load call should be an asynchronous or synchronous request. true is the default value, indicating that documents should be loaded asynchronously.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/async
	 */
	this.async = '';

	/**
	 * @brief bgColor gets/sets the background color of the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/bgColor
	 */
	this.bgColor = '';

	/**
	 * @brief Returns the &lt;body> or &lt;frameset> node of the current document, or null if no such element exists.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/body
	 */
	this.body = '';

	/**
	 * @brief Returns the character encoding of the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/characterSet
	 */
	this.characterSet = '';

	/**
	 * @brief Indicates whether the document is rendered in Quirks mode or Standards mode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/compatMode
	 */
	this.compatMode = '';

	/**
	 * @brief Returns the MIME type that the document is being rendered as.  This may come from HTTP headers or other sources of MIME information, and might be affected by automatic type conversions performed by either the browser or extensions.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/contentType
	 */
	this.contentType = '';

	/**
	 * @brief Returns the &lt;script> element whose script is currently being processed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/currentScript
	 */
	this.currentScript = '';

	/**
	 * @brief In browsers returns the window object associated with the document or null if none available.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/defaultView
	 */
	this.defaultView = '';

	/**
	 * @brief document.designMode controls whether the entire document is editable.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/designMode
	 */
	this.designMode = '';

	/**
	 * @brief The Document.dir property is a DOMString representing the directionality of the text of the document, whether left to right (default) or right to left. Possible values are 'rtl', right to left, and 'ltr', left to right.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/dir
	 */
	this.dir = '';

	/**
	 * @brief Returns the Document Type Declaration (DTD) associated with current document. The returned object implements the DocumentType interface. Use DOMImplementation.createDocumentType() to create a DocumentType.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/doctype
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
	 * @brief The Document.documentURIObject read-only property returns an nsIURI object representing the URI of the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/documentURIObject
	 */
	this.documentURIObject = '';

	/**
	 * @brief Gets/sets the domain portion of the origin of the current document, as used by the same origin policy.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/domain
	 */
	this.domain = '';

	/**
	 * @brief This should return the DOMConfiguration for the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/domConfig
	 */
	this.domConfig = '';

	/**
	 * @brief embeds returns a list of the embedded OBJECTS within the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/embeds
	 */
	this.embeds = '';

	/**
	 * @brief fgColor gets/sets the foreground color, or text color, of the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/fgColor
	 */
	this.fgColor = '';

	/**
	 * @brief forms returns a collection (an HTMLCollection) of the form elements within the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/forms
	 */
	this.forms = '';

	/**
	 * @brief Returns the &lt;head> element of the current document. If there are more than one &lt;head> elements, the first one is returned.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/head
	 */
	this.head = '';

	/**
	 * @brief Returns the height of the &lt;body> element of the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/height
	 */
	this.height = '';

	/**
	 * @brief document.images returns a collection of the images in the current HTML document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/images
	 */
	this.images = '';

	/**
	 * @brief Returns a DOMImplementation object associated with the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/implementation
	 */
	this.implementation = '';

	/**
	 * @brief Returns a string representing the encoding under which the document was parsed (e.g. ISO-8859-1).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/inputEncoding
	 */
	this.inputEncoding = '';

	/**
	 * @brief Returns a string containing the date and time on which the current document was last modified.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/lastModified
	 */
	this.lastModified = '';

	/**
	 * @brief Returns the last enabled style sheet set; this property's value changes whenever the document.selectedStyleSheetSet property is changed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/lastStyleSheetSet
	 */
	this.lastStyleSheetSet = '';

	/**
	 * @brief linkColor gets/sets the color of links within the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/linkColor
	 */
	this.linkColor = '';

	/**
	 * @brief The links property returns a collection of all &lt;area> elements and &lt;a> elements in a document with a value for the href attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/links
	 */
	this.links = '';

	/**
	 * @brief The Document.location read-only property returns a Location object, which contains information about the URL of the document and provides methods for changing that URL and loading another URL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/location
	 */
	this.location = '';

	/**
	 * @brief Reports whether or not the document is currently displaying content in fullscreen mode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozFullScreen
	 */
	this.mozFullScreen = '';

	/**
	 * @brief Returns the Element that is currently being presented in full-screen mode in this document, or null if full-screen mode is not currently in use.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozFullScreenElement
	 */
	this.mozFullScreenElement = '';

	/**
	 * @brief Reports whether or not full-screen mode is available. Full screen mode is available only for a page that has no windowed plug-ins in any of its documents, and if all &lt;iframe> elements which contain the document have their allowfullscreen attribute set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozFullScreenEnabled
	 */
	this.mozFullScreenEnabled = '';

	/**
	 * @brief Indicates whether or not the document is a synthetic one; that is, a document representing a standalone image, video, audio, or the like.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozSyntheticDocument
	 */
	this.mozSyntheticDocument = '';

	/**
	 * @brief Fired when a static &lt;script> element  finishes executing its script. Does not fire if the element is added dynamically, eg with appendChild().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/onafterscriptexecute
	 */
	this.onafterscriptexecute = '';

	/**
	 * @brief Fired when the code in a &lt;script> element declared in an HTML document is about to start executing. Does not fire if the element is added dynamically, eg with appendChild().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/onbeforescriptexecute
	 */
	this.onbeforescriptexecute = '';

	/**
	 * @brief This event handler is called when an offline is fired on body and bubbles up, when navigator.onLine property changes and becomes false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/onoffline
	 */
	this.onoffline = '';

	/**
	 * @brief &quot;online&quot; event is fired on the &lt;body> of each page when the browser switches between online and offline mode. Additionally, the events bubble up from document.body, to document, ending at window. Both events are non-cancellable (you can't prevent the user from coming online, or going offline).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/ononline
	 */
	this.ononline = '';

	/**
	 * @brief The Document.origin read-only property returns the document's origin. In most cases, this property is equivalent to document.defaultView.location.origin.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/origin
	 */
	this.origin = '';

	/**
	 * @brief Returns an HTMLCollection object containing one or more HTMLEmbedElements or null which represent the &lt;embed> elements in the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/plugins
	 */
	this.plugins = '';

	/**
	 * @brief The pointerLockElement property provides the element set as the target for mouse events while the pointer is locked. It is null if lock is pending, pointer is unlocked, or the target is in another document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/pointerLockElement
	 */
	this.pointerLockElement = '';

	/**
	 * @brief When a popup attached via the popup or context attributes is opened, the XUL document's popupNode property is set to the node that was clicked on. This will be the target of the mouse event that activated the popup. If the popup was opened via the keyboard, the popup node may be set to null. Typically, this property will be checked during a popupshowing event handler for a context menu to initialize the menu based on the context.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/popupNode
	 */
	this.popupNode = '';

	/**
	 * @brief Returns the preferred style sheet set as set by the page author.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/preferredStyleSheetSet
	 */
	this.preferredStyleSheetSet = '';

	/**
	 * @brief Returns the URI of the page that linked to this page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/readyState
	 */
	this.referrer = '';

	/**
	 * @brief Returns a list of the &lt;script> elements in the document. The returned object is an HTMLCollection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/scripts
	 */
	this.scripts = '';

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
	 * @brief Gets or sets the title of the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/title
	 */
	this.title = '';

	/**
	 * @brief Returns the node which is the target of the current tooltip.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/tooltipNode
	 */
	this.tooltipNode = '';

	/**
	 * @brief Returns the string URL of the HTML document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/URL
	 */
	this.URL = '';

	/**
	 * @brief Gets/sets the color of links that the user has visited in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/vlinkColor
	 */
	this.vlinkColor = '';

	/**
	 * @brief Returns the width of the &lt;body> element of the current document in pixels.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/width
	 */
	this.width = '';

	/**
	 * @brief Returns the encoding as determined by the XML declaration. Should be null if unspecified or unknown.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/xmlEncoding
	 */
	this.xmlEncoding = '';

	/**
	 * @brief Adopts a node from an external document. The node and its subtree is removed from the document it's in (if any), and its ownerDocument is changed to the current document. The node can then be inserted into the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/adoptNode
	 */
	this.adoptNode = function() {};

	/**
	 * @brief This method is used to retrieve the caret position in a document based on two coordinates. A CaretPosition is returned, containing the found DOM node and the character offset in that node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/caretPositionFromPoint
	 * @param x - Horizontal point on the current viewport where to determine the caret position.
	 * @param y - Vertical point on the current viewport at where to determine the caret position.
	 */
	this.caretPositionFromPoint = function(x, y) {};

	/**
	 * @brief This method used to clear the whole specified document in early (pre-1.0) versions of Mozilla.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/clear
	 */
	this.clear = function() {};

	/**
	 * @brief The document.close() method finishes writing to a document, opened with document.open().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/close
	 */
	this.close = function() {};

	/**
	 * @brief createAttribute creates a new attribute node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createAttribute
	 */
	this.createAttribute = function() {};

	/**
	 * @brief createCDATASection() creates a new CDATA section node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createCDATASection
	 */
	this.createCDATASection = function() {};

	/**
	 * @brief createComment() creates a new comment node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createComment
	 * @param data - A string containing the data to be added to the Comment.
	 */
	this.createComment = function(data) {};

	/**
	 * @brief Creates a new empty DocumentFragment.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createDocumentFragment
	 */
	this.createDocumentFragment = function() {};

	/**
	 * @brief In an HTML document, the Document.createElement() method creates the specified HTML element or an HTMLUnknownElement if the given element name isn't a known one.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createElement
	 */
	this.createElement = function() {};

	/**
	 * @brief Creates an element with the specified namespace URI and qualified name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createElementNS
	 */
	this.createElementNS = function() {};

	/**
	 * @brief Prior to Gecko 7.0 this method showed up as present, due to bug bug 9850, it always only returned null. The only workaround is to create a text node, CDATA section, attribute node value, etc. which has the value referred to by the entity, using Unicode escape sequences or fromCharCode() as necessary.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createEntityReference
	 */
	this.createEntityReference = function() {};

	/**
	 * @brief Creates an event of the type specified. The returned object should be first initialized and can then be passed to element.dispatchEvent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createEvent
	 */
	this.createEvent = function() {};

	/**
	 * @brief This method compiles an XPathExpression which can then be used for (repeated) evaluations.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createExpression
	 */
	this.createExpression = function() {};

	/**
	 * @brief Returns a new NodeIterator object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createNodeIterator
	 */
	this.createNodeIterator = function() {};

	/**
	 * @brief Creates an XPathNSResolver which resolves namespaces with respect to the definitions in scope for a specified node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createNSResolver
	 */
	this.createNSResolver = function() {};

	/**
	 * @brief createProcessingInstruction() creates a new processing instruction node, and returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createProcessingInstruction
	 */
	this.createProcessingInstruction = function() {};

	/**
	 * @brief Returns a new Range object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createRange
	 */
	this.createRange = function() {};

	/**
	 * @brief Creates a new Text node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTextNode
	 */
	this.createTextNode = function() {};

	/**
	 * @brief This method creates and returns a new Touch object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTouch
	 * @param view - The window in which the touch occurred.
	 * @param target - The EventTarget for the touch.
	 * @param identifier - The value for Touch.identifier.
	 * @param pageX - The value for Touch.pageX.
	 * @param pageY - The value for Touch.pageY.
	 * @param screenX - The value for Touch.screenX.
	 * @param screenY - The value for Touch.screenY.
	 * @param clientX - The value for Touch.clientX.
	 * @param clientY - The value for Touch.clientY.
	 * @param radiusX - The value for Touch.radiusX.
	 * @param radiusY - The value for Touch.radiusY.
	 * @param rotationAngle - The value for Touch.rotationAngle.
	 * @param force - The value for Touch.force.
	 */
	this.createTouch = function(view, target, identifier, pageX, pageY, screenX, screenY, clientX, clientY, radiusX, radiusY, rotationAngle, force) {};

	/**
	 * @brief This method creates and returns a new TouchList object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTouchList
	 * @param touches - Either a single Touch object or an array of Touch objects.
	 */
	this.createTouchList = function(touches) {};

	/**
	 * @brief The Document.createTreeWalker() creator method returns a newly created TreeWalker object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/createTreeWalker
	 * @param root - Is the root Node of this TreeWalker traversal. Typically this will be an element owned by the document.
	 * @param whatToShow Optional - Is an optional unsigned long representing a bitmask created by combining the constant properties of NodeFilter. It is a convenient way of filtering for certain types of node. It defaults to 0xFFFFFFFF representing the SHOW_ALL constant.              Constant     Numerical value     Description             NodeFilter.SHOW_ALL     -1 (that is the max value of unsigned long)     Shows all nodes.             NodeFilter.SHOW_ATTRIBUTE       2     Shows attribute Attr nodes. This is meaningful only when creating a TreeWalker with an Attr node as its root; in this case, it means that the attribute node will appear in the first position of the iteration or traversal. Since attributes are never children of other nodes, they do not appear when traversing over the document tree.             NodeFilter.SHOW_CDATA_SECTION       8     Shows CDATASection nodes.             NodeFilter.SHOW_COMMENT     128     Shows Comment nodes.             NodeFilter.SHOW_DOCUMENT     256     Shows Document nodes.             NodeFilter.SHOW_DOCUMENT_FRAGMENT     1024     Shows DocumentFragment nodes.             NodeFilter.SHOW_DOCUMENT_TYPE     512     Shows DocumentType nodes.             NodeFilter.SHOW_ELEMENT     1     Shows Element nodes.             NodeFilter.SHOW_ENTITY       32     Shows Entity nodes. This is meaningful only when creating a TreeWalker with an Entity node as its root; in this case, it means that the Entity node will appear in the first position of the traversal. Since entities are not part of the document tree, they do not appear when traversing over the document tree.             NodeFilter.SHOW_ENTITY_REFERENCE       16     Shows EntityReference nodes.             NodeFilter.SHOW_NOTATION       2048     Shows Notation nodes. This is meaningful only when creating a TreeWalker with a Notation node as its root; in this case, it means that the Notation node will appear in the first position of the traversal. Since entities are not part of the document tree, they do not appear when traversing over the document tree.             NodeFilter.SHOW_PROCESSING_INSTRUCTION     64     Shows ProcessingInstruction nodes.             NodeFilter.SHOW_TEXT     4     Shows Text nodes.
	 * @param filter Optional - Is an optional NodeFilter, that is an object with a method acceptNode, which is called by the TreeWalker to determine whether or not to accept a node that has passed the whatToShow check.
	 * @param entityReferenceExpansion Optional - Is a Boolean flag indicating if when discarding an EntityReference its whole sub-tree must be discarded at the same time.
	 */
	this.createTreeWalker = function(root, whatToShow Optional, filter Optional, entityReferenceExpansion Optional) {};

	/**
	 * @brief Returns the element from the document whose elementFromPoint method is being called which is the topmost element which lies under the given point.  To get an element, specify the point via coordinates, in CSS pixels, relative to the upper-left-most point in the window or frame containing the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/elementFromPoint
	 */
	this.elementFromPoint = function() {};

	/**
	 * @brief Enables the style sheets matching the specified name in the current style sheet set, and disables all other style sheets (except those without a title, which are always enabled).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/enableStyleSheetsForSet
	 * @param name - The name of the style sheets to enable. All style sheets with a title that match this name will be enabled, while all others that have a title will be disabled. Specify an empty string for the name parameter to disable all alternate and preferred style sheets (but not the persistent style sheets; that is, those with no title attribute).
	 */
	this.enableStyleSheetsForSet = function(name) {};

	/**
	 * @brief Returns an XPathResult based on an XPath expression and other given parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/evaluate
	 */
	this.evaluate = function() {};

	/**
	 * @brief When an HTML document has been switched to designMode, the document object exposes the execCommand method which allows one to run commands to manipulate the contents of the editable region. Most commands affect the document's selection (bold, italics, etc), while others insert new elements (adding a link) or affect an entire line (indenting). When using contentEditable, calling execCommand will affect the currently active editable element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/execCommand
	 * @param aCommandName - A DOMString representing the name of the command
	 * @param aShowDefaultUI - A Boolean indicating whether the default user interface should be shown. This is not implemented in Mozilla.
	 * @param aValueArgument - A DOMString representing some commands (such as insertimage) require an extra value argument (the image's url). Pass an argument of null if no argument is needed.
	 */
	this.execCommand = function(aCommandName, aShowDefaultUI, aValueArgument) {};

	/**
	 * @brief The exitPointerLock asynchronously releases a pointer lock previously requested through Element.requestPointerLock.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/exitPointerLock
	 */
	this.exitPointerLock = function() {};

	/**
	 * @brief Returns a boxObject (x, y, width, height) for a specified element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getBoxObjectFor
	 */
	this.getBoxObjectFor = function() {};

	/**
	 * @brief Returns a reference to the element by its ID.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementById
	 */
	this.getElementById = function() {};

	/**
	 * @brief Returns an array-like object of all child elements which have all of the given class names. When called on the document object, the complete document is searched, including the root node. You may also call getElementsByClassName() on any element; it will return only elements which are descendants of the specified root element with the given class names.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByClassName
	 */
	this.getElementsByClassName = function() {};

	/**
	 * @brief Returns a nodelist collection with a given name in the (X)HTML document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByName
	 */
	this.getElementsByName = function() {};

	/**
	 * @brief Returns an HTMLCollection of elements with the given tag name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/document/getElementsByTagName
	 */
	this.getElementsByTagName = function() {};

	/**
	 * @brief Returns a list of elements with the given tag name belonging to the given namespace. The complete document is searched, including the root node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementsByTagNameNS
	 */
	this.getElementsByTagNameNS = function() {};

	/**
	 * @brief This method functions identically to the Window.getSelection() method; it returns a Selection object representing the text currently selected in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/getSelection
	 */
	this.getSelection = function() {};

	/**
	 * @brief Returns a Boolean value indicating whether the document or any element inside the document has focus. This method can be used to determine whether the active element in a document has focus.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/hasFocus
	 */
	this.hasFocus = function() {};

	/**
	 * @brief Creates a copy of a node from an external document that can be inserted into the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/importNode
	 */
	this.importNode = function() {};

	/**
	 * @brief Loads a XUL overlay and merges it with the current document, notifying an observer when the merge is complete.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/loadOverlay
	 */
	this.loadOverlay = function() {};

	/**
	 * @brief Takes the document out of full-screen mode; this is used to reverse the effects of a call to make an element in the document full-screen using its element.mozRequestFullScreen() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozCancelFullScreen
	 */
	this.mozCancelFullScreen = function() {};

	/**
	 * @brief Changes the element being used as the CSS background for a background with a given background element ID.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/mozSetImageElement
	 */
	this.mozSetImageElement = function() {};

	/**
	 * @brief The document.open() method opens a document for writing.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/open
	 */
	this.open = function() {};

	/**
	 * @brief Reports whether or not the specified editor query command is supported by the browser.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/queryCommandSupported
	 */
	this.queryCommandSupported = function() {};

	/**
	 * @brief Returns the first element within the document (using depth-first pre-order traversal of the document's nodes) that matches the specified group of selectors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/querySelector
	 */
	this.querySelector = function() {};

	/**
	 * @brief Returns a list of the elements within the document (using depth-first pre-order traversal of the document's nodes) that match the specified group of selectors. The object returned is a NodeList.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/querySelectorAll
	 */
	this.querySelectorAll = function() {};

	/**
	 * @brief The Document.registerElement() method registers a new custom element in the browser and returns a constructor for the new element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/registerElement
	 * @param tag-name - The name of the custom element. The name must contain a dash (-), for example my-tag.
	 * @param options Optional - An object that names the prototype to base the custom element on, and an existing tag to extend. Both of these are optional.
	 */
	this.registerElement = function(tag-name, options Optional) {};

	/**
	 * @brief Releases mouse capture if it's currently enabled on an element within this document. Enabling mouse capture on an element is done by calling element.setCapture().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/releaseCapture
	 */
	this.releaseCapture = function() {};

	/**
	 * @brief Writes a string of text to a document stream opened by document.open().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/write
	 */
	this.write = function() {};

	/**
	 * @brief Writes a string of text followed by a newline character to a document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Document/writeln
	 */
	this.writeln = function() {};

}

