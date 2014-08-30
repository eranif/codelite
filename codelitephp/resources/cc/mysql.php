<?php

// Start of mysql v.1.0

/**
 * Open a connection to a MySQL Server
 * @link http://www.php.net/manual/en/function.mysql-connect.php
 * @param server string[optional] <p>
 *       The MySQL server. It can also include a port number. e.g. 
 *       "hostname:port" or a path to a local socket e.g. ":/path/to/socket" for 
 *       the localhost.
 *      </p>
 *      <p>
 *       If the PHP directive 
 *       mysql.default_host is undefined (default), then the default 
 *       value is 'localhost:3306'. In &sqlsafemode;, this parameter is ignored
 *       and value 'localhost:3306' is always used.
 *      </p>
 * @param username string[optional] <p>
 *       The username. Default value is defined by mysql.default_user. In
 *       &sqlsafemode;, this parameter is ignored and the name of the user that
 *       owns the server process is used.
 *      </p>
 * @param password string[optional] <p>
 *       The password. Default value is defined by mysql.default_password. In
 *       &sqlsafemode;, this parameter is ignored and empty password is used.
 *      </p>
 * @param new_link bool[optional] <p>
 *       If a second call is made to mysql_connect
 *       with the same arguments, no new link will be established, but
 *       instead, the link identifier of the already opened link will be
 *       returned. The new_link parameter modifies this 
 *       behavior and makes mysql_connect always open 
 *       a new link, even if mysql_connect was called 
 *       before with the same parameters.
 *       In &sqlsafemode;, this parameter is ignored.
 *      </p>
 * @param client_flags int[optional] <p>
 *       The client_flags parameter can be a combination 
 *       of the following constants:
 *       128 (enable LOAD DATA LOCAL handling),
 *       MYSQL_CLIENT_SSL,
 *       MYSQL_CLIENT_COMPRESS, 
 *       MYSQL_CLIENT_IGNORE_SPACE or
 *       MYSQL_CLIENT_INTERACTIVE.
 *       Read the section about  for further information.
 *       In &sqlsafemode;, this parameter is ignored.
 *      </p>
 * @return resource a MySQL link identifier on success&return.falseforfailure;.
 */
function mysql_connect ($server = null, $username = null, $password = null, $new_link = null, $client_flags = null) {}

/**
 * Open a persistent connection to a MySQL server
 * @link http://www.php.net/manual/en/function.mysql-pconnect.php
 * @param server string[optional] <p>
 *       The MySQL server. It can also include a port number. e.g. 
 *       "hostname:port" or a path to a local socket e.g. ":/path/to/socket" for 
 *       the localhost.
 *      </p>
 *      <p>
 *       If the PHP directive 
 *       mysql.default_host is undefined (default), then the default 
 *       value is 'localhost:3306'
 *      </p>
 * @param username string[optional] <p>
 *       The username. Default value is the name of the user that owns the
 *       server process.
 *      </p>
 * @param password string[optional] <p>
 *       The password. Default value is an empty password.
 *      </p>
 * @param client_flags int[optional] <p>
 *       The client_flags parameter can be a combination 
 *       of the following constants:
 *       128 (enable LOAD DATA LOCAL handling),
 *       MYSQL_CLIENT_SSL, 
 *       MYSQL_CLIENT_COMPRESS, 
 *       MYSQL_CLIENT_IGNORE_SPACE or
 *       MYSQL_CLIENT_INTERACTIVE.
 *      </p>
 * @return resource a MySQL persistent link identifier on success, or false on 
 *   failure.
 */
function mysql_pconnect ($server = null, $username = null, $password = null, $client_flags = null) {}

/**
 * Close MySQL connection
 * @link http://www.php.net/manual/en/function.mysql-close.php
 * @param link_identifier resource[optional] 
 * @return bool Returns true on success or false on failure.
 */
function mysql_close ($link_identifier = null) {}

/**
 * Select a MySQL database
 * @link http://www.php.net/manual/en/function.mysql-select-db.php
 * @param database_name string <p>
 *       The name of the database that is to be selected.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return bool Returns true on success or false on failure.
 */
