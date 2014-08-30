<?php

// Start of SQLite v.2.0-dev

/**
 * @link http://www.php.net/manual/en/class.sqlitedatabase.php
 */
class SQLiteDatabase  {

	/**
	 * @param filename
	 * @param mode[optional]
	 * @param error_message[optional]
	 */
	final public function __construct ($filename, $mode, &$error_message) {}

	/**
	 * @param query
	 * @param result_type[optional]
	 * @param error_message[optional]
	 */
	public function query ($query, $result_type, &$error_message) {}

	/**
	 * @param query
	 * @param error_message[optional]
	 */
	public function queryExec ($query, &$error_message) {}

	/**
	 * @param query
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function arrayQuery ($query, $result_type, $decode_binary) {}

	/**
	 * @param query
	 * @param first_row_only[optional]
	 * @param decode_binary[optional]
	 */
	public function singleQuery ($query, $first_row_only, $decode_binary) {}

	/**
	 * @param query
	 * @param result_type[optional]
	 * @param error_message[optional]
	 */
	public function unbufferedQuery ($query, $result_type, &$error_message) {}

	public function lastInsertRowid () {}

	public function changes () {}

	/**
	 * @param funcname
	 * @param step_func
	 * @param finalize_func
	 * @param num_args[optional]
	 */
	public function createAggregate ($funcname, $step_func, $finalize_func, $num_args) {}

	/**
	 * @param funcname
	 * @param callback
	 * @param num_args[optional]
	 */
	public function createFunction ($funcname, $callback, $num_args) {}

	/**
	 * @param ms
	 */
	public function busyTimeout ($ms) {}

	public function lastError () {}

	/**
	 * @param table_name
	 * @param result_type[optional]
	 */
	public function fetchColumnTypes ($table_name, $result_type) {}

}

/**
 * @link http://www.php.net/manual/en/class.sqliteresult.php
 */
final class SQLiteResult implements Iterator, Traversable, Countable {

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function fetch ($result_type, $decode_binary) {}

	/**
	 * @param class_name[optional]
	 * @param ctor_params[optional]
	 * @param decode_binary[optional]
	 */
	public function fetchObject ($class_name, $ctor_params, $decode_binary) {}

	/**
	 * @param decode_binary[optional]
	 */
	public function fetchSingle ($decode_binary) {}

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function fetchAll ($result_type, $decode_binary) {}

	/**
	 * @param index_or_name
	 * @param decode_binary[optional]
	 */
	public function column ($index_or_name, $decode_binary) {}

	public function numFields () {}

	/**
	 * @param field_index
	 */
	public function fieldName ($field_index) {}

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function current ($result_type, $decode_binary) {}

	public function key () {}

	public function next () {}

	public function valid () {}

	public function rewind () {}

	public function count () {}

	public function prev () {}

	public function hasPrev () {}

	public function numRows () {}

	/**
	 * @param row
	 */
	public function seek ($row) {}

}

/**
 * @link http://www.php.net/manual/en/class.sqliteunbuffered.php
 */
final class SQLiteUnbuffered  {

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function fetch ($result_type, $decode_binary) {}

	/**
	 * @param class_name[optional]
	 * @param ctor_params[optional]
	 * @param decode_binary[optional]
	 */
	public function fetchObject ($class_name, $ctor_params, $decode_binary) {}

	/**
	 * @param decode_binary[optional]
	 */
	public function fetchSingle ($decode_binary) {}

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function fetchAll ($result_type, $decode_binary) {}

	/**
	 * @param index_or_name
	 * @param decode_binary[optional]
	 */
	public function column ($index_or_name, $decode_binary) {}

	public function numFields () {}

	/**
	 * @param field_index
	 */
	public function fieldName ($field_index) {}

	/**
	 * @param result_type[optional]
	 * @param decode_binary[optional]
	 */
	public function current ($result_type, $decode_binary) {}

	public function next () {}

	public function valid () {}

}

final class SQLiteException extends RuntimeException  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;


	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

