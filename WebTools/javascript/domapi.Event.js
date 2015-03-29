
/**
 * @brief The Event interface represents any event of the DOM. It contains common properties and methods to any event.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event
 */
function Event() {

	/**
	 * @brief Indicates whether the given event bubbles up through the DOM or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/bubbles
	 */
	this.bubbles = '';

	/**
	 * @brief Indicates whether the event is cancelable or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/cancelable
	 */
	this.cancelable = '';

	/**
	 * @brief Identifies the current target for the event, as the event traverses the DOM. It always refers to the element the event handler has been attached to as opposed to event.target which identifies the element on which the event occurred.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/currentTarget
	 */
	this.currentTarget = '';

	/**
	 * @brief Returns a boolean indicating whether or not event.preventDefault() was called on the event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/defaultPrevented
	 */
	this.defaultPrevented = '';

	/**
	 * @brief Returns additional numerical information about the event, depending on the type of event. See the Notes section for details.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/detail
	 */
	this.detail = '';

	/**
	 * @brief Indicates which phase of the event flow is currently being evaluated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/eventPhase
	 */
	this.eventPhase = '';

	/**
	 * @brief The explicit original target of the event. (Mozilla-specific)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/explicitOriginalTarget
	 */
	this.explicitOriginalTarget = '';

	/**
	 * @brief A boolean value indicating whether the event is trusted.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/isTrusted
	 */
	this.isTrusted = '';

	/**
	 * @brief The original target of the event before any retargetings. (Mozilla-specific)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/originalTarget
	 */
	this.originalTarget = '';

	/**
	 * @brief A reference to the object that dispatched the event. It is different than event.currentTarget when the event handler is called during the bubbling or capturing phase of the event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/target
	 */
	this.target = '';

	/**
	 * @brief Returns the time (in milliseconds since the epoch) at which the event was created.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/timeStamp
	 */
	this.timeStamp = '';

	/**
	 * @brief Returns a string containing the type of event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/type
	 */
	this.type = '';

	/**
	 * @brief Creates a new event, which must then be initialized by calling its init() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/createEvent
	 */
	this.vent.createEvent = function() {};

	/**
	 * @brief The Event.initEvent() method is used to initialize the value of an event created using Document.createEvent().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/initEvent
	 */
	this.vent.initEvent = function() {};

	/**
	 * @brief Cancels the event if it is cancelable, without stopping further propagation of the event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault
	 */
	this.vent.preventDefault = function() {};

	/**
	 * @brief Prevents other listeners of the same event from being called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/stopImmediatePropagation
	 */
	this.vent.stopImmediatePropagation = function() {};

	/**
	 * @brief Prevents further propagation of the current event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Event/stopPropagation
	 */
	this.vent.stopPropagation = function() {};

}