function mysql_select_db ($database_name, $link_identifier = null) {}

/**
 * Send a MySQL query
 * @link http://www.php.net/manual/en/function.mysql-query.php
 * @param query string <p>
 *       An SQL query
 *      </p>
 *      <p>
 *       The query string should not end with a semicolon.
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return resource For SELECT, SHOW, DESCRIBE, EXPLAIN and other statements returning resultset,
 *   mysql_query 
 *   returns a resource on success, or false on
 *   error.
 *  </p>
 *  <p>
 *   For other type of SQL statements, INSERT, UPDATE, DELETE, DROP, etc,
 *   mysql_query returns true on success
 *   or false on error.
 *  </p>
 *  <p>
 *   The returned result resource should be passed to
 *   mysql_fetch_array, and other
 *   functions for dealing with result tables, to access the returned data.
 *  </p>
 *  <p>
 *   Use mysql_num_rows to find out how many rows
 *   were returned for a SELECT statement or
 *   mysql_affected_rows to find out how many
 *   rows were affected by a DELETE, INSERT, REPLACE, or UPDATE
 *   statement.
 *  </p>
 *  <p>
 *   mysql_query will also fail and return false
 *   if the user does not have permission to access the table(s) referenced by
 *   the query.
 */
function mysql_query ($query, $link_identifier = null) {}

/**
 * Send an SQL query to MySQL without fetching and buffering the result rows.
 * @link http://www.php.net/manual/en/function.mysql-unbuffered-query.php
 * @param query string <p>
 *       The SQL query to execute.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return resource For SELECT, SHOW, DESCRIBE or EXPLAIN statements,
 *   mysql_unbuffered_query 
 *   returns a resource on success, or false on
 *   error.
 *  </p>
 *  <p>
 *   For other type of SQL statements, UPDATE, DELETE, DROP, etc,
 *   mysql_unbuffered_query returns true on success
 *   or false on error.
 */
function mysql_unbuffered_query ($query, $link_identifier = null) {}

/**
 * Selects a database and executes a query on it
 * @link http://www.php.net/manual/en/function.mysql-db-query.php
 * @param database string <p>
 *       The name of the database that will be selected.
 *      </p>
 * @param query string <p>
 *       The MySQL query.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return resource a positive MySQL result resource to the query result,
 *   or false on error. The function also returns true/false for
 *   INSERT/UPDATE/DELETE
 *   queries to indicate success/failure.
 */
function mysql_db_query ($database, $query, $link_identifier = null) {}

/**
 * List databases available on a MySQL server
 * @link http://www.php.net/manual/en/function.mysql-list-dbs.php
 * @param link_identifier resource[optional] 
 * @return resource a result pointer resource on success, or false on
 *   failure. Use the mysql_tablename function to traverse 
 *   this result pointer, or any function for result tables, such as 
 *   mysql_fetch_array.
 */
function mysql_list_dbs ($link_identifier = null) {}

/**
 * List tables in a MySQL database
 * @link http://www.php.net/manual/en/function.mysql-list-tables.php
 * @param database string <p>
 *       The name of the database
 *      </p>
 * @param link_identifier resource[optional] 
 * @return resource A result pointer resource on success&return.falseforfailure;.
 *  </p>
 *  <p>
 *   Use the mysql_tablename function to
 *   traverse this result pointer, or any function for result tables,
 *   such as mysql_fetch_array.
 */
function mysql_list_tables ($database, $link_identifier = null) {}

/**
 * List MySQL table fields
 * @link http://www.php.net/manual/en/function.mysql-list-fields.php
 * @param database_name string <p>
 *       The name of the database that's being queried.
 *      </p>
 * @param table_name string <p>
 *       The name of the table that's being queried.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return resource A result pointer resource on success, or false on
 *   failure.
 *  </p>
 *  <p>
 *   The returned result can be used with mysql_field_flags,
 *   mysql_field_len,
 *   mysql_field_name&listendand;
 *   mysql_field_type.
 */
function mysql_list_fields ($database_name, $table_name, $link_identifier = null) {}

