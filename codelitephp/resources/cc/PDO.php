<?php

// Start of PDO v.1.0.4dev

class PDOException extends RuntimeException  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;
	public $errorInfo;


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

class PDO  {
	const PARAM_BOOL = 5;
	const PARAM_NULL = 0;
	const PARAM_INT = 1;
	const PARAM_STR = 2;
	const PARAM_LOB = 3;
	const PARAM_STMT = 4;
	const PARAM_INPUT_OUTPUT = -2147483648;
	const PARAM_EVT_ALLOC = 0;
	const PARAM_EVT_FREE = 1;
	const PARAM_EVT_EXEC_PRE = 2;
	const PARAM_EVT_EXEC_POST = 3;
	const PARAM_EVT_FETCH_PRE = 4;
	const PARAM_EVT_FETCH_POST = 5;
	const PARAM_EVT_NORMALIZE = 6;
	const FETCH_LAZY = 1;
	const FETCH_ASSOC = 2;
	const FETCH_NUM = 3;
	const FETCH_BOTH = 4;
	const FETCH_OBJ = 5;
	const FETCH_BOUND = 6;
	const FETCH_COLUMN = 7;
	const FETCH_CLASS = 8;
	const FETCH_INTO = 9;
	const FETCH_FUNC = 10;
	const FETCH_GROUP = 65536;
	const FETCH_UNIQUE = 196608;
	const FETCH_KEY_PAIR = 12;
	const FETCH_CLASSTYPE = 262144;
	const FETCH_SERIALIZE = 524288;
	const FETCH_PROPS_LATE = 1048576;
	const FETCH_NAMED = 11;
	const ATTR_AUTOCOMMIT = 0;
	const ATTR_PREFETCH = 1;
	const ATTR_TIMEOUT = 2;
	const ATTR_ERRMODE = 3;
	const ATTR_SERVER_VERSION = 4;
	const ATTR_CLIENT_VERSION = 5;
	const ATTR_SERVER_INFO = 6;
	const ATTR_CONNECTION_STATUS = 7;
	const ATTR_CASE = 8;
	const ATTR_CURSOR_NAME = 9;
	const ATTR_CURSOR = 10;
	const ATTR_ORACLE_NULLS = 11;
	const ATTR_PERSISTENT = 12;
	const ATTR_STATEMENT_CLASS = 13;
	const ATTR_FETCH_TABLE_NAMES = 14;
	const ATTR_FETCH_CATALOG_NAMES = 15;
	const ATTR_DRIVER_NAME = 16;
	const ATTR_STRINGIFY_FETCHES = 17;
	const ATTR_MAX_COLUMN_LEN = 18;
	const ATTR_EMULATE_PREPARES = 20;
	const ATTR_DEFAULT_FETCH_MODE = 19;
	const ERRMODE_SILENT = 0;
	const ERRMODE_WARNING = 1;
	const ERRMODE_EXCEPTION = 2;
	const CASE_NATURAL = 0;
	const CASE_LOWER = 2;
	const CASE_UPPER = 1;
	const NULL_NATURAL = 0;
	const NULL_EMPTY_STRING = 1;
	const NULL_TO_STRING = 2;
	const ERR_NONE = 00000;
	const FETCH_ORI_NEXT = 0;
	const FETCH_ORI_PRIOR = 1;
	const FETCH_ORI_FIRST = 2;
	const FETCH_ORI_LAST = 3;
	const FETCH_ORI_ABS = 4;
	const FETCH_ORI_REL = 5;
	const CURSOR_FWDONLY = 0;
	const CURSOR_SCROLL = 1;
	const MYSQL_ATTR_USE_BUFFERED_QUERY = 1000;
	const MYSQL_ATTR_LOCAL_INFILE = 1001;
	const MYSQL_ATTR_INIT_COMMAND = 1002;
	const MYSQL_ATTR_MAX_BUFFER_SIZE = 1005;
	const MYSQL_ATTR_READ_DEFAULT_FILE = 1003;
	const MYSQL_ATTR_READ_DEFAULT_GROUP = 1004;
	const MYSQL_ATTR_COMPRESS = 1006;
	const MYSQL_ATTR_DIRECT_QUERY = 1007;
	const MYSQL_ATTR_FOUND_ROWS = 1008;
	const MYSQL_ATTR_IGNORE_SPACE = 1009;
	const MYSQL_ATTR_SSL_KEY = 1010;
	const MYSQL_ATTR_SSL_CERT = 1011;
	const MYSQL_ATTR_SSL_CA = 1012;
	const MYSQL_ATTR_SSL_CAPATH = 1013;
	const MYSQL_ATTR_SSL_CIPHER = 1014;
	const PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT = 1000;
	const PGSQL_TRANSACTION_IDLE = 0;
	const PGSQL_TRANSACTION_ACTIVE = 1;
	const PGSQL_TRANSACTION_INTRANS = 2;
	const PGSQL_TRANSACTION_INERROR = 3;
	const PGSQL_TRANSACTION_UNKNOWN = 4;


