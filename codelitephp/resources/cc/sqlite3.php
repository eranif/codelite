<?php

// Start of sqlite3 v.0.7-dev

class SQLite3  {

	/**
	 * Opens an SQLite database
	 * @link http://www.php.net/manual/en/sqlite3.open.php
	 * @param filename string <p>
	 *       Path to the SQLite database, or :memory: to use in-memory database.
	 *      </p>
	 * @param flags int[optional] <p>
	 *       Optional flags used to determine how to open the SQLite database. By
	 *       default, open uses SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE.
	 *       
	 *        
	 *         <p>
	 *          SQLITE3_OPEN_READONLY: Open the database for
	 *          reading only.
	 *         </p>
	 * @param encryption_key string[optional] <p>
	 *       An optional encryption key used when encrypting and decrypting an
	 *       SQLite database.
	 *      </p>
	 * @return void 
	 */
	public function open ($filename, $flags = null, $encryption_key = null) {}

	/**
	 * Closes the database connection
	 * @link http://www.php.net/manual/en/sqlite3.close.php
	 * @return bool true on success, false on failure.
	 */
	public function close () {}

	/**
	 * Executes a result-less query against a given database
	 * @link http://www.php.net/manual/en/sqlite3.exec.php
	 * @param query string <p>
	 *       The SQL query to execute (typically an INSERT, UPDATE, or DELETE
	 *       query).
	 *      </p>
	 * @return bool true if the query succeeded, false on failure.
	 */
	public function exec ($query) {}

	/**
	 * Returns the SQLite3 library version as a string constant and as a number
	 * @link http://www.php.net/manual/en/sqlite3.version.php
	 * @return array an associative array with the keys "versionString" and
	 *   "versionNumber".
	 */
	public static function version () {}

	/**
	 * Returns the row ID of the most recent INSERT into the database
	 * @link http://www.php.net/manual/en/sqlite3.lastinsertrowid.php
	 * @return int the row ID of the most recent INSERT into the database
	 */
	public function lastInsertRowID () {}

	/**
	 * Returns the numeric result code of the most recent failed SQLite request
	 * @link http://www.php.net/manual/en/sqlite3.lasterrorcode.php
	 * @return int an integer value representing the numeric result code of the most
	 *   recent failed SQLite request.
	 */
	public function lastErrorCode () {}

	/**
	 * Returns English text describing the most recent failed SQLite request
	 * @link http://www.php.net/manual/en/sqlite3.lasterrormsg.php
	 * @return string an English string describing the most recent failed SQLite request.
	 */
	public function lastErrorMsg () {}

	/**
	 * Sets the busy connection handler
	 * @link http://www.php.net/manual/en/sqlite3.busytimeout.php
	 * @param msecs int <p>
	 *       The milliseconds to sleep. Setting this value to a value less than
	 *       or equal to zero, will turn off an already set timeout handler.
	 *      </p>
	 * @return bool true on success, false on failure.
	 */
	public function busyTimeout ($msecs) {}

	/**
	 * Attempts to load an SQLite extension library
	 * @link http://www.php.net/manual/en/sqlite3.loadextension.php
	 * @param shared_library string <p>
	 *       The name of the library to load. The library must be located in the
	 *       directory specified in the configure option sqlite3.extension_dir.
	 *      </p>
	 * @return bool true if the extension is successfully loaded, false on failure.
	 */
	public function loadExtension ($shared_library) {}

	/**
	 * Returns the number of database rows that were changed (or inserted or
   deleted) by the most recent SQL statement
	 * @link http://www.php.net/manual/en/sqlite3.changes.php
	 * @return int an integer value corresponding to the number of
	 *   database rows changed (or inserted or deleted) by the most recent SQL
	 *   statement.
	 */
	public function changes () {}

	/**
	 * Returns a string that has been properly escaped
	 * @link http://www.php.net/manual/en/sqlite3.escapestring.php
	 * @param value string <p>
	 *       The string to be escaped.
	 *      </p>
	 * @return string a properly escaped string that may be used safely in an SQL
	 *   statement.
	 */
	public static function escapeString ($value) {}

	/**
	 * Prepares an SQL statement for execution
	 * @link http://www.php.net/manual/en/sqlite3.prepare.php
	 * @param query string <p>
	 *       The SQL query to prepare.
	 *      </p>
	 * @return SQLite3Stmt an SQLite3Stmt object on success&return.falseforfailure;.
	 */
	public function prepare ($query) {}

	/**
	 * Executes an SQL query
	 * @link http://www.php.net/manual/en/sqlite3.query.php
	 * @param query string <p>
	 *       The SQL query to execute.
	 *      </p>
	 * @return SQLite3Result an SQLite3Result object if the query returns results. Otherwise,
	 *   returns true if the query succeeded, false on failure.
	 */
	public function query ($query) {}

