
/**
 * @brief The window object represents a window containing a DOM document; the document property points to the DOM document loaded in that window.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window
 */
function Window() {

	/**
	 * @brief Returns a reference to the application cache object for the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/applicationCache
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
	this.name = '';

	/**
	 * @brief The Window.navigator read-only property returns a reference to the Navigator object, which can be queried for information about the application running the script.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/navigator
	 */
	this.opener = '';

	/**
	 * @brief Window.outerHeight gets the height in pixels of the whole browser window. It represents the height of the whole browser window including sidebar (if expanded), window chrome and window resizing borders/handles.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/outerHeight
	 */
	this.status = '';

	/**
	 * @brief The EventTarget.addEventListener() method registers the specified listener on the EventTarget it's called on. The event target may be an Element in a document, the Document itself, a Window, or any other object that supports events (such as XMLHttpRequest).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
	 */
	this.addEventListener = function() {};

	/**
	 * @brief The Window.alert() method displays an alert dialog with the optional specified content and an OK button.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/alert
	 */
	this.alert = function() {};

	/**
	 * @brief The WindowBase64.atob() function decodes a string of data which has been encoded using base-64 encoding. You can use the window.btoa() method to encode and transmit data which may otherwise cause communication problems, then transmit it and use the window.atob() method to decode the data again. For example, you can encode, transmit, and decode control characters such as ASCII values 0 through 31.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowBase64/atob
	 */
	this.atob = function() {};

	/**
	 * @brief Returns the window to the previous item in the history. This was a Gecko-specific method. Use the standard history.back method instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/back
	 */
	this.back = function() {};

	/**
	 * @brief Shifts focus away from the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/blur
	 */
	this.blur = function() {};

	/**
	 * @brief Registers the window to capture all events of the specified type.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowBase64/btoa" title='Creates a base-64 encoded ASCII string from a "string" of binary data.'><code>WindowBase64.btoa()</code></a></dt>
 <dd>Creates a base-64 encoded ASCII string from a string of binary data.</dd>
 <dt><a href=
	 */
	this.captureEvents = function() {};

	/**
	 * @brief This method clears the action specified by window.setImmediate.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/clearImmediate
	 */
	this.clearImmediate = function() {};

	/**
	 * @brief Cancels repeated action which was set up using setInterval.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/clearInterval
	 */
	this.clearInterval = function() {};

	/**
	 * @brief Clears the delay set by WindowTimers.setTimeout().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/clearTimeout
	 */
	this.clearTimeout = function() {};

	/**
	 * @brief Closes the current window, or the window on which it was called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/close
	 */
	this.close = function() {};

	/**
	 * @brief The Window.confirm() method displays a modal dialog with an optional message and two buttons, OK and Cancel.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/confirm
	 */
	this.confirm = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/disableExternalCapture
	 */
	this.disableExternalCapture = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/dispatchEvent
	 */
	this.dispatchEvent = function() {};

	/**
	 * @brief Prints messages to the (native) console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/dump
	 */
	this.dump = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/enableExternalCapture
	 */
	this.enableExternalCapture = function() {};

	/**
	 * @brief The fetch() method of the GlobalFetch interface starts the process of fetching a resource. This returns a promise that resolves to the Response object representing the response to your request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalFetch/fetch
	 * @param input - This defines the resource that you wish to fetch. This can either be:
 
  A USVString containing the direct URL of the resource you want to fetch.
  A Request object.
	 * @param init Optional - An options object containing any custom settings that you want to apply to the request. The possible options are:
 
  method: The request method, e.g., GET, POST.
  headers: Any headers you want to add to your request, contained within a Headers object or ByteString.
  body: Any body that you want to add to your request: this can be a Blob, BufferSource, FormData, URLSearchParams, or USVString object. Note that a request using the GET or HEAD method cannot have a body.
  mode: The mode you want to use for the request, e.g., cors, no-cors, or same-origin.
  credentials: The request credentials you want to use for the request: omit, same-origin, or include.
  cache: The cache mode you want to use for the request: default, no-store, reload, no-cache, force-cache, or only-if-cached.
	 */
	this.fetch = function(input, init Optional) {};

	/**
	 * @brief Finds a string in a window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/find
	 */
	this.find = function() {};

	/**
	 * @brief Makes a request to bring the window to the front. It may fail due to user settings and the window isn't guaranteed to be frontmost before this method returns.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/focus
	 */
	this.focus = function() {};

	/**
	 * @brief Moves the window one document forward in the history. This was a Gecko-specific method. Use the standard history.forward method instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/forward
	 */
	this.forward = function() {};

	/**
	 * @brief Attempts to get the user's attention. How this happens varies based on OS and window manager.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getAttention
	 */
	this.getAttention = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getAttentionWithCycleCount
	 */
	this.getAttentionWithCycleCount = function() {};

	/**
	 * @brief The Window.getComputedStyle() method gives the values of all the CSS properties of an element after applying the active stylesheets and resolving any basic computation those values may contain.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getComputedStyle
	 */
	this.getComputedStyle = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getDefaulComputedStyle
	 */
	this.getDefaulComputedStyle = function() {};

	/**
	 * @brief Returns a selection object representing the range of text selected by the user.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/getSelection
	 */
	this.getSelection = function() {};

	/**
	 * @brief Returns the window to the home page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/home
	 */
	this.home = function() {};

	/**
	 * @brief Returns a new MediaQueryList object representing the parsed results of the specified media query string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/matchMedia
	 */
	this.matchMedia = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/maximize
	 */
	this.maximize = function() {};

	/**
	 * @brief Sets a window to minimized state (a way to undo it programatically is by calling window.moveTo()).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/minimize
	 */
	this.minimize = function() {};

	/**
	 * @brief Moves the current window by a specified amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/moveBy
	 */
	this.moveBy = function() {};

	/**
	 * @brief Moves the window to the specified coordinates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/moveTo
	 */
	this.moveTo = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/mozRequestAnimationFrame
	 */
	this.mozRequestAnimationFrame = function() {};

	/**
	 * @brief Loads a resource into either a new browsing context (such as a window) or one that already exists, depending on the specified parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/open
	 */
	this.open = function() {};

	/**
	 * @brief window.openDialog is an extension to window.open. It behaves the same, except that it can optionally take one or more parameters past windowFeatures, and windowFeatures itself is treated a little differently.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/openDialog
	 */
	this.openDialog = function() {};

	/**
	 * @brief The window.postMessage method safely enables cross-origin communication. Normally, scripts on different pages are allowed to access each other if and only if the pages that executed them are at locations with the same protocol (usually both http), port number (80 being the default for http), and host (modulo document.domain being set by both pages to the same value). window.postMessage provides a controlled mechanism to circumvent this restriction in a way which is secure when properly used.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/postMessage
	 */
	this.postMessage = function() {};

	/**
	 * @brief Opens the Print Dialog to print the current document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/print
	 */
	this.print = function() {};

	/**
	 * @brief The Window.prompt() displays a dialog with an optional message prompting the user to input some text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/prompt
	 */
	this.prompt = function() {};

	/**
	 * @brief Releases the window from trapping events of a specific type.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/releaseEvents
	 */
	this.releaseEvents = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Element/removeEventListener
	 */
	this.removeEventListener = function() {};

	/**
	 * @brief Resizes the current window by a certain amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/resizeBy
	 */
	this.resizeBy = function() {};

	/**
	 * @brief Dynamically resizes window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/resizeTo
	 */
	this.resizeTo = function() {};

	/**
	 * @brief This method is currently not working, but you can use:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/restore
	 */
	this.restore = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/routeEvent
	 */
	this.routeEvent = function() {};

	/**
	 * @brief Scrolls the window to a particular place in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scroll
	 */
	this.scroll = function() {};

	/**
	 * @brief Scrolls the document in the window by the given amount.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollBy
	 */
	this.scrollBy = function() {};

	/**
	 * @brief Scrolls the document by the given number of lines.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollByLines
	 */
	this.scrollByLines = function() {};

	/**
	 * @brief Scrolls the current document by the specified number of pages.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollByPages
	 */
	this.scrollByPages = function() {};

	/**
	 * @brief Scrolls to a particular set of coordinates in the document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/scrollTo
	 */
	this.scrollTo = function() {};

	/**
	 * @brief Changes the cursor for the current window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/setCursor
	 */
	this.setCursor = function() {};

	/**
	 * @brief This method is used to break up long running operations and run a callback function immediately after the browser has completed other operations such as events and display updates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/setImmediate
	 */
	this.setImmediate = function() {};

	/**
	 * @brief Calls a function or executes a code snippet repeatedly, with a fixed time delay between each call to that function. Returns an intervalID.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/setInterval
	 */
	this.setInterval = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/setResizable
	 */
	this.indow.setResizable

	/**
	 * @brief Calls a function or executes a code snippet after a specified delay.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WindowTimers/setTimeout
	 */
	this.setTimeout = function() {};

	/**
	 * @brief The Window.showModalDialog() creates and displays a modal dialog box containing a specified HTML document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/showModalDialog
	 */
	this.showModalDialog = function() {};

	/**
	 * @brief The Window.sizeToContent() method sizes the window according to its content. In order for it to work, the DOM content should be loaded when this function is called, for example once the DOMContentLoaded event has been thrown.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/sizeToContent
	 */
	this.sizeToContent = function() {};

	/**
	 * @brief This method stops window loading.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/stop
	 */
	this.stop = function() {};

	/**
	 * @brief Updates the state of commands of the current chrome window (UI).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Window/updateCommands
	 */
	this.updateCommands = function() {};

}

