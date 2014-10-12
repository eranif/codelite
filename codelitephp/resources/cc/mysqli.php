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
	 * @param mode
	 */
	public function autocommit ($mode) {}

	/**
	 * @param flags[optional]
	 * @param name[optional]
	 */
	public function begin_transaction ($flags, $name) {}

	/**
	 * @param user
	 * @param password
	 * @param database
	 */
	public function change_user ($user, $password, $database) {}

	public function character_set_name () {}

	public function close () {}

	/**
	 * @param flags[optional]
	 * @param name[optional]
	 */
	public function commit ($flags, $name) {}

	/**
	 * @param host[optional]
	 * @param user[optional]
	 * @param password[optional]
	 * @param database[optional]
	 * @param port[optional]
	 * @param socket[optional]
	 */
	public function connect ($host, $user, $password, $database, $port, $socket) {}

	public function dump_debug_info () {}

	/**
	 * @param debug_options
	 */
	public function debug ($debug_options) {}

	public function get_charset () {}

	public function get_client_info () {}

	public function get_connection_stats () {}

	public function get_server_info () {}

	public function get_warnings () {}

	public function init () {}

	/**
	 * @param connection_id
	 */
	public function kill ($connection_id) {}

	/**
	 * @param query
	 */
	public function multi_query ($query) {}

	/**
	 * @param host[optional]
	 * @param user[optional]
	 * @param password[optional]
	 * @param database[optional]
	 * @param port[optional]
	 * @param socket[optional]
	 */
	public function mysqli ($host, $user, $password, $database, $port, $socket) {}

	public function more_results () {}

	public function next_result () {}

	/**
	 * @param option
	 * @param value
	 */
	public function options ($option, $value) {}

	public function ping () {}

	/**
	 * @param read
	 * @param write
	 * @param error
	 * @param sec
	 * @param usec[optional]
	 */
	public static function poll (array &$read = nullarray , &$write = nullarray , &$error = null, $sec, $usec) {}

	/**
	 * @param query
	 */
	public function prepare ($query) {}

	/**
	 * @param query
	 */
	public function query ($query) {}

	/**
	 * @param host[optional]
	 * @param user[optional]
	 * @param password[optional]
	 * @param database[optional]
	 * @param port[optional]
	 * @param socket[optional]
	 * @param flags[optional]
	 */
	public function real_connect ($host, $user, $password, $database, $port, $socket, $flags) {}

	/**
	 * @param string_to_escape
	 */
	public function real_escape_string ($string_to_escape) {}

	public function reap_async_query () {}

	/**
	 * @param string_to_escape
	 */
	public function escape_string ($string_to_escape) {}

	/**
	 * @param query
	 */
	public function real_query ($query) {}

	/**
	 * @param name
	 */
	public function release_savepoint ($name) {}

	/**
	 * @param flags[optional]
	 * @param name[optional]
	 */
	public function rollback ($flags, $name) {}

	/**
	 * @param name
	 */
	public function savepoint ($name) {}

	/**
	 * @param database
	 */
	public function select_db ($database) {}

	/**
	 * @param charset
	 */
	public function set_charset ($charset) {}

	/**
	 * @param option
	 * @param value
	 */
	public function set_opt ($option, $value) {}

	/**
	 * @param key
	 * @param cert
	 * @param certificate_authority
	 * @param certificate_authority_path
	 * @param cipher
	 */
	public function ssl_set ($key, $cert, $certificate_authority, $certificate_authority_path, $cipher) {}

	public function stat () {}

	public function stmt_init () {}

	/**
	 * @param flags[optional]
	 */
	public function store_result ($flags) {}

	public function thread_safe () {}

	public function use_result () {}

	/**
	 * @param options
	 */
	public function refresh ($options) {}

}

final class mysqli_warning  {
	public $message;
	public $sqlstate;
	public $errno;


	protected function __construct () {}

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

	public function free () {}

	/**
	 * @param offset
	 */
	public function data_seek ($offset) {}

	public function fetch_field () {}

	public function fetch_fields () {}

	/**
	 * @param field_nr
	 */
	public function fetch_field_direct ($field_nr) {}

	public function fetch_all () {}

	/**
	 * @param result_type[optional]
	 */
	public function fetch_array ($result_type) {}

	public function fetch_assoc () {}

	/**
	 * @param class_name[optional]
	 * @param params[optional]
	 */
	public function fetch_object ($class_namearray , $params) {}

	public function fetch_row () {}

	/**
	 * @param field_nr
	 */
	public function field_seek ($field_nr) {}

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
	 * @param attribute
	 */
	public function attr_get ($attribute) {}

	/**
	 * @param attribute
	 * @param value
	 */
	public function attr_set ($attribute, $value) {}

	/**
	 * @param types
	 * @param vars
	 */
	public function bind_param ($types, &$vars) {}

	/**
	 * @param vars
	 */
	public function bind_result (&$vars) {}

	public function close () {}

	/**
	 * @param offset
	 */
	public function data_seek ($offset) {}

	public function execute () {}

	public function fetch () {}

	public function get_warnings () {}

	public function result_metadata () {}

	public function more_results () {}

	public function next_result () {}

	public function num_rows () {}

	/**
	 * @param param_nr
	 * @param data
	 */
	public function send_long_data ($param_nr, $data) {}

	public function free_result () {}

	public function reset () {}

	/**
	 * @param query
	 */
	public function prepare ($query) {}

	public function store_result () {}

	public function get_result () {}

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
 * @param link
 */
function mysqli_get_client_version ($link) {}

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