	/**
	 * Executes a query and returns a single result
	 * @link http://www.php.net/manual/en/sqlite3.querysingle.php
	 * @param query string <p>
	 *       The SQL query to execute.
	 *      </p>
	 * @param entire_row bool[optional] <p>
	 *       By default, querySingle returns the value of the
	 *       first column returned by the query. If
	 *       entire_row is true, then it returns an array
	 *       of the entire first row.
	 *      </p>
	 * @return mixed the value of the first column of results or an array of the entire
	 *   first row (if entire_row is true).
	 *  </p>
	 *  <p>
	 *   If the query is valid but no results are returned, then &null; will be
	 *   returned if entire_row is false, otherwise an
	 *   empty array is returned.
	 *  </p>
	 *  <p>
	 *   Invalid or failing queries will return false.
	 */
	public function querySingle ($query, $entire_row = null) {}

	/**
	 * Registers a PHP function for use as an SQL scalar function
	 * @link http://www.php.net/manual/en/sqlite3.createfunction.php
	 * @param name string <p>
	 *       Name of the SQL function to be created or redefined.
	 *      </p>
	 * @param callback mixed <p>
	 *       The name of a PHP function or user-defined function to apply as a
	 *       callback, defining the behavior of the SQL function.
	 *      </p>
	 * @param argument_count int[optional] <p>
	 *       The number of arguments that the SQL function takes. If
	 *       this parameter is negative, then the SQL function may take
	 *       any number of arguments.
	 *      </p>
	 * @return bool true upon successful creation of the function, false on failure.
	 */
	public function createFunction ($name, $callback, $argument_count = null) {}

	/**
	 * Registers a PHP function for use as an SQL aggregate function
	 * @link http://www.php.net/manual/en/sqlite3.createaggregate.php
	 * @param name string <p>
	 *       Name of the SQL aggregate to be created or redefined.
	 *      </p>
	 * @param step_callback mixed <p>
	 *       The name of a PHP function or user-defined function to apply as a
	 *       callback for every item in the aggregate.
	 *      </p>
	 * @param final_callback mixed <p>
	 *       The name of a PHP function or user-defined function to apply as a
	 *       callback at the end of the aggregate data.
	 *      </p>
	 * @param argument_count int[optional] <p>
	 *       The number of arguments that the SQL aggregate takes. If
	 *       this parameter is negative, then the SQL aggregate may take
	 *       any number of arguments.
	 *      </p>
	 * @return bool true upon successful creation of the aggregate, false on
	 *   failure.
	 */
	public function createAggregate ($name, $step_callback, $final_callback, $argument_count = null) {}

	/**
	 * @param table
	 * @param column
	 * @param rowid
	 * @param dbname[optional]
	 */
	public function openBlob ($table, $column, $rowid, $dbname) {}

	/**
	 * @param enableExceptions
	 */
	public function enableExceptions ($enableExceptions) {}

	/**
	 * Instantiates an SQLite3 object and opens an SQLite 3 database
	 * @link http://www.php.net/manual/en/sqlite3.construct.php
	 * @param filename
	 * @param flags
	 * @param encryption_key
	 */
	public function __construct ($filename, $flags, $encryption_key) {}

}

class SQLite3Stmt  {

	/**
	 * Returns the number of parameters within the prepared statement
	 * @link http://www.php.net/manual/en/sqlite3stmt.paramcount.php
	 * @return int the number of parameters within the prepared statement.
	 */
	public function paramCount () {}

	/**
	 * Closes the prepared statement
	 * @link http://www.php.net/manual/en/sqlite3stmt.close.php
	 * @return bool true
	 */
	public function close () {}

	/**
	 * Resets the prepared statement
	 * @link http://www.php.net/manual/en/sqlite3stmt.reset.php
	 * @return bool true if the statement is successfully reset, false on failure.
	 */
	public function reset () {}

	/**
	 * Clears all current bound parameters
	 * @link http://www.php.net/manual/en/sqlite3stmt.clear.php
	 * @return bool true on successful clearing of bound parameters, false on
	 *   failure.
	 */
	public function clear () {}

	/**
	 * Executes a prepared statement and returns a result set object
	 * @link http://www.php.net/manual/en/sqlite3stmt.execute.php
	 * @return SQLite3Result an SQLite3Result object on successful execution of the prepared
	 *   statement, false on failure.
	 */
	public function execute () {}

	/**
	 * Binds a parameter to a statement variable
	 * @link http://www.php.net/manual/en/sqlite3stmt.bindparam.php
	 * @param sql_param string <p>
	 *       An string identifying the statement variable to which the
	 *       parameter should be bound.
	 *      </p>
	 * @param param mixed <p>
	 *       The parameter to bind to a statement variable.
	 *      </p>
	 * @param type int[optional] <p>
	 *       The data type of the parameter to bind.
	 *       
	 *        
	 *         <p>
	 *          SQLITE3_INTEGER: The value is a signed integer,
	 *          stored in 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of
	 *          the value.
	 *         </p>
	 * @return bool true if the parameter is bound to the statement variable, false
	 *   on failure.
	 */
	public function bindParam ($sql_param, &$param, $type = null) {}