	/**
	 * Creates a PDO instance representing a connection to a database
	 * @link http://www.php.net/manual/en/pdo.construct.php
	 * @param dsn
	 * @param username
	 * @param passwd
	 * @param options[optional]
	 */
	public function __construct ($dsn, $username, $passwd, $options) {}

	/**
	 * Prepares a statement for execution and returns a statement object
	 * @link http://www.php.net/manual/en/pdo.prepare.php
	 * @param statement string <p>
	 *       This must be a valid SQL statement for the target database server.
	 *      </p>
	 * @param driver_options array[optional] <p>
	 *       This array holds one or more key=>value pairs to set
	 *       attribute values for the PDOStatement object that this method
	 *       returns. You would most commonly use this to set the
	 *       PDO::ATTR_CURSOR value to
	 *       PDO::CURSOR_SCROLL to request a scrollable cursor.
	 *       Some drivers have driver specific options that may be set at
	 *       prepare-time.
	 *      </p>
	 * @return PDOStatement If the database server successfully prepares the statement,
	 *   PDO::prepare returns a
	 *   PDOStatement object.
	 *   If the database server cannot successfully prepare the statement,
	 *   PDO::prepare returns false or emits
	 *   PDOException (depending on error handling).
	 *  </p>
	 *  
	 *   <p>
	 *    Emulated prepared statements does not communicate with the database server
	 *    so PDO::prepare does not check the statement.
	 */
	public function prepare ($statement, array $driver_options = null) {}

	/**
	 * Initiates a transaction
	 * @link http://www.php.net/manual/en/pdo.begintransaction.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function beginTransaction () {}

	/**
	 * Commits a transaction
	 * @link http://www.php.net/manual/en/pdo.commit.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function commit () {}

	/**
	 * Rolls back a transaction
	 * @link http://www.php.net/manual/en/pdo.rollback.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function rollBack () {}

	/**
	 * Checks if inside a transaction
	 * @link http://www.php.net/manual/en/pdo.intransaction.php
	 * @return bool true if a transaction is currently active, and false if not.
	 */
	public function inTransaction () {}

	/**
	 * Set an attribute
	 * @link http://www.php.net/manual/en/pdo.setattribute.php
	 * @param attribute int 
	 * @param value mixed 
	 * @return bool Returns true on success or false on failure.
	 */
	public function setAttribute ($attribute, $value) {}

	/**
	 * Execute an SQL statement and return the number of affected rows
	 * @link http://www.php.net/manual/en/pdo.exec.php
	 * @param statement string <p>
	 *        The SQL statement to prepare and execute.
	 *       </p>
	 *       <p>
	 *        Data inside the query should be properly escaped.
	 *       </p>
	 * @return int PDO::exec returns the number of rows that were modified
	 *   or deleted by the SQL statement you issued. If no rows were affected, 
	 *   PDO::exec returns 0. 
	 *  </p>
	 *  &return.falseproblem;
	 *  <p>
	 *   The following example incorrectly relies on the return value of
	 *   PDO::exec, wherein a statement that affected 0 rows
	 *   results in a call to die:
	 *   
	 * exec() or die(print_r($db->errorInfo(), true));
	 * ?>
	 * ]]>
	 */
	public function exec ($statement) {}

