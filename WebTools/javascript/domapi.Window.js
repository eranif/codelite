
/**
 * @brief The window object represents a window containing a DOM document; the document property points to the DOM document loaded in that window.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window
 */
function Window() {

	/**
	 * @brief Returns a reference to the application cache object for the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/applicationCache
	 */
	this.applicationCache = '';

	/**
	 * @brief The Window.caches read-only property returns the CacheStorage object associated with the current origin. This object enables service worker functionality such as storing assets for offline use, and generating custom responses to requests.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/caches
	 */
	this.caches = '';

	/**
	 * @brief This read-only property indicates whether the referenced window is closed or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/closed
	 */
	this.closed = '';

	/**
	 * @brief Returns the XUL controllers of the chrome window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/controllers
	 */
	this.controllers = '';

	/**
	 * @brief The Window.crypto read-only property returns the Crypto object associated to the global object. This object allows web pages access to certain cryptographic related services.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/crypto
	 */
	this.crypto = '';

	/**
	 * @brief Gets/sets the status bar text for the given window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/defaultStatus
	 */
	this.defaultStatus = '';

	/**
	 * @brief The Window.devicePixelRatio read-only property returns the ratio of the (vertical) size of one physical pixel on the current display device to the size of one CSS pixel. Note that as the page is zoomed in the number of device pixels that one CSS pixel covers increases, and therefore the value of devicePixelRatio will also increase.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/devicePixelRatio
	 */
	this.devicePixelRatio = '';

	/**
	 * @brief The dialogArguments property returns the parameters that were passed into the window.showModalDialog() method. This lets you determine what parameters were specified when the modal dialog was created.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/dialogArguments
	 */
	this.dialogArguments = '';

	/**
	 * @brief Returned the window personalbar toolbar object. Use the window.personalbar instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/directories
	 */
	this.directories = '';

	/**
	 * @brief Returns a reference to the document contained in the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/document
	 */
	this.document = '';

	/**
	 * @brief Returns the element (such as &lt;iframe> or &lt;object>) in which the window is embedded, or null if the window is top-level.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/frameElement
	 */
	this.frameElement = '';

	/**
	 * @brief Returns the window itself, which is an array-like object, listing the direct sub-frames of the current window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/frames
	 */
	this.frames = '';

	/**
	 * @brief This property indicates whether the window is displayed in full screen mode or not. It is only reliable in Gecko 1.9 (Firefox 3) and later, see the Notes below.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/fullScreen
	 */
	this.fullScreen = '';

	/**
	 * @brief The Window.history read-only property returns a reference to the History object, which provides an interface for manipulating the browser session history (pages visited in the tab or frame that the current page is loaded in).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/history
	 */
	this.history = '';

	/**
	 * @brief An IDBFactory object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBEnvironment/indexedDB
	 */
	this.indexedDB = '';

	/**
	 * @brief Height (in pixels) of the browser window viewport including, if rendered, the horizontal scrollbar.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/innerHeight
	 */
	this.innerHeight = '';

	/**
	 * @brief Width (in pixels) of the browser window viewport including, if rendered, the vertical scrollbar.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/innerWidth
	 */
	this.innerWidth = '';

	/**
	 * @brief Returns the number of frames (either frame or iframe elements) in the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/length
	 */
	this.length = '';

	/**
	 * @brief The localStorage property allows you to access a local Storage object. localStorage is similar to sessionStorage. The only difference is that, while data stored in localStorage has no expiration time, data stored in sessionStorage gets cleared when the browsing session ends - that is when the browser is closed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage
	 */
	this.localStorage = '';

	/**
	 * @brief The Window.location read-only property returns a Location object with information about the current location of the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/location
	 */
	this.location = '';

	/**
	 * @brief Returns the locationbar object, whose visibility can be checked.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/locationbar
	 */
	this.locationbar = '';

	/**
	 * @brief Returns the menubar object, whose visibility can be checked.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/menubar
	 */
	this.menubar = '';

	/**
	 * @brief Returns the message manager object for this window. See The message manager for details.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/messageManager
	 */
	this.messageManager = '';

	/**
	 * @brief Returns the time, in milliseconds since the epoch, at which animations started now should be considered to have started. This value should be used instead of, for example, Date.now(), because this value will be the same for all animations started in this window during this refresh interval, allowing them to remain in sync with one another.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/mozAnimationStartTime
	 */
	this.mozAnimationStartTime = '';

	/**
	 * @brief Gets the X coordinate of the top-left corner of the window's viewport, in screen coordinates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/mozInnerScreenX
	 */
	this.mozInnerScreenX = '';

	/**
	 * @brief Gets the Y coordinate of the top-left corner of the window's viewport, in screen coordinates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/mozInnerScreenY
	 */
	this.mozInnerScreenY = '';

	/**
	 * @brief Returns the number of times the current document has been painted to the screen in this window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/mozPaintCount
	 */
	this.mozPaintCount = '';

	/**
	 * @brief Gets/sets the name of the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/name
	 */
	this.name = '';

	/**
	 * @brief The Window.navigator read-only property returns a reference to the Navigator object, which can be queried for information about the application running the script.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/navigator
	 */
	this.navigator = '';

	/**
	 * @brief An event handler for abort events sent to the window. (Not available with Firefox 2 or Safari)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onabort
	 */
	this.onabort = '';

	/**
	 * @brief The WindowEventHandlers.onafterprint property sets and returns the onafterprint EventHandler for the current window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onafterprint
	 */
	this.onafterprint = '';

	/**
	 * @brief An event that fires when a window is about to unload its resources. The document is still visible and the event is still cancelable.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onbeforeunload
	 */
	this.onbeforeunload = '';

	/**
	 * @brief The onblur property returns the onBlur event handler code, if any, that exists on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onblur
	 */
	this.onblur = '';

	/**
	 * @brief The onchange property sets and returns the event handler for the change event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onchange
	 */
	this.onchange = '';

	/**
	 * @brief The onclick property returns the click event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onclick
	 */
	this.onclick = '';

	/**
	 * @brief An event handler for close events sent to the window. (Not available with Firefox 2 or Safari)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onclose
	 */
	this.onclose = '';

	/**
	 * @brief The ondblclick property returns the onDblClick event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oncontextmenu
	 */
	this.ondblclick = '';

	/**
	 * @brief Specifies an event listener to receive devicelight events. These events occur when the device's light level sensor detects a change in the intensity of the ambient light level.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/ondevicelight
	 */
	this.ondevicelight = '';

	/**
	 * @brief An event handler for the devicemotion events sent to the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/ondevicemotion
	 */
	this.ondevicemotion = '';

	/**
	 * @brief An event handler for the deviceorientation events sent to the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/ondeviceorientation
	 */
	this.ondeviceorientation = '';

	/**
	 * @brief Specifies an event listener to receive deviceproximity events. These events occur when the device sensor detects that an object becomes closer to or farther from the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/ondeviceproximity
	 */
	this.ondeviceproximity = '';

	/**
	 * @brief An event handler for drag and drop events sent to the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/ondragdrop
	 */
	this.ondragdrop = '';

	/**
	 * @brief An event handler for runtime script errors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onerror
	 */
	this.onerror = '';

	/**
	 * @brief The onfocus property returns the onFocus event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onfocus
	 */
	this.onfocus = '';

	/**
	 * @brief The hashchange event fires when a window's hash changes (see location.hash).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onhashchange
	 */
	this.onhashchange = '';

	/**
	 * @brief An event handler for the input event on the window. The input event is raised when an &lt;input> element value changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oninput
	 */
	this.oninput = '';

	/**
	 * @brief The onkeydown property returns the onKeyDown event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeydown
	 */
	this.onkeydown = '';

	/**
	 * @brief The onkeypress property sets and returns the onKeyPress event handler code for the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeypress
	 */
	this.onkeypress = '';

	/**
	 * @brief The onkeyup property returns the onKeyUp event handler code for the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeyup
	 */
	this.onkeyup = '';

	/**
	 * @brief The WindowEventHandlers.onlanguagechange event handler is a property containing the code to execute when the languagechange event, of type Event, is received by the object implementing this interface, usually a Window, an HTMLBodyElement, or an HTMLIFrameElement. Such an event is sent by the browser to inform that the preferred languages list has been updated. The list is accessible via NavigatorLanguage.languages.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onlanguagechange
	 */
	this.onlanguagechange = '';

	/**
	 * @brief An event handler for the load event of a window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onload
	 */
	this.onload = '';

	/**
	 * @brief The onmousedown property returns the onmousedown event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmousedown
	 */
	this.onmousedown = '';

	/**
	 * @brief The onmousemove property returns the mousemove event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmousemove
	 */
	this.onmousemove = '';

	/**
	 * @brief The onmouseout property returns the onMouseOut event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseout
	 */
	this.onmouseout = '';

	/**
	 * @brief The onmouseover property returns the onMouseOver event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseover
	 */
	this.onmouseover = '';

	/**
	 * @brief The onmouseup property returns the onMouseUp event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseup
	 */
	this.onmouseup = '';

	/**
	 * @brief An event handler for the MozBeforePaint event. This is used in concert with the window.mozRequestAnimationFrame() method to perform smooth, synchronized animations from JavaScript code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/onmozbeforepaint
	 */
	this.onmozbeforepaint = '';

	/**
	 * @brief An event handler for the moztimechange events sent to the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/onmoztimechange
	 */
	this.onmoztimechange = '';

	/**
	 * @brief An event handler for the paint event on the window. Not working in Gecko-based applications currently, see Notes section!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/onpaint
	 */
	this.onpaint = '';

	/**
	 * @brief An event handler for the popstate event on the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onpopstate
	 */
	this.onpopstate = '';

	/**
	 * @brief The GlobalEventHandlers.onreset property contains an EventHandler triggered when a reset event is received.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onreset
	 */
	this.onreset = '';

	/**
	 * @brief The GlobalEventHandlers.onreset property contains an EventHandler triggered when a resize event is received.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onresize
	 */
	this.onresize = '';

	/**
	 * @brief An event handler for scroll events on element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onscroll
	 */
	this.onscroll = '';

	/**
	 * @brief An event handler for the select event on the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onselect
	 */
	this.onselect = '';

	/**
	 * @brief An event handler for the submit event on the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onsubmit
	 */
	this.onsubmit = '';

	/**
	 * @brief The unload event is raised when the window is unloading its content and resources. The resources removal is processed after the unload event occurs.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowEventHandlers/onunload
	 */
	this.onunload = '';

	/**
	 * @brief The Window.onuserproxymity property represents an EventHandler, that is a function to be called when the userproximity event occurs. These events are of type UserProximityEvent and occur when the the device sensor detects that an object becomes nearby.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/onuserproximity
	 */
	this.onuserproximity = '';

	/**
	 * @brief Returns a reference to the window that opened this current window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/opener
	 */
	this.opener = '';

	/**
	 * @brief Window.outerHeight gets the height in pixels of the whole browser window. It represents the height of the whole browser window including sidebar (if expanded), window chrome and window resizing borders/handles.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/outerHeight
	 */
	this.outerHeight = '';

	/**
	 * @brief Window.outerWidth gets the width of the outside of the browser window. It represents the width of the whole browser window including sidebar (if expanded), window chrome and window resizing borders/handles.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/outerWidth
	 */
	this.outerWidth = '';

	/**
	 * @brief A reference to the parent of the current window or subframe.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/parent
	 */
	this.parent = '';

	/**
	 * @brief The Web Performance API allows web pages access to certain functions for measuring the performance of web pages and web applications, including the Navigation Timing API and high-resolution time data.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/performance
	 */
	this.performance = '';

	/**
	 * @brief Returns the personalbar object, whose visibility can be toggled in the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/personalbar
	 */
	this.personalbar = '';

	/**
	 * @brief Returns the pkcs11 object, which is used to install drivers and other software associated with the pkcs11 protocol. If pkcs11 isn't supported, this property returns null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/pkcs11
	 */
	this.pkcs11 = '';

	/**
	 * @brief Returns a reference to the screen object associated with the window. The screen object, implementing the Screen interface, is a special object for inspecting properties of the screen on which the current window is being rendered.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/screen
	 */
	this.screen = '';

	/**
	 * @brief The Window.screenX read-only property returns the horizontal distance, in CSS pixels, of the left border of the user's browser from the left side of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/screenX
	 */
	this.screenX = '';

	/**
	 * @brief The Window.screenY read-only property returns the vertical distance, in CSS pixels of the top border of the user's browser from the top edge of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/screenY
	 */
	this.screenY = '';

	/**
	 * @brief Returns the scrollbars object, whose visibility can be checked.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollbars
	 */
	this.scrollbars = '';

	/**
	 * @brief Returns the maximum number of pixels that the document can be scrolled horizontally.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollMaxX
	 */
	this.scrollMaxX = '';

	/**
	 * @brief Returns the maximum number of pixels that the document can be scrolled vertically.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollMaxY
	 */
	this.scrollMaxY = '';

	/**
	 * @brief Returns the number of pixels that the document has already been scrolled horizontally.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollX
	 */
	this.scrollX = '';

	/**
	 * @brief Returns the number of pixels that the document has already been scrolled vertically.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollY
	 */
	this.scrollY = '';

	/**
	 * @brief The Window.self read-only property returns the window itself, as a WindowProxy. It can be used with dot notation on a window object (that is, window.self) or standalone (self). The advantage of the standalone notation is that a similar notation exists for non-window contexts, such as in Web Workers. By using self, you can refer to the global scope in a way that will work not only in a window context (self will resolve to window.self) but also in a worker context (self will then resolve to WorkerGlobalScope.self).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/self
	 */
	this.self = '';

	/**
	 * @brief The sessionStorage property allows you to access a session Storage object. sessionStorage is similar to localStorage, the only difference is while data stored in localStorage has no expiration set, data stored in sessionStorage gets cleared when the page session ends. A page session lasts for as long as the browser is open and survives over page reloads and restores. Opening a page in a new tab or window will cause a new session to be initiated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/sessionStorage
	 */
	this.sessionStorage = '';

	/**
	 * @brief Sets the text in the status bar at the bottom of the browser or returns the previously set text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/status
	 */
	this.status = '';

	/**
	 * @brief Returns the statusbar object, whose visibility can be toggled in the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/statusbar
	 */
	this.statusbar = '';

	/**
	 * @brief Returns the toolbar object, whose visibility can be toggled in the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/toolbar
	 */
	this.toolbar = '';

	/**
	 * @brief Returns a reference to the topmost window in the window hierarchy.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/top
	 */
	this.top = '';

	/**
	 * @brief The Window.URL property returns a URL object that provides static methods used for creating and managing object URLs.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/URL
	 */
	this.URL = '';

	/**
	 * @brief The window property of a window object points to the window object itself. Thus the following expressions all return the same window object:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/window
	 */
	this.window = '';

	/**
	 * @brief The Window.alert() method displays an alert dialog with the optional specified content and an OK button.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/alert
	 */
	this.indow.alert = function() {};

	/**
	 * @brief The WindowBase64.atob() function decodes a string of data which has been encoded using base-64 encoding. You can use the window.btoa() method to encode and transmit data which may otherwise cause communication problems, then transmit it and use the window.atob() method to decode the data again. For example, you can encode, transmit, and decode control characters such as ASCII values 0 through 31.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowBase64/atob
	 */
	this.indowBase64.atob = function() {};

	/**
	 * @brief Returns the window to the previous item in the history. This was a Gecko-specific method. Use the standard history.back method instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/back
	 */
	this.indow.back = function() {};

	/**
	 * @brief Shifts focus away from the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/blur
	 */
	this.indow.blur = function() {};

	/**
	 * @brief This is an experimental API that should not be used in production code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowBase64/btoa
	 */
	this.indow.cancelAnimationFrame = function() {};

	/**
	 * @brief Registers the window to capture all events of the specified type.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/captureEvents
	 */
	this.indow.captureEvents = function() {};

	/**
	 * @brief This method clears the action specified by window.setImmediate.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/clearImmediate
	 */
	this.indow.clearImmediate = function() {};

	/**
	 * @brief Cancels repeated action which was set up using setInterval.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/clearInterval
	 */
	this.indowTimers.clearInterval = function() {};

	/**
	 * @brief Clears the delay set by WindowTimers.setTimeout().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/clearTimeout
	 */
	this.indowTimers.clearTimeout = function() {};

	/**
	 * @brief Closes the current window, or the window on which it was called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/close
	 */
	this.indow.close = function() {};

	/**
	 * @brief The Window.confirm() method displays a modal dialog with an optional message and two buttons, OK and Cancel.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/confirm
	 */
	this.indow.confirm = function() {};

	/**
	 * @brief The Window.convertPointFromNodeToPage() method converts a Point object from coordinates based on the given CSS node to coordinates based on the page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/convertPointFromNodeToPage
	 */
	this.indow.convertPointFromNodeToPage = function() {};

	/**
	 * @brief Prints messages to the (native) console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/dump
	 */
	this.indow.dump = function() {};

	/**
	 * @brief The fetch() method of the GlobalFetch interface starts the process of fetching a resource. This returns a promise that resolves to the Response object representing the response to your request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalFetch/fetch
	 * @param input - This defines the resource that you wish to fetch. This can either be:   A USVString containing the direct URL of the resource you want to fetch.  A Request object.
	 * @param init Optional - An options object containing any custom settings that you want to apply to the request. The possible options are:   method: The request method, e.g., GET, POST.  headers: Any headers you want to add to your request, contained within a Headers object or ByteString.  body: Any body that you want to add to your request: this can be a Blob, BufferSource, FormData, URLSearchParams, or USVString object. Note that a request using the GET or HEAD method cannot have a body.  mode: The mode you want to use for the request, e.g., cors, no-cors, or same-origin.  credentials: The request credentials you want to use for the request: omit, same-origin, or include.  cache: The cache mode you want to use for the request: default, no-store, reload, no-cache, force-cache, or only-if-cached.
	 */
	this.lobalFetch.fetch = function(input, init Optional) {};

	/**
	 * @brief Finds a string in a window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/find
	 */
	this.indow.find = function() {};

	/**
	 * @brief Makes a request to bring the window to the front. It may fail due to user settings and the window isn't guaranteed to be frontmost before this method returns.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/focus
	 */
	this.indow.focus = function() {};

	/**
	 * @brief Moves the window one document forward in the history. This was a Gecko-specific method. Use the standard history.forward method instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/forward
	 */
	this.indow.forward = function() {};

	/**
	 * @brief Attempts to get the user's attention. How this happens varies based on OS and window manager.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getAttention
	 */
	this.indow.getAttention = function() {};

	/**
	 * @brief The Window.getComputedStyle() method gives the values of all the CSS properties of an element after applying the active stylesheets and resolving any basic computation those values may contain.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getComputedStyle
	 */
	this.indow.getComputedStyle = function() {};

	/**
	 * @brief getDefaultComputedStyle() gives the default computed values of all the CSS properties of an element, ignoring author styling.  That is, only user-agent and user styles are taken into account.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/window/getDefaultComputedStyle
	 */
	this.indow.getDefaultComputedStyle = function() {};

	/**
	 * @brief Returns a selection object representing the range of text selected by the user.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getSelection
	 */
	this.indow.getSelection = function() {};

	/**
	 * @brief Returns the window to the home page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/home
	 */
	this.indow.home = function() {};

	/**
	 * @brief Because opening windows on mobile isn't necessarily appropriate, the Firefox Mobile team designed the importDialog() method to replace window.openDialog(). Instead of opening a new window, it merges the specified XUL dialog into the main window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/importDialog
	 */
	this.indow.importDialog = function() {};

	/**
	 * @brief Returns a new MediaQueryList object representing the parsed results of the specified media query string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/matchMedia
	 */
	this.indow.matchMedia = function() {};

	/**
	 * @brief Sets a window to minimized state (a way to undo it programatically is by calling window.moveTo()).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/minimize
	 */
	this.indow.minimize = function() {};

	/**
	 * @brief Moves the current window by a specified amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/moveBy
	 */
	this.indow.moveBy = function() {};

	/**
	 * @brief Moves the window to the specified coordinates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/moveTo
	 */
	this.indow.moveTo = function() {};

	/**
	 * @brief Loads a resource into either a new browsing context (such as a window) or one that already exists, depending on the specified parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/open
	 */
	this.indow.open = function() {};

	/**
	 * @brief window.openDialog is an extension to window.open. It behaves the same, except that it can optionally take one or more parameters past windowFeatures, and windowFeatures itself is treated a little differently.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/openDialog
	 */
	this.indow.openDialog = function() {};

	/**
	 * @brief The window.postMessage method safely enables cross-origin communication. Normally, scripts on different pages are allowed to access each other if and only if the pages that executed them are at locations with the same protocol (usually both http), port number (80 being the default for http), and host (modulo document.domain being set by both pages to the same value). window.postMessage provides a controlled mechanism to circumvent this restriction in a way which is secure when properly used.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/postMessage
	 */
	this.indow.postMessage = function() {};

	/**
	 * @brief Opens the Print Dialog to print the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/print
	 */
	this.indow.print = function() {};

	/**
	 * @brief The Window.prompt() displays a dialog with an optional message prompting the user to input some text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/prompt
	 */
	this.indow.prompt = function() {};

	/**
	 * @brief Releases the window from trapping events of a specific type.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/releaseEvents
	 */
	this.indow.releaseEvents = function() {};

	/**
	 * @brief You should call this method whenever you're ready to update your animation onscreen. This will request that your animation function be called before the browser performs the next repaint. The number of callbacks is usually 60 times per second, but will generally match the display refresh rate in most web browsers as per W3C recommendation. The callback rate may be reduced to a lower rate when running in background tabs.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/window/requestAnimationFrame
	 * @param callback - A parameter specifying a function to call when it's time to update your animation for the next repaint. The callback has one single argument, a DOMHighResTimeStamp, which indicates the current time for when requestAnimationFrame starts to fire callbacks.
	 */
	this.indow.requestAnimationFrame = function(callback) {};

	/**
	 * @brief Resizes the current window by a certain amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/resizeBy
	 */
	this.indow.resizeBy = function() {};

	/**
	 * @brief Dynamically resizes window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/resizeTo
	 */
	this.indow.resizeTo = function() {};

	/**
	 * @brief This method is currently not working, but you can use:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/restore
	 */
	this.indow.restore = function() {};

	/**
	 * @brief 
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/routeEvent
	 */
	this.indow.routeEvent = function() {};

	/**
	 * @brief Scrolls the window to a particular place in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scroll
	 */
	this.indow.scroll = function() {};

	/**
	 * @brief Scrolls the document in the window by the given amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollBy
	 */
	this.indow.scrollBy = function() {};

	/**
	 * @brief Scrolls the document by the given number of lines.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollByLines
	 */
	this.indow.scrollByLines = function() {};

	/**
	 * @brief Scrolls the current document by the specified number of pages.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollByPages
	 */
	this.indow.scrollByPages = function() {};

	/**
	 * @brief Scrolls to a particular set of coordinates in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollTo
	 */
	this.indow.scrollTo = function() {};

	/**
	 * @brief Changes the cursor for the current window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/setCursor
	 */
	this.indow.setCursor = function() {};

	/**
	 * @brief This method is used to break up long running operations and run a callback function immediately after the browser has completed other operations such as events and display updates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/setImmediate
	 */
	this.indow.setImmediate = function() {};

	/**
	 * @brief Calls a function or executes a code snippet repeatedly, with a fixed time delay between each call to that function. Returns an intervalID.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/setInterval
	 */
	this.indowTimers.setInterval = function() {};

	/**
	 * @brief Calls a function or executes a code snippet after a specified delay.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/setTimeout
	 */
	this.indowTimers.setTimeout = function() {};

	/**
	 * @brief The Window.showModalDialog() creates and displays a modal dialog box containing a specified HTML document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/showModalDialog
	 */
	this.indow.showModalDialog = function() {};

	/**
	 * @brief The Window.sizeToContent() method sizes the window according to its content. In order for it to work, the DOM content should be loaded when this function is called, for example once the DOMContentLoaded event has been thrown.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/sizeToContent
	 */
	this.indow.sizeToContent = function() {};

	/**
	 * @brief This method stops window loading.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/stop
	 */
	this.indow.stop = function() {};

	/**
	 * @brief Updates the state of commands of the current chrome window (UI).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/updateCommands
	 */
	this.indow.updateCommands = function() {};

}

