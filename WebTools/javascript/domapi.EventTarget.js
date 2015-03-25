
/**
 * @brief EventTarget is an interface implemented by objects that can receive events and may have listeners for them.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/EventTarget
 */
function EventTarget() {

	/**
	 * @brief The EventTarget.addEventListener() method registers the specified listener on the EventTarget it's called on. The event target may be an Element in a document, the Document itself, a Window, or any other object that supports events (such as XMLHttpRequest).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
	 */
	this.addEventListener = function() {};

	/**
	 * @brief Dispatches an Event at the specified EventTarget, invoking the affected EventListeners in the appropriate order. The normal event processing rules (including the capturing and optional bubbling phase) apply to events dispatched manually with dispatchEvent().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/dispatchEvent
	 */
	this.dispatchEvent = function() {};

	/**
	 * @brief Removes the event listener previously registered with EventTarget.addEventListener.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/removeEventListener
	 */
	this.removeEventListener = function() {};

}

