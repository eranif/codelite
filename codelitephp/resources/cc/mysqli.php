<?php

// Start of mysqli v.0.1

class mysqli_sql_exception extends RuntimeException  {
	protected $message;
	protected $file;
	protected $line;
	protected $code;
	protected $sqlstate;


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

final class mysqli_driver  {
	public $client_info;
	public $client_version;
	public $driver_version;
	public $embedded;
	public $reconnect;
	public $report_mode;

}

class mysqli  {
	public $affected_rows;
	public $client_info;
	public $client_version;
	public $connect_errno;
	public $connect_error;
	public $errno;
	public $error;
	public $error_list;
	public $field_count;
	public $host_info;
	public $info;
	public $insert_id;
	public $server_info;
	public $server_version;
	public $stat;
	public $sqlstate;
	public $protocol_version;
	public $thread_id;
	public $warning_count;


	/**
	 * Turns on or off auto-committing database modifications
	 * @link http://www.php.net/manual/en/mysqli.autocommit.php
	 * @param link mysqli 
	 * @param mode bool <p>
	 *       Whether to turn on auto-commit or not.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function autocommit (mysqli $link, $mode) {}

	/**
	 * Starts a transaction
	 * @link http://www.php.net/manual/en/mysqli.begin-transaction.php
	 * @param flags int[optional] <p>
	 *       
	 *      </p>
	 * @param name string[optional] <p>
	 *       
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function begin_transaction ($flags = null, $name = null) {}

	/**
	 * Changes the user of the specified database connection
	 * @link http://www.php.net/manual/en/mysqli.change-user.php
	 * @param link mysqli 
	 * @param user string <p>
	 *       The MySQL user name.
	 *      </p>
	 * @param password string <p>
	 *       The MySQL password.
	 *      </p>
	 * @param database string <p>
	 *       The database to change to.
	 *      </p>
	 *      <p>
	 *       If desired, the &null; value may be passed resulting in only changing
	 *       the user and not selecting a database. To select a database in this
	 *       case use the mysqli_select_db function.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function change_user (mysqli $link, $user, $password, $database) {}

	/**
	 * Returns the default character set for the database connection
	 * @link http://www.php.net/manual/en/mysqli.character-set-name.php
	 * @param link mysqli 
	 * @return string The default character set for the current connection
	 */
	public function character_set_name (mysqli $link) {}

	/**
	 * Closes a previously opened database connection
	 * @link http://www.php.net/manual/en/mysqli.close.php
	 * @param link mysqli 
	 * @return bool Returns true on success, false on failure.
	 */
	public function close (mysqli $link) {}

	/**
	 * Commits the current transaction
	 * @link http://www.php.net/manual/en/mysqli.commit.php
	 * @param link mysqli 
	 * @param flags int[optional] <p>
	 *       A bitmask of MYSQLI_TRANS_COR_* constants.
	 *      </p>
	 * @param name string[optional] <p>
	 *       If provided then COMMIT/*name*/ is executed.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function commit (mysqli $link, $flags = null, $name = null) {}

	/**
	 * @param host[optional]
	 * @param user[optional]
	 * @param password[optional]
	 * @param database[optional]
	 * @param port[optional]
	 * @param socket[optional]
	 */
	public function connect ($host, $user, $password, $database, $port, $socket) {}

	/**
	 * Dump debugging information into the log
	 * @link http://www.php.net/manual/en/mysqli.dump-debug-info.php
	 * @param link mysqli 
	 * @return bool Returns true on success, false on failure.
	 */
	public function dump_debug_info (mysqli $link) {}

	/**
	 * Performs debugging operations
	 * @link http://www.php.net/manual/en/mysqli.debug.php
	 * @param message string <p>
	 *       A string representing the debugging operation to perform
	 *      </p>
	 * @return bool true.
	 */
	public function debug ($message) {}

	/**
	 * Returns a character set object
	 * @link http://www.php.net/manual/en/mysqli.get-charset.php
	 * @param link mysqli 
	 * @return object The function returns a character set object with the following properties:
	 *   
	 *    
	 *     charset
	 *     <p>Character set name</p>
	 *    
	 *    
	 *     collation
	 *     <p>Collation name</p>
	 *    
	 *    
	 *     dir
	 *     <p>Directory the charset description was fetched from (?) or "" for built-in character sets</p>
	 *    
	 *    
	 *     min_length
	 *     <p>Minimum character length in bytes</p>
	 *    
	 *    
	 *     max_length
	 *     <p>Maximum character length in bytes</p>
	 *    
	 *    
	 *     number
	 *     <p>Internal character set number</p>
	 *    
	 *    
	 *     state
	 *     <p>Character set status (?)</p>
	 */
	public function get_charset (mysqli $link) {}

	/**
	 * Get MySQL client info
	 * @link http://www.php.net/manual/en/mysqli.get-client-info.php
	 * @param link mysqli 
	 * @return string A string that represents the MySQL client library version
	 */
	public function get_client_info (mysqli $link) {}

	/**
	 * Returns statistics about the client connection
	 * @link http://www.php.net/manual/en/mysqli.get-connection-stats.php
	 * @param link mysqli 
	 * @return array an array with connection stats if success, false otherwise.
	 */
	public function get_connection_stats (mysqli $link) {}

	public function get_server_info () {}

	/**
	 * Get result of SHOW WARNINGS
	 * @link http://www.php.net/manual/en/mysqli.get-warnings.php
	 * @param link mysqli 
	 * @return mysqli_warning 
	 */
	public function get_warnings (mysqli $link) {}

	/**
	 * Initializes MySQLi and returns a resource for use with mysqli_real_connect()
	 * @link http://www.php.net/manual/en/mysqli.init.php
	 * @return mysqli an object.
	 */
	public function init () {}

	/**
	 * Asks the server to kill a MySQL thread
	 * @link http://www.php.net/manual/en/mysqli.kill.php
	 * @param link mysqli 
	 * @param processid int 
	 * @return bool Returns true on success, false on failure.
	 */
	public function kill (mysqli $link, $processid) {}

	/**
	 * Performs a query on the database
	 * @link http://www.php.net/manual/en/mysqli.multi-query.php
	 * @param link mysqli 
	 * @param query string <p>
	 *       The query, as a string.
	 *      </p>
	 *      <p>
	 *       Data inside the query should be properly escaped.
	 *      </p>
	 * @return bool false if the first statement failed.
	 *   To retrieve subsequent errors from other statements you have to call
	 *   mysqli_next_result first.
	 */
	public function multi_query (mysqli $link, $query) {}

	/**
	 * @param host[optional]
	 * @param user[optional]
	 * @param password[optional]
	 * @param database[optional]
	 * @param port[optional]
	 * @param socket[optional]
	 */
	public function mysqli ($host, $user, $password, $database, $port, $socket) {}

	/**
	 * Check if there are any more query results from a multi query
	 * @link http://www.php.net/manual/en/mysqli.more-results.php
	 * @param link mysqli 
	 * @return bool true if one or more result sets are available from a previous call to
	 *   mysqli_multi_query, otherwise false.
	 */
	public function more_results (mysqli $link) {}

	/**
	 * Prepare next result from multi_query
	 * @link http://www.php.net/manual/en/mysqli.next-result.php
	 * @param link mysqli 
	 * @return bool Returns true on success, false on failure.
	 */
	public function next_result (mysqli $link) {}

