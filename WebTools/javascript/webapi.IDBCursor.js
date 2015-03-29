
/**
 * @brief The IDBCursor interface of the IndexedDB API represents a cursor for traversing or iterating over multiple records in a database.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBCursor
 */
function IDBCursor() {

	/**
	 * @brief The advance() method of the IDBCursor interface sets the number times a cursor should move its position forward.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBCursor/advance
	 */
	this.DBCursor.advance

	/**
	 * @brief This method may raise a DOMException with a DOMError of one of the following types:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBCursor/continue
	 * @param optionalKey - The key to position the cursor at.
	 */
	this.DBCursor.continue

	/**
	 * @brief An IDBRequest object on which subsequent events related to this operation are fired. The result attribute is set to undefined.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBCursor/delete
	 */
	this.DBCursor.delete

	/**
	 * @brief An IDBRequest object on which subsequent events related to this operation are fired.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBCursor/update
	 */
	this.DBCursor.update

}