	/**
	 * Executes an SQL statement, returning a result set as a PDOStatement object
	 * @link http://www.php.net/manual/en/pdo.query.php
	 * @param statement string <p>
	 *        The SQL statement to prepare and execute.
	 *       </p>
	 *       <p>
	 *        Data inside the query should be properly escaped.
	 *       </p>
	 * @return PDOStatement PDO::query returns a PDOStatement object, or false
	 *    on failure.
	 */
	public function query ($statement) {}

	/**
	 * Returns the ID of the last inserted row or sequence value
	 * @link http://www.php.net/manual/en/pdo.lastinsertid.php
	 * @param name string[optional] <p>
	 *       Name of the sequence object from which the ID should be returned.
	 *      </p>
	 * @return string If a sequence name was not specified for the name
	 *   parameter, PDO::lastInsertId returns a
	 *   string representing the row ID of the last row that was inserted into
	 *   the database.
	 *  </p>
	 *  <p>
	 *   If a sequence name was specified for the name
	 *   parameter, PDO::lastInsertId returns a
	 *   string representing the last value retrieved from the specified sequence
	 *   object.
	 *  </p>
	 *  <p>
	 *   If the PDO driver does not support this capability,
	 *   PDO::lastInsertId triggers an
	 *   IM001 SQLSTATE.
	 */
	public function lastInsertId ($name = null) {}

	/**
	 * Fetch the SQLSTATE associated with the last operation on the database handle
	 * @link http://www.php.net/manual/en/pdo.errorcode.php
	 * @return mixed an SQLSTATE, a five characters alphanumeric identifier defined in
	 *   the ANSI SQL-92 standard.  Briefly, an SQLSTATE consists of a
	 *   two characters class value followed by a three characters subclass value. A
	 *   class value of 01 indicates a warning and is accompanied by a return code
	 *   of SQL_SUCCESS_WITH_INFO. Class values other than '01', except for the
	 *   class 'IM', indicate an error.  The class 'IM' is specific to warnings
	 *   and errors that derive from the implementation of PDO (or perhaps ODBC,
	 *   if you're using the ODBC driver) itself.  The subclass value '000' in any
	 *   class indicates that there is no subclass for that SQLSTATE.
	 *  </p>
	 *  <p>
	 *   PDO::errorCode only retrieves error codes for operations
	 *   performed directly on the database handle. If you create a PDOStatement
	 *   object through PDO::prepare or
	 *   PDO::query and invoke an error on the statement
	 *   handle, PDO::errorCode will not reflect that error.
	 *   You must call PDOStatement::errorCode to return the error
	 *   code for an operation performed on a particular statement handle.
	 *  </p>
	 *  <p>
	 *   Returns &null; if no operation has been run on the database handle.
	 */
	public function errorCode () {}

	/**
	 * Fetch extended error information associated with the last operation on the database handle
	 * @link http://www.php.net/manual/en/pdo.errorinfo.php
	 * @return array PDO::errorInfo returns an array of error information
	 *   about the last operation performed by this database handle. The array
	 *   consists of the following fields:
	 *   
	 *    
	 *     
	 *      <tr valign="top">
	 *       <td>Element</td>
	 *       <td>Information</td>
	 *      </tr>
	 *     
	 *     
	 *      <tr valign="top">
	 *       <td>0</td>
	 *       <td>SQLSTATE error code (a five characters alphanumeric identifier defined
	 *               in the ANSI SQL standard).</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>1</td>
	 *       <td>Driver-specific error code.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>2</td>
	 *       <td>Driver-specific error message.</td>
	 *      </tr>
	 *     
	 *    
	 *   
	 *  </p>
	 *  
	 *   <p>
	 *    If the SQLSTATE error code is not set or there is no driver-specific
	 *    error, the elements following element 0 will be set to &null;.
	 *    </p>
	 *  
	 *  <p>
	 *   PDO::errorInfo only retrieves error information for
	 *   operations performed directly on the database handle. If you create a
	 *   PDOStatement object through PDO::prepare or
	 *   PDO::query and invoke an error on the statement
	 *   handle, PDO::errorInfo will not reflect the error
	 *   from the statement handle. You must call
	 *   PDOStatement::errorInfo to return the error
	 *   information for an operation performed on a particular statement handle.
	 */
	public function errorInfo () {}