/**
 * Opens an SQLite database and create the database if it does not exist
 * @link http://www.php.net/manual/en/function.sqlite-open.php
 * @param filename string <p>
 *       The filename of the SQLite database.  If the file does not exist, SQLite
 *       will attempt to create it.  PHP must have write permissions to the file
 *       if data is inserted, the database schema is modified or to create the
 *       database if it does not exist.
 *      </p>
 * @param mode int[optional] <p>
 *       The mode of the file. Intended to be used to open the database in
 *       read-only mode.  Presently, this parameter is ignored by the sqlite
 *       library.  The default value for mode is the octal value
 *       0666 and this is the recommended value.
 *      </p>
 * @param error_message string[optional] <p>
 *       Passed by reference and is set to hold a descriptive error message
 *       explaining why the database could not be opened if there was an error.
 *      </p>
 */
function sqlite_open ($filename, $mode = null, &$error_message = null) {}

/**
 * Opens a persistent handle to an SQLite database and create the database if it does not exist
 * @link http://www.php.net/manual/en/function.sqlite-popen.php
 * @param filename string <p>
 *       The filename of the SQLite database.  If the file does not exist, SQLite
 *       will attempt to create it.  PHP must have write permissions to the file
 *       if data is inserted, the database schema is modified or to create the
 *       database if it does not exist.
 *      </p>
 * @param mode int[optional] <p>
 *       The mode of the file. Intended to be used to open the database in
 *       read-only mode.  Presently, this parameter is ignored by the sqlite
 *       library.  The default value for mode is the octal value
 *       0666 and this is the recommended value.
 *      </p>
 * @param error_message string[optional] <p>
 *       Passed by reference and is set to hold a descriptive error message
 *       explaining why the database could not be opened if there was an error.
 *      </p>
 * @return resource a resource (database handle) on success, false on error.
 */
function sqlite_popen ($filename, $mode = null, &$error_message = null) {}

/**
 * Closes an open SQLite database
 * @link http://www.php.net/manual/en/function.sqlite-close.php
 * @param dbhandle resource <p>
 *       The SQLite Database resource; returned from sqlite_open
 *       when used procedurally.
 *      </p>
 * @return void 
 */
function sqlite_close ($dbhandle) {}

/**
 * Executes a query against a given database and returns a result handle
 * @link http://www.php.net/manual/en/function.sqlite-query.php
 * @param query string <p>
 *       The query to be executed.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param result_type int[optional] &sqlite.result-type;
 * @param error_msg string[optional] <p>
 *       The specified variable will be filled if an error occurs. This is
 *       specially important because SQL syntax errors can't be fetched using
 *       the sqlite_last_error function.
 *      </p>
 * @return SQLiteResult This function will return a result handle&return.falseforfailure;.
 *   For queries that return rows, the result handle can then be used with
 *   functions such as sqlite_fetch_array and
 *   sqlite_seek.
 *  </p>
 *  <p>
 *   Regardless of the query type, this function will return false if the
 *   query failed.
 *  </p>
 *  <p>
 *   sqlite_query returns a buffered, seekable result
 *   handle.  This is useful for reasonably small queries where you need to
 *   be able to randomly access the rows.  Buffered result handles will
 *   allocate memory to hold the entire result and will not return until it
 *   has been fetched.  If you only need sequential access to the data, it is
 *   recommended that you use the much higher performance
 *   sqlite_unbuffered_query instead.
 */
function sqlite_query ($query, $result_type = null, &$error_msg = null) {}

/**
 * Executes a result-less query against a given database
 * @link http://www.php.net/manual/en/function.sqlite-exec.php
 * @param query string <p>
 *       The query to be executed.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param error_msg string[optional] <p>
 *       The specified variable will be filled if an error occurs. This is
 *       specially important because SQL syntax errors can't be fetched using
 *       the sqlite_last_error function.
 *      </p>
 * @return bool This function will return a boolean result; true for success or false for failure.
 *   If you need to run a query that returns rows, see sqlite_query.
 */
