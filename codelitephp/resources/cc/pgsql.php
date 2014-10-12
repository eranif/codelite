<?php

// Start of pgsql v.

/**
 * @param connection_string
 * @param connect_type[optional]
 * @param host[optional]
 * @param port[optional]
 * @param options[optional]
 * @param tty[optional]
 * @param database[optional]
 */
function pg_connect ($connection_string, $connect_type, $host, $port, $options, $tty, $database) {}

/**
 * @param connection_string
 * @param host[optional]
 * @param port[optional]
 * @param options[optional]
 * @param tty[optional]
 * @param database[optional]
 */
function pg_pconnect ($connection_string, $host, $port, $options, $tty, $database) {}

/**
 * @param connection[optional]
 */
function pg_connect_poll ($connection) {}

/**
 * @param connection[optional]
 */
function pg_close ($connection) {}

/**
 * @param connection
 */
function pg_connection_status ($connection) {}

/**
 * @param connection
 */
function pg_connection_busy ($connection) {}

/**
 * @param connection
 */
function pg_connection_reset ($connection) {}

/**
 * @param connection[optional]
 */
function pg_host ($connection) {}

/**
 * @param connection[optional]
 */
function pg_dbname ($connection) {}

/**
 * @param connection[optional]
 */
function pg_port ($connection) {}

/**
 * @param connection[optional]
 */
function pg_tty ($connection) {}

/**
 * @param connection[optional]
 */
function pg_options ($connection) {}

/**
 * @param connection[optional]
 */
function pg_version ($connection) {}

/**
 * @param connection[optional]
 */
function pg_ping ($connection) {}

/**
 * @param connection
 * @param param_name[optional]
 */
function pg_parameter_status ($connection, $param_name) {}

/**
 * @param connection
 */
function pg_transaction_status ($connection) {}

/**
 * @param connection[optional]
 * @param query[optional]
 */
function pg_query ($connection, $query) {}

/**
 * @param connection[optional]
 * @param query[optional]
 * @param params[optional]
 */
function pg_query_params ($connection, $query, $params) {}

/**
 * @param connection[optional]
 * @param stmtname[optional]
 * @param query[optional]
 */
function pg_prepare ($connection, $stmtname, $query) {}

/**
 * @param connection[optional]
 * @param stmtname[optional]
 * @param params[optional]
 */
function pg_execute ($connection, $stmtname, $params) {}

/**
 * @param connection
 * @param query
 */
function pg_send_query ($connection, $query) {}

/**
 * @param connection
 * @param query
 * @param params
 */
function pg_send_query_params ($connection, $query, $params) {}

/**
 * @param connection
 * @param stmtname
 * @param query
 */
function pg_send_prepare ($connection, $stmtname, $query) {}

/**
 * @param connection
 * @param stmtname
 * @param params
 */
function pg_send_execute ($connection, $stmtname, $params) {}

/**
 * @param connection
 */
function pg_cancel_query ($connection) {}

/**
 * @param result
 * @param row_number[optional]
 * @param field_name[optional]
 */
function pg_fetch_result ($result, $row_number, $field_name) {}

/**
 * @param result
 * @param row[optional]
 * @param result_type[optional]
 */
function pg_fetch_row ($result, $row, $result_type) {}

/**
 * @param result
 * @param row[optional]
 */
function pg_fetch_assoc ($result, $row) {}

/**
 * @param result
 * @param row[optional]
 * @param result_type[optional]
 */
function pg_fetch_array ($result, $row, $result_type) {}

/**
 * @param result
 * @param row[optional]
 * @param class_name[optional]
 * @param l[optional]
 * @param ctor_params[optional]
 */
function pg_fetch_object ($result, $row, $class_name, $l, $ctor_params) {}

/**
 * @param result
 */
function pg_fetch_all ($result) {}

/**
 * @param result
 * @param column_number[optional]
 */
function pg_fetch_all_columns ($result, $column_number) {}

/**
 * @param result
 */
function pg_affected_rows ($result) {}

/**
 * @param connection
 */
function pg_get_result ($connection) {}

/**
 * @param result
 * @param offset
 */
function pg_result_seek ($result, $offset) {}

/**
 * @param result
 * @param result_type[optional]
 */
function pg_result_status ($result, $result_type) {}

/**
 * @param result
 */
function pg_free_result ($result) {}

/**
 * @param result
 */
function pg_last_oid ($result) {}

/**
 * @param result
 */
function pg_num_rows ($result) {}

/**
 * @param result
 */
function pg_num_fields ($result) {}

/**
 * @param result
 * @param field_number
 */
