<?php

// Start of sqlite3 v.0.7-dev

class SQLite3  {

	/**
	 * @param filename
	 * @param flags
	 * @param encryption_key
	 */
	public function open ($filename, $flags, $encryption_key) {}

	public function close () {}

	/**
	 * @param query
	 */
	public function exec ($query) {}

	public static function version () {}

	public function lastInsertRowID () {}

	public function lastErrorCode () {}

	public function lastErrorMsg () {}

	/**
	 * @param ms
	 */
	public function busyTimeout ($ms) {}

	/**
	 * @param shared_library
	 */
	public function loadExtension ($shared_library) {}

	public function changes () {}

	/**
	 * @param value
	 */
	public static function escapeString ($value) {}

	/**
	 * @param query
	 */
	public function prepare ($query) {}

	/**
	 * @param query
	 */
	public function query ($query) {}

	/**
	 * @param query
	 * @param entire_row[optional]
	 */
	public function querySingle ($query, $entire_row) {}

	/**
	 * @param name
	 * @param callback
	 * @param argument_count[optional]
	 */
	public function createFunction ($name, $callback, $argument_count) {}

	/**
	 * @param name
	 * @param step_callback
	 * @param final_callback
	 * @param argument_count[optional]
	 */
	public function createAggregate ($name, $step_callback, $final_callback, $argument_count) {}

	/**
	 * @param name
	 * @param callback
	 */
	public function createCollation ($name, $callback) {}

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
	 * @param filename
	 * @param flags
	 * @param encryption_key
	 */
	public function __construct ($filename, $flags, $encryption_key) {}

}

class SQLite3Stmt  {

	public function paramCount () {}

	public function close () {}

	public function reset () {}

	public function clear () {}

	public function execute () {}

	/**
	 * @param param_number
	 * @param param
	 * @param type[optional]
	 */
	public function bindParam ($param_number, &$param, $type) {}

	/**
	 * @param param_number
	 * @param param
	 * @param type[optional]
	 */
	public function bindValue ($param_number, $param, $type) {}

	public function readOnly () {}

	/**
	 * @param sqlite3
	 */
	private function __construct ($sqlite3) {}

}

class SQLite3Result  {

	public function numColumns () {}

	/**
	 * @param column_number
	 */
	public function columnName ($column_number) {}

	/**
	 * @param column_number
	 */
	public function columnType ($column_number) {}

	/**
	 * @param mode
	 */
	public function fetchArray ($mode) {}

	public function reset () {}

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
