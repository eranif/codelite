<?php

// Start of oci8 v.2.0.8

class OCI_Lob  {

	public function load () {}

	public function tell () {}

	/**
	 * @param length[optional]
	 */
	public function truncate ($length) {}

	/**
	 * @param offset[optional]
	 * @param length[optional]
	 */
	public function erase ($offset, $length) {}

	/**
	 * @param flag[optional]
	 */
	public function flush ($flag) {}

	/**
	 * @param mode
	 */
	public function setbuffering ($mode) {}

	public function getbuffering () {}

	public function rewind () {}

	/**
	 * @param length
	 */
	public function read ($length) {}

	public function eof () {}

	/**
	 * @param offset
	 * @param whence[optional]
	 */
	public function seek ($offset, $whence) {}

	/**
	 * @param string
	 * @param length[optional]
	 */
	public function write ($string, $length) {}

	/**
	 * @param lob_descriptor_from
	 */
	public function append ($lob_descriptor_from) {}

	public function size () {}

	/**
	 * @param filename
	 * @param start[optional]
	 * @param length[optional]
	 */
	public function writetofile ($filename, $start, $length) {}

	/**
	 * @param filename
	 * @param start[optional]
	 * @param length[optional]
	 */
	public function export ($filename, $start, $length) {}

	/**
	 * @param filename
	 */
	public function import ($filename) {}

	/**
	 * @param data
	 * @param type[optional]
	 */
	public function writetemporary ($data, $type) {}

	public function close () {}

	/**
	 * @param data
	 * @param offset[optional]
	 */
	public function save ($data, $offset) {}

	/**
	 * @param filename
	 */
	public function savefile ($filename) {}

	public function free () {}

}

class OCI_Collection  {

	/**
	 * @param value
	 */
	public function append ($value) {}

	/**
	 * @param index
	 */
	public function getelem ($index) {}

	/**
	 * @param index
	 * @param value
	 */
	public function assignelem ($index, $value) {}

	/**
	 * @param collection_from
	 */
	public function assign ($collection_from) {}

	public function size () {}

	public function max () {}

	/**
	 * @param number
	 */
	public function trim ($number) {}

	public function free () {}

}

/**
 * @param statement_resource
 * @param column_name
 * @param variable
 * @param type[optional]
 */
function oci_define_by_name ($statement_resource, $column_name, &$variable, $type) {}

/**
 * @param statement_resource
 * @param column_name
 * @param variable
 * @param maximum_length[optional]
 * @param type[optional]
 */
function oci_bind_by_name ($statement_resource, $column_name, &$variable, $maximum_length, $type) {}

/**
 * @param statement_resource
 * @param column_name
 * @param variable
 * @param maximum_array_length
 * @param maximum_item_length[optional]
 * @param type[optional]
 */
function oci_bind_array_by_name ($statement_resource, $column_name, &$variable, $maximum_array_length, $maximum_item_length, $type) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function oci_field_is_null ($statement_resource, $column_number_or_name) {}

/**
 * @param statement_resource
 * @param column_number
 */
function oci_field_name ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function oci_field_size ($statement_resource, $column_number_or_name) {}

/**
 * @param statement_resource
 * @param column_number
 */
function oci_field_scale ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function oci_field_precision ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function oci_field_type ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function oci_field_type_raw ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param mode[optional]
 */
function oci_execute ($statement_resource, $mode) {}

/**
 * @param statement_resource
 */
function oci_cancel ($statement_resource) {}

/**
 * @param statement_resource
 */
function oci_fetch ($statement_resource) {}

/**
 * @param statement_resource
 */
function oci_fetch_object ($statement_resource) {}

/**
 * @param statement_resource
 */
function oci_fetch_row ($statement_resource) {}

/**
 * @param statement_resource
 */
function oci_fetch_assoc ($statement_resource) {}

/**
 * @param statement_resource
 * @param mode[optional]
 */
function oci_fetch_array ($statement_resource, $mode) {}

/**
 * @param statement_resource
 * @param result
 * @param mode[optional]
 */
function ocifetchinto ($statement_resource, &$result, $mode) {}

/**
 * @param statement_resource
 * @param output
 * @param skip[optional]
 * @param maximum_rows[optional]
 * @param flags[optional]
 */
function oci_fetch_all ($statement_resource, &$output, $skip, $maximum_rows, $flags) {}

