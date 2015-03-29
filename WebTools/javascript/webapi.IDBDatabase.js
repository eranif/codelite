
/**
 * @brief Inherits from: EventTarget
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBDatabase
 */
function IDBDatabase() {

	/**
	 * @brief The connection is not actually closed until all transactions created using this connection are complete. No new transactions can be created for this connection once this method is called. Methods that create transactions throw an exception if a closing operation is pending.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBDatabase/close
	 */
	this.DBDatabase.close

	/**
	 * @brief The method takes the name of the store as well as a parameter object that lets you define important optional properties. You can use the property to uniquely identify individual objects in the store. As the property is an identifier, it should be unique to every object, and every object should have that property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBDatabase/createObjectStore
	 */
	this.DBDatabase.createObjectStore

	/**
	 * @brief As with createObjectStore, this method can be called only within a versionchange transaction. So for WebKit browsers you must call the IDBVersionChangeRequest.setVersion method first before you can remove any object store or index.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBDatabase/deleteObjectStore
	 */
	this.DBDatabase.deleteObjectStore

	/**
	 * @brief An IDBTransaction object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBDatabase/transaction
	 */
	this.DBDatabase.transaction

}

