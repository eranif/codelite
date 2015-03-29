
/**
 * @brief The MouseEvent interface represents events that occur due to the user interacting with a pointing device (such as a mouse). Common events using this interface include click, dblclick, mouseup, mousedown.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent
 */
function MouseEvent() {

	/**
	 * @brief The MouseEvent.getModifierState() method returns the current state of the specified modifier key: true if the modifier is active (i.e., the modifier key is pressed or locked), otherwise, false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/getModifierState
	 */
	this.getModifierState = function() {};

	/**
	 * @brief The MouseEvent.initMouseEvent() method initializes the value of a mouse event once it's been created (normally using the Document.createEvent() method).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/initMouseEvent
	 * @param type - the string to set the event's type to. Possible types for mouse events include: click, mousedown, mouseup, mouseover, mousemove, mouseout.
	 * @param canBubble - whether or not the event can bubble. Sets the value of Event.bubbles.
	 * @param cancelable - whether or not the event's default action can be prevented. Sets the value of Event.cancelable.
	 * @param view - the event's AbstractView. You should pass the window object here. Sets the value of Event.view.
	 * @param detail - the event's mouse click count. Sets the value of Event.detail.
	 * @param screenX - the event's screen x coordinate. Sets the value of Event.screenX.
	 * @param screenY - the event's screen y coordinate. Sets the value of Event.screenY.
	 * @param clientX - the event's client x coordinate. Sets the value of Event.clientX.
	 * @param clientY - the event's client y coordinate. Sets the value of Event.clientY.
	 * @param ctrlKey - whether or not control key was depressed during the Event. Sets the value of Event.ctrlKey.
	 * @param altKey - whether or not alt key was depressed during the Event. Sets the value of Event.altKey.
	 * @param shiftKey - whether or not shift key was depressed during the Event. Sets the value of Event.shiftKey.
	 * @param metaKey - whether or not meta key was depressed during the Event. Sets the value of Event.metaKey.
	 * @param button - the event's mouse button.
	 * @param relatedTarget - the event's related EventTarget. Only used with some event types (e.g. mouseover and mouseout). In other cases, pass null.
	 */
	this.initMouseEvent = function(type, canBubble, cancelable, view, detail, screenX, screenY, clientX, clientY, ctrlKey, altKey, shiftKey, metaKey, button, relatedTarget) {};

}

