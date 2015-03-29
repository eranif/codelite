
/**
 * @brief The DataStore interface of the Data Store API represents a retrieved set of data, and includes standard properties for accessing the store's name, owner, etc., methods for reading, modifying and syncing data, and the onchange event handler for reacting to changes to the data.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore
 */
function DataStore() {

	/**
	 * @brief The get() method of the DataStore interface retrieves one or more objects from the current data store.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/get
	 * @param id - get() can passed one or more ids of specific records that you want to retrieve from the data store. When you want to retrieve multiple ids, you pass them in as multiple parameters, rather than an array or object, e.g. get(1,2,3).
	 */
	this.get = function(id) {};

	/**
	 * @brief The add() method of the DataStore interface adds a new record to the data store; if the record you are attempting to add already exists, it will throw an exception.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/add
	 * @param object - The object you want to add to the data store.
	 * @param id - You can specify an optional second parameter that will be a DataStoreKey of type unsigned long or DOMString, if you want to give the added record a specific id. If you don't specify this, the object's id will be numeric (auto incremented).
	 * @param revisionId - You can specify an optional third parameter that will be a revisionId (a DOMString). This can be used to prevent conflicts. If the revisionId is not the current revisionId for the current Data Store, the operation is aborted. This means that the developer has a 'old' revisionId and will have to manage the conflict somehow.
	 */
	this.add = function(object, id, revisionId) {};

	/**
	 * @brief The put() method of the DataStore interface updates an existing record in the data store.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/put
	 * @param obj - The object containing the data you want to update the record's value to.
	 * @param id - The id of the record to be updated. This will be either a DataStoreKey of type unsigned long or DOMString, if an id was specified by the developer when the record was created, or a numeric id if not.
	 * @param revisionId - You can specify an optional third parameter that will be a revisionId (a DOMString). This can be used to prevent conflicts. If the revisionId is not the current revisionId for the current Data Store, the operation is aborted. This means that the developer has a 'old' revisionId and will have to manage the conflict somehow.
	 */
	this.put = function(obj, id, revisionId) {};

	/**
	 * @brief The remove() method of the DataStore interface deletes one or more objects from the current data store.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/remove
	 * @param id - remove() can be passed one or more ids of specific records that you want to delete from the data store. When you want to delete multiple ids, you pass them in as multiple parameters, rather than an array or object, e.g. remove(1,2,3).
	 * @param revisionId - You can specify an optional second parameter that will be a revisionId (a DOMString). This can be used to prevent conflicts. If the revisionId is not the current revisionId for the current Data Store, the operation is aborted. This means that the developer has a 'old' revisionId and will have to manage the conflict somehow.
	 */
	this.remove = function(id, revisionId) {};

	/**
	 * @brief The clear() method of the DataStore interface deletes all records from the data store, leaving it empty.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/clear
	 * @param revisionId - You can specify an optional parameter that will be a revisionId (a DOMString). This can be used to prevent conflicts. If the revisionId is not the current revisionId for the current Data Store, the operation is aborted. This means that the developer has a 'old' revisionId and will have to manage the conflict somehow.
	 */
	this.clear = function(revisionId) {};

	/**
	 * @brief The getLength() method of the DataStore interface returns the number of records stored in the current data store.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/getLength
	 */
	this.getLength = function() {};

	/**
	 * @brief The sync() method of the DataStore interface opens a cursor that allows you to step through any changes that have taken place in the data store going back to a particular revision ID, and run code in response to different types of change.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DataStore/sync
	 * @param revisionId Optional - You can optionally pass sync() a revision ID that specifies the place in the revision history to go back to when running through the tasks. If you don't specify this, it runs through all tasks going back to the beginning of the history.
	 */
	this.sync = function(revisionId Optional) {};

}