/**
 * @param statement_resource
 */
function oci_free_statement ($statement_resource) {}

/**
 * @param mode
 */
function oci_internal_debug ($mode) {}

/**
 * @param statement_resource
 */
function oci_num_fields ($statement_resource) {}

/**
 * @param connection_resource
 * @param sql_text
 */
function oci_parse ($connection_resource, $sql_text) {}

/**
 * @param statement_resource
 */
function oci_get_implicit_resultset ($statement_resource) {}

/**
 * @param connection_resource
 */
function oci_new_cursor ($connection_resource) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function oci_result ($statement_resource, $column_number_or_name) {}

function oci_client_version () {}

/**
 * @param connection_resource
 */
function oci_server_version ($connection_resource) {}

/**
 * @param statement_resource
 */
function oci_statement_type ($statement_resource) {}

/**
 * @param statement_resource
 */
function oci_num_rows ($statement_resource) {}

/**
 * @param connection_resource
 */
function oci_close ($connection_resource) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function oci_connect ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function oci_new_connect ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function oci_pconnect ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param connection_or_statement_resource[optional]
 */
function oci_error ($connection_or_statement_resource) {}

/**
 * @param lob_descriptor
 */
function oci_free_descriptor ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param data
 * @param offset[optional]
 */
function oci_lob_save ($lob_descriptor, $data, $offset) {}

/**
 * @param lob_descriptor
 * @param filename
 */
function oci_lob_import ($lob_descriptor, $filename) {}

/**
 * @param lob_descriptor
 */
function oci_lob_size ($lob_descriptor) {}

/**
 * @param lob_descriptor
 */
function oci_lob_load ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param length
 */
function oci_lob_read ($lob_descriptor, $length) {}

/**
 * @param lob_descriptor
 */
function oci_lob_eof ($lob_descriptor) {}

/**
 * @param lob_descriptor
 */
function oci_lob_tell ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param length[optional]
 */
function oci_lob_truncate ($lob_descriptor, $length) {}

/**
 * @param lob_descriptor
 * @param offset[optional]
 * @param length[optional]
 */
function oci_lob_erase ($lob_descriptor, $offset, $length) {}

/**
 * @param lob_descriptor
 * @param flag[optional]
 */
function oci_lob_flush ($lob_descriptor, $flag) {}

/**
 * @param lob_descriptor
 * @param mode
 */
function ocisetbufferinglob ($lob_descriptor, $mode) {}

/**
 * @param lob_descriptor
 */
function ocigetbufferinglob ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param lob_descriptor
 */
function oci_lob_is_equal ($lob_descriptor, $lob_descriptor) {}

/**
 * @param lob_descriptor
 */
function oci_lob_rewind ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param string
 * @param length[optional]
 */
function oci_lob_write ($lob_descriptor, $string, $length) {}

/**
 * @param lob_descriptor_to
 * @param lob_descriptor_from
 */
function oci_lob_append ($lob_descriptor_to, $lob_descriptor_from) {}

/**
 * @param lob_descriptor_to
 * @param lob_descriptor_from
 * @param length[optional]
 */
function oci_lob_copy ($lob_descriptor_to, $lob_descriptor_from, $length) {}

/**
 * @param lob_descriptor
 * @param filename
 * @param start[optional]
 * @param length[optional]
 */
function oci_lob_export ($lob_descriptor, $filename, $start, $length) {}

/**
 * @param lob_descriptor
 * @param offset
 * @param whence[optional]
 */
function oci_lob_seek ($lob_descriptor, $offset, $whence) {}

/**
 * @param connection_resource
 */
function oci_commit ($connection_resource) {}

/**
 * @param connection_resource
 */
function oci_rollback ($connection_resource) {}

/**
 * @param connection_resource
 * @param type[optional]
 */
function oci_new_descriptor ($connection_resource, $type) {}

/**
 * @param statement_resource
 * @param number_of_rows
 */
function oci_set_prefetch ($statement_resource, $number_of_rows) {}

/**
 * @param connection_resource
 * @param client_identifier
 */
function oci_set_client_identifier ($connection_resource, $client_identifier) {}

/**
 * @param edition_name
 */
function oci_set_edition ($edition_name) {}

/**
 * @param connection_resource
 * @param module_name
 */
function oci_set_module_name ($connection_resource, $module_name) {}

/**
 * @param connection_resource
 * @param action
 */