	/**
	 * Set options
	 * @link http://www.php.net/manual/en/mysqli.options.php
	 * @param link mysqli 
	 * @param option int <p>
	 *       The option that you want to set. It can be one of the following values:
	 *       <table>
	 *        Valid options
	 *        
	 *         
	 *          <tr valign="top">
	 *           <td>Name</td>
	 *           <td>Description</td>
	 *          </tr>
	 *         
	 *         
	 *          <tr valign="top">
	 *           <td>MYSQLI_OPT_CONNECT_TIMEOUT</td>
	 *           <td>connection timeout in seconds (supported on Windows with TCP/IP since PHP 5.3.1)</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_OPT_LOCAL_INFILE</td>
	 *           <td>enable/disable use of LOAD LOCAL INFILE</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_INIT_COMMAND</td>
	 *           <td>command to execute after when connecting to MySQL server</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_READ_DEFAULT_FILE</td>
	 *           <td>
	 *            Read options from named option file instead of my.cnf
	 *           </td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_READ_DEFAULT_GROUP</td>
	 *           <td>
	 *            Read options from the named group from my.cnf
	 *            or the file specified with MYSQL_READ_DEFAULT_FILE.
	 *           </td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_SERVER_PUBLIC_KEY</td>
	 *           <td>
	 *             RSA public key file used with the SHA-256 based authentication.
	 *           </td>
	 *          </tr>
	 *         
	 *        
	 *       </table>
	 *      </p>
	 * @param value mixed <p>
	 *       The value for the option.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function options (mysqli $link, $option, $value) {}

	/**
	 * Pings a server connection, or tries to reconnect if the connection has gone down
	 * @link http://www.php.net/manual/en/mysqli.ping.php
	 * @param link mysqli 
	 * @return bool Returns true on success, false on failure.
	 */
	public function ping (mysqli $link) {}

	/**
	 * Poll connections
	 * @link http://www.php.net/manual/en/mysqli.poll.php
	 * @param read array <p>
	 *       List of connections to check for outstanding results that can be read.
	 *      </p>
	 * @param error array <p>
	 *      List of connections on which an error occurred, for example, query
	 *      failure or lost connection.
	 *      </p>
	 * @param reject array <p>
	 *      List of connections rejected because no asynchronous query
	 *      has been run on for which the function could poll results.
	 *      </p>
	 * @param sec int <p>
	 *       Number of seconds to wait, must be non-negative.
	 *      </p>
	 * @param usec int[optional] <p>
	 *       Number of microseconds to wait, must be non-negative.
	 *      </p>
	 * @return int number of ready connections upon success, false otherwise.
	 */
	public static function poll (array &$read, array &$error, array &$reject, $sec, $usec = null) {}

	/**
	 * Prepare an SQL statement for execution
	 * @link http://www.php.net/manual/en/mysqli.prepare.php
	 * @param link mysqli 
	 * @param query string <p>
	 *       The query, as a string.
	 *      </p>
	 *      
	 *       <p>
	 *        You should not add a terminating semicolon or \g
	 *        to the statement.
	 *       </p>
	 *      
	 *      <p>
	 *       This parameter can include one or more parameter markers in the SQL
	 *       statement by embedding question mark (?) characters
	 *       at the appropriate positions.
	 *      </p>
	 *      
	 *       <p>
	 *        The markers are legal only in certain places in SQL statements.
	 *        For example, they are allowed in the VALUES()
	 *        list of an INSERT statement (to specify column
	 *        values for a row), or in a comparison with a column in a
	 *        WHERE clause to specify a comparison value.
	 *       </p>
	 *       <p>
	 *        However, they are not allowed for identifiers (such as table or
	 *        column names), in the select list that names the columns to be
	 *        returned by a SELECT statement, or to specify both
	 *        operands of a binary operator such as the = equal
	 *        sign. The latter restriction is necessary because it would be
	 *        impossible to determine the parameter type. It's not allowed to
	 *        compare marker with NULL by 
	 *        ? IS NULL too. In general, parameters are legal
	 *        only in Data Manipulation Language (DML) statements, and not in Data
	 *        Definition Language (DDL) statements.
	 *       </p>
	 * @return mysqli_stmt mysqli_prepare returns a statement object or false if an error occurred.
	 */
	public function prepare (mysqli $link, $query) {}

	/**
	 * Performs a query on the database
	 * @link http://www.php.net/manual/en/mysqli.query.php
	 * @param link mysqli 
	 * @param query string <p>
	 *       The query string.
	 *      </p>
	 *      <p>
	 *       Data inside the query should be properly escaped.
	 *      </p>
	 * @param resultmode int[optional] <p>
	 *       Either the constant MYSQLI_USE_RESULT or
	 *       MYSQLI_STORE_RESULT depending on the desired
	 *       behavior. By default, MYSQLI_STORE_RESULT is used.
	 *      </p>
	 *      <p>
	 *       If you use MYSQLI_USE_RESULT all subsequent calls
	 *       will return error Commands out of sync unless you
	 *       call mysqli_free_result
	 *      </p>
	 *      <p>
	 *       With MYSQLI_ASYNC (available with mysqlnd), it is
	 *       possible to perform query asynchronously.
	 *       mysqli_poll is then used to get results from such
	 *       queries.
	 *      </p>
	 * @return mixed false on failure. For successful SELECT, SHOW, DESCRIBE or
	 *   EXPLAIN queries mysqli_query will return
	 *   a mysqli_result object. For other successful queries mysqli_query will
	 *   return true.
	 */
	public function query (mysqli $link, $query, $resultmode = null) {}

	/**
	 * Opens a connection to a mysql server
	 * @link http://www.php.net/manual/en/mysqli.real-connect.php
	 * @param link mysqli 
	 * @param host string[optional] <p>
	 *       Can be either a host name or an IP address. Passing the &null; value
	 *       or the string "localhost" to this parameter, the local host is
	 *       assumed. When possible, pipes will be used instead of the TCP/IP
	 *       protocol.
	 *      </p>
	 * @param username string[optional] <p>
	 *       The MySQL user name.
	 *      </p>
	 * @param passwd string[optional] <p>
	 *       If provided or &null;, the MySQL server will attempt to authenticate
	 *       the user against those user records which have no password only. This
	 *       allows one username to be used with different permissions (depending
	 *       on if a password as provided or not).
	 *      </p>
	 * @param dbname string[optional] <p>
	 *       If provided will specify the default database to be used when
	 *       performing queries.
	 *      </p>
	 * @param port int[optional] <p>
	 *       Specifies the port number to attempt to connect to the MySQL server.
	 *      </p>
	 * @param socket string[optional] <p>
	 *       Specifies the socket or named pipe that should be used.
	 *      </p>
	 *      
	 *       <p>
	 *        Specifying the socket parameter will not
	 *        explicitly determine the type of connection to be used when
	 *        connecting to the MySQL server. How the connection is made to the
	 *        MySQL database is determined by the host
	 *        parameter.
	 *       </p>
	 * @param flags int[optional] <p>
	 *       With the parameter flags you can set different
	 *       connection options:
	 *      </p>
	 *      <table>
	 *       Supported flags
	 *       
	 *       
	 *        <tr valign="top">
	 *         <td>Name</td>
	 *         <td>Description</td>
	 *        </tr>
	 *       
	 *       
	 *        <tr valign="top">
	 *         <td>MYSQLI_CLIENT_COMPRESS</td>
	 *         <td>Use compression protocol</td>
	 *        </tr>
	 *        <tr valign="top">
	 *         <td>MYSQLI_CLIENT_FOUND_ROWS</td>
	 *         <td>return number of matched rows, not the number of affected rows</td>
	 *        </tr>
	 *        <tr valign="top">
	 *         <td>MYSQLI_CLIENT_IGNORE_SPACE</td>
	 *         <td>Allow spaces after function names. Makes all function names reserved words.</td>
	 *        </tr>
	 *        <tr valign="top">
	 *         <td>MYSQLI_CLIENT_INTERACTIVE</td>
	 *         <td>
	 *          Allow interactive_timeout seconds (instead of
	 *          wait_timeout seconds) of inactivity before closing the connection
	 *         </td>
	 *        </tr>
	 *        <tr valign="top">
	 *         <td>MYSQLI_CLIENT_SSL</td>
	 *         <td>Use SSL (encryption)</td>
	 *        </tr>
	 *        
	 *       
	 *      </table>
	 *      
	 *       <p>
	 *        For security reasons the MULTI_STATEMENT flag is
	 *        not supported in PHP. If you want to execute multiple queries use the
	 *        mysqli_multi_query function.
	 *       </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function real_connect (mysqli $link, $host = null, $username = null, $passwd = null, $dbname = null, $port = null, $socket = null, $flags = null) {}

	/**
	 * Escapes special characters in a string for use in an SQL statement, taking into account the current charset of the connection
	 * @link http://www.php.net/manual/en/mysqli.real-escape-string.php
	 * @param escapestr string <p>
	 *       The string to be escaped.
	 *      </p>
	 *      <p>
	 *       Characters encoded are NUL (ASCII 0), \n, \r, \, ', ", and
	 *       Control-Z.
	 *      </p>
	 * @return string an escaped string.
	 */
	public function real_escape_string ($escapestr) {}