function sqlite_exec ($query, &$error_msg = null) {}

/**
 * Execute a query against a given database and returns an array
 * @link http://www.php.net/manual/en/function.sqlite-array-query.php
 * @param query string <p>
 *       The query to be executed.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param result_type int[optional] &sqlite.result-type;
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return array an array of the entire result set; false otherwise.
 */
function sqlite_array_query ($query, $result_type = null, $decode_binary = null) {}

/**
 * Executes a query and returns either an array for one single column or the value of the first row
 * @link http://www.php.net/manual/en/function.sqlite-single-query.php
 * @param query string 
 * @param first_row_only bool[optional] 
 * @param decode_binary bool[optional] 
 * @return array 
 */
function sqlite_single_query ($query, $first_row_only = null, $decode_binary = null) {}

/**
 * Fetches the next row from a result set as an array
 * @link http://www.php.net/manual/en/function.sqlite-fetch-array.php
 * @param result_type int[optional] &sqlite.result-type;
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return array an array of the next row from a result set; false if the
 *   next position is beyond the final row.
 */
function sqlite_fetch_array ($result_type = null, $decode_binary = null) {}

/**
 * Fetches the next row from a result set as an object
 * @link http://www.php.net/manual/en/function.sqlite-fetch-object.php
 * @param class_name string[optional] 
 * @param ctor_params array[optional] 
 * @param decode_binary bool[optional] 
 * @return object 
 */
function sqlite_fetch_object ($class_name = null, array $ctor_params = null, $decode_binary = null) {}

/**
 * Fetches the first column of a result set as a string
 * @link http://www.php.net/manual/en/function.sqlite-fetch-single.php
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return string the first column value, as a string.
 */
function sqlite_fetch_single ($decode_binary = null) {}

/**
 * &Alias; <function>sqlite_fetch_single</function>
 * @link http://www.php.net/manual/en/function.sqlite-fetch-string.php
 * @param result
 * @param decode_binary[optional]
 */
function sqlite_fetch_string ($result, $decode_binary) {}

/**
 * Fetches all rows from a result set as an array of arrays
 * @link http://www.php.net/manual/en/function.sqlite-fetch-all.php
 * @param result_type int[optional] &sqlite.result-type;
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return array an array of the remaining rows in a result set. If called right
 *   after sqlite_query, it returns all rows. If called
 *   after sqlite_fetch_array, it returns the rest. If
 *   there are no rows in a result set, it returns an empty array.
 */
function sqlite_fetch_all ($result_type = null, $decode_binary = null) {}

/**
 * Fetches the current row from a result set as an array
 * @link http://www.php.net/manual/en/function.sqlite-current.php
 * @param result_type int[optional] &sqlite.result-type;
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return array an array of the current row from a result set; false if the
 *   current position is beyond the final row.
 */
function sqlite_current ($result_type = null, $decode_binary = null) {}

/**
 * Fetches a column from the current row of a result set
 * @link http://www.php.net/manual/en/function.sqlite-column.php
 * @param index_or_name mixed <p>
 *       The column index or name to fetch.
 *      </p>
 * @param decode_binary bool[optional] &sqlite.decode-bin;
 * @return mixed the column value.
 */
function sqlite_column ($index_or_name, $decode_binary = null) {}

/**
 * Returns the version of the linked SQLite library
 * @link http://www.php.net/manual/en/function.sqlite-libversion.php
 * @return string the library version, as a string.
 */
function sqlite_libversion () {}

/**
 * Returns the encoding of the linked SQLite library
 * @link http://www.php.net/manual/en/function.sqlite-libencoding.php
 * @return string the library encoding.
 */
function sqlite_libencoding () {}

/**
 * Returns the number of rows that were changed by the most
   recent SQL statement
 * @link http://www.php.net/manual/en/function.sqlite-changes.php
 * @param db
 * @return int the number of changed rows.
 */
function sqlite_changes ($db) {}