	/**
	 * Retrieve a database connection attribute
	 * @link http://www.php.net/manual/en/pdo.getattribute.php
	 * @param attribute int <p>
	 *       One of the PDO::ATTR_* constants. The constants that
	 *       apply to database connections are as follows:
	 *        
	 *        PDO::ATTR_AUTOCOMMIT
	 *        PDO::ATTR_CASE
	 *        PDO::ATTR_CLIENT_VERSION
	 *        PDO::ATTR_CONNECTION_STATUS
	 *        PDO::ATTR_DRIVER_NAME
	 *        PDO::ATTR_ERRMODE
	 *        PDO::ATTR_ORACLE_NULLS
	 *        PDO::ATTR_PERSISTENT
	 *        PDO::ATTR_PREFETCH
	 *        PDO::ATTR_SERVER_INFO
	 *        PDO::ATTR_SERVER_VERSION
	 *        PDO::ATTR_TIMEOUT
	 *        
	 *      </p>
	 * @return mixed A successful call returns the value of the requested PDO attribute.
	 *   An unsuccessful call returns null.
	 */
	public function getAttribute ($attribute) {}

	/**
	 * Quotes a string for use in a query.
	 * @link http://www.php.net/manual/en/pdo.quote.php
	 * @param string string <p>
	 *        The string to be quoted.
	 *       </p>
	 * @param parameter_type int[optional] <p>
	 *        Provides a data type hint for drivers that have alternate quoting styles.
	 *       </p>
	 * @return string a quoted string that is theoretically safe to pass into an
	 *   SQL statement.  Returns false if the driver does not support quoting in
	 *   this way.
	 */
	public function quote ($string, $parameter_type = null) {}

	final public function __wakeup () {}

	final public function __sleep () {}

	/**
	 * Return an array of available PDO drivers
	 * @link http://www.php.net/manual/en/pdo.getavailabledrivers.php
	 * @return array PDO::getAvailableDrivers returns an array of PDO driver names. If
	 *   no drivers are available, it returns an empty array.
	 */
	public static function getAvailableDrivers () {}

}

class PDOStatement implements Traversable {
	public $queryString;


	/**
	 * Executes a prepared statement
	 * @link http://www.php.net/manual/en/pdostatement.execute.php
	 * @param input_parameters array[optional] <p>
	 *       An array of values with as many elements as there are bound
	 *       parameters in the SQL statement being executed.
	 *       All values are treated as PDO::PARAM_STR.
	 *      </p>
	 *      <p>
	 *       You cannot bind multiple values to a single parameter; for example,
	 *       you cannot bind two values to a single named parameter in an IN()
	 *       clause.
	 *      </p>
	 *      <p>
	 *       You cannot bind more values than specified; if more keys exist in 
	 *       input_parameters than in the SQL specified 
	 *       in the PDO::prepare, then the statement will 
	 *       fail and an error is emitted.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function execute (array $input_parameters = null) {}

	/**
	 * Fetches the next row from a result set
	 * @link http://www.php.net/manual/en/pdostatement.fetch.php
	 * @param fetch_style int[optional] <p>
	 *       Controls how the next row will be returned to the caller. This value
	 *       must be one of the PDO::FETCH_* constants,
	 *       defaulting to value of PDO::ATTR_DEFAULT_FETCH_MODE
	 *       (which defaults to PDO::FETCH_BOTH).
	 *       
	 *        <p>
	 *         PDO::FETCH_ASSOC: returns an array indexed by column
	 *         name as returned in your result set
	 *        </p>
	 * @param cursor_orientation int[optional] <p>
	 *       For a PDOStatement object representing a scrollable cursor, this
	 *       value determines which row will be returned to the caller. This value
	 *       must be one of the PDO::FETCH_ORI_* constants,
	 *       defaulting to PDO::FETCH_ORI_NEXT. To request a
	 *       scrollable cursor for your PDOStatement object, you must set the
	 *       PDO::ATTR_CURSOR attribute to
	 *       PDO::CURSOR_SCROLL when you prepare the SQL
	 *       statement with PDO::prepare.
	 *      </p>
	 * @param cursor_offset int[optional] 
	 * @return mixed The return value of this function on success depends on the fetch type. In
	 *   all cases, false is returned on failure.
	 */
	public function fetch ($fetch_style = null, $cursor_orientation = null, $cursor_offset = null) {}