function oci_set_action ($connection_resource, $action) {}

/**
 * @param connection_resource
 * @param client_information
 */
function oci_set_client_info ($connection_resource, $client_information) {}

/**
 * @param connection_resource_or_connection_string
 * @param username
 * @param old_password
 * @param new_password
 */
function oci_password_change ($connection_resource_or_connection_string, $username, $old_password, $new_password) {}

/**
 * @param collection
 */
function oci_free_collection ($collection) {}

/**
 * @param collection
 * @param value
 */
function oci_collection_append ($collection, $value) {}

/**
 * @param collection
 * @param index
 */
function oci_collection_element_get ($collection, $index) {}

/**
 * @param collection
 * @param index
 * @param value
 */
function oci_collection_element_assign ($collection, $index, $value) {}

/**
 * @param collection_to
 * @param collection_from
 */
function oci_collection_assign ($collection_to, $collection_from) {}

/**
 * @param collection
 */
function oci_collection_size ($collection) {}

/**
 * @param collection
 */
function oci_collection_max ($collection) {}

/**
 * @param collection
 * @param number
 */
function oci_collection_trim ($collection, $number) {}

/**
 * @param connection_resource
 * @param type_name
 * @param schema_name[optional]
 */
function oci_new_collection ($connection_resource, $type_name, $schema_name) {}

/**
 * @param statement_resource
 */
function oci_free_cursor ($statement_resource) {}

/**
 * @param statement_resource
 */
function ocifreecursor ($statement_resource) {}

/**
 * @param statement_resource
 * @param column_name
 * @param variable
 * @param maximum_length[optional]
 * @param type[optional]
 */
function ocibindbyname ($statement_resource, $column_name, &$variable, $maximum_length, $type) {}

/**
 * @param statement_resource
 * @param column_name
 * @param variable
 * @param type[optional]
 */
function ocidefinebyname ($statement_resource, $column_name, &$variable, $type) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function ocicolumnisnull ($statement_resource, $column_number_or_name) {}

/**
 * @param statement_resource
 * @param column_number
 */
function ocicolumnname ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function ocicolumnsize ($statement_resource, $column_number_or_name) {}

/**
 * @param statement_resource
 * @param column_number
 */
function ocicolumnscale ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function ocicolumnprecision ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function ocicolumntype ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param column_number
 */
function ocicolumntyperaw ($statement_resource, $column_number) {}

/**
 * @param statement_resource
 * @param mode[optional]
 */
function ociexecute ($statement_resource, $mode) {}

/**
 * @param statement_resource
 */
function ocicancel ($statement_resource) {}

/**
 * @param statement_resource
 */
function ocifetch ($statement_resource) {}

/**
 * @param statement_resource
 * @param output
 * @param skip[optional]
 * @param maximum_rows[optional]
 * @param flags[optional]
 */
function ocifetchstatement ($statement_resource, &$output, $skip, $maximum_rows, $flags) {}

/**
 * @param statement_resource
 */
function ocifreestatement ($statement_resource) {}

/**
 * @param mode
 */
function ociinternaldebug ($mode) {}

/**
 * @param statement_resource
 */
function ocinumcols ($statement_resource) {}

/**
 * @param connection_resource
 * @param sql_text
 */
function ociparse ($connection_resource, $sql_text) {}

/**
 * @param connection_resource
 */
function ocinewcursor ($connection_resource) {}

/**
 * @param statement_resource
 * @param column_number_or_name
 */
function ociresult ($statement_resource, $column_number_or_name) {}

/**
 * @param connection_resource
 */
function ociserverversion ($connection_resource) {}

/**
 * @param statement_resource
 */
function ocistatementtype ($statement_resource) {}

/**
 * @param statement_resource
 */
function ocirowcount ($statement_resource) {}

/**
 * @param connection_resource
 */
function ocilogoff ($connection_resource) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function ocilogon ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function ocinlogon ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param username
 * @param password
 * @param connection_string[optional]
 * @param character_set[optional]
 * @param session_mode[optional]
 */
function ociplogon ($username, $password, $connection_string, $character_set, $session_mode) {}

/**
 * @param connection_or_statement_resource[optional]
 */
function ocierror ($connection_or_statement_resource) {}

/**
 * @param lob_descriptor
 */
function ocifreedesc ($lob_descriptor) {}