/**
 * Returns the rowid of the most recently inserted row
 * @link http://www.php.net/manual/en/function.sqlite-last-insert-rowid.php
 * @param db
 * @return int the row id, as an integer.
 */
function sqlite_last_insert_rowid ($db) {}

/**
 * Returns the number of rows in a buffered result set
 * @link http://www.php.net/manual/en/function.sqlite-num-rows.php
 * @param result
 * @return int the number of rows, as an integer.
 */
function sqlite_num_rows ($result) {}

/**
 * Returns the number of fields in a result set
 * @link http://www.php.net/manual/en/function.sqlite-num-fields.php
 * @param result
 * @return int the number of fields, as an integer.
 */
function sqlite_num_fields ($result) {}

/**
 * Returns the name of a particular field
 * @link http://www.php.net/manual/en/function.sqlite-field-name.php
 * @param field_index int <p>
 *       The ordinal column number in the result set.
 *      </p>
 * @return string the name of a field in an SQLite result set, given the ordinal
 *   column number; false on error.
 */
function sqlite_field_name ($field_index) {}

/**
 * Seek to a particular row number of a buffered result set
 * @link http://www.php.net/manual/en/function.sqlite-seek.php
 * @param rownum int <p>
 *       The ordinal row number to seek to.  The row number is zero-based (0 is
 *       the first row).
 *      </p>
 *      &sqlite.no-unbuffered;
 * @return bool false if the row does not exist, true otherwise.
 */
function sqlite_seek ($rownum) {}

/**
 * Seek to the first row number
 * @link http://www.php.net/manual/en/function.sqlite-rewind.php
 * @param result
 * @return bool false if there are no rows in the result set, true otherwise.
 */
function sqlite_rewind ($result) {}

/**
 * Seek to the next row number
 * @link http://www.php.net/manual/en/function.sqlite-next.php
 * @param result
 * @return bool true on success, or false if there are no more rows.
 */
function sqlite_next ($result) {}

/**
 * Seek to the previous row number of a result set
 * @link http://www.php.net/manual/en/function.sqlite-prev.php
 * @param result
 * @return bool true on success, or false if there are no more previous rows.
 */
function sqlite_prev ($result) {}

/**
 * Returns whether more rows are available
 * @link http://www.php.net/manual/en/function.sqlite-valid.php
 * @param result
 * @return bool true if there are more rows available from the
 *   result handle, or false otherwise.
 */
function sqlite_valid ($result) {}

/**
 * Finds whether or not more rows are available
 * @link http://www.php.net/manual/en/function.sqlite-has-more.php
 * @param result resource <p>
 *       The SQLite result resource.
 *      </p>
 * @return bool true if there are more rows available from the
 *   result handle, or false otherwise.
 */
function sqlite_has_more ($result) {}

/**
 * Returns whether or not a previous row is available
 * @link http://www.php.net/manual/en/function.sqlite-has-prev.php
 * @param result
 * @return bool true if there are more previous rows available from the
 *   result handle, or false otherwise.
 */
function sqlite_has_prev ($result) {}

/**
 * Escapes a string for use as a query parameter
 * @link http://www.php.net/manual/en/function.sqlite-escape-string.php
 * @param item string <p>
 *       The string being quoted.
 *      </p>
 *      <p>
 *       If the item contains a NUL
 *       character, or if it begins with a character whose ordinal value is
 *       0x01, PHP will apply a binary encoding scheme so that
 *       you can safely store and retrieve binary data.
 *      </p>
 * @return string an escaped string for use in an SQLite SQL statement.
 */
function sqlite_escape_string ($item) {}

/**
 * Set busy timeout duration, or disable busy handlers
 * @link http://www.php.net/manual/en/function.sqlite-busy-timeout.php
 * @param milliseconds int <p>
 *       The number of milliseconds.  When set to
 *       0, busy handlers will be disabled and SQLite will
 *       return immediately with a SQLITE_BUSY status code
 *       if another process/thread has the database locked for an update.
 *      </p>
 *      <p>
 *       PHP sets the default busy timeout to be 60 seconds when the database is
 *       opened.
 *      </p>
 *      
 *       <p>
 *        There are one thousand (1000) milliseconds in one second.
 *       </p>
 * @return void 
 */
