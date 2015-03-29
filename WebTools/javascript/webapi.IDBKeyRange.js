
/**
 * @brief A key range can be a single value or a range with upper and lower bounds or endpoints. If the key range has both upper and lower bounds, then it is bounded; if it has no bounds, it is unbounded. A bounded key range can either be open (the endpoints are excluded) or closed (the endpoints are included). To retrieve all keys within a certain range, you can use the following code constructs:
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBKeyRange
 */
function IDBKeyRange() {

	/**
	 * @brief The bounds can be open (that is, the bounds exclude the endpoint values) or closed (that is, the bounds include the endpoint values). By default, the bounds are closed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBKeyRange/bound
	 */
	this.DBKeyRange.bound

	/**
	 * @brief IDBKeyRange: The newly created key range.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBKeyRange/only
	 */
	this.DBKeyRange.only

	/**
	 * @brief By default, it includes the lower endpoint value and is closed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBKeyRange/lowerBound
	 */
	this.DBKeyRange.lowerBound

	/**
	 * @brief By default, it includes the upper endpoint value and is closed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBKeyRange/upperBound
	 */
	this.DBKeyRange.upperBound

}