	/**
	 * Get result from async query
	 * @link http://www.php.net/manual/en/mysqli.reap-async-query.php
	 * @param link mysql 
	 * @return mysqli_result mysqli_result in success, false otherwise.
	 */
	public function reap_async_query ($link) {}

	/**
	 * @param string_to_escape
	 */
	public function escape_string ($string_to_escape) {}

	/**
	 * Execute an SQL query
	 * @link http://www.php.net/manual/en/mysqli.real-query.php
	 * @param link mysqli 
	 * @param query string <p>
	 *       The query, as a string.
	 *      </p>
	 *      <p>
	 *       Data inside the query should be properly escaped.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function real_query (mysqli $link, $query) {}

	/**
	 * Rolls back a transaction to the named savepoint
	 * @link http://www.php.net/manual/en/mysqli.release-savepoint.php
	 * @param name string <p>
	 *       
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function release_savepoint ($name) {}

	/**
	 * Rolls back current transaction
	 * @link http://www.php.net/manual/en/mysqli.rollback.php
	 * @param link mysqli 
	 * @param flags int[optional] <p>
	 *       A bitmask of MYSQLI_TRANS_COR_* constants.
	 *      </p>
	 * @param name string[optional] <p>
	 *       If provided then ROLLBACK/*name*/ is executed.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function rollback (mysqli $link, $flags = null, $name = null) {}

	/**
	 * Set a named transaction savepoint
	 * @link http://www.php.net/manual/en/mysqli.savepoint.php
	 * @param name string <p>
	 *       
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function savepoint ($name) {}

	/**
	 * Selects the default database for database queries
	 * @link http://www.php.net/manual/en/mysqli.select-db.php
	 * @param link mysqli 
	 * @param dbname string <p>
	 *       The database name.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function select_db (mysqli $link, $dbname) {}

	/**
	 * Sets the default client character set
	 * @link http://www.php.net/manual/en/mysqli.set-charset.php
	 * @param link mysqli 
	 * @param charset string <p>
	 *       The charset to be set as default.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function set_charset (mysqli $link, $charset) {}

	/**
	 * Alias of <function>mysqli_options</function>
	 * @link http://www.php.net/manual/en/function.mysqli-set-opt.php
	 * @param option
	 * @param value
	 */
	public function set_opt ($option, $value) {}

	/**
	 * Used for establishing secure connections using SSL
	 * @link http://www.php.net/manual/en/mysqli.ssl-set.php
	 * @param link mysqli 
	 * @param key string <p>
	 *       The path name to the key file.
	 *      </p>
	 * @param cert string <p>
	 *       The path name to the certificate file.
	 *      </p>
	 * @param ca string <p>
	 *       The path name to the certificate authority file.
	 *      </p>
	 * @param capath string <p>
	 *       The pathname to a directory that contains trusted SSL CA certificates
	 *       in PEM format.
	 *      </p>
	 * @param cipher string <p>
	 *       A list of allowable ciphers to use for SSL encryption.
	 *      </p>
	 * @return bool This function always returns true value. If SSL setup is
	 *   incorrect mysqli_real_connect will return an error
	 *   when you attempt to connect.
	 */
	public function ssl_set (mysqli $link, $key, $cert, $ca, $capath, $cipher) {}

	/**
	 * Gets the current system status
	 * @link http://www.php.net/manual/en/mysqli.stat.php
	 * @param link mysqli 
	 * @return string A string describing the server status. false if an error occurred.
	 */
	public function stat (mysqli $link) {}

	/**
	 * Initializes a statement and returns an object for use with mysqli_stmt_prepare
	 * @link http://www.php.net/manual/en/mysqli.stmt-init.php
	 * @param link mysqli 
	 * @return mysqli_stmt an object.
	 */
	public function stmt_init (mysqli $link) {}

	/**
	 * Transfers a result set from the last query
	 * @link http://www.php.net/manual/en/mysqli.store-result.php
	 * @param option int[optional] <p>
	 *       The option that you want to set. It can be one of the following values:
	 *       <table>
	 *        Valid options
	 *        
	 *         
	 *          <tr valign="top">
	 *           <td>Name</td>
	 *           <td>Description</td>
	 *          </tr>
	 *         
	 *         
	 *          <tr valign="top">
	 *           <td>MYSQLI_STORE_RESULT_COPY_DATA</td>
	 *           <td>Copy results from the internal mysqlnd buffer into the PHP variables fetched. By default,
	 *           mysqlnd will use a reference logic to avoid copying and duplicating results held in memory.
	 *           For certain result sets, for example, result sets with many small rows, the copy approach can
	 *           reduce the overall memory usage because PHP variables holding results may be
	 *           released earlier (available with mysqlnd only, since PHP 5.6.0)</td>
	 *          </tr>
	 *         
	 *        
	 *       </table>
	 *      </p>
	 * @return mysqli_result a buffered result object or false if an error occurred.
	 *  </p>
	 *  
	 *   <p>
	 *    mysqli_store_result returns false in case the query
	 *    didn't return a result set (if the query was, for example an INSERT
	 *    statement). This function also returns false if the reading of the
	 *    result set failed. You can check if you have got an error by checking
	 *    if mysqli_error doesn't return an empty string, if
	 *    mysqli_errno returns a non zero value, or if
	 *    mysqli_field_count returns a non zero value.
	 *    Also possible reason for this function returning false after
	 *    successful call to mysqli_query can be too large
	 *    result set (memory for it cannot be allocated). If
	 *    mysqli_field_count returns a non-zero value, the
	 *    statement should have produced a non-empty result set.
	 */
	public function store_result ($option = null) {}

	/**
	 * Returns whether thread safety is given or not
	 * @link http://www.php.net/manual/en/mysqli.thread-safe.php
	 * @return bool true if the client library is thread-safe, otherwise false.
	 */
	public function thread_safe () {}

	/**
	 * Initiate a result set retrieval
	 * @link http://www.php.net/manual/en/mysqli.use-result.php
	 * @param link mysqli 
	 * @return mysqli_result an unbuffered result object or false if an error occurred.
	 */
	public function use_result (mysqli $link) {}

	/**
	 * Refreshes
	 * @link http://www.php.net/manual/en/mysqli.refresh.php
	 * @param link resource 
	 * @param options int <p>
	 *       The options to refresh, using the MYSQLI_REFRESH_* constants as documented
	 *       within the MySQLi constants documentation.
	 *      </p>
	 *      <p>
	 *       See also the official MySQL Refresh
	 *       documentation.
	 *      </p>
	 * @return int true if the refresh was a success, otherwise false
	 */
	public function refresh ($link, $options) {}

}

final class mysqli_warning  {
	public $message;
	public $sqlstate;
	public $errno;


	/**
	 * The __construct purpose
	 * @link http://www.php.net/manual/en/mysqli-warning.construct.php
	 */
	protected function __construct () {}

	/**
	 * The next purpose
	 * @link http://www.php.net/manual/en/mysqli-warning.next.php
	 */
	public function next () {}

}

class mysqli_result implements Traversable {
	public $current_field;
	public $field_count;
	public $lengths;
	public $num_rows;
	public $type;


	public function __construct () {}

	public function close () {}

	/**
	 * Frees the memory associated with a result
	 * @link http://www.php.net/manual/en/mysqli-result.free.php
	 * @return void 
	 */
	public function free () {}