/**
 * List MySQL processes
 * @link http://www.php.net/manual/en/function.mysql-list-processes.php
 * @param link_identifier resource[optional] 
 * @return resource A result pointer resource on success&return.falseforfailure;.
 */
function mysql_list_processes ($link_identifier = null) {}

/**
 * Returns the text of the error message from previous MySQL operation
 * @link http://www.php.net/manual/en/function.mysql-error.php
 * @param link_identifier resource[optional] 
 * @return string the error text from the last MySQL function, or
 *   '' (empty string) if no error occurred.
 */
function mysql_error ($link_identifier = null) {}

/**
 * Returns the numerical value of the error message from previous MySQL operation
 * @link http://www.php.net/manual/en/function.mysql-errno.php
 * @param link_identifier resource[optional] 
 * @return int the error number from the last MySQL function, or
 *   0 (zero) if no error occurred.
 */
function mysql_errno ($link_identifier = null) {}

/**
 * Get number of affected rows in previous MySQL operation
 * @link http://www.php.net/manual/en/function.mysql-affected-rows.php
 * @param link_identifier resource[optional] 
 * @return int the number of affected rows on success, and -1 if the last query
 *   failed.
 *  </p>
 *  <p>
 *   If the last query was a DELETE query with no WHERE clause, all
 *   of the records will have been deleted from the table but this
 *   function will return zero with MySQL versions prior to 4.1.2.
 *  </p>
 *  <p>
 *   When using UPDATE, MySQL will not update columns where the new value is the 
 *   same as the old value.  This creates the possibility that 
 *   mysql_affected_rows may not actually equal the number 
 *   of rows matched, only the number of rows that were literally affected by 
 *   the query.
 *  </p>
 *  <p>
 *   The REPLACE statement first deletes the record with the same primary key 
 *   and then inserts the new record. This function returns the number of 
 *   deleted records plus the number of inserted records.
 */
function mysql_affected_rows ($link_identifier = null) {}

/**
 * Get the ID generated in the last query
 * @link http://www.php.net/manual/en/function.mysql-insert-id.php
 * @param link_identifier resource[optional] 
 * @return int The ID generated for an AUTO_INCREMENT column by the previous 
 *   query on success, 0 if the previous
 *   query does not generate an AUTO_INCREMENT value, or false if
 *   no MySQL connection was established.
 */
function mysql_insert_id ($link_identifier = null) {}

/**
 * Get result data
 * @link http://www.php.net/manual/en/function.mysql-result.php
 * @param result resource 
 * @param row int <p>
 *       The row number from the result that's being retrieved. Row numbers 
 *       start at 0.
 *      </p>
 * @param field mixed[optional] <p>
 *       The name or offset of the field being retrieved.
 *      </p>
 *      <p>
 *       It can be the field's offset, the field's name, or the field's table 
 *       dot field name (tablename.fieldname). If the column name has been
 *       aliased ('select foo as bar from...'), use the alias instead of the 
 *       column name. If undefined, the first field is retrieved.
 *      </p>
 * @return string The contents of one cell from a MySQL result set on success, or 
 *   false on failure.
 */
function mysql_result ($result, $row, $field = null) {}

/**
 * Get number of rows in result
 * @link http://www.php.net/manual/en/function.mysql-num-rows.php
 * @param result resource 
 * @return int The number of rows in a result set on success&return.falseforfailure;.
 */
function mysql_num_rows ($result) {}

/**
 * Get number of fields in result
 * @link http://www.php.net/manual/en/function.mysql-num-fields.php
 * @param result resource 
 * @return int the number of fields in the result set resource on
 *   success&return.falseforfailure;.
 */
function mysql_num_fields ($result) {}

/**
 * Get a result row as an enumerated array
 * @link http://www.php.net/manual/en/function.mysql-fetch-row.php
 * @param result resource 
 * @return array an numerical array of strings that corresponds to the fetched row, or 
 *   false if there are no more rows.
 *  </p>
 *  <p>
 *   mysql_fetch_row fetches one row of data from
 *   the result associated with the specified result identifier.  The
 *   row is returned as an array.  Each result column is stored in an
 *   array offset, starting at offset 0.
 */
