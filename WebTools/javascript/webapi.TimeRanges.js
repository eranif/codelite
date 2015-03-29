
/**
 * @brief The TimeRanges interface is used to represent a set of time ranges, primarily for the purpose of tracking which portions of media have been buffered when loading it for use by the &lt;audio> and &lt;video> elements.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TimeRanges
 */
function TimeRanges() {

	/**
	 * @brief Returns the time offset at which a specified time range begins.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TimeRanges/start
	 */
	this.start = function() {};

	/**
	 * @brief Returns the time offset at which a specified time range ends.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TimeRanges/end
	 */
	this.end = function() {};

}

