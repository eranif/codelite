
/**
 * @brief This example shows a variety of different uses of ObjectStores, from updating the data structure with IDBObjectStore.createIndex inside an onupgradeneeded function, to adding a new item to our object store with IDBObjectStore.add. For a full working example, see our To-do Notifications app (view example live.)
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore
 */
function IDBObjectStore() {

	/**
	 * @brief To determine if the add operation has completed successfully, listen for the transaction’s complete event in addition to the IDBObjectStore.add request’s success event, because the transaction may still fail after the success event fires. In other words, the success event is only triggered when the transaction has been successfully queued.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/add
	 */
	this.DBObjectStore.add

	/**
	 * @brief Clearing an object store consists of removing all records from the object store and removing all records in indexes that reference the object store.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/clear
	 */
	this.DBObjectStore.clear

	/**
	 * @brief An IDBRequest object on which subsequent events related to this operation are fired. The request.result attribute is set to undefined.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/delete
	 */
	this.DBObjectStore.delete

	/**
	 * @brief If a value is successfully found, then a structured clone of it is created and set as the result of the request object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/get
	 */
	this.DBObjectStore.get

	/**
	 * @brief Note that this method must be called only from a VersionChange transaction mode callback.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/createIndex
	 */
	this.DBObjectStore.createIndex

	/**
	 * @brief Note that this method must be called only from a VersionChange transaction mode callback. Note that this method synchronously modifies the IDBObjectStore.indexNames property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/deleteIndex
	 */
	this.DBObjectStore.deleteIndex

	/**
	 * @brief An IDBIndex object for accessing the index.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/index
	 */
	this.DBObjectStore.index

	/**
	 * @brief If the record is successfully stored, then a success event is fired on the returned request object with the result set to the key for the stored record, and the transaction set to the transaction in which this object store is opened.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/put
	 */
	this.DBObjectStore.put

	/**
	 * @brief To determine if the add operation has completed successfully, listen for the results’s success event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/openCursor
	 */
	this.DBObjectStore.openCursor

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBObjectStore/count
	 * @param optionalKeyRange - A key or IDBKeyRange object that specifies a range of records you want to count.
	 */
	this.DBObjectStore.count

}