	/**
	 * Binds a parameter to the specified variable name
	 * @link http://www.php.net/manual/en/pdostatement.bindparam.php
	 * @param parameter mixed <p>
	 *        Parameter identifier. For a prepared statement using named
	 *        placeholders, this will be a parameter name of the form
	 *        :name. For a prepared statement using
	 *        question mark placeholders, this will be the 1-indexed position of
	 *        the parameter. 
	 *       </p>
	 * @param variable mixed <p>
	 *        Name of the PHP variable to bind to the SQL statement parameter.
	 *       </p>
	 * @param data_type int[optional] <p>
	 *        Explicit data type for the parameter using the PDO::PARAM_*
	 *        constants.
	 *        To return an INOUT parameter from a stored procedure, 
	 *        use the bitwise OR operator to set the PDO::PARAM_INPUT_OUTPUT bits
	 *        for the data_type parameter.
	 *       </p>
	 * @param length int[optional] <p>
	 *        Length of the data type. To indicate that a parameter is an OUT
	 *        parameter from a stored procedure, you must explicitly set the
	 *        length.
	 *       </p>
	 * @param driver_options mixed[optional] <p>
	 *         
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function bindParam ($parameter, &$variable, $data_type = null, $length = null, $driver_options = null) {}

	/**
	 * Bind a column to a PHP variable
	 * @link http://www.php.net/manual/en/pdostatement.bindcolumn.php
	 * @param column mixed <p>
	 *       Number of the column (1-indexed) or name of the column in the result set.
	 *       If using the column name, be aware that the name should match the
	 *       case of the column, as returned by the driver.
	 *      </p>
	 * @param param mixed <p>
	 *       Name of the PHP variable to which the column will be bound.
	 *      </p>
	 * @param type int[optional] <p>
	 *       Data type of the parameter, specified by the PDO::PARAM_* constants.
	 *      </p>
	 * @param maxlen int[optional] <p>
	 *       A hint for pre-allocation.
	 *      </p>
	 * @param driverdata mixed[optional] <p>
	 *       Optional parameter(s) for the driver.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function bindColumn ($column, &$param, $type = null, $maxlen = null, $driverdata = null) {}

	/**
	 * Binds a value to a parameter
	 * @link http://www.php.net/manual/en/pdostatement.bindvalue.php
	 * @param parameter mixed <p>
	 *        Parameter identifier. For a prepared statement using named
	 *        placeholders, this will be a parameter name of the form
	 *        :name. For a prepared statement using
	 *        question mark placeholders, this will be the 1-indexed position of
	 *        the parameter. 
	 *       </p>
	 * @param value mixed <p>
	 *        The value to bind to the parameter.
	 *       </p>
	 * @param data_type int[optional] <p>
	 *        Explicit data type for the parameter using the PDO::PARAM_*
	 *        constants.
	 *       </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function bindValue ($parameter, $value, $data_type = null) {}

	/**
	 * Returns the number of rows affected by the last SQL statement
	 * @link http://www.php.net/manual/en/pdostatement.rowcount.php
	 * @return int the number of rows.
	 */
	public function rowCount () {}

	/**
	 * Returns a single column from the next row of a result set
	 * @link http://www.php.net/manual/en/pdostatement.fetchcolumn.php
	 * @param column_number int[optional] <p>
	 *       0-indexed number of the column you wish to retrieve from the row. If
	 *       no value is supplied, PDOStatement::fetchColumn
	 *       fetches the first column.
	 *      </p>
	 * @return string PDOStatement::fetchColumn returns a single column
	 *   in the next row of a result set.
	 *  </p>
	 *  
	 *   <p>
	 *    There is no way to return another column from the same row if you
	 *    use PDOStatement::fetchColumn to retrieve data.
	 */
	public function fetchColumn ($column_number = null) {}

