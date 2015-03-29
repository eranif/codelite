
/**
 * @brief A DOMCursor object represents an ongoing operation over a list of results. It is an enhanced DOMRequest that allows to iterate through a list of results asynchronously. Each time its continue() method is called, the DOMCursor tries to reach the next result in the list and calls its result's success or error accordingly.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMCursor
 */
function DOMCursor() {

	/**
	 * @brief The done property indicate if the DOMCursor has reach the last result.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMCursor/done
	 */
	this.done = '';

	/**
	 * @brief The continue method is used to tell the cursor to move to the next result. The DOMCursor's success or error is called with the DOMCursor's result updated accordingly.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMCursor/continue
	 */
	this.continue = function() {};

}

