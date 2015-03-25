
/**
 * @brief This interface inherits properties from its parent, Event.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/CustomEvent
 */
function CustomEvent() {

	/**
	 * @brief The CustomEvent.initCustomEvent() method initializes a CustomEvent object. If the event has already being dispatched, this method does nothing.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CustomEvent/initCustomEvent
	 * @param type - Is a DOMString containing the name of the event.
	 * @param canBubble - Is a Boolean indicating whether the event bubbles up through the DOM or not.
	 * @param cancelable - Is a Boolean indicating whether the event is cancelable.
	 * @param detail - The data passed when initializing the event.
	 */
	this.initCustomEvent = function(type, canBubble, cancelable, detail) {};

}

