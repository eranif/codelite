
/**
 * @brief There are two kinds of AudioParam, a-rate and k-rate parameters:
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam
 */
function AudioParam() {

	/**
	 * @brief Though value can be set, any modifications happening while there are automation events scheduled — that is, events scheduled using the methods of the AudioParam — are ignored, without raising any exception.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/value
	 */
	this.value = '';

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/setValueAtTime
	 */
	this.setValueAtTime = function() {};

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/linearRampToValueAtTime
	 */
	this.linearRampToValueAtTime = function() {};

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/exponentialRampToValueAtTime
	 */
	this.exponentialRampToValueAtTime = function() {};

	/**
	 * @brief The change starts at the time specified in startTime and exponentially moves towards the value given by the target parameter. The exponential decay rate is defined by the timeConstant parameter. This is the time it takes a first-order linear continuous time-invariant system to reach the value 1 - 1/e (around 63.2%) given a step input response (transition from 0 to 1 value): basically, the larger it is, the slower the transition will be.  This is useful for decay or release portions of envelopes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/setTargetAtTime
	 */
	this.setTargetAtTime = function() {};

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/setValueCurveAtTime
	 */
	this.setValueCurveAtTime = function() {};

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioParam/cancelScheduledValues
	 */
	this.cancelScheduledValues = function() {};

}

