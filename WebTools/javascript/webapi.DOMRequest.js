
/**
 * @brief A DOMRequest object represents an ongoing operation. It provides callbacks that are called when the operation completes, as well as a reference to the operation's result. A DOM method that initiates an ongoing operation may return a DOMRequest object that you can use to monitor the progress of that operation.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest
 */
function DOMRequest() {

	/**
	 * @brief This property specifies a callback function to be run when a DOMRequest completes successfully.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest/onsuccess
	 */
	this.onsuccess = '';

	/**
	 * @brief This property specifies a callback function to be run when a DOMRequest fails.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest/onerror
	 */
	this.onerror = '';

	/**
	 * @brief This property is a string that indicates whether or not a DOMRequest operation has finished running.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest/readyState
	 */
	this.readyState = '';

	/**
	 * @brief This property provides the result value from a DOMRequest operation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest/result
	 */
	this.result = '';

	/**
	 * @brief This property can contain error information in a DOMError object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMRequest/error
	 */
	this.error = '';

}

