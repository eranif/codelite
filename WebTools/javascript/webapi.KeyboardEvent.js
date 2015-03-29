
/**
 * @brief KeyboardEvent objects describe a user interaction with the keyboard. Each event describes a key; the event type (keydown, keypress, or keyup) identifies what kind of activity was performed.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent
 */
function KeyboardEvent() {

	/**
	 * @brief The KeyboardEvent.altKey read-only property is a Boolean indicates if the alt key was pressed (true) or not (false) when the event occured.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/altKey
	 */
	this.location Read only  = '';

	/**
	 * @brief The KeyboardEvent.getModifierState() method returns the current state of the specified modifier key, true if the modifier is active (that is the modifier key is pressed or locked), otherwise, false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/getModifierState
	 */
	this.getModifierState = function() {};

	/**
	 * @brief The KeyboardEvent.initKeyboardEvent() initializes the attributes of a keyboard event object. This method was introduced in draft of DOM Level 3 Events, but deprecated in newer draft. Gecko won't support this feature since implementing this method as experimental broke existing web apps (see bug 999645). Web applications should use constructor instead of this if it's available.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/initKeyEvent" title='The KeyboardEvent.initKeyEvent method is used to initialize the value of an event created using document.createEvent("KeyboardEvent"). Events initialized in this way must have been created with the document.createEvent("KeyboardEvent") method. initKeyEvent must be called to set the event before it is dispatched.'><code>KeyboardEvent.initKeyEvent()</code></a><span title="This deprecated API should no longer be used, but will probably still work."><i class="icon-thumbs-down-alt"> </i></span></dt>
 <dd>Initializes a <code>KeyboardEvent</code> object. This has only been implemented by Gecko (others used <a href=
	 */
	this.initKeyboardEvent = function() {};

}

