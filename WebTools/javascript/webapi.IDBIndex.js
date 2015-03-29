
/**
 * @brief You can retrieve records in an object store through the primary key or by using an index. An index lets you look up records in an object store using properties of the values in the object stores records other than the primary key
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex
 */
function IDBIndex() {

	/**
	 * @brief A IDBRequest object on which subsequent events related to this operation are fired.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex/count
	 */
	this.DBIndex.count

	/**
	 * @brief If a value is successfully found, then a structured clone of it is created and set as the result of the request object: this returns the record the key is associated with.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex/get
	 */
	this.DBIndex.get

	/**
	 * @brief If a key is successfully found it is set as the result of the request object: this returns the primary key of the record the key is associated with, not the whole record as IDBIndex.get does.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex/getKey
	 */
	this.DBIndex.getKey

	/**
	 * @brief The method sets the position of the cursor to the appropriate record, based on the specified direction.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex/openCursor
	 */
	this.DBIndex.openCursor

	/**
	 * @brief The method sets the position of the cursor to the appropriate key, based on the specified direction.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBIndex/openKeyCursor
	 */
	this.DBIndex.openKeyCursor

}