	/**
	 * Returns an array containing all of the result set rows
	 * @link http://www.php.net/manual/en/pdostatement.fetchall.php
	 * @param fetch_style int[optional] <p>
	 *       Controls the contents of the returned array as documented in
	 *       PDOStatement::fetch.
	 *       Defaults to value of PDO::ATTR_DEFAULT_FETCH_MODE
	 *       (which defaults to PDO::FETCH_BOTH)
	 *      </p>
	 *      <p>
	 *       To return an array consisting of all values of a single column from
	 *       the result set, specify PDO::FETCH_COLUMN. You
	 *       can specify which column you want with the
	 *       column-index parameter.
	 *      </p>
	 *      <p>
	 *       To fetch only the unique values of a single column from the result set,
	 *       bitwise-OR PDO::FETCH_COLUMN with
	 *       PDO::FETCH_UNIQUE.
	 *      </p>
	 *      <p>
	 *       To return an associative array grouped by the values of a specified
	 *       column, bitwise-OR PDO::FETCH_COLUMN with
	 *       PDO::FETCH_GROUP.
	 *      </p>
	 * @param fetch_argument mixed[optional] <p>
	 *       This argument have a different meaning depending on the value of 
	 *       the fetch_style parameter:
	 *       
	 *        
	 *         <p>
	 *          PDO::FETCH_COLUMN: Returns the indicated 0-indexed 
	 *           column.
	 *         </p>
	 * @param ctor_args array[optional] <p>
	 *       Arguments of custom class constructor when the fetch_style 
	 *       parameter is PDO::FETCH_CLASS.
	 *      </p>
	 * @return array PDOStatement::fetchAll returns an array containing
	 *   all of the remaining rows in the result set. The array represents each
	 *   row as either an array of column values or an object with properties
	 *   corresponding to each column name.
	 *  </p>
	 *  <p>
	 *   Using this method to fetch large result sets will result in a heavy
	 *   demand on system and possibly network resources. Rather than retrieving
	 *   all of the data and manipulating it in PHP, consider using the database
	 *   server to manipulate the result sets. For example, use the WHERE and
	 *   ORDER BY clauses in SQL to restrict results before retrieving and
	 *   processing them with PHP.
	 */
	public function fetchAll ($fetch_style = null, $fetch_argument = null, array $ctor_args = null) {}

	/**
	 * Fetches the next row and returns it as an object.
	 * @link http://www.php.net/manual/en/pdostatement.fetchobject.php
	 * @param class_name string[optional] <p>
	 *       Name of the created class.
	 *      </p>
	 * @param ctor_args array[optional] <p>
	 *       Elements of this array are passed to the constructor.
	 *      </p>
	 * @return mixed an instance of the required class with property names that
	 *   correspond to the column names &return.falseforfailure;.
	 */
	public function fetchObject ($class_name = null, array $ctor_args = null) {}

	/**
	 * Fetch the SQLSTATE associated with the last operation on the statement handle
	 * @link http://www.php.net/manual/en/pdostatement.errorcode.php
	 * @return string Identical to PDO::errorCode, except that 
	 *   PDOStatement::errorCode only retrieves error codes
	 *   for operations performed with PDOStatement objects.
	 */
	public function errorCode () {}

	/**
	 * Fetch extended error information associated with the last operation on the statement handle
	 * @link http://www.php.net/manual/en/pdostatement.errorinfo.php
	 * @return array PDOStatement::errorInfo returns an array of
	 *   error information about the last operation performed by this
	 *   statement handle. The array consists of the following fields:
	 *   
	 *    
	 *     
	 *      <tr valign="top">
	 *       <td>Element</td>
	 *       <td>Information</td>
	 *      </tr>
	 *     
	 *     
	 *      <tr valign="top">
	 *       <td>0</td>
	 *       <td>SQLSTATE error code (a five characters alphanumeric identifier defined
	 *               in the ANSI SQL standard).</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>1</td>
	 *       <td>Driver specific error code.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>2</td>
	 *       <td>Driver specific error message.</td>
	 *      </tr>
	 */
	public function errorInfo () {}