function pg_field_name ($result, $field_number) {}

/**
 * @param result
 * @param field_name
 */
function pg_field_num ($result, $field_name) {}

/**
 * @param result
 * @param field_number
 */
function pg_field_size ($result, $field_number) {}

/**
 * @param result
 * @param field_number
 */
function pg_field_type ($result, $field_number) {}

/**
 * @param result
 * @param field_number
 */
function pg_field_type_oid ($result, $field_number) {}

/**
 * @param result
 * @param row[optional]
 * @param field_name_or_number[optional]
 */
function pg_field_prtlen ($result, $row, $field_name_or_number) {}

/**
 * @param result
 * @param row[optional]
 * @param field_name_or_number[optional]
 */
function pg_field_is_null ($result, $row, $field_name_or_number) {}

/**
 * @param result
 * @param field_number
 * @param oid_only[optional]
 */
function pg_field_table ($result, $field_number, $oid_only) {}

/**
 * @param connection[optional]
 * @param e[optional]
 */
function pg_get_notify ($connection, $e) {}

/**
 * @param connection
 */
function pg_socket ($connection) {}

/**
 * @param connection
 */
function pg_consume_input ($connection) {}

/**
 * @param connection
 */
function pg_flush ($connection) {}

/**
 * @param connection[optional]
 */
function pg_get_pid ($connection) {}

/**
 * @param result
 */
function pg_result_error ($result) {}

/**
 * @param result
 * @param fieldcode
 */
function pg_result_error_field ($result, $fieldcode) {}

/**
 * @param connection[optional]
 */
function pg_last_error ($connection) {}

/**
 * @param connection
 */
function pg_last_notice ($connection) {}

/**
 * @param connection[optional]
 * @param query[optional]
 */
function pg_put_line ($connection, $query) {}

/**
 * @param connection[optional]
 */
function pg_end_copy ($connection) {}

/**
 * @param connection
 * @param table_name
 * @param delimiter[optional]
 * @param null_as[optional]
 */
function pg_copy_to ($connection, $table_name, $delimiter, $null_as) {}

/**
 * @param connection
 * @param table_name
 * @param rows
 * @param delimiter[optional]
 * @param null_as[optional]
 */
function pg_copy_from ($connection, $table_name, $rows, $delimiter, $null_as) {}

/**
 * @param filename
 * @param mode[optional]
 * @param connection[optional]
 */
function pg_trace ($filename, $mode, $connection) {}

/**
 * @param connection[optional]
 */
function pg_untrace ($connection) {}

/**
 * @param connection[optional]
 * @param large_object_id[optional]
 */
function pg_lo_create ($connection, $large_object_id) {}

/**
 * @param connection[optional]
 * @param large_object_oid[optional]
 */
function pg_lo_unlink ($connection, $large_object_oid) {}

/**
 * @param connection[optional]
 * @param large_object_oid[optional]
 * @param mode[optional]
 */
function pg_lo_open ($connection, $large_object_oid, $mode) {}

/**
 * @param large_object
 */
function pg_lo_close ($large_object) {}

/**
 * @param large_object
 * @param len[optional]
 */
function pg_lo_read ($large_object, $len) {}

/**
 * @param large_object
 * @param buf
 * @param len[optional]
 */
function pg_lo_write ($large_object, $buf, $len) {}

/**
 * @param large_object
 */
function pg_lo_read_all ($large_object) {}

/**
 * @param connection[optional]
 * @param filename[optional]
 * @param large_object_oid[optional]
 */
function pg_lo_import ($connection, $filename, $large_object_oid) {}

/**
 * @param connection[optional]
 * @param objoid[optional]
 * @param filename[optional]
 */
function pg_lo_export ($connection, $objoid, $filename) {}

/**
 * @param large_object
 * @param offset
 * @param whence[optional]
 */
function pg_lo_seek ($large_object, $offset, $whence) {}

/**
 * @param large_object
 */
function pg_lo_tell ($large_object) {}

/**
 * @param large_object
 * @param size[optional]
 */
function pg_lo_truncate ($large_object, $size) {}

/**
 * @param connection[optional]
 * @param data[optional]
 */
function pg_escape_string ($connection, $data) {}

/**
 * @param connection[optional]
 * @param data[optional]
 */
function pg_escape_bytea ($connection, $data) {}

/**
 * @param data
 */
function pg_unescape_bytea ($data) {}

/**
 * @param connection[optional]
 * @param data[optional]
 */
function pg_escape_literal ($connection, $data) {}

/**
 * @param connection[optional]
 * @param data[optional]
 */