function mysql_fetch_row ($result) {}

/**
 * Fetch a result row as an associative array, a numeric array, or both
 * @link http://www.php.net/manual/en/function.mysql-fetch-array.php
 * @param result resource 
 * @param result_type int[optional] <p>
 *       The type of array that is to be fetched. It's a constant and can
 *       take the following values: MYSQL_ASSOC, 
 *       MYSQL_NUM, and
 *       MYSQL_BOTH.
 *      </p>
 * @return array an array of strings that corresponds to the fetched row, or false
 *   if there are no more rows. The type of returned array depends on
 *   how result_type is defined. By using 
 *   MYSQL_BOTH (default), you'll get an array with both 
 *   associative and number indices. Using MYSQL_ASSOC, you 
 *   only get associative indices (as mysql_fetch_assoc 
 *   works), using MYSQL_NUM, you only get number indices 
 *   (as mysql_fetch_row works).
 *  </p>
 *  <p>
 *   If two or more columns of the result have the same field names,
 *   the last column will take precedence. To access the other column(s)
 *   of the same name, you must use the numeric index of the column or
 *   make an alias for the column. For aliased columns, you cannot
 *   access the contents with the original column name.
 */
function mysql_fetch_array ($result, $result_type = null) {}

/**
 * Fetch a result row as an associative array
 * @link http://www.php.net/manual/en/function.mysql-fetch-assoc.php
 * @param result resource 
 * @return array an associative array of strings that corresponds to the fetched row, or 
 *   false if there are no more rows.
 *  </p>
 *  <p>
 *   If two or more columns of the result have the same field names,
 *   the last column will take precedence. To access the other
 *   column(s) of the same name, you either need to access the
 *   result with numeric indices by using
 *   mysql_fetch_row or add alias names.
 *   See the example at the mysql_fetch_array
 *   description about aliases.
 */
function mysql_fetch_assoc ($result) {}

/**
 * Fetch a result row as an object
 * @link http://www.php.net/manual/en/function.mysql-fetch-object.php
 * @param result resource 
 * @param class_name string[optional] <p>
 *        The name of the class to instantiate, set the properties of and return.
 *        If not specified, a stdClass object is returned.
 *       </p>
 * @param params array[optional] <p>
 *        An optional array of parameters to pass to the constructor
 *        for class_name objects.
 *       </p>
 * @return object an object with string properties that correspond to the
 *   fetched row, or false if there are no more rows.
 */
function mysql_fetch_object ($result, $class_name = null, array $params = null) {}

/**
 * Move internal result pointer
 * @link http://www.php.net/manual/en/function.mysql-data-seek.php
 * @param result resource 
 * @param row_number int <p>
 *       The desired row number of the new result pointer.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function mysql_data_seek ($result, $row_number) {}

/**
 * Get the length of each output in a result
 * @link http://www.php.net/manual/en/function.mysql-fetch-lengths.php
 * @param result resource 
 * @return array An array of lengths on success&return.falseforfailure;.
 */
function mysql_fetch_lengths ($result) {}

/**
 * Get column information from a result and return as an object
 * @link http://www.php.net/manual/en/function.mysql-fetch-field.php
 * @param result resource 
 * @param field_offset int[optional] <p>
 *       The numerical field offset. If the field offset is not specified, the 
 *       next field that was not yet retrieved by this function is retrieved. 
 *       The field_offset starts at 0.
 *      </p>
 * @return object an object containing field information. The properties 
 *   of the object are:
 *  </p>
 *  <p>
 *   
 *    
 *     
 *      name - column name
 *     
 *    
 *    
 *     
 *      table - name of the table the column belongs to
 *     
 *    
 *    
 *     
 *      max_length - maximum length of the column
 *     
 *    
 *    
 *     
 *      not_null - 1 if the column cannot be &null;
 *     
 *    
 *    
 *     
 *      primary_key - 1 if the column is a primary key
 *     
 *    
 *    
 *     
 *      unique_key - 1 if the column is a unique key
 *     
 *    
 *    
 *     
 *      multiple_key - 1 if the column is a non-unique key
 *     
 *    
 *    
 *     
 *      numeric - 1 if the column is numeric
 *     
 *    
 *    
 *     
 *      blob - 1 if the column is a BLOB
 *     
 *    
 *    
 *     
 *      type - the type of the column
 *     
 *    
 *    
 *     
 *      unsigned - 1 if the column is unsigned
 *     
 *    
 *    
 *     
 *      zerofill - 1 if the column is zero-filled
 */