function sqlite_busy_timeout ($milliseconds) {}

/**
 * Returns the error code of the last error for a database
 * @link http://www.php.net/manual/en/function.sqlite-last-error.php
 * @param db
 * @return int an error code, or 0 if no error occurred.
 */
function sqlite_last_error ($db) {}

/**
 * Returns the textual description of an error code
 * @link http://www.php.net/manual/en/function.sqlite-error-string.php
 * @param error_code int <p>
 *       The error code being used, which might be passed in from
 *       sqlite_last_error.
 *      </p>
 * @return string a human readable description of the error_code,
 *   as a string.
 */
function sqlite_error_string ($error_code) {}

/**
 * Execute a query that does not prefetch and buffer all data
 * @link http://www.php.net/manual/en/function.sqlite-unbuffered-query.php
 * @param query string <p>
 *       The query to be executed.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param result_type int[optional] &sqlite.result-type;
 * @param error_msg string[optional] <p>
 *       The specified variable will be filled if an error occurs. This is
 *       specially important because SQL syntax errors can't be fetched using
 *       the sqlite_last_error function.
 *      </p>
 * @return SQLiteUnbuffered a result handle&return.falseforfailure;.
 *  </p>
 *  <p>
 *   sqlite_unbuffered_query returns a sequential
 *   forward-only result set that can only be used to read each row, one after
 *   the other.
 */
function sqlite_unbuffered_query ($query, $result_type = null, &$error_msg = null) {}

/**
 * Register an aggregating UDF for use in SQL statements
 * @link http://www.php.net/manual/en/function.sqlite-create-aggregate.php
 * @param function_name string <p>
 *       The name of the function used in SQL statements.
 *      </p>
 * @param step_func callback <p>
 *       Callback function called for each row of the result set.
 *       Function parameters are &$context, $value, ....
 *      </p>
 * @param finalize_func callback <p>
 *       Callback function to aggregate the "stepped" data from each row.
 *       Function parameter is &$context and the function
 *       should return the final result of aggregation.
 *      </p>
 * @param num_args int[optional] <p>
 *       Hint to the SQLite parser if the callback function accepts a
 *       predetermined number of arguments.
 *      </p>
 * @return void 
 */
function sqlite_create_aggregate ($function_name, $step_func, $finalize_func, $num_args = null) {}

/**
 * Registers a "regular" User Defined Function for use in SQL statements
 * @link http://www.php.net/manual/en/function.sqlite-create-function.php
 * @param function_name string <p>
 *       The name of the function used in SQL statements.
 *      </p>
 * @param callback callback <p>
 *       Callback function to handle the defined SQL function.
 *      </p>
 *      
 *       
 *        Callback functions should return a type understood by SQLite (i.e.
 *        scalar type).
 * @param num_args int[optional] <p>
 *       Hint to the SQLite parser if the callback function accepts a
 *       predetermined number of arguments.
 *      </p>
 * @return void 
 */
function sqlite_create_function ($function_name, $callback, $num_args = null) {}

/**
 * Opens an SQLite database and returns an SQLiteDatabase object
 * @link http://www.php.net/manual/en/function.sqlite-factory.php
 * @param filename string <p>
 *       The filename of the SQLite database.
 *      </p>
 * @param mode int[optional] <p>
 *       The mode of the file. Intended to be used to open the database in
 *       read-only mode.  Presently, this parameter is ignored by the sqlite
 *       library.  The default value for mode is the octal value
 *       0666 and this is the recommended value.
 *      </p>
 * @param error_message string[optional] <p>
 *       Passed by reference and is set to hold a descriptive error message
 *       explaining why the database could not be opened if there was an error.
 *      </p>
 * @return SQLiteDatabase an SQLiteDatabase object on success, &null; on error.
 */