	/**
	 * Adjusts the result pointer to an arbitrary row in the result
	 * @link http://www.php.net/manual/en/mysqli-result.data-seek.php
	 * @param result mysqli_result 
	 * @param offset int <p>
	 *       The field offset. Must be between zero and the total number of rows
	 *       minus one (0..mysqli_num_rows - 1).
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function data_seek (mysqli_result $result, $offset) {}

	/**
	 * Returns the next field in the result set
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-field.php
	 * @param result mysqli_result 
	 * @return object an object which contains field definition information or false
	 *   if no field information is available.
	 *  </p>
	 *  <p>
	 *  <table>
	 *   Object properties
	 *   
	 *    
	 *     <tr valign="top">
	 *      <td>Property</td>
	 *      <td>Description</td>
	 *     </tr>
	 *    
	 *    
	 *     <tr valign="top">
	 *      <td>name</td>
	 *      <td>The name of the column</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgname</td>
	 *      <td>Original column name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>table</td>
	 *      <td>The name of the table this field belongs to (if not calculated)</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgtable</td>
	 *      <td>Original table name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>def</td>
	 *      <td>Reserved for default value, currently always ""</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>db</td>
	 *      <td>Database (since PHP 5.3.6)</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>catalog</td>
	 *      <td>The catalog name, always "def" (since PHP 5.3.6)</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>max_length</td>
	 *      <td>The maximum width of the field for the result set.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>length</td>
	 *      <td>The width of the field, as specified in the table definition.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>charsetnr</td>
	 *      <td>The character set number for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>flags</td>
	 *      <td>An integer representing the bit-flags for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>type</td>
	 *      <td>The data type used for this field</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>decimals</td>
	 *      <td>The number of decimals used (for integer fields)</td>
	 *     </tr>
	 *    
	 *   
	 *  </table>
	 */
	public function fetch_field (mysqli_result $result) {}

	/**
	 * Returns an array of objects representing the fields in a result set
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-fields.php
	 * @param result mysqli_result 
	 * @return array an array of objects which contains field definition information or
	 *   false if no field information is available.
	 *  </p>
	 *  <p>
	 *  <table>
	 *   Object properties
	 *   
	 *    
	 *     <tr valign="top">
	 *      <td>Property</td>
	 *      <td>Description</td>
	 *     </tr>
	 *    
	 *    
	 *     <tr valign="top">
	 *      <td>name</td>
	 *      <td>The name of the column</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgname</td>
	 *      <td>Original column name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>table</td>
	 *      <td>The name of the table this field belongs to (if not calculated)</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgtable</td>
	 *      <td>Original table name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>max_length</td>
	 *      <td>The maximum width of the field for the result set.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>length</td>
	 *      <td>The width of the field, as specified in the table definition.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>charsetnr</td>
	 *      <td>The character set number for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>flags</td>
	 *      <td>An integer representing the bit-flags for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>type</td>
	 *      <td>The data type used for this field</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>decimals</td>
	 *      <td>The number of decimals used (for integer fields)</td>
	 *     </tr>
	 *    
	 *   
	 *  </table>
	 */
	public function fetch_fields (mysqli_result $result) {}

	/**
	 * Fetch meta-data for a single field
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-field-direct.php
	 * @param result mysqli_result 
	 * @param fieldnr int <p>
	 *       The field number. This value must be in the range from 
	 *       0 to number of fields - 1.
	 *      </p>
	 * @return object an object which contains field definition information or false
	 *   if no field information for specified fieldnr is 
	 *   available.
	 *  </p>
	 *  <p>
	 *  <table>
	 *   Object attributes
	 *   
	 *    
	 *     <tr valign="top">
	 *      <td>Attribute</td>
	 *      <td>Description</td>
	 *     </tr>
	 *    
	 *    
	 *     <tr valign="top">
	 *      <td>name</td>
	 *      <td>The name of the column</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgname</td>
	 *      <td>Original column name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>table</td>
	 *      <td>The name of the table this field belongs to (if not calculated)</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>orgtable</td>
	 *      <td>Original table name if an alias was specified</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>def</td>
	 *      <td>The default value for this field, represented as a string</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>max_length</td>
	 *      <td>The maximum width of the field for the result set.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>length</td>
	 *      <td>The width of the field, as specified in the table definition.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>charsetnr</td>
	 *      <td>The character set number for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>flags</td>
	 *      <td>An integer representing the bit-flags for the field.</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>type</td>
	 *      <td>The data type used for this field</td>
	 *     </tr>
	 *     <tr valign="top">
	 *      <td>decimals</td>
	 *      <td>The number of decimals used (for integer fields)</td>
	 *     </tr>
	 *    
	 *   
	 *  </table>
	 */
	public function fetch_field_direct (mysqli_result $result, $fieldnr) {}

	/**
	 * Fetches all result rows as an associative array, a numeric array, or both
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-all.php
	 * @param result mysqli_result 
	 * @param resulttype int[optional] <p>
	 *       This optional parameter is a constant indicating what type of array
	 *       should be produced from the current row data. The possible values for
	 *       this parameter are the constants MYSQLI_ASSOC,
	 *       MYSQLI_NUM, or MYSQLI_BOTH.
	 *      </p>
	 * @return mixed an array of associative or numeric arrays holding result rows.
	 */
	public function fetch_all (mysqli_result $result, $resulttype = null) {}

	/**
	 * Fetch a result row as an associative, a numeric array, or both
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-array.php
	 * @param result mysqli_result 
	 * @param resulttype int[optional] <p>
	 *       This optional parameter is a constant indicating what type of array
	 *       should be produced from the current row data. The possible values for
	 *       this parameter are the constants MYSQLI_ASSOC,
	 *       MYSQLI_NUM, or MYSQLI_BOTH.
	 *      </p>
	 *      <p>
	 *       By using the MYSQLI_ASSOC constant this function
	 *       will behave identically to the mysqli_fetch_assoc,
	 *       while MYSQLI_NUM will behave identically to the
	 *       mysqli_fetch_row function. The final option 
	 *       MYSQLI_BOTH will create a single array with the
	 *       attributes of both.
	 *      </p>
	 * @return mixed an array of strings that corresponds to the fetched row or &null; if there
	 *   are no more rows in resultset.
	 */
	public function fetch_array (mysqli_result $result, $resulttype = null) {}

	/**
	 * Fetch a result row as an associative array
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-assoc.php
	 * @param result mysqli_result 
	 * @return array an associative array of strings representing the fetched row in the result
	 *   set, where each key in the array represents the name of one of the result
	 *   set's columns or &null; if there are no more rows in resultset.
	 *  </p>
	 *  <p>
	 *   If two or more columns of the result have the same field names, the last
	 *   column will take precedence. To access the other column(s) of the same
	 *   name, you either need to access the result with numeric indices by using
	 *   mysqli_fetch_row or add alias names.
	 */
	public function fetch_assoc (mysqli_result $result) {}

	/**
	 * Returns the current row of a result set as an object
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-object.php
	 * @param result mysqli_result 
	 * @param class_name string[optional] <p>
	 *       The name of the class to instantiate, set the properties of and return.
	 *       If not specified, a stdClass object is returned.
	 *      </p>
	 * @param params array[optional] <p>
	 *       An optional array of parameters to pass to the constructor
	 *       for class_name objects.
	 *      </p>
	 * @return object an object with string properties that corresponds to the fetched
	 *   row or &null; if there are no more rows in resultset.
	 */
	public function fetch_object (mysqli_result $result, $class_name = null, array $params = null) {}

	/**
	 * Get a result row as an enumerated array
	 * @link http://www.php.net/manual/en/mysqli-result.fetch-row.php
	 * @param result mysqli_result 
	 * @return mixed mysqli_fetch_row returns an array of strings that corresponds to the fetched row
	 *   or &null; if there are no more rows in result set.
	 */
	public function fetch_row (mysqli_result $result) {}

