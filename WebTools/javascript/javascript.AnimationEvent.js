
/**
 * @brief The AnimationEvent interface represents events providing information related to animations.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnimationEvent
 */
function AnimationEvent() {

	/**
	 * @brief The AnimationEvent.animationName read-only property is a DOMString containing the value of the animation-name CSS property associated with the transition.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnimationEvent/animationName
	 */
	this.animationName = '';

	/**
	 * @brief This is an experimental API that should not be used in production code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AnimationEvent/elapsedTime
	 */
	this.pseudoElement = '';

}