/**
 * @param lob_descriptor
 * @param data
 * @param offset[optional]
 */
function ocisavelob ($lob_descriptor, $data, $offset) {}

/**
 * @param lob_descriptor
 * @param filename
 */
function ocisavelobfile ($lob_descriptor, $filename) {}

/**
 * @param lob_descriptor
 * @param filename
 * @param start[optional]
 * @param length[optional]
 */
function ociwritelobtofile ($lob_descriptor, $filename, $start, $length) {}

/**
 * @param lob_descriptor
 */
function ociloadlob ($lob_descriptor) {}

/**
 * @param connection_resource
 */
function ocicommit ($connection_resource) {}

/**
 * @param connection_resource
 */
function ocirollback ($connection_resource) {}

/**
 * @param connection_resource
 * @param type[optional]
 */
function ocinewdescriptor ($connection_resource, $type) {}

/**
 * @param statement_resource
 * @param number_of_rows
 */
function ocisetprefetch ($statement_resource, $number_of_rows) {}

/**
 * @param connection_resource_or_connection_string
 * @param username
 * @param old_password
 * @param new_password
 */
function ocipasswordchange ($connection_resource_or_connection_string, $username, $old_password, $new_password) {}

/**
 * @param collection
 */
function ocifreecollection ($collection) {}

/**
 * @param connection_resource
 * @param type_name
 * @param schema_name[optional]
 */
function ocinewcollection ($connection_resource, $type_name, $schema_name) {}

/**
 * @param collection
 * @param value
 */
function ocicollappend ($collection, $value) {}

/**
 * @param collection
 * @param index
 */
function ocicollgetelem ($collection, $index) {}

/**
 * @param collection
 * @param index
 * @param value
 */
function ocicollassignelem ($collection, $index, $value) {}

/**
 * @param collection
 */
function ocicollsize ($collection) {}

/**
 * @param collection
 */
function ocicollmax ($collection) {}

/**
 * @param collection
 * @param number
 */
function ocicolltrim ($collection, $number) {}


