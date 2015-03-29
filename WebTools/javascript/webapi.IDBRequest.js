
/**
 * @brief The request object does not initially contain any information about the result of the operation, but once information becomes available, an event is fired on the request, and the information becomes available through the properties of the IDBRequest instance.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest
 */
function IDBRequest() {

	/**
	 * @brief An IDBObjectStore containing the result of the request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest/result
	 */
	this.DBRequest.result

	/**
	 * @brief A DOMError containing the relevant error. The following error codes are returned under certain conditions:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest/error
	 */
	this.DBRequest.error

	/**
	 * @brief An object representing the source of the request, such as an IDBIndex, IDBObjectStore or IDBCursor.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest/source
	 */
	this.DBRequest.source

	/**
	 * @brief An IDBTransaction.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest/transaction
	 */
	this.DBRequest.transaction

	/**
	 * @brief The IDBRequestReadyState of the request, which takes one of the following two values:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBRequest/readyState
	 */
	this.DBRequest.readyState

}