function mysql_fetch_field ($result, $field_offset = null) {}

/**
 * Set result pointer to a specified field offset
 * @link http://www.php.net/manual/en/function.mysql-field-seek.php
 * @param result resource 
 * @param field_offset int 
 * @return bool Returns true on success or false on failure.
 */
function mysql_field_seek ($result, $field_offset) {}

/**
 * Free result memory
 * @link http://www.php.net/manual/en/function.mysql-free-result.php
 * @param result resource 
 * @return bool Returns true on success or false on failure.
 *  </p>
 *  <p>
 *   If a non-resource is used for the result, an
 *   error of level E_WARNING will be emitted.  It's worth noting that
 *   mysql_query only returns a resource
 *   for SELECT, SHOW, EXPLAIN, and DESCRIBE queries.
 */
function mysql_free_result ($result) {}

/**
 * Get the name of the specified field in a result
 * @link http://www.php.net/manual/en/function.mysql-field-name.php
 * @param result resource 
 * @param field_offset int 
 * @return string The name of the specified field index on success&return.falseforfailure;.
 */
function mysql_field_name ($result, $field_offset) {}

/**
 * Get name of the table the specified field is in
 * @link http://www.php.net/manual/en/function.mysql-field-table.php
 * @param result resource 
 * @param field_offset int 
 * @return string The name of the table on success.
 */
function mysql_field_table ($result, $field_offset) {}

/**
 * Returns the length of the specified field
 * @link http://www.php.net/manual/en/function.mysql-field-len.php
 * @param result resource 
 * @param field_offset int 
 * @return int The length of the specified field index on success&return.falseforfailure;.
 */
function mysql_field_len ($result, $field_offset) {}

/**
 * Get the type of the specified field in a result
 * @link http://www.php.net/manual/en/function.mysql-field-type.php
 * @param result resource 
 * @param field_offset int 
 * @return string The returned field type
 *   will be one of "int", "real", 
 *   "string", "blob", and others as
 *   detailed in the MySQL
 *   documentation.
 */
function mysql_field_type ($result, $field_offset) {}

/**
 * Get the flags associated with the specified field in a result
 * @link http://www.php.net/manual/en/function.mysql-field-flags.php
 * @param result resource 
 * @param field_offset int 
 * @return string a string of flags associated with the result&return.falseforfailure;.
 *  </p>
 *  <p>
 *   The following flags are reported, if your version of MySQL
 *   is current enough to support them: "not_null", 
 *   "primary_key", "unique_key", 
 *   "multiple_key", "blob", 
 *   "unsigned", "zerofill",
 *   "binary", "enum", 
 *   "auto_increment" and "timestamp".
 */
function mysql_field_flags ($result, $field_offset) {}

/**
 * Escapes a string for use in a mysql_query
 * @link http://www.php.net/manual/en/function.mysql-escape-string.php
 * @param unescaped_string string <p>
 *       The string that is to be escaped.
 *      </p>
 * @return string the escaped string.
 */
function mysql_escape_string ($unescaped_string) {}

/**
 * Escapes special characters in a string for use in an SQL statement
 * @link http://www.php.net/manual/en/function.mysql-real-escape-string.php
 * @param unescaped_string string <p>
 *       The string that is to be escaped.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return string the escaped string, or false on error.
 */
function mysql_real_escape_string ($unescaped_string, $link_identifier = null) {}

/**
 * Get current system status
 * @link http://www.php.net/manual/en/function.mysql-stat.php
 * @param link_identifier resource[optional] 
 * @return string a string with the status for uptime, threads, queries, open tables,
 *   flush tables and queries per second. For a complete list of other status
 *   variables, you have to use the SHOW STATUS SQL command.
 *   If link_identifier is invalid, &null; is returned.
 */