function pg_escape_identifier ($connection, $data) {}

/**
 * @param connection[optional]
 * @param verbosity[optional]
 */
function pg_set_error_verbosity ($connection, $verbosity) {}

/**
 * @param connection[optional]
 */
function pg_client_encoding ($connection) {}

/**
 * @param connection[optional]
 * @param encoding[optional]
 */
function pg_set_client_encoding ($connection, $encoding) {}

/**
 * @param db
 * @param table
 */
function pg_meta_data ($db, $table) {}

/**
 * @param db
 * @param table
 * @param values
 * @param options[optional]
 */
function pg_convert ($db, $table, $values, $options) {}

/**
 * @param db
 * @param table
 * @param values
 * @param options[optional]
 */
function pg_insert ($db, $table, $values, $options) {}

/**
 * @param db
 * @param table
 * @param fields
 * @param ids
 * @param options[optional]
 */
function pg_update ($db, $table, $fields, $ids, $options) {}

/**
 * @param db
 * @param table
 * @param ids
 * @param options[optional]
 */
function pg_delete ($db, $table, $ids, $options) {}

/**
 * @param db
 * @param table
 * @param ids
 * @param options[optional]
 */
function pg_select ($db, $table, $ids, $options) {}

/**
 * @param connection[optional]
 * @param query[optional]
 */
function pg_exec ($connection, $query) {}

/**
 * @param result
 */
function pg_getlastoid ($result) {}

/**
 * @param result
 */
function pg_cmdtuples ($result) {}

/**
 * @param connection[optional]
 */
function pg_errormessage ($connection) {}

/**
 * @param result
 */
function pg_numrows ($result) {}

/**
 * @param result
 */
function pg_numfields ($result) {}

/**
 * @param result
 * @param field_number
 */
function pg_fieldname ($result, $field_number) {}

/**
 * @param result
 * @param field_number
 */
function pg_fieldsize ($result, $field_number) {}

/**
 * @param result
 * @param field_number
 */
function pg_fieldtype ($result, $field_number) {}

/**
 * @param result
 * @param field_name
 */
function pg_fieldnum ($result, $field_name) {}

/**
 * @param result
 * @param row[optional]
 * @param field_name_or_number[optional]
 */
function pg_fieldprtlen ($result, $row, $field_name_or_number) {}

/**
 * @param result
 * @param row[optional]
 * @param field_name_or_number[optional]
 */
function pg_fieldisnull ($result, $row, $field_name_or_number) {}

/**
 * @param result
 */
function pg_freeresult ($result) {}

/**
 * @param connection
 */
function pg_result ($connection) {}

/**
 * @param large_object
 */
function pg_loreadall ($large_object) {}

/**
 * @param connection[optional]
 * @param large_object_id[optional]
 */
function pg_locreate ($connection, $large_object_id) {}

/**
 * @param connection[optional]
 * @param large_object_oid[optional]
 */
function pg_lounlink ($connection, $large_object_oid) {}

/**
 * @param connection[optional]
 * @param large_object_oid[optional]
 * @param mode[optional]
 */
function pg_loopen ($connection, $large_object_oid, $mode) {}

/**
 * @param large_object
 */
function pg_loclose ($large_object) {}

/**
 * @param large_object
 * @param len[optional]
 */
function pg_loread ($large_object, $len) {}

/**
 * @param large_object
 * @param buf
 * @param len[optional]
 */
function pg_lowrite ($large_object, $buf, $len) {}

/**
 * @param connection[optional]
 * @param filename[optional]
 * @param large_object_oid[optional]
 */
function pg_loimport ($connection, $filename, $large_object_oid) {}

/**
 * @param connection[optional]
 * @param objoid[optional]
 * @param filename[optional]
 */
function pg_loexport ($connection, $objoid, $filename) {}

/**
 * @param connection[optional]
 */
function pg_clientencoding ($connection) {}

/**
 * @param connection[optional]
 * @param encoding[optional]
 */
function pg_setclientencoding ($connection, $encoding) {}