	/**
	 * Set result pointer to a specified field offset
	 * @link http://www.php.net/manual/en/mysqli-result.field-seek.php
	 * @param result mysqli_result 
	 * @param fieldnr int <p>
	 *       The field number. This value must be in the range from 
	 *       0 to number of fields - 1.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function field_seek (mysqli_result $result, $fieldnr) {}

	public function free_result () {}

}

class mysqli_stmt  {
	public $affected_rows;
	public $insert_id;
	public $num_rows;
	public $param_count;
	public $field_count;
	public $errno;
	public $error;
	public $error_list;
	public $sqlstate;
	public $id;


	public function __construct () {}

	/**
	 * Used to get the current value of a statement attribute
	 * @link http://www.php.net/manual/en/mysqli-stmt.attr-get.php
	 * @param stmt mysqli_stmt 
	 * @param attr int <p>
	 *       The attribute that you want to get.
	 *      </p>
	 * @return int false if the attribute is not found, otherwise returns the value of the attribute.
	 */
	public function attr_get (mysqli_stmt $stmt, $attr) {}

	/**
	 * Used to modify the behavior of a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.attr-set.php
	 * @param stmt mysqli_stmt 
	 * @param attr int <p>
	 *       The attribute that you want to set. It can have one of the following values:
	 *       <table>
	 *        Attribute values
	 *        
	 *         
	 *          <tr valign="top">
	 *           <td>Character</td>
	 *           <td>Description</td>
	 *          </tr>
	 *         
	 *         
	 *          <tr valign="top">
	 *           <td>MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH</td>
	 *           <td>
	 *            Setting to true causes mysqli_stmt_store_result to
	 *            update the metadata MYSQL_FIELD->max_length value.
	 *           </td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_STMT_ATTR_CURSOR_TYPE</td>
	 *           <td>
	 *            Type of cursor to open for statement when mysqli_stmt_execute
	 *            is invoked. mode can be MYSQLI_CURSOR_TYPE_NO_CURSOR
	 *            (the default) or MYSQLI_CURSOR_TYPE_READ_ONLY.
	 *           </td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>MYSQLI_STMT_ATTR_PREFETCH_ROWS</td>
	 *           <td>
	 *            Number of rows to fetch from server at a time when using a cursor.
	 *            mode can be in the range from 1 to the maximum
	 *            value of unsigned long. The default is 1.
	 *           </td>
	 *          </tr>
	 *         
	 *        
	 *       </table>
	 *      </p>
	 *      <p>
	 *       If you use the MYSQLI_STMT_ATTR_CURSOR_TYPE option with
	 *       MYSQLI_CURSOR_TYPE_READ_ONLY, a cursor is opened for the
	 *       statement when you invoke mysqli_stmt_execute. If there
	 *       is already an open cursor from a previous mysqli_stmt_execute call,
	 *       it closes the cursor before opening a new one. mysqli_stmt_reset
	 *       also closes any open cursor before preparing the statement for re-execution.
	 *       mysqli_stmt_free_result closes any open cursor.
	 *      </p>
	 *      <p>
	 *       If you open a cursor for a prepared statement, mysqli_stmt_store_result
	 *       is unnecessary.
	 *      </p>
	 * @param mode int <p>The value to assign to the attribute.</p>
	 * @return bool 
	 */
	public function attr_set (mysqli_stmt $stmt, $attr, $mode) {}

	/**
	 * Binds variables to a prepared statement as parameters
	 * @link http://www.php.net/manual/en/mysqli-stmt.bind-param.php
	 * @param stmt mysqli_stmt 
	 * @param types string <p>
	 *       A string that contains one or more characters which specify the types
	 *       for the corresponding bind variables:
	 *       <table>
	 *        Type specification chars
	 *        
	 *         
	 *          <tr valign="top">
	 *           <td>Character</td>
	 *           <td>Description</td>
	 *          </tr>
	 *         
	 *         
	 *          <tr valign="top">
	 *           <td>i</td>
	 *           <td>corresponding variable has type integer</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>d</td>
	 *           <td>corresponding variable has type double</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>s</td>
	 *           <td>corresponding variable has type string</td>
	 *          </tr>
	 *          <tr valign="top">
	 *           <td>b</td>
	 *           <td>corresponding variable is a blob and will be sent in packets</td>
	 *          </tr>
	 *         
	 *        
	 *       </table>
	 *      </p>
	 * @param var1 mixed <p>
	 *       The number of variables and length of string 
	 *       types must match the parameters in the statement.
	 *      </p>
	 * @param _ mixed[optional] 
	 * @return bool Returns true on success, false on failure.
	 */
	public function bind_param (mysqli_stmt $stmt, $types, &$var1, &$_ = null) {}

	/**
	 * Binds variables to a prepared statement for result storage
	 * @link http://www.php.net/manual/en/mysqli-stmt.bind-result.php
	 * @param stmt mysqli_stmt 
	 * @param var1 mixed <p>
	 *       The variable to be bound.
	 *      </p>
	 * @param _ mixed[optional] 
	 * @return bool Returns true on success, false on failure.
	 */
	public function bind_result (mysqli_stmt $stmt, &$var1, &$_ = null) {}

	/**
	 * Closes a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.close.php
	 * @param stmt mysqli_stmt 
	 * @return bool Returns true on success, false on failure.
	 */
	public function close (mysqli_stmt $stmt) {}

	/**
	 * Seeks to an arbitrary row in statement result set
	 * @link http://www.php.net/manual/en/mysqli-stmt.data-seek.php
	 * @param stmt mysqli_stmt 
	 * @param offset int <p>
	 *       Must be between zero and the total number of rows minus one (0..
	 *       mysqli_stmt_num_rows - 1).
	 *      </p>
	 * @return void 
	 */
	public function data_seek (mysqli_stmt $stmt, $offset) {}

	/**
	 * Executes a prepared Query
	 * @link http://www.php.net/manual/en/mysqli-stmt.execute.php
	 * @param stmt mysqli_stmt 
	 * @return bool Returns true on success, false on failure.
	 */
	public function execute (mysqli_stmt $stmt) {}

	/**
	 * Fetch results from a prepared statement into the bound variables
	 * @link http://www.php.net/manual/en/mysqli-stmt.fetch.php
	 * @param stmt mysqli_stmt 
	 * @return bool 
	 */
	public function fetch (mysqli_stmt $stmt) {}

	/**
	 * Get result of SHOW WARNINGS
	 * @link http://www.php.net/manual/en/mysqli-stmt.get-warnings.php
	 * @param stmt mysqli_stmt 
	 * @return object 
	 */
	public function get_warnings (mysqli_stmt $stmt) {}

	/**
	 * Returns result set metadata from a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.result-metadata.php
	 * @param stmt mysqli_stmt 
	 * @return mysqli_result a result object or false if an error occurred.
	 */
	public function result_metadata (mysqli_stmt $stmt) {}

	/**
	 * Check if there are more query results from a multiple query
	 * @link http://www.php.net/manual/en/mysqli-stmt.more-results.php
	 * @return bool true if more results exist, otherwise false.
	 */
	public function more_results () {}

	/**
	 * Reads the next result from a multiple query
	 * @link http://www.php.net/manual/en/mysqli-stmt.next-result.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function next_result () {}

	public function num_rows () {}

	/**
	 * Send data in blocks
	 * @link http://www.php.net/manual/en/mysqli-stmt.send-long-data.php
	 * @param stmt mysqli_stmt 
	 * @param param_nr int <p>
	 *       Indicates which parameter to associate the data with. Parameters are
	 *       numbered beginning with 0.
	 *      </p>
	 * @param data string <p>
	 *       A string containing data to be sent.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function send_long_data (mysqli_stmt $stmt, $param_nr, $data) {}

	/**
	 * Frees stored result memory for the given statement handle
	 * @link http://www.php.net/manual/en/mysqli-stmt.free-result.php
	 * @param stmt mysqli_stmt 
	 * @return void 
	 */
	public function free_result (mysqli_stmt $stmt) {}

	/**
	 * Resets a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.reset.php
	 * @param stmt mysqli_stmt 
	 * @return bool Returns true on success, false on failure.
	 */
	public function reset (mysqli_stmt $stmt) {}