/**
 * See OCI_NO_AUTO_COMMIT.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_DEFAULT', 0);

/**
 * Used with oci_connect to connect with
 *           the SYSOPER privilege. The &php.ini; setting
 *           oci8.privileged_connect
 *           should be enabled to use this.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SYSOPER', 4);

/**
 * Used with oci_connect to connect with
 *           the SYSDBA privilege. The &php.ini; setting
 *           oci8.privileged_connect
 *           should be enabled to use this.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SYSDBA', 2);

/**
 * Used with oci_connect for using
 *           Oracles' External or OS authentication.  Introduced in PHP
 *           5.3 and PECL OCI8 1.3.4.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_CRED_EXT', -2147483648);

/**
 * Statement execution mode
 *           for oci_execute.  Use this mode if you
 *           want meta data such as the column names but don't want to
 *           fetch rows from the query.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_DESCRIBE_ONLY', 16);

/**
 * Statement execution mode for oci_execute
 *           call. Automatically commit changes when the statement has
 *           succeeded.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_COMMIT_ON_SUCCESS', 32);

/**
 * Statement execution mode
 *           for oci_execute. The transaction is not
 *           automatically committed when using this mode.  For
 *           readability in new code, use this value instead of the
 *           older, equivalent OCI_DEFAULT constant.
 *           Introduced in PHP 5.3.2 (PECL OCI8 1.4).
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_NO_AUTO_COMMIT', 0);

/**
 * Obsolete. Statement fetch mode. Used when the application
 *           knows in advance exactly how many rows it will be fetching.
 *           This mode turns prefetching off for Oracle release 8 or
 *           later mode. The cursor is canceled after the desired rows
 *           are fetched which may result in reduced server-side
 *           resource usage.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_EXACT_FETCH', 2);

/**
 * Used with  to set the seek position.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SEEK_SET', 0);

/**
 * Used with  to set the seek position.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SEEK_CUR', 1);

/**
 * Used with  to set the seek position.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SEEK_END', 2);

/**
 * Used with  to free
 *           buffers used.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_LOB_BUFFER_FREE', 1);

/**
 * The same as OCI_B_BFILE.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_BFILEE', 114);

/**
 * The same as OCI_B_CFILEE.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_CFILEE', 115);

/**
 * The same as OCI_B_CLOB.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_CLOB', 112);

/**
 * The same as OCI_B_BLOB.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_BLOB', 113);

/**
 * The same as OCI_B_ROWID.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_RDD', 104);

/**
 * The same as OCI_B_INT.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_INT', 3);

/**
 * The same as OCI_B_NUM.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_NUM', 2);

/**
 * The same as OCI_B_CURSOR.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_RSET', 116);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           CHAR.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_AFC', 96);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           VARCHAR2.
 *           Also used with oci_bind_by_name.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_CHR', 1);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           VARCHAR.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_VCS', 9);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           VARCHAR2.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_AVC', 97);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           STRING.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_STR', 5);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           LONG VARCHAR.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_LVC', 94);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           FLOAT.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_FLT', 4);

/**
 * Not supported.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_UIN', 68);

/**
 * Used with oci_bind_by_name to bind LONG values.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_LNG', 8);

/**
 * Used with oci_bind_by_name to bind LONG RAW values.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_LBI', 24);

/**
 * The same as OCI_B_BIN.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_BIN', 23);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           LONG.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_ODT', 156);

/**
 * Not supported.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_BDOUBLE', 22);

/**
 * Not supported.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_BFLOAT', 21);

/**
 * Used with oci_bind_by_name when binding
 *           named data types. Note: in PHP < 5.0 it was called
 *           OCI_B_SQLT_NTY.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_NTY', 108);

/**
 * The same as OCI_B_NTY.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('SQLT_NTY', 108);

/**
 * Obsolete.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_SYSDATE', "SYSDATE");

/**
 * Used with oci_bind_by_name when binding
 *           BFILEs.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_BFILE', 114);

/**
 * Used with oci_bind_by_name when binding
 *           CFILEs.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_CFILEE', 115);

/**
 * Used with oci_bind_by_name when binding
 *           CLOBs.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_CLOB', 112);

/**
 * Used with oci_bind_by_name when
 *           binding BLOBs.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_BLOB', 113);

/**
 * Used with oci_bind_by_name when binding
 *           ROWIDs.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_ROWID', 104);

/**
 * Used with oci_bind_by_name when binding
 *           cursors, previously allocated
 *           with oci_new_descriptor.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_CURSOR', 116);

/**
 * Used with oci_bind_by_name to bind RAW values.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_BIN', 23);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           INTEGER.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_INT', 3);

/**
 * Used with oci_bind_array_by_name to bind arrays of
 *           NUMBER.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_B_NUM', 2);

/**
 * Default mode of oci_fetch_all.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_FETCHSTATEMENT_BY_COLUMN', 16);

/**
 * Alternative mode of oci_fetch_all.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_FETCHSTATEMENT_BY_ROW', 32);

/**
 * Used with oci_fetch_all and
 *           oci_fetch_array to get results as an associative
 *           array.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_ASSOC', 1);

/**
 * Used with oci_fetch_all and
 *           oci_fetch_array to get results as an
 *           enumerated array.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_NUM', 2);

/**
 * Used with oci_fetch_all and
 *           oci_fetch_array to get results as an
 *           array with both associative and number indices.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_BOTH', 3);

/**
 * Used with oci_fetch_array to get empty
 *           array elements if the row items value is &null;.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_RETURN_NULLS', 4);

/**
 * Used with oci_fetch_array to get the
 *           data value of the LOB instead of the descriptor.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_RETURN_LOBS', 8);

/**
 * This flag tells oci_new_descriptor to
 *           initialize a new FILE descriptor.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_DTYPE_FILE', 56);

/**
 * This flag tells oci_new_descriptor to
 *           initialize a new LOB descriptor.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_DTYPE_LOB', 50);

/**
 * This flag tells oci_new_descriptor to
 *           initialize a new ROWID descriptor.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_DTYPE_ROWID', 54);

/**
 * The same as OCI_DTYPE_FILE.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_D_FILE', 56);

/**
 * The same as OCI_DTYPE_LOB.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_D_LOB', 50);

/**
 * The same as OCI_DTYPE_ROWID.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_D_ROWID', 54);

/**
 * Used with 
 *           to indicate that a temporary CLOB should be created.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_TEMP_CLOB', 2);

/**
 * Used with 
 *           to indicate that a temporary BLOB should be created.
 * @link http://www.php.net/manual/en/oci8.constants.php
 */
define ('OCI_TEMP_BLOB', 1);

// End of oci8 v.2.0.8
?>