/**
 * Short libpq version that contains only numbers and dots.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_LIBPQ_VERSION', "8.4.17");

/**
 * Long libpq version that includes compiler information.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_LIBPQ_VERSION_STR', "PostgreSQL 8.4.17 on x86_64-unknown-linux-gnu, compiled by GCC gcc (Debian 4.4.5-8) 4.4.5, 64-bit");

/**
 * Passed to pg_connect to force the creation of a new connection,
 *      rather than re-using an existing identical connection.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONNECT_FORCE_NEW', 2);

/**
 * Passed to pg_connect to create an asynchronous
 *      connection. Added in PHP 5.6.0.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONNECT_ASYNC', 4);

/**
 * Passed to pg_fetch_array. Return an associative array of field
 *      names and values.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_ASSOC', 1);

/**
 * Passed to pg_fetch_array. Return a numerically indexed array of field
 *      numbers and values.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_NUM', 2);

/**
 * Passed to pg_fetch_array. Return an array of field values
 *      that is both numerically indexed (by field number) and associated (by field name).
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_BOTH', 3);

/**
 * Returned by pg_connection_status indicating that the database
 *      connection is in an invalid state.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONNECTION_BAD', 1);

/**
 * Returned by pg_connection_status indicating that the database
 *      connection is in a valid state.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONNECTION_OK', 0);
define ('PGSQL_CONNECTION_STARTED', 2);
define ('PGSQL_CONNECTION_MADE', 3);
define ('PGSQL_CONNECTION_AWAITING_RESPONSE', 4);
define ('PGSQL_CONNECTION_AUTH_OK', 5);
define ('PGSQL_CONNECTION_SETENV', 6);

/**
 * Returned by pg_connect_poll to indicate that the
 *     connection attempt failed.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_POLLING_FAILED', 0);

/**
 * Returned by pg_connect_poll to indicate that the
 *     connection is waiting for the PostgreSQL socket to be readable.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_POLLING_READING', 1);

/**
 * Returned by pg_connect_poll to indicate that the
 *     connection is waiting for the PostgreSQL socket to be writable.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_POLLING_WRITING', 2);

/**
 * Returned by pg_connect_poll to indicate that the
 *     connection is ready to be used.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_POLLING_OK', 3);

/**
 * Returned by pg_connect_poll to indicate that the
 *     connection is currently active.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_POLLING_ACTIVE', 4);

/**
 * Returned by pg_transaction_status. Connection is
 *     currently idle, not in a transaction.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TRANSACTION_IDLE', 0);

/**
 * Returned by pg_transaction_status. A command
 *     is in progress on the connection.  A query has been sent via the connection
 *     and not yet completed.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TRANSACTION_ACTIVE', 1);

/**
 * Returned by pg_transaction_status. The connection
 *     is idle, in a transaction block.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TRANSACTION_INTRANS', 2);

/**
 * Returned by pg_transaction_status. The connection
 *     is idle, in a failed transaction block.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TRANSACTION_INERROR', 3);

/**
 * Returned by pg_transaction_status. The connection
 *     is bad.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TRANSACTION_UNKNOWN', 4);

/**
 * Passed to pg_set_error_verbosity.
 *     Specified that returned messages include severity, primary text, 
 *     and position only; this will normally fit on a single line.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_ERRORS_TERSE', 0);

/**
 * Passed to pg_set_error_verbosity.
 *     The default mode produces messages that include the above 
 *     plus any detail, hint, or context fields (these may span 
 *     multiple lines).
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_ERRORS_DEFAULT', 1);

/**
 * Passed to pg_set_error_verbosity.
 *     The verbose mode includes all available fields.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_ERRORS_VERBOSE', 2);

/**
 * Passed to pg_lo_seek. Seek operation is to begin
 *     from the start of the object.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_SEEK_SET', 0);

/**
 * Passed to pg_lo_seek. Seek operation is to begin
 *     from the current position.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_SEEK_CUR', 1);

/**
 * Passed to pg_lo_seek. Seek operation is to begin
 *     from the end of the object.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_SEEK_END', 2);

/**
 * Passed to pg_result_status.  Indicates that
 *     numerical result code is desired.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_STATUS_LONG', 1);

/**
 * Passed to pg_result_status.  Indicates that
 *     textual result command tag is desired.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_STATUS_STRING', 2);

/**
 * Returned by pg_result_status. The string sent to the server
 *     was empty.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_EMPTY_QUERY', 0);

/**
 * Returned by pg_result_status. Successful completion of a 
 *     command returning no data.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_COMMAND_OK', 1);

/**
 * Returned by pg_result_status. Successful completion of a command 
 *     returning data (such as a SELECT or SHOW).
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_TUPLES_OK', 2);

/**
 * Returned by pg_result_status. Copy Out (from server) data 
 *     transfer started.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_COPY_OUT', 3);

/**
 * Returned by pg_result_status. Copy In (to server) data 
 *     transfer started.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_COPY_IN', 4);

/**
 * Returned by pg_result_status. The server's response 
 *     was not understood.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_BAD_RESPONSE', 5);

/**
 * Returned by pg_result_status. A nonfatal error 
 *     (a notice or warning) occurred.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_NONFATAL_ERROR', 6);

/**
 * Returned by pg_result_status. A fatal error 
 *     occurred.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_FATAL_ERROR', 7);

/**
 * Passed to pg_result_error_field.
 *     The severity; the field contents are ERROR, 
 *     FATAL, or PANIC (in an error message), or 
 *     WARNING, NOTICE, DEBUG, 
 *     INFO, or LOG (in a notice message), or a localized 
 *     translation of one of these. Always present.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_SEVERITY', 83);

/**
 * Passed to pg_result_error_field.
 *     The SQLSTATE code for the error. The SQLSTATE code identifies the type of error 
 *     that has occurred; it can be used by front-end applications to perform specific 
 *     operations (such as error handling) in response to a particular database error. 
 *     This field is not localizable, and is always present.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_SQLSTATE', 67);

/**
 * Passed to pg_result_error_field.
 *     The primary human-readable error message (typically one line). Always present.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_MESSAGE_PRIMARY', 77);

/**
 * Passed to pg_result_error_field.
 *     Detail: an optional secondary error message carrying more detail about the problem. May run to multiple lines.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_MESSAGE_DETAIL', 68);

/**
 * Passed to pg_result_error_field.
 *     Hint: an optional suggestion what to do about the problem. This is intended to differ from detail in that it
 *     offers advice (potentially inappropriate) rather than hard facts. May run to multiple lines.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_MESSAGE_HINT', 72);

/**
 * Passed to pg_result_error_field.
 *     A string containing a decimal integer indicating an error cursor position as an index into the original 
 *     statement string. The first character has index 1, and positions are measured in characters not bytes.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_STATEMENT_POSITION', 80);

/**
 * Passed to pg_result_error_field.
 *     This is defined the same as the PG_DIAG_STATEMENT_POSITION field, but 
 *     it is used when the cursor position refers to an internally generated 
 *     command rather than the one submitted by the client. The 
 *     PG_DIAG_INTERNAL_QUERY field will always appear when this 
 *     field appears.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_INTERNAL_POSITION', 112);

/**
 * Passed to pg_result_error_field.
 *     The text of a failed internally-generated command. This could be, for example, a 
 *     SQL query issued by a PL/pgSQL function.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_INTERNAL_QUERY', 113);

/**
 * Passed to pg_result_error_field.
 *     An indication of the context in which the error occurred. Presently 
 *     this includes a call stack traceback of active procedural language 
 *     functions and internally-generated queries. The trace is one entry 
 *     per line, most recent first.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_CONTEXT', 87);

/**
 * Passed to pg_result_error_field.
 *     The file name of the PostgreSQL source-code location where the error 
 *     was reported.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_SOURCE_FILE', 70);

/**
 * Passed to pg_result_error_field.
 *     The line number of the PostgreSQL source-code location where the 
 *     error was reported.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_SOURCE_LINE', 76);

/**
 * Passed to pg_result_error_field.
 *     The name of the PostgreSQL source-code function reporting the error.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DIAG_SOURCE_FUNCTION', 82);

/**
 * Passed to pg_convert.
 *     Ignore default values in the table during conversion.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONV_IGNORE_DEFAULT', 2);

/**
 * Passed to pg_convert.
 *     Use SQL NULL in place of an empty string.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONV_FORCE_NULL', 4);

/**
 * Passed to pg_convert.
 *     Ignore conversion of &null; into SQL NOT NULL columns.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONV_IGNORE_NOT_NULL', 8);

/**
 * Passed to pg_insert, pg_select,
 *     pg_update and pg_delete.
 *     Apply escape to all parameters instead of calling pg_convert
 *     internally. This option omits meta data look up. Query could be as fast as
 *     pg_query and pg_send_query.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DML_ESCAPE', 4096);

/**
 * Passed to pg_insert, pg_select,
 *     pg_update and pg_delete.
 *     All parameters passed as is. Manual escape is required
 *     if parameters contain user supplied data. Use pg_escape_string
 *     for it.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DML_NO_CONV', 256);

/**
 * Passed to pg_insert, pg_select,
 *     pg_update and pg_delete.
 *     Execute query by these functions.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DML_EXEC', 512);

/**
 * Passed to pg_insert, pg_select,
 *     pg_update and pg_delete.
 *     Execute asynchronous query by these functions.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DML_ASYNC', 1024);

/**
 * Passed to pg_insert, pg_select,
 *     pg_update and pg_delete.
 *     Return executed query string.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_DML_STRING', 2048);

// End of pgsql v.
?>