	/**
	 * Prepare an SQL statement for execution
	 * @link http://www.php.net/manual/en/mysqli-stmt.prepare.php
	 * @param stmt mysqli_stmt 
	 * @param query string <p>
	 *       The query, as a string. It must consist of a single SQL statement.
	 *      </p>
	 *      <p>
	 *       You can include one or more parameter markers in the SQL statement by
	 *       embedding question mark (?) characters at the
	 *       appropriate positions.
	 *      </p>
	 *      
	 *       <p>
	 *        You should not add a terminating semicolon or \g
	 *        to the statement.
	 *       </p>
	 *      
	 *      
	 *       <p>
	 *        The markers are legal only in certain places in SQL statements.
	 *        For example, they are allowed in the VALUES() list of an INSERT statement
	 *        (to specify column values for a row), or in a comparison with a column in
	 *        a WHERE clause to specify a comparison value.
	 *       </p>
	 *       <p>
	 *        However, they are not allowed for identifiers (such as table or column names),
	 *        in the select list that names the columns to be returned by a SELECT statement),
	 *        or to specify both operands of a binary operator such as the =
	 *        equal sign. The latter restriction is necessary because it would be impossible
	 *        to determine the parameter type. In general, parameters are legal only in Data
	 *        Manipulation Language (DML) statements, and not in Data Definition Language
	 *        (DDL) statements.
	 *       </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function prepare (mysqli_stmt $stmt, $query) {}

	/**
	 * Transfers a result set from a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.store-result.php
	 * @param stmt mysqli_stmt 
	 * @return bool Returns true on success, false on failure.
	 */
	public function store_result (mysqli_stmt $stmt) {}

