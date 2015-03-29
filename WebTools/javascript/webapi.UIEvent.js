
/**
 * @brief The UIEvent interface represents simple user interface events.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/UIEvent
 */
function UIEvent() {

	/**
	 * @brief The UIEvent.initUIEvent() method initializes a UI event once it's been created.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/UIEvent/initUIEvent
	 * @param type - Is a DOMString defining the type of event.
	 * @param canBubble - Is a Boolean deciding whether the event should bubble up through the event chain or not. Once set, the read-only property Event.bubbles will give its value.
	 * @param cancelable - Is a Boolean defining whether the event can be canceled. Once set, the read-only property Event.cancelable will give its value.
	 * @param view - Is the WindowProxy associated with the event.
	 * @param detail - Is an unsigned long  specifying some detail information about the event, depending on the type of event. For mouse events, it indicates how many times the mouse has been clicked on a given screen location.
	 */
	this.initUIEvent = function(type, canBubble, cancelable, view, detail) {};

}