	/**
	 * Binds the value of a parameter to a statement variable
	 * @link http://www.php.net/manual/en/sqlite3stmt.bindvalue.php
	 * @param sql_param string <p>
	 *       An string identifying the statement variable to which the
	 *       value should be bound.
	 *      </p>
	 * @param value mixed <p>
	 *       The value to bind to a statement variable.
	 *      </p>
	 * @param type int[optional] <p>
	 *       The data type of the value to bind.
	 *       
	 *        
	 *         <p>
	 *          SQLITE3_INTEGER: The value is a signed integer,
	 *          stored in 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of
	 *          the value.
	 *         </p>
	 * @return bool true if the value is bound to the statement variable, false
	 *   on failure.
	 */
	public function bindValue ($sql_param, $value, $type = null) {}

	public function readOnly () {}

	/**
	 * @param sqlite3
	 */
	private function __construct ($sqlite3) {}

}

class SQLite3Result  {

	/**
	 * Returns the number of columns in the result set
	 * @link http://www.php.net/manual/en/sqlite3result.numcolumns.php
	 * @return int the number of columns in the result set.
	 */
	public function numColumns () {}

	/**
	 * Returns the name of the nth column
	 * @link http://www.php.net/manual/en/sqlite3result.columnname.php
	 * @param column_number int <p>
	 *       The numeric zero-based index of the column.
	 *      </p>
	 * @return string the string name of the column identified by
	 *   column_number.
	 */
	public function columnName ($column_number) {}

	/**
	 * Returns the type of the nth column
	 * @link http://www.php.net/manual/en/sqlite3result.columntype.php
	 * @param column_number int <p>
	 *       The numeric zero-based index of the column.
	 *      </p>
	 * @return int the data type index of the column identified by
	 *   column_number (one of
	 *   SQLITE3_INTEGER, SQLITE3_FLOAT,
	 *   SQLITE3_TEXT, SQLITE3_BLOB, or
	 *   SQLITE3_NULL).
	 */
	public function columnType ($column_number) {}

	/**
	 * Fetches a result row as an associative or numerically indexed array or both
	 * @link http://www.php.net/manual/en/sqlite3result.fetcharray.php
	 * @param mode int[optional] <p>
	 *       Controls how the next row will be returned to the caller. This value
	 *       must be one of either SQLITE3_ASSOC,
	 *       SQLITE3_NUM, or SQLITE3_BOTH.
	 *       
	 *        
	 *         <p>
	 *          SQLITE3_ASSOC: returns an array indexed by column
	 *          name as returned in the corresponding result set
	 *         </p>
	 * @return array a result row as an associatively or numerically indexed array or
	 *   both.
	 */
	public function fetchArray ($mode = null) {}

	/**
	 * Resets the result set back to the first row
	 * @link http://www.php.net/manual/en/sqlite3Result.reset.php
	 * @return bool true if the result set is successfully reset
	 *   back to the first row, false on failure.
	 */
	public function reset () {}

	/**
	 * Closes the result set
	 * @link http://www.php.net/manual/en/sqlite3result.finalize.php
	 * @return bool true.
	 */
	public function finalize () {}

	private function __construct () {}

}

/**
 * Specifies that the Sqlite3Result::fetchArray
 *      method shall return an array indexed by column name as returned in the
 *      corresponding result set.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_ASSOC', 1);

/**
 * Specifies that the Sqlite3Result::fetchArray
 *      method shall return an array indexed by column number as returned in the
 *      corresponding result set, starting at column 0.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_NUM', 2);

/**
 * Specifies that the Sqlite3Result::fetchArray
 *      method shall return an array indexed by both column name and number as
 *      returned in the corresponding result set, starting at column 0.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_BOTH', 3);

/**
 * Represents the SQLite3 INTEGER storage class.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_INTEGER', 1);

/**
 * Represents the SQLite3 REAL (FLOAT) storage class.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_FLOAT', 2);

/**
 * Represents the SQLite3 TEXT storage class.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_TEXT', 3);

/**
 * Represents the SQLite3 BLOB storage class.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_BLOB', 4);

/**
 * Represents the SQLite3 NULL storage class.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_NULL', 5);

/**
 * Specifies that the SQLite3 database be opened for reading only.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_OPEN_READONLY', 1);

/**
 * Specifies that the SQLite3 database be opened for reading and writing.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_OPEN_READWRITE', 2);

/**
 * Specifies that the SQLite3 database be created if it does not already
 *      exist.
 * @link http://www.php.net/manual/en/sqlite3.constants.php
 */
define ('SQLITE3_OPEN_CREATE', 4);

// End of sqlite3 v.0.7-dev
?>