	/**
	 * Gets a result set from a prepared statement
	 * @link http://www.php.net/manual/en/mysqli-stmt.get-result.php
	 * @param stmt mysqli_stmt 
	 * @return mysqli_result a resultset &return.falseforfailure;.
	 */
	public function get_result (mysqli_stmt $stmt) {}

}

/**
 * @param link
 */
function mysqli_affected_rows ($link) {}

/**
 * @param link
 * @param mode
 */
function mysqli_autocommit ($link, $mode) {}

/**
 * @param link
 * @param flags[optional]
 * @param name[optional]
 */
function mysqli_begin_transaction ($link, $flags, $name) {}

/**
 * @param link
 * @param user
 * @param password
 * @param database
 */
function mysqli_change_user ($link, $user, $password, $database) {}

/**
 * @param link
 */
function mysqli_character_set_name ($link) {}

/**
 * @param link
 */
function mysqli_close ($link) {}

/**
 * @param link
 * @param flags[optional]
 * @param name[optional]
 */
function mysqli_commit ($link, $flags, $name) {}

/**
 * &Alias; <methodname>mysqli::__construct</methodname>
 * @link http://www.php.net/manual/en/function.mysqli-connect.php
 * @param host[optional]
 * @param user[optional]
 * @param password[optional]
 * @param database[optional]
 * @param port[optional]
 * @param socket[optional]
 */
function mysqli_connect ($host, $user, $password, $database, $port, $socket) {}

function mysqli_connect_errno () {}

function mysqli_connect_error () {}

/**
 * @param result
 * @param offset
 */
function mysqli_data_seek ($result, $offset) {}

/**
 * @param link
 */
function mysqli_dump_debug_info ($link) {}

/**
 * @param debug_options
 */
function mysqli_debug ($debug_options) {}

/**
 * @param link
 */
function mysqli_errno ($link) {}

/**
 * @param link
 */
function mysqli_error ($link) {}

/**
 * @param link
 */
function mysqli_error_list ($link) {}

/**
 * @param stmt
 */
function mysqli_stmt_execute ($stmt) {}

/**
 * Alias for <function>mysqli_stmt_execute</function>
 * @link http://www.php.net/manual/en/function.mysqli-execute.php
 * @param stmt
 */
function mysqli_execute ($stmt) {}

/**
 * @param result
 */
function mysqli_fetch_field ($result) {}

/**
 * @param result
 */
function mysqli_fetch_fields ($result) {}

/**
 * @param result
 * @param field_nr
 */
function mysqli_fetch_field_direct ($result, $field_nr) {}

/**
 * @param result
 */
function mysqli_fetch_lengths ($result) {}

/**
 * @param result
 */
function mysqli_fetch_all ($result) {}

/**
 * @param result
 * @param result_type[optional]
 */
function mysqli_fetch_array ($result, $result_type) {}

/**
 * @param result
 */
function mysqli_fetch_assoc ($result) {}

/**
 * @param result
 * @param class_name[optional]
 * @param params[optional]
 */
function mysqli_fetch_object ($result, $class_namearray , $params) {}

/**
 * @param result
 */
function mysqli_fetch_row ($result) {}

/**
 * @param link
 */
function mysqli_field_count ($link) {}

/**
 * @param result
 * @param field_nr
 */
function mysqli_field_seek ($result, $field_nr) {}

/**
 * @param result
 */
function mysqli_field_tell ($result) {}

/**
 * @param result
 */
function mysqli_free_result ($result) {}

/**
 * @param link
 */
function mysqli_get_connection_stats ($link) {}

/**
 * Returns client per-process statistics
 * @link http://www.php.net/manual/en/mysqli.get-client-stats.php
 * @return array an array with client stats if success, false otherwise.
 */
function mysqli_get_client_stats () {}

/**
 * @param link
 */
function mysqli_get_charset ($link) {}

/**
 * @param link
 */
function mysqli_get_client_info ($link) {}

/**
 * Returns the MySQL client version as an integer
 * @link http://www.php.net/manual/en/mysqli.get-client-version.php
 * @param link mysqli 
 * @return int A number that represents the MySQL client library version in format:
 *   main_version*10000 + minor_version *100 + sub_version.
 *   For example, 4.1.0 is returned as 40100.
 *  </p>
 *  <p>
 *   This is useful to quickly determine the version of the client library
 *   to know if some capability exits.
 */
function mysqli_get_client_version (mysqli $link) {}

/**
 * Return information about open and cached links
 * @link http://www.php.net/manual/en/function.mysqli-get-links-stats.php
 * @return array mysqli_get_links_stats returns an associative array
 *   with three elements, keyed as follows:
 *   
 *    
 *     total
 *     
 *      <p>
 *       An integer indicating the total number of open links in
 *       any state.
 *      </p>
 *     
 *    
 *    
 *     active_plinks
 *     
 *      <p>
 *       An integer representing the number of active persistent
 *       connections.
 *      </p>
 *     
 *    
 *    
 *     cached_plinks
 *     
 *      <p>
 *       An integer representing the number of inactive persistent
 *       connections.
 *      </p>
 */
function mysqli_get_links_stats () {}

/**
 * @param link
 */
function mysqli_get_host_info ($link) {}

/**
 * @param link
 */
function mysqli_get_proto_info ($link) {}

/**
 * @param link
 */
function mysqli_get_server_info ($link) {}

/**
 * @param link
 */
function mysqli_get_server_version ($link) {}

/**
 * @param link
 */
function mysqli_get_warnings ($link) {}

function mysqli_init () {}

/**
 * @param link
 */
function mysqli_info ($link) {}

/**
 * @param link
 */
function mysqli_insert_id ($link) {}

/**
 * @param link
 * @param connection_id
 */
function mysqli_kill ($link, $connection_id) {}

/**
 * @param link
 */
function mysqli_more_results ($link) {}

/**
 * @param link
 * @param query
 */
function mysqli_multi_query ($link, $query) {}

/**
 * @param link
 */
function mysqli_next_result ($link) {}

/**
 * @param result
 */
function mysqli_num_fields ($result) {}

/**
 * @param result
 */
function mysqli_num_rows ($result) {}

/**
 * @param link
 * @param option
 * @param value
 */
function mysqli_options ($link, $option, $value) {}

/**
 * @param link
 */
function mysqli_ping ($link) {}

/**
 * @param read
 * @param write
 * @param error
 * @param sec
 * @param usec[optional]
 */
function mysqli_poll (array &$read = nullarray , &$write = nullarray , &$error = null, $sec, $usec) {}

/**
 * @param link
 * @param query
 */
function mysqli_prepare ($link, $query) {}

/**
 * &Alias; of <link linkend="mysqli-driver.report-mode">mysqli_driver->report_mode</link>
 * @link http://www.php.net/manual/en/function.mysqli-report.php
 * @param flags
 */
function mysqli_report ($flags) {}

/**
 * @param link
 * @param query
 */
function mysqli_query ($link, $query) {}

/**
 * @param link
 * @param host[optional]
 * @param user[optional]
 * @param password[optional]
 * @param database[optional]
 * @param port[optional]
 * @param socket[optional]
 * @param flags[optional]
 */
function mysqli_real_connect ($link, $host, $user, $password, $database, $port, $socket, $flags) {}

/**
 * @param link
 * @param string_to_escape
 */
function mysqli_real_escape_string ($link, $string_to_escape) {}

/**
 * @param link
 * @param query
 */
function mysqli_real_query ($link, $query) {}

/**
 * @param link
 */
function mysqli_reap_async_query ($link) {}

/**
 * @param link
 * @param name
 */
function mysqli_release_savepoint ($link, $name) {}

/**
 * @param link
 * @param flags[optional]
 * @param name[optional]
 */
function mysqli_rollback ($link, $flags, $name) {}

/**
 * @param link
 * @param name
 */
function mysqli_savepoint ($link, $name) {}

/**
 * @param link
 * @param database
 */
function mysqli_select_db ($link, $database) {}

/**
 * @param link
 * @param charset
 */
function mysqli_set_charset ($link, $charset) {}

/**
 * @param stmt
 */
function mysqli_stmt_affected_rows ($stmt) {}

/**
 * @param stmt
 * @param attribute
 */
function mysqli_stmt_attr_get ($stmt, $attribute) {}

/**
 * @param stmt
 * @param attribute
 * @param value
 */
function mysqli_stmt_attr_set ($stmt, $attribute, $value) {}

/**
 * @param stmt
 * @param types
 * @param vars
 */
function mysqli_stmt_bind_param ($stmt, $types, &$vars) {}

/**
 * @param stmt
 * @param vars
 */
function mysqli_stmt_bind_result ($stmt, &$vars) {}

/**
 * @param stmt
 */
function mysqli_stmt_close ($stmt) {}

/**
 * @param stmt
 * @param offset
 */
function mysqli_stmt_data_seek ($stmt, $offset) {}

/**
 * @param stmt
 */
function mysqli_stmt_errno ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_error ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_error_list ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_fetch ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_field_count ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_free_result ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_get_result ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_get_warnings ($stmt) {}

/**
 * @param link
 */
function mysqli_stmt_init ($link) {}

/**
 * @param stmt
 */
function mysqli_stmt_insert_id ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_more_results ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_next_result ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_num_rows ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_param_count ($stmt) {}

/**
 * @param stmt
 * @param query
 */
function mysqli_stmt_prepare ($stmt, $query) {}

/**
 * @param stmt
 */
function mysqli_stmt_reset ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_result_metadata ($stmt) {}

/**
 * @param stmt
 * @param param_nr
 * @param data
 */
function mysqli_stmt_send_long_data ($stmt, $param_nr, $data) {}

/**
 * @param stmt
 */
function mysqli_stmt_store_result ($stmt) {}

/**
 * @param stmt
 */
function mysqli_stmt_sqlstate ($stmt) {}

/**
 * @param link
 */
function mysqli_sqlstate ($link) {}

/**
 * @param link
 * @param key
 * @param cert
 * @param certificate_authority
 * @param certificate_authority_path
 * @param cipher
 */
function mysqli_ssl_set ($link, $key, $cert, $certificate_authority, $certificate_authority_path, $cipher) {}

/**
 * @param link
 */
function mysqli_stat ($link) {}

/**
 * @param link
 * @param flags[optional]
 */
function mysqli_store_result ($link, $flags) {}

/**
 * @param link
 */
function mysqli_thread_id ($link) {}

function mysqli_thread_safe () {}

/**
 * @param link
 */
function mysqli_use_result ($link) {}

/**
 * @param link
 */
function mysqli_warning_count ($link) {}

/**
 * @param link
 * @param options
 */
function mysqli_refresh ($link, $options) {}

/**
 * &Alias; <function>mysqli_real_escape_string</function>
 * @link http://www.php.net/manual/en/function.mysqli-escape-string.php
 * @param link
 * @param query
 */
function mysqli_escape_string ($link, $query) {}

function mysqli_set_opt () {}


/**
 * <p>
 *      Read options from the named group from my.cnf
 *      or the file specified with MYSQLI_READ_DEFAULT_FILE
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_READ_DEFAULT_GROUP', 5);

/**
 * <p>
 *      Read options from the named option file instead of from my.cnf
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_READ_DEFAULT_FILE', 4);

/**
 * <p>
 *      Connect timeout in seconds
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_OPT_CONNECT_TIMEOUT', 0);

/**
 * <p>
 *      Enables command LOAD LOCAL INFILE
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_OPT_LOCAL_INFILE', 8);

/**
 * <p>
 *      Command to execute when connecting to MySQL server. Will automatically be re-executed when reconnecting.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_INIT_COMMAND', 3);
define ('MYSQLI_OPT_NET_CMD_BUFFER_SIZE', 202);
define ('MYSQLI_OPT_NET_READ_BUFFER_SIZE', 203);
define ('MYSQLI_OPT_INT_AND_FLOAT_NATIVE', 201);
define ('MYSQLI_OPT_SSL_VERIFY_SERVER_CERT', 21);
define ('MYSQLI_SERVER_PUBLIC_KEY', 27);

/**
 * <p>
 *      Use SSL (encrypted protocol). This option should not be set by application programs; 
 *      it is set internally in the MySQL client library
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CLIENT_SSL', 2048);

/**
 * <p>
 *      Use compression protocol
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CLIENT_COMPRESS', 32);

/**
 * <p>
 *      Allow interactive_timeout seconds
 *      (instead of wait_timeout seconds) of inactivity before
 *      closing the connection.  The client's session
 *      wait_timeout variable will be set to
 *      the value of the session interactive_timeout variable. 
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CLIENT_INTERACTIVE', 1024);

/**
 * <p>
 *      Allow spaces after function names. Makes all functions names reserved words. 
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CLIENT_IGNORE_SPACE', 256);

/**
 * <p>
 *      Don't allow the db_name.tbl_name.col_name syntax.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CLIENT_NO_SCHEMA', 16);
define ('MYSQLI_CLIENT_FOUND_ROWS', 2);
define ('MYSQLI_CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS', 4194304);
define ('MYSQLI_OPT_CAN_HANDLE_EXPIRED_PASSWORDS', 29);

/**
 * <p>
 *      For using buffered resultsets
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_STORE_RESULT', 0);

/**
 * <p>
 *      For using unbuffered resultsets
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_USE_RESULT', 1);
define ('MYSQLI_ASYNC', 8);
define ('MYSQLI_STORE_RESULT_COPY_DATA', 16);

/**
 * <p>
 *      Columns are returned into the array having the fieldname as the array index.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_ASSOC', 1);

/**
 * <p>
 *      Columns are returned into the array having an enumerated index.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_NUM', 2);

/**
 * <p>
 *      Columns are returned into the array having both a numerical index and the fieldname as the associative index. 
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_BOTH', 3);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH', 0);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_STMT_ATTR_CURSOR_TYPE', 1);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CURSOR_TYPE_NO_CURSOR', 0);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CURSOR_TYPE_READ_ONLY', 1);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CURSOR_TYPE_FOR_UPDATE', 2);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_CURSOR_TYPE_SCROLLABLE', 4);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_STMT_ATTR_PREFETCH_ROWS', 2);

/**
 * <p>
 *      Indicates that a field is defined as NOT NULL
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_NOT_NULL_FLAG', 1);

/**
 * <p>
 *      Field is part of a primary index
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_PRI_KEY_FLAG', 2);

/**
 * <p>
 *      Field is part of a unique index.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_UNIQUE_KEY_FLAG', 4);

/**
 * <p>
 *      Field is part of an index.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_MULTIPLE_KEY_FLAG', 8);

/**
 * <p>
 *      Field is defined as BLOB
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_BLOB_FLAG', 16);

/**
 * <p>
 *      Field is defined as UNSIGNED
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_UNSIGNED_FLAG', 32);

/**
 * <p>
 *      Field is defined as ZEROFILL
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_ZEROFILL_FLAG', 64);

/**
 * <p>
 *      Field is defined as AUTO_INCREMENT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_AUTO_INCREMENT_FLAG', 512);

/**
 * <p>
 *      Field is defined as TIMESTAMP
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TIMESTAMP_FLAG', 1024);

/**
 * <p>
 *      Field is defined as SET
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_SET_FLAG', 2048);

/**
 * <p>
 *      Field is defined as NUMERIC
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_NUM_FLAG', 32768);

/**
 * <p>
 *      Field is part of an multi-index
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_PART_KEY_FLAG', 16384);

/**
 * <p>
 *      Field is part of GROUP BY
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_GROUP_FLAG', 32768);

/**
 * <p>
 *     Field is defined as ENUM. Available since PHP 5.3.0.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_ENUM_FLAG', 256);

/**
 * <p>
 *      Field is defined as BINARY. Available since PHP 5.3.0.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_BINARY_FLAG', 128);
define ('MYSQLI_NO_DEFAULT_VALUE_FLAG', 4096);
define ('MYSQLI_ON_UPDATE_NOW_FLAG', 8192);

/**
 * <p>
 *      Field is defined as DECIMAL
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_DECIMAL', 0);

/**
 * <p>
 *      Field is defined as TINYINT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_TINY', 1);

/**
 * <p>
 *      Field is defined as SMALLINT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_SHORT', 2);

/**
 * <p>
 *      Field is defined as INT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_LONG', 3);

/**
 * <p>
 *      Field is defined as FLOAT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_FLOAT', 4);

/**
 * <p>
 *      Field is defined as DOUBLE
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_DOUBLE', 5);

/**
 * <p>
 *      Field is defined as DEFAULT NULL
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_NULL', 6);

/**
 * <p>
 *      Field is defined as TIMESTAMP
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_TIMESTAMP', 7);

/**
 * <p>
 *      Field is defined as BIGINT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_LONGLONG', 8);

/**
 * <p>
 *      Field is defined as MEDIUMINT
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_INT24', 9);

/**
 * <p>
 *      Field is defined as DATE
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_DATE', 10);

/**
 * <p>
 *      Field is defined as TIME
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_TIME', 11);

/**
 * <p>
 *      Field is defined as DATETIME
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_DATETIME', 12);

/**
 * <p>
 *      Field is defined as YEAR
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_YEAR', 13);

/**
 * <p>
 *      Field is defined as DATE
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_NEWDATE', 14);

/**
 * <p>
 *      Field is defined as ENUM
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_ENUM', 247);

/**
 * <p>
 *      Field is defined as SET
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_SET', 248);

/**
 * <p>
 *      Field is defined as TINYBLOB
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_TINY_BLOB', 249);

/**
 * <p>
 *      Field is defined as MEDIUMBLOB
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_MEDIUM_BLOB', 250);

/**
 * <p>
 *      Field is defined as LONGBLOB
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_LONG_BLOB', 251);

/**
 * <p>
 *      Field is defined as BLOB
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_BLOB', 252);

/**
 * <p>
 *      Field is defined as VARCHAR
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_VAR_STRING', 253);

/**
 * <p>
 *      Field is defined as CHAR or BINARY
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_STRING', 254);

/**
 * <p>
 *      Field is defined as TINYINT.
 *      For CHAR, see MYSQLI_TYPE_STRING
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_CHAR', 1);

/**
 * <p>
 *      Field is defined as INTERVAL
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_INTERVAL', 247);

/**
 * <p>
 *      Field is defined as GEOMETRY
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_GEOMETRY', 255);

/**
 * <p>
 *      Precision math DECIMAL or NUMERIC field (MySQL 5.0.3 and up)
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_NEWDECIMAL', 246);

/**
 * <p>
 *      Field is defined as BIT (MySQL 5.0.3 and up)
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TYPE_BIT', 16);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_SET_CHARSET_NAME', 7);
define ('MYSQLI_SET_CHARSET_DIR', 6);

/**
 * <p>
 *      No more data available for bind variable
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_NO_DATA', 100);

/**
 * <p>
 *      Data truncation occurred. Available since PHP 5.1.0 and MySQL 5.0.5.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_DATA_TRUNCATED', 101);

/**
 * <p>
 *      Report if no index or bad index was used in a query.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REPORT_INDEX', 4);

/**
 * <p>
 *      Report errors from mysqli function calls.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REPORT_ERROR', 1);

/**
 * <p>
 *      Throw a mysqli_sql_exception for errors instead of warnings.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REPORT_STRICT', 2);

/**
 * <p>
 *      Set all options on (report all).
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REPORT_ALL', 255);

/**
 * <p>
 *      Turns reporting off.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REPORT_OFF', 0);

/**
 * <p>
 *      Is set to 1 if mysqli_debug functionality is enabled.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_DEBUG_TRACE_ENABLED', 0);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED', 16);

/**
 * <p>
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_SERVER_QUERY_NO_INDEX_USED', 32);
define ('MYSQLI_SERVER_QUERY_WAS_SLOW', 2048);
define ('MYSQLI_SERVER_PS_OUT_PARAMS', 4096);

/**
 * <p>
 *      Refreshes the grant tables.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_GRANT', 1);

/**
 * <p>
 *      Flushes the logs, like executing the
 *      FLUSH LOGS SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_LOG', 2);

/**
 * <p>
 *      Flushes the table cache, like executing the
 *      FLUSH TABLES SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_TABLES', 4);

/**
 * <p>
 *      Flushes the host cache, like executing the
 *      FLUSH HOSTS SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_HOSTS', 8);

/**
 * <p>
 *      Reset the status variables, like executing the
 *      FLUSH STATUS SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_STATUS', 16);

/**
 * <p>
 *      Flushes the thread cache.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_THREADS', 32);

/**
 * <p>
 *      On a slave replication server: resets the master server information, and 
 *      restarts the slave. Like executing the RESET SLAVE
 *      SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_SLAVE', 64);

/**
 * <p>
 *      On a master replication server: removes the binary log files listed in the
 *      binary log index, and truncates the index file. Like executing the
 *      RESET MASTER SQL statement.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_REFRESH_MASTER', 128);
define ('MYSQLI_REFRESH_BACKUP_LOG', 2097152);
define ('MYSQLI_TRANS_START_WITH_CONSISTENT_SNAPSHOT', 1);
define ('MYSQLI_TRANS_START_READ_WRITE', 2);
define ('MYSQLI_TRANS_START_READ_ONLY', 4);

/**
 * <p>
 *      Appends "AND CHAIN" to mysqli_commit or
 *      mysqli_rollback.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TRANS_COR_AND_CHAIN', 1);

/**
 * <p>
 *      Appends "AND NO CHAIN" to mysqli_commit or
 *      mysqli_rollback.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TRANS_COR_AND_NO_CHAIN', 2);

/**
 * <p>
 *      Appends "RELEASE" to mysqli_commit or
 *      mysqli_rollback.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TRANS_COR_RELEASE', 4);

/**
 * <p>
 *      Appends "NO RELEASE" to mysqli_commit or
 *      mysqli_rollback.
 *     </p>
 * @link http://www.php.net/manual/en/mysqli.constants.php
 */
define ('MYSQLI_TRANS_COR_NO_RELEASE', 8);

// End of mysqli v.0.1
?>
