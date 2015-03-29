
/**
 * @brief The DataStoreCursor interface of the Data Store API represents a cursor that allows apps to iterate through a list of DataStoreTask objects representing the change history of the data store, for use when synchronizing the data.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStoreCursor
 */
function DataStoreCursor() {

	/**
	 * @brief The next() method of the DataStoreCursor interface makes a request to retrieve information about the next operation that changes a record in the data store. Returns a promise of type DataStoreTask.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStoreCursor/next
	 */
	this.next = function() {};

	/**
	 * @brief The close() method of the DataStoreCursor interface makes a request to terminate the cursor.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStoreCursor/close
	 */
	this.close = function() {};

}