function mysql_stat ($link_identifier = null) {}

/**
 * Return the current thread ID
 * @link http://www.php.net/manual/en/function.mysql-thread-id.php
 * @param link_identifier resource[optional] 
 * @return int The thread ID on success&return.falseforfailure;.
 */
function mysql_thread_id ($link_identifier = null) {}

/**
 * Returns the name of the character set
 * @link http://www.php.net/manual/en/function.mysql-client-encoding.php
 * @param link_identifier resource[optional] 
 * @return string the default character set name for the current connection.
 */
function mysql_client_encoding ($link_identifier = null) {}

/**
 * Ping a server connection or reconnect if there is no connection
 * @link http://www.php.net/manual/en/function.mysql-ping.php
 * @param link_identifier resource[optional] 
 * @return bool true if the connection to the server MySQL server is working, 
 *   otherwise false.
 */
function mysql_ping ($link_identifier = null) {}

/**
 * Get MySQL client info
 * @link http://www.php.net/manual/en/function.mysql-get-client-info.php
 * @return string The MySQL client version.
 */
function mysql_get_client_info () {}

/**
 * Get MySQL host info
 * @link http://www.php.net/manual/en/function.mysql-get-host-info.php
 * @param link_identifier resource[optional] 
 * @return string a string describing the type of MySQL connection in use for the 
 *   connection&return.falseforfailure;.
 */
function mysql_get_host_info ($link_identifier = null) {}

/**
 * Get MySQL protocol info
 * @link http://www.php.net/manual/en/function.mysql-get-proto-info.php
 * @param link_identifier resource[optional] 
 * @return int the MySQL protocol on success&return.falseforfailure;.
 */
function mysql_get_proto_info ($link_identifier = null) {}

/**
 * Get MySQL server info
 * @link http://www.php.net/manual/en/function.mysql-get-server-info.php
 * @param link_identifier resource[optional] 
 * @return string the MySQL server version on success&return.falseforfailure;.
 */
function mysql_get_server_info ($link_identifier = null) {}

/**
 * Get information about the most recent query
 * @link http://www.php.net/manual/en/function.mysql-info.php
 * @param link_identifier resource[optional] 
 * @return string information about the statement on success, or false on
 *   failure. See the example below for which statements provide information,
 *   and what the returned value may look like. Statements that are not listed 
 *   will return false.
 */
function mysql_info ($link_identifier = null) {}

/**
 * Sets the client character set
 * @link http://www.php.net/manual/en/function.mysql-set-charset.php
 * @param charset string <p>
 *       A valid character set name.
 *      </p>
 * @param link_identifier resource[optional] 
 * @return bool Returns true on success or false on failure.
 */
function mysql_set_charset ($charset, $link_identifier = null) {}

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
 * Retrieves database name from the call to <function>mysql_list_dbs</function>
 * @link http://www.php.net/manual/en/function.mysql-db-name.php
 * @param result resource <p>
 *       The result pointer from a call to mysql_list_dbs.
 *      </p>
 * @param row int <p>
 *       The index into the result set.
 *      </p>
 * @param field mixed[optional] <p>
 *       The field name.
 *      </p>
 * @return string the database name on success, and false on failure. If false
 *   is returned, use mysql_error to determine the nature
 *   of the error.
 */
function mysql_db_name ($result, $row, $field = null) {}

/**
 * @param result
 * @param row
 * @param field[optional]
 */
function mysql_dbname ($result, $row, $field) {}

/**
 * Get table name of field
 * @link http://www.php.net/manual/en/function.mysql-tablename.php
 * @param result resource <p>
 *       A result pointer resource that's returned from  
 *       mysql_list_tables.
 *      </p>
 * @param i int <p>
 *       The integer index (row/table number)
 *      </p>
 * @return string The name of the table on success&return.falseforfailure;.
 *  </p>
 *  <p>
 *   Use the mysql_tablename function to
 *   traverse this result pointer, or any function for result tables,
 *   such as mysql_fetch_array.
 */
function mysql_tablename ($result, $i) {}

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
