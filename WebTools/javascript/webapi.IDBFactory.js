
/**
 * @brief In the following code snippet, we make a request to open a database, and include handlers for the success and error cases. For a full working example, see our To-do Notifications app (view example live.)
 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBFactory
 */
function IDBFactory() {

	/**
	 * @brief If an error occurs while the database connection is being opened, then an error event is fired on the request object returned from this method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBFactory/open
	 */
	this.DBFactory.open

	/**
	 * @brief If the database is successfully deleted, then a success event is fired on the request object returned from this method, with its result set to null. If an error occurs while the database is being deleted, then an error event is fired on the request object that is returned from this method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBFactory.open-obsolete">IDBFactory.open</a></code>, the original version <span title="This is an obsolete API and is no longer guaranteed to work."><i class="icon-trash"> </i></span></dt>
 <dd>An obsolete method to request opening a <a href="https://developer.mozilla.org/en-US/docs/IndexedDB#gloss_database_connection">connection to a database</a>, still implemented by some browsers.</dd>
 <dt><a href=
	 */
	this.DBFactory.deleteDatabase

	/**
	 * @brief An integer that indicates the result of the comparison; the table below lists the possible values and their meanings:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/IDBFactory/cmp
	 */
	this.DBFactory.cmp

}

