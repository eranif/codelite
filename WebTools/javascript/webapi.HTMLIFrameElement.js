
/**
 * @brief The HTMLIFrameElement interface provides special properties and methods (beyond those of the HTMLElement interface it also has available to it by inheritance) for manipulating the layout and presentation of inline frame elements.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement
 */
function HTMLIFrameElement() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/align
	 */
	this.height = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/longDesc
	 */
	this.name = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/sandbox
	 */
	this.sandbox = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/scrolling
	 */
	this.src = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/srcdoc
	 */
	this.srcdoc = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/width
	 */
	this.width = '';

	/**
	 * @brief The setVisible method is used to change the visibility state of the browser &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/setVisible
	 * @param visible - A boolean that indicates if the browser &lt;iframe&gt; visible state is true or false.
	 */
	this.TMLIFrameElement.setVisible(visible)

	/**
	 * @brief The getVisible method is used to request the current visible state of the browser &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/getVisible
	 */
	this.getVisible = function() {};

	/**
	 * @brief The sendMouseEvent method allows to fake a mouse event and send it to the browser &lt;iframe>'s content.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/sendMouseEvent
	 * @param type - A string representing the event type. Possible values are mousedown, mouseup, mousemove, mouseover, mouseout, or contextmenu.
	 * @param x - A number representing the x position of the cursor relative to the browser &lt;iframe&gt;'s visible area in CSS pixels.
	 * @param y - A number representing the y position of the cursor relative to the browser &lt;iframe&gt;'s visible area in CSS pixels.
	 * @param button - A number representing which button has been pressed on the mouse: 0 (Left button), 1 (middle button), or 2 (right button).
	 * @param clickCount - Number of clicks that have been performed.
	 * @param modifiers - A number representing a key pressed at the same time the mouse button was clicked:
    1 : The ALT key
    2 : The CONTROL key
    4 : The SHIFT key
    8 : The META key
   16 : The ALTGRAPH key
   32 : The CAPSLOCK key
   64 : The FN key
  128 : The NUMLOCK key
  256 : The SCROLL key
  512 : The SYMBOLLOCK key
 1024 : The WIN key
	 */
	this.TMLIFrameElement.sendMouseEvent(type, x, y, button, clickCount, modifiers)

	/**
	 * @brief The sendTouchEvent method allows to fake a touch event and send it to the browser &lt;iframe>'s content.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/sendTouchEvent
	 * @param type - A string representing the event type. Possible values are touchstart, touchend, touchmove, or touchcancel.
	 * @param x - An array of numbers representing the x position of each touch point relative to the browser &lt;iframe&gt;'s visible area in CSS pixels.
	 * @param y - An array of numbers representing the y position of each touch point relative to the browser &lt;iframe&gt;'s visible area in CSS pixels.
	 * @param rx - An array of numbers representing the x radius of each touch point in CSS pixels.
	 * @param ry - An array of numbers representing the y radius of each touch point in CSS pixels.
	 * @param rotationAngles - An array of numbers representing the angle of each touch point in degrees.
	 * @param forces - An array of numbers representing the intensity of each touch in the range [0,1].
	 * @param count - Number of touches that have been performed.
	 * @param modifiers - A number representing a key pressed at the same time the touches were performed:
    1 : The ALT key
    2 : The CONTROL key
    4 : The SHIFT key
    8 : The META key
   16 : The ALTGRAPH key
   32 : The CAPSLOCK key
   64 : The FN key
  128 : The NUMLOCK key
  256 : The SCROLL key
  512 : The SYMBOLLOCK key
 1024 : The WIN key
	 */
	this.TMLIFrameElement.sendTouchEvent(type, identifiers, touchesX, touchesY,radiisX, radiisY, rotationAngles, forces,count, modifiers)

	/**
	 * @brief The goBack method is used to navigate backwards in the browser &lt;iframe>'s history.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/goBack
	 */
	this.goBack = function() {};

	/**
	 * @brief The goForward method is used to navigate forward in the browser &lt;iframe>'s history.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/goForward
	 */
	this.goForward = function() {};

	/**
	 * @brief The reload method is used to reload the content of the &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/reload
	 * @param hardReload - A boolean that indicates if all the resources to reload must be revalidated (true) or taken directly from the browser cache (false).
	 */
	this.reload = function(hardReload) {};

	/**
	 * @brief The stop method is used to stop loading the content of the &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/stop
	 */
	this.stop = function() {};

	/**
	 * @brief The purgeHistory method is used to clear the browsing history associated with the browser &lt;iframe>. It doesn't delete cookies or anything like that; just the history data.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/purgeHistory
	 */
	this.purgeHistory = function() {};

	/**
	 * @brief The getScreenshot() method lets you request a screenshot of an &lt;iframe>, scaled to fit within a specified maximum width and height; the image may be cropped if necessary but will not be squished vertically or horizontally..
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/getScreenshot
	 * @param maxWidth - A number representing the maximum width of the screenshot in CSS pixels.
	 * @param maxHeight - A number representing the maximum height of the screenshot in CSS pixels.
	 * @param mimeType - MIME type specifying the format of the image to be returned; the default is image/jpeg. Use image/png to capture the alpha channel of the rendering result by returning a PNG-format image. This lets you get the transparent background of the &lt;iframe&gt;.
	 */
	this.TMLIFrameElement.getScreenshot(maxWidth, maxHeight)

	/**
	 * @brief The addNextPaintListener method is used to define a handler to listen the next MozAfterPaint event coming from the browser &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/addNextPaintListener
	 * @param FuncRef - A function handler to listen for a MozAfterPaint event.
	 */
	this.addNextPaintListener = function(FuncRef) {};

	/**
	 * @brief The removeNextPaintListener method is used to remove a handler previously set with the addNextPaintListener method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/removeNextPaintListener
	 * @param FuncRef - A function handler previously set with addNextPaintListener.
	 */
	this.removeNextPaintListener = function(FuncRef) {};

	/**
	 * @brief The getCanGoBack method is used to know if it's possible to go back in the navigation history of the browser &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/getCanGoBack
	 */
	this.getCanGoBack = function() {};

	/**
	 * @brief The getCanGoForward method is used to know if it's possible to go forward in the navigation history of the browser &lt;iframe>.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLIFrameElement/getCanGoForward
	 */
	this.getCanGoForward = function() {};

}