	/**
	 * Set a statement attribute
	 * @link http://www.php.net/manual/en/pdostatement.setattribute.php
	 * @param attribute int 
	 * @param value mixed 
	 * @return bool Returns true on success or false on failure.
	 */
	public function setAttribute ($attribute, $value) {}

	/**
	 * Retrieve a statement attribute
	 * @link http://www.php.net/manual/en/pdostatement.getattribute.php
	 * @param attribute int 
	 * @return mixed the attribute value.
	 */
	public function getAttribute ($attribute) {}

	/**
	 * Returns the number of columns in the result set
	 * @link http://www.php.net/manual/en/pdostatement.columncount.php
	 * @return int the number of columns in the result set represented by the
	 *   PDOStatement object. If there is no result set,
	 *   PDOStatement::columnCount returns 0.
	 */
	public function columnCount () {}

	/**
	 * Returns metadata for a column in a result set
	 * @link http://www.php.net/manual/en/pdostatement.getcolumnmeta.php
	 * @param column int <p>
	 *        The 0-indexed column in the result set.
	 *       </p>
	 * @return array an associative array containing the following values representing
	 *   the metadata for a single column:
	 *  </p>
	 *   <table>
	 *    Column metadata
	 *    
	 *      
	 *      
	 *     
	 *      <tr valign="top">
	 *       <td>Name</td>
	 *       <td>Value</td>
	 *      </tr>
	 *     
	 *     
	 *      <tr valign="top">
	 *       <td>native_type</td>
	 *       <td>The PHP native type used to represent the column value.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>driver:decl_type</td>
	 *       <td>The SQL type used to represent the column value in the database.
	 *       If the column in the result set is the result of a function, this value
	 *       is not returned by PDOStatement::getColumnMeta.
	 *       </td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>flags</td>
	 *       <td>Any flags set for this column.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>name</td>
	 *       <td>The name of this column as returned by the database.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>table</td>
	 *       <td>The name of this column's table as returned by the database.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>len</td>
	 *       <td>The length of this column. Normally -1 for
	 *       types other than floating point decimals.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>precision</td>
	 *       <td>The numeric precision of this column. Normally
	 *       0 for types other than floating point
	 *       decimals.</td>
	 *      </tr>
	 *      <tr valign="top">
	 *       <td>pdo_type</td>
	 *       <td>The type of this column as represented by the
	 *       PDO::PARAM_* constants.</td>
	 *      </tr>
	 *     
	 *    
	 *   </table>
	 *  <p>
	 *   Returns false if the requested column does not exist in the result set,
	 *   or if no result set exists.
	 */
	public function getColumnMeta ($column) {}

	/**
	 * Set the default fetch mode for this statement
	 * @link http://www.php.net/manual/en/pdostatement.setfetchmode.php
	 * @param mode int <p>
	 *       The fetch mode must be one of the PDO::FETCH_* constants.
	 *      </p>
	 * @return bool 1 on success&return.falseforfailure;.
	 */
	public function setFetchMode ($mode) {}

	/**
	 * Advances to the next rowset in a multi-rowset statement handle
	 * @link http://www.php.net/manual/en/pdostatement.nextrowset.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function nextRowset () {}

	/**
	 * Closes the cursor, enabling the statement to be executed again.
	 * @link http://www.php.net/manual/en/pdostatement.closecursor.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function closeCursor () {}

	/**
	 * Dump an SQL prepared command
	 * @link http://www.php.net/manual/en/pdostatement.debugdumpparams.php
	 * @return bool 
	 */
	public function debugDumpParams () {}

	final public function __wakeup () {}

	final public function __sleep () {}

}

final class PDORow  {
}

function pdo_drivers () {}

// End of PDO v.1.0.4dev
?>