function sqlite_factory ($filename, $mode = null, &$error_message = null) {}

/**
 * Encode binary data before returning it from an UDF
 * @link http://www.php.net/manual/en/function.sqlite-udf-encode-binary.php
 * @param data string <p>
 *       The string being encoded.
 *      </p>
 * @return string The encoded string.
 */
function sqlite_udf_encode_binary ($data) {}

/**
 * Decode binary data passed as parameters to an <acronym>UDF</acronym>
 * @link http://www.php.net/manual/en/function.sqlite-udf-decode-binary.php
 * @param data string <p>
 *       The encoded data that will be decoded, data that was applied by either
 *       sqlite_udf_encode_binary or
 *       sqlite_escape_string.       
 *      </p>
 * @return string The decoded string.
 */
function sqlite_udf_decode_binary ($data) {}

/**
 * Return an array of column types from a particular table
 * @link http://www.php.net/manual/en/function.sqlite-fetch-column-types.php
 * @param table_name string <p>
 *       The table name to query.
 *      </p>
 * @param result_type int[optional] <p>
 *       The optional result_type parameter accepts a
 *       constant and determines how the returned array will be indexed. Using
 *       SQLITE_ASSOC will return only associative indices
 *       (named fields) while SQLITE_NUM will return only
 *       numerical indices (ordinal field numbers).
 *       SQLITE_ASSOC is the default for
 *       this function.
 *      </p>
 * @return array an array of column data types; false on error.
 */
function sqlite_fetch_column_types ($table_name, $result_type = null) {}


/**
 * Columns are returned into the array having both a numerical index
 *     and the field name as the array index.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_BOTH', 3);

/**
 * Columns are returned into the array having a numerical index to the
 *     fields. This index starts with 0, the first field in the result.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_NUM', 2);

/**
 * Columns are returned into the array having the field name as the array
 *     index.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_ASSOC', 1);

/**
 * Successful result.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_OK', 0);

/**
 * SQL error or missing database.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_ERROR', 1);

/**
 * An internal logic error in SQLite.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_INTERNAL', 2);

/**
 * Access permission denied.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_PERM', 3);

/**
 * Callback routine requested an abort.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_ABORT', 4);

/**
 * The database file is locked.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_BUSY', 5);

/**
 * A table in the database is locked.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_LOCKED', 6);

/**
 * Memory allocation failed.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_NOMEM', 7);

/**
 * Attempt to write a readonly database.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_READONLY', 8);

/**
 * Operation terminated internally.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_INTERRUPT', 9);

/**
 * Disk I/O error occurred.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_IOERR', 10);

/**
 * The database disk image is malformed.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_CORRUPT', 11);

/**
 * (Internal) Table or record not found.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_NOTFOUND', 12);

/**
 * Insertion failed because database is full.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_FULL', 13);

/**
 * Unable to open the database file.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_CANTOPEN', 14);

/**
 * Database lock protocol error.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_PROTOCOL', 15);

/**
 * (Internal) Database table is empty.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_EMPTY', 16);

/**
 * The database schema changed.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_SCHEMA', 17);

/**
 * Too much data for one row of a table.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_TOOBIG', 18);

/**
 * Abort due to constraint violation.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_CONSTRAINT', 19);

/**
 * Data type mismatch.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_MISMATCH', 20);

/**
 * Library used incorrectly.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_MISUSE', 21);

/**
 * Uses of OS features not supported on host.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_NOLFS', 22);

/**
 * Authorized failed.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_AUTH', 23);

/**
 * File opened that is not a database file.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_NOTADB', 26);

/**
 * Auxiliary database format error.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_FORMAT', 24);

/**
 * Internal process has another row ready.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_ROW', 100);

/**
 * Internal process has finished executing.
 * @link http://www.php.net/manual/en/sqlite.constants.php
 */
define ('SQLITE_DONE', 101);

// End of SQLite v.2.0-dev
?>
