
/**
 * @brief Inherits from: EventTarget
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBTransaction
 */
function IDBTransaction() {

	/**
	 * @brief All pending IDBRequest objects created during this transaction have their IDBRequest.error attribute set to AbortError.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBTransaction/abort
	 */
	this.DBTransaction.abort

	/**
	 * @brief Every call to this method on the same transaction object, with the same name, returns the same IDBObjectStore instance. If this method is called on a different transaction object, a different IDBObjectStore instance is returned.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBTransaction/objectStore
	 */
	this.DBTransaction.objectStore

}

