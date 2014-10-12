<?php

// Start of mysql v.1.0

/**
 * @param hostname[optional]
 * @param username[optional]
 * @param password[optional]
 * @param new[optional]
 * @param flags[optional]
 */
function mysql_connect ($hostname, $username, $password, $new, $flags) {}

/**
 * @param hostname[optional]
 * @param username[optional]
 * @param password[optional]
 * @param flags[optional]
 */
function mysql_pconnect ($hostname, $username, $password, $flags) {}

/**
 * @param link_identifier[optional]
 */
function mysql_close ($link_identifier) {}

/**
 * @param database_name
 * @param link_identifier[optional]
 */
function mysql_select_db ($database_name, $link_identifier) {}

/**
 * @param query
 * @param link_identifier[optional]
 */
function mysql_query ($query, $link_identifier) {}

/**
 * @param query
 * @param link_identifier[optional]
 */
function mysql_unbuffered_query ($query, $link_identifier) {}

/**
 * @param database_name
 * @param query
 * @param link_identifier[optional]
 */
function mysql_db_query ($database_name, $query, $link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_list_dbs ($link_identifier) {}

/**
 * @param database_name
 * @param link_identifier[optional]
 */
function mysql_list_tables ($database_name, $link_identifier) {}

/**
 * @param database_name
 * @param table_name
 * @param link_identifier[optional]
 */
function mysql_list_fields ($database_name, $table_name, $link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_list_processes ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_error ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_errno ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_affected_rows ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_insert_id ($link_identifier) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_result ($result, $row, $field) {}

/**
 * @param result
 */
function mysql_num_rows ($result) {}

/**
 * @param result
 */
function mysql_num_fields ($result) {}

/**
 * @param result
 */
function mysql_fetch_row ($result) {}

/**
 * @param result
 * @param result_type[optional]
 */
function mysql_fetch_array ($result, $result_type) {}

/**
 * @param result
 */
function mysql_fetch_assoc ($result) {}

/**
 * @param result
 * @param class_name[optional]
 * @param ctor_params[optional]
 */
function mysql_fetch_object ($result, $class_name, $ctor_params) {}

/**
 * @param result
 * @param row_number
 */
function mysql_data_seek ($result, $row_number) {}

/**
 * @param result
 */
function mysql_fetch_lengths ($result) {}

/**
 * @param result
 * @param field_offset[optional]
 */
function mysql_fetch_field ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_field_seek ($result, $field_offset) {}

/**
 * @param result
 */
function mysql_free_result ($result) {}

/**
 * @param result
 * @param field_index
 */
function mysql_field_name ($result, $field_index) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_field_table ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_field_len ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_field_type ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_field_flags ($result, $field_offset) {}

/**
 * @param string
 */
function mysql_escape_string ($string) {}

/**
 * @param string
 * @param link_identifier[optional]
 */
function mysql_real_escape_string ($string, $link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_stat ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_thread_id ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_client_encoding ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_ping ($link_identifier) {}

function mysql_get_client_info () {}

/**
 * @param link_identifier[optional]
 */
function mysql_get_host_info ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_get_proto_info ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_get_server_info ($link_identifier) {}

/**
 * @param link_identifier[optional]
 */
function mysql_info ($link_identifier) {}

/**
 * @param charset_name
 * @param link_identifier[optional]
 */
function mysql_set_charset ($charset_name, $link_identifier) {}

/**
 * @param database_name
 * @param query
 * @param link_identifier[optional]
 */
function mysql ($database_name, $query, $link_identifier) {}

/**
 * @param result
 * @param field_index
 */
function mysql_fieldname ($result, $field_index) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_fieldtable ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_fieldlen ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_fieldtype ($result, $field_offset) {}

/**
 * @param result
 * @param field_offset
 */
function mysql_fieldflags ($result, $field_offset) {}

/**
 * @param database_name
 * @param link_identifier[optional]
 */
function mysql_selectdb ($database_name, $link_identifier) {}

/**
 * @param result
 */
function mysql_freeresult ($result) {}

/**
 * @param result
 */
function mysql_numfields ($result) {}

/**
 * @param result
 */
function mysql_numrows ($result) {}

/**
 * @param link_identifier[optional]
 */
function mysql_listdbs ($link_identifier) {}

/**
 * @param database_name
 * @param link_identifier[optional]
 */
function mysql_listtables ($database_name, $link_identifier) {}

/**
 * @param database_name
 * @param table_name
 * @param link_identifier[optional]
 */
function mysql_listfields ($database_name, $table_name, $link_identifier) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_db_name ($result, $row, $field) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_dbname ($result, $row, $field) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_tablename ($result, $row, $field) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_table_name ($result, $row, $field) {}


/**
 * Columns are returned into the array having the fieldname as the array
 *       index.
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_ASSOC', 1);

/**
 * Columns are returned into the array having a numerical index to the
 *       fields. This index starts with 0, the first field in the result.
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_NUM', 2);

/**
 * Columns are returned into the array having both a numerical index
 *       and the fieldname as the array index.
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_BOTH', 3);

/**
 * Use compression protocol
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_CLIENT_COMPRESS', 32);

/**
 * Use SSL encryption. This flag is only available with version 4.x
 *        of the MySQL client library or newer. Version 3.23.x is bundled both
 *        with PHP 4 and Windows binaries of PHP 5.
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_CLIENT_SSL', 2048);

/**
 * Allow interactive_timeout seconds (instead of wait_timeout) of
 *        inactivity before closing the connection.
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_CLIENT_INTERACTIVE', 1024);

/**
 * Allow space after function names
 * @link http://www.php.net/manual/en/mysql.constants.php
 */
define ('MYSQL_CLIENT_IGNORE_SPACE', 256);

// End of mysql v.1.0
?>
