<?php

// Start of pgsql v.

/**
 * Open a PostgreSQL connection
 * @link http://www.php.net/manual/en/function.pg-connect.php
 * @param connection_string string <p>
 *       The connection_string can be empty to use all default parameters, or it 
 *       can contain one or more parameter settings separated by whitespace. 
 *       Each parameter setting is in the form keyword = value. Spaces around 
 *       the equal sign are optional. To write an empty value or a value 
 *       containing spaces, surround it with single quotes, e.g., keyword = 
 *       'a value'. Single quotes and backslashes within the value must be 
 *       escaped with a backslash, i.e., \' and \\.  
 *      </p>
 *      <p>
 *       The currently recognized parameter keywords are:
 *       host, hostaddr, port,
 *       dbname (defaults to value of user),
 *       user,
 *       password, connect_timeout,
 *       options, tty (ignored), sslmode,
 *       requiressl (deprecated in favor of sslmode), and
 *       service.  Which of these arguments exist depends
 *       on your PostgreSQL version.
 *      </p>
 *      <p>
 *       The options parameter can be used to set command line parameters 
 *       to be invoked by the server.
 *      </p>
 * @param connect_type int[optional] <p>
 *       If PGSQL_CONNECT_FORCE_NEW is passed, then a new connection
 *       is created, even if the connection_string is identical to
 *       an existing connection.
 *      </p>
 * @return resource PostgreSQL connection resource on success, false on failure.
 */
function pg_connect ($connection_string, $connect_type = null) {}

/**
 * Open a persistent PostgreSQL connection
 * @link http://www.php.net/manual/en/function.pg-pconnect.php
 * @param connection_string string <p>
 *       The connection_string can be empty to use all default parameters, or it 
 *       can contain one or more parameter settings separated by whitespace. 
 *       Each parameter setting is in the form keyword = value. Spaces around 
 *       the equal sign are optional. To write an empty value or a value 
 *       containing spaces, surround it with single quotes, e.g., keyword = 
 *       'a value'. Single quotes and backslashes within the value must be 
 *       escaped with a backslash, i.e., \' and \\.  
 *      </p>
 *      <p>
 *       The currently recognized parameter keywords are:
 *       host, hostaddr, port,
 *       dbname, user,
 *       password, connect_timeout,
 *       options, tty (ignored), sslmode,
 *       requiressl (deprecated in favor of sslmode), and
 *       service.  Which of these arguments exist depends
 *       on your PostgreSQL version.
 *      </p>
 * @param connect_type int[optional] <p>
 *       If PGSQL_CONNECT_FORCE_NEW is passed, then a new connection
 *       is created, even if the connection_string is identical to
 *       an existing connection.
 *      </p>
 * @return resource PostgreSQL connection resource on success, false on failure.
 */
function pg_pconnect ($connection_string, $connect_type = null) {}

/**
 * Closes a PostgreSQL connection
 * @link http://www.php.net/manual/en/function.pg-close.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_close ($connection = null) {}

/**
 * Get connection status
 * @link http://www.php.net/manual/en/function.pg-connection-status.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return int PGSQL_CONNECTION_OK or 
 *    PGSQL_CONNECTION_BAD.
 */
function pg_connection_status ($connection) {}

/**
 * Get connection is busy or not
 * @link http://www.php.net/manual/en/function.pg-connection-busy.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return bool true if the connection is busy, false otherwise.
 */
function pg_connection_busy ($connection) {}

/**
 * Reset connection (reconnect)
 * @link http://www.php.net/manual/en/function.pg-connection-reset.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_connection_reset ($connection) {}

/**
 * Returns the host name associated with the connection
 * @link http://www.php.net/manual/en/function.pg-host.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string A string containing the name of the host the 
 *   connection is to, or false on error.
 */
function pg_host ($connection = null) {}

/**
 * Get the database name
 * @link http://www.php.net/manual/en/function.pg-dbname.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string A string containing the name of the database the 
 *   connection is to, or false on error.
 */
function pg_dbname ($connection = null) {}

/**
 * Return the port number associated with the connection
 * @link http://www.php.net/manual/en/function.pg-port.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return int An int containing the port number of the database
 *   server the connection is to, 
 *   or false on error.
 */
function pg_port ($connection = null) {}

/**
 * Return the TTY name associated with the connection
 * @link http://www.php.net/manual/en/function.pg-tty.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string A string containing the debug TTY of 
 *   the connection, or false on error.
 */
function pg_tty ($connection = null) {}

/**
 * Get the options associated with the connection
 * @link http://www.php.net/manual/en/function.pg-options.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string A string containing the connection
 *   options, or false on error.
 */
function pg_options ($connection = null) {}

/**
 * Returns an array with client, protocol and server version (when available)
 * @link http://www.php.net/manual/en/function.pg-version.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return array an array with client, protocol 
 *   and server keys and values (if available).  Returns
 *   false on error or invalid connection.
 */
function pg_version ($connection = null) {}

/**
 * Ping database connection
 * @link http://www.php.net/manual/en/function.pg-ping.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_ping ($connection = null) {}

/**
 * Looks up a current parameter setting of the server.
 * @link http://www.php.net/manual/en/function.pg-parameter-status.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param param_name string <p>
 *         Possible param_name values include server_version, 
 *        server_encoding, client_encoding, 
 *        is_superuser, session_authorization, 
 *        DateStyle, TimeZone, and 
 *        integer_datetimes.
 *        </p>
 * @return string A string containing the value of the parameter, false on failure or invalid
 *  param_name.
 */
function pg_parameter_status ($connection = null, $param_name) {}

/**
 * Returns the current in-transaction status of the server.
 * @link http://www.php.net/manual/en/function.pg-transaction-status.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return int The status can be PGSQL_TRANSACTION_IDLE (currently idle),
 *    PGSQL_TRANSACTION_ACTIVE (a command is in progress),
 *    PGSQL_TRANSACTION_INTRANS (idle, in a valid transaction block),
 *    or PGSQL_TRANSACTION_INERROR (idle, in a failed transaction block).
 *    PGSQL_TRANSACTION_UNKNOWN is reported if the connection is bad.
 *    PGSQL_TRANSACTION_ACTIVE is reported only when a query
 *    has been sent to the server and not yet completed.
 */
function pg_transaction_status ($connection) {}

/**
 * Execute a query
 * @link http://www.php.net/manual/en/function.pg-query.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param query string <p>
 *       The SQL statement or statements to be executed. When multiple statements are passed to the function,
 *       they are automatically executed as one transaction, unless there are explicit BEGIN/COMMIT commands
 *       included in the query string. However, using multiple transactions in one function call is not recommended.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @return resource A query result resource on success&return.falseforfailure;.
 */
function pg_query ($connection = null, $query) {}

/**
 * Submits a command to the server and waits for the result, with the ability to pass parameters separately from the SQL command text.
 * @link http://www.php.net/manual/en/function.pg-query-params.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param query string <p>
 *       The parameterized SQL statement.  Must contain only a single statement.
 *       (multiple statements separated by semi-colons are not allowed.)  If any parameters 
 *       are used, they are referred to as $1, $2, etc.
 *      </p>
 * @param params array <p>
 *        An array of parameter values to substitute for the $1, $2, etc. placeholders
 *        in the original prepared query string.  The number of elements in the array
 *        must match the number of placeholders.
 *      </p>
 * @return resource A query result resource on success&return.falseforfailure;.
 */
function pg_query_params ($connection = null, $query, array $params) {}

/**
 * Submits a request to create a prepared statement with the 
  given parameters, and waits for completion.
 * @link http://www.php.net/manual/en/function.pg-prepare.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param stmtname string <p>
 *       The name to give the prepared statement.  Must be unique per-connection.  If
 *       "" is specified, then an unnamed statement is created, overwriting any
 *       previously defined unnamed statement.
 *      </p>
 * @param query string <p>
 *       The parameterized SQL statement.  Must contain only a single statement.
 *       (multiple statements separated by semi-colons are not allowed.)  If any parameters 
 *       are used, they are referred to as $1, $2, etc.
 *      </p>
 * @return resource A query result resource on success&return.falseforfailure;.
 */
function pg_prepare ($connection = null, $stmtname, $query) {}

/**
 * Sends a request to execute a prepared statement with given parameters, and waits for the result.
 * @link http://www.php.net/manual/en/function.pg-execute.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param stmtname string <p>
 *       The name of the prepared statement to execute.  if
 *       "" is specified, then the unnamed statement is executed.  The name must have
 *       been previously prepared using pg_prepare, 
 *       pg_send_prepare or a PREPARE SQL
 *       command.
 *      </p>
 * @param params array <p>
 *        An array of parameter values to substitute for the $1, $2, etc. placeholders
 *        in the original prepared query string.  The number of elements in the array
 *        must match the number of placeholders.
 *      </p>
 *      
 *       <p>
 *        Elements are converted to strings by calling this function.
 *       </p>
 * @return resource A query result resource on success&return.falseforfailure;.
 */
function pg_execute ($connection = null, $stmtname, array $params) {}

/**
 * Sends asynchronous query
 * @link http://www.php.net/manual/en/function.pg-send-query.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param query string <p>
 *       The SQL statement or statements to be executed.
 *      </p>
 *      <p>
 *       Data inside the query should be properly escaped.
 *      </p>
 * @return bool Returns true on success or false on failure.</p>
 *  <p>
 *   Use pg_get_result to determine the query result.
 */
function pg_send_query ($connection, $query) {}

/**
 * Submits a command and separate parameters to the server without waiting for the result(s).
 * @link http://www.php.net/manual/en/function.pg-send-query-params.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param query string <p>
 *       The parameterized SQL statement.  Must contain only a single statement.
 *       (multiple statements separated by semi-colons are not allowed.)  If any parameters 
 *       are used, they are referred to as $1, $2, etc.
 *      </p>
 * @param params array <p>
 *        An array of parameter values to substitute for the $1, $2, etc. placeholders
 *        in the original prepared query string.  The number of elements in the array
 *        must match the number of placeholders.
 *      </p>
 * @return bool Returns true on success or false on failure.</p>
 *  <p>
 *   Use pg_get_result to determine the query result.
 */
function pg_send_query_params ($connection, $query, array $params) {}

/**
 * Sends a request to create a prepared statement with the given parameters, without waiting for completion.
 * @link http://www.php.net/manual/en/function.pg-send-prepare.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param stmtname string <p>
 *       The name to give the prepared statement.  Must be unique per-connection.  If
 *       "" is specified, then an unnamed statement is created, overwriting any
 *       previously defined unnamed statement.
 *      </p>
 * @param query string <p>
 *       The parameterized SQL statement.  Must contain only a single statement.
 *       (multiple statements separated by semi-colons are not allowed.)  If any parameters 
 *       are used, they are referred to as $1, $2, etc.
 *      </p>
 * @return bool true on success, false on failure.  Use pg_get_result
 *  to determine the query result.
 */
function pg_send_prepare ($connection, $stmtname, $query) {}

/**
 * Sends a request to execute a prepared statement with given parameters, without waiting for the result(s).
 * @link http://www.php.net/manual/en/function.pg-send-execute.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param stmtname string <p>
 *       The name of the prepared statement to execute.  if
 *       "" is specified, then the unnamed statement is executed.  The name must have
 *       been previously prepared using pg_prepare, 
 *       pg_send_prepare or a PREPARE SQL
 *       command.
 *      </p>
 * @param params array <p>
 *        An array of parameter values to substitute for the $1, $2, etc. placeholders
 *        in the original prepared query string.  The number of elements in the array
 *        must match the number of placeholders.
 *      </p>
 * @return bool true on success, false on failure.  Use pg_get_result
 *  to determine the query result.
 */
function pg_send_execute ($connection, $stmtname, array $params) {}

/**
 * Cancel an asynchronous query
 * @link http://www.php.net/manual/en/function.pg-cancel-query.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_cancel_query ($connection) {}

/**
 * Returns values from a result resource
 * @link http://www.php.net/manual/en/function.pg-fetch-result.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int <p>
 *       Row number in result to fetch.  Rows are numbered from 0 upwards.  If omitted,
 *       next row is fetched.
 *      </p>
 * @param field mixed <p>       
 *       A string representing the name of the field (column) to fetch, otherwise
 *       an int representing the field number to fetch.  Fields are
 *       numbered from 0 upwards.
 *      </p>
 * @return string Boolean is returned as &quot;t&quot; or &quot;f&quot;.  All
 *   other types, including arrays are returned as strings formatted
 *   in the same default PostgreSQL manner that you would see in the
 *   psql program.  Database NULL
 *   values are returned as &null;.
 *  </p>
 *  <p>
 *   false is returned if row exceeds the number
 *   of rows in the set, or on any other error.
 */
function pg_fetch_result ($result, $row, $field) {}

/**
 * Get a row as an enumerated array
 * @link http://www.php.net/manual/en/function.pg-fetch-row.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int[optional] <p>
 *       Row number in result to fetch. Rows are numbered from 0 upwards. If
 *       omitted or &null;, the next row is fetched.
 *      </p>
 * @param result_type int[optional] 
 * @return array An array, indexed from 0 upwards, with each value
 *   represented as a string.  Database NULL
 *   values are returned as &null;.
 *  </p>
 *  <p>
 *   false is returned if row exceeds the number
 *   of rows in the set, there are no more rows, or on any other error.
 */
function pg_fetch_row ($result, $row = null, $result_type = null) {}

/**
 * Fetch a row as an associative array
 * @link http://www.php.net/manual/en/function.pg-fetch-assoc.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int[optional] <p>
 *       Row number in result to fetch. Rows are numbered from 0 upwards. If
 *       omitted or &null;, the next row is fetched.
 *      </p>
 * @return array An array indexed associatively (by field name).
 *   Each value in the array is represented as a 
 *   string.  Database NULL
 *   values are returned as &null;.
 *  </p>
 *  <p>
 *   false is returned if row exceeds the number
 *   of rows in the set, there are no more rows, or on any other error.
 */
function pg_fetch_assoc ($result, $row = null) {}

/**
 * Fetch a row as an array
 * @link http://www.php.net/manual/en/function.pg-fetch-array.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int[optional] <p>
 *       Row number in result to fetch. Rows are numbered from 0 upwards. If
 *       omitted or &null;, the next row is fetched.
 *      </p>
 * @param result_type int[optional] <p>
 *       An optional parameter that controls
 *       how the returned array is indexed.
 *       result_type is a constant and can take the
 *       following values: PGSQL_ASSOC, 
 *       PGSQL_NUM and PGSQL_BOTH.
 *       Using PGSQL_NUM, pg_fetch_array
 *       will return an array with numerical indices, using
 *       PGSQL_ASSOC it will return only associative indices
 *       while PGSQL_BOTH, the default, will return both
 *       numerical and associative indices.
 *      </p>
 * @return array An array indexed numerically (beginning with 0) or
 *   associatively (indexed by field name), or both.
 *   Each value in the array is represented as a 
 *   string.  Database NULL
 *   values are returned as &null;.
 *  </p>
 *  <p>
 *   false is returned if row exceeds the number
 *   of rows in the set, there are no more rows, or on any other error.
 */
function pg_fetch_array ($result, $row = null, $result_type = null) {}

/**
 * Fetch a row as an object
 * @link http://www.php.net/manual/en/function.pg-fetch-object.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int[optional] <p>
 *       Row number in result to fetch. Rows are numbered from 0 upwards. If
 *       omitted or &null;, the next row is fetched.
 *      </p>
 * @param result_type int[optional] <p>
 *        Ignored and deprecated.
 *       </p>
 * @return object An object with one attribute for each field
 *   name in the result.  Database NULL
 *   values are returned as &null;.
 *  </p>
 *  <p>
 *   false is returned if row exceeds the number
 *   of rows in the set, there are no more rows, or on any other error.
 */
function pg_fetch_object ($result, $row = null, $result_type = null) {}

/**
 * Fetches all rows from a result as an array
 * @link http://www.php.net/manual/en/function.pg-fetch-all.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return array An array with all rows in the result.  Each row is an array
 *   of field values indexed by field name.
 *  </p>
 *  <p>
 *   false is returned if there are no rows in the result, or on any
 *   other error.
 */
function pg_fetch_all ($result) {}

/**
 * Fetches all rows in a particular result column as an array
 * @link http://www.php.net/manual/en/function.pg-fetch-all-columns.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param column int[optional] <p>
 *       Column number, zero-based, to be retrieved from the result resource.  Defaults
 *       to the first column if not specified.
 *      </p>
 * @return array An array with all values in the result column.
 *  </p>
 *  <p>
 *   false is returned if column is larger than the number 
 *   of columns in the result, or on any other error.
 */
function pg_fetch_all_columns ($result, $column = null) {}

/**
 * Returns number of affected records (tuples)
 * @link http://www.php.net/manual/en/function.pg-affected-rows.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return int The number of rows affected by the query.  If no tuple is
 *   affected, it will return 0.
 */
function pg_affected_rows ($result) {}

/**
 * Get asynchronous query result
 * @link http://www.php.net/manual/en/function.pg-get-result.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return resource The result resource, or false if no more results are available.
 */
function pg_get_result ($connection = null) {}

/**
 * Set internal row offset in result resource
 * @link http://www.php.net/manual/en/function.pg-result-seek.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param offset int <p>
 *       Row to move the internal offset to in the result resource.
 *       Rows are numbered starting from zero.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_result_seek ($result, $offset) {}

/**
 * Get status of query result
 * @link http://www.php.net/manual/en/function.pg-result-status.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param type int[optional] <p>
 *       Either PGSQL_STATUS_LONG to return the numeric status 
 *       of the result, or PGSQL_STATUS_STRING 
 *       to return the command tag of the result.
 *       If not specified, PGSQL_STATUS_LONG is the default.
 *      </p>
 * @return mixed Possible return values are PGSQL_EMPTY_QUERY,
 *   PGSQL_COMMAND_OK, PGSQL_TUPLES_OK, PGSQL_COPY_OUT,
 *   PGSQL_COPY_IN, PGSQL_BAD_RESPONSE, PGSQL_NONFATAL_ERROR and
 *   PGSQL_FATAL_ERROR if PGSQL_STATUS_LONG is
 *   specified.  Otherwise, a string containing the PostgreSQL command tag is returned.
 */
function pg_result_status ($result, $type = null) {}

/**
 * Free result memory
 * @link http://www.php.net/manual/en/function.pg-free-result.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_free_result ($result) {}

/**
 * Returns the last row's OID
 * @link http://www.php.net/manual/en/function.pg-last-oid.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return string A string containing the OID assigned to the most recently inserted
 *   row in the specified connection, or false on error or
 *   no available OID.
 */
function pg_last_oid ($result) {}

/**
 * Returns the number of rows in a result
 * @link http://www.php.net/manual/en/function.pg-num-rows.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return int The number of rows in the result. On error, -1 is returned.
 */
function pg_num_rows ($result) {}

/**
 * Returns the number of fields in a result
 * @link http://www.php.net/manual/en/function.pg-num-fields.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return int The number of fields (columns) in the result. On error, -1 is returned.
 */
function pg_num_fields ($result) {}

/**
 * Returns the name of a field
 * @link http://www.php.net/manual/en/function.pg-field-name.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_number int <p>
 *        Field number, starting from 0.
 *       </p>
 * @return string The field name, or false on error.
 */
function pg_field_name ($result, $field_number) {}

/**
 * Returns the field number of the named field
 * @link http://www.php.net/manual/en/function.pg-field-num.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_name string <p>
 *        The name of the field.
 *       </p>
 * @return int The field number (numbered from 0), or -1 on error.
 */
function pg_field_num ($result, $field_name) {}

/**
 * Returns the internal storage size of the named field
 * @link http://www.php.net/manual/en/function.pg-field-size.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_number int <p>
 *        Field number, starting from 0.
 *       </p>
 * @return int The internal field storage size (in bytes).  -1 indicates a variable
 *   length field.  false is returned on error.
 */
function pg_field_size ($result, $field_number) {}

/**
 * Returns the type name for the corresponding field number
 * @link http://www.php.net/manual/en/function.pg-field-type.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_number int <p>
 *        Field number, starting from 0.
 *       </p>
 * @return string A string containing the base name of the field's type, or false
 *   on error.
 */
function pg_field_type ($result, $field_number) {}

/**
 * Returns the type ID (OID) for the corresponding field number
 * @link http://www.php.net/manual/en/function.pg-field-type-oid.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_number int <p>
 *        Field number, starting from 0.
 *       </p>
 * @return int The OID of the field's base type. false is returned on error.
 */
function pg_field_type_oid ($result, $field_number) {}

/**
 * Returns the printed length
 * @link http://www.php.net/manual/en/function.pg-field-prtlen.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row_number int 
 * @param field_name_or_number mixed 
 * @return int The field printed length, or false on error.
 */
function pg_field_prtlen ($result, $row_number, $field_name_or_number) {}

/**
 * Test if a field is SQL <literal>NULL</literal>
 * @link http://www.php.net/manual/en/function.pg-field-is-null.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param row int <p>
 *       Row number in result to fetch.  Rows are numbered from 0 upwards.  If omitted,
 *       current row is fetched.
 *      </p>
 * @param field mixed <p>
 *        Field number (starting from 0) as an integer or 
 *        the field name as a string.
 *       </p>
 * @return int 1 if the field in the given row is SQL NULL, 0
 *   if not.  false is returned if the row is out of range, or upon any other error.
 */
function pg_field_is_null ($result, $row, $field) {}

/**
 * Returns the name or oid of the tables field
 * @link http://www.php.net/manual/en/function.pg-field-table.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @param field_number int <p>
 *        Field number, starting from 0.
 *       </p>
 * @param oid_only bool[optional] <p>
 *        By default the tables name that field belongs to is returned but
 *        if oid_only is set to true, then the
 *        oid will instead be returned.
 *       </p>
 * @return mixed On success either the fields table name or oid. Or, false on failure.
 */
function pg_field_table ($result, $field_number, $oid_only = null) {}

/**
 * Gets SQL NOTIFY message
 * @link http://www.php.net/manual/en/function.pg-get-notify.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param result_type int[optional] <p>
 *       An optional parameter that controls
 *       how the returned array is indexed.
 *       result_type is a constant and can take the
 *       following values: PGSQL_ASSOC, 
 *       PGSQL_NUM and PGSQL_BOTH.
 *       Using PGSQL_NUM, pg_get_notify
 *       will return an array with numerical indices, using
 *       PGSQL_ASSOC it will return only associative indices
 *       while PGSQL_BOTH, the default, will return both
 *       numerical and associative indices.
 *      </p>
 * @return array An array containing the NOTIFY message name and backend PID.
 *   Otherwise if no NOTIFY is waiting, then false is returned.
 */
function pg_get_notify ($connection, $result_type = null) {}

/**
 * Gets the backend's process ID
 * @link http://www.php.net/manual/en/function.pg-get-pid.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return int The backend database process ID.
 */
function pg_get_pid ($connection) {}

/**
 * Get error message associated with result
 * @link http://www.php.net/manual/en/function.pg-result-error.php
 * @param result resource <p>
 *       PostgreSQL query result resource, returned by pg_query,
 *       pg_query_params or pg_execute
 *       (among others).
 *      </p>
 * @return string a string if there is an error associated with the
 *   result parameter, false otherwise.
 */
function pg_result_error ($result) {}

/**
 * Returns an individual field of an error report.
 * @link http://www.php.net/manual/en/function.pg-result-error-field.php
 * @param result resource <p>
 *       A PostgreSQL query result resource from a previously executed
 *       statement.
 *      </p>
 * @param fieldcode int <p>
 *         Possible fieldcode values are: PGSQL_DIAG_SEVERITY,
 *         PGSQL_DIAG_SQLSTATE, PGSQL_DIAG_MESSAGE_PRIMARY,
 *         PGSQL_DIAG_MESSAGE_DETAIL,
 *         PGSQL_DIAG_MESSAGE_HINT, PGSQL_DIAG_STATEMENT_POSITION,
 *         PGSQL_DIAG_INTERNAL_POSITION (PostgreSQL 8.0+ only),
 *         PGSQL_DIAG_INTERNAL_QUERY (PostgreSQL 8.0+ only),
 *         PGSQL_DIAG_CONTEXT, PGSQL_DIAG_SOURCE_FILE,
 *         PGSQL_DIAG_SOURCE_LINE or
 *         PGSQL_DIAG_SOURCE_FUNCTION.
 *        </p>
 * @return string A string containing the contents of the error field, &null; if the field does not exist or false
 *  on failure.
 */
function pg_result_error_field ($result, $fieldcode) {}

/**
 * Get the last error message string of a connection
 * @link http://www.php.net/manual/en/function.pg-last-error.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string A string containing the last error message on the 
 *   given connection, or false on error.
 */
function pg_last_error ($connection = null) {}

/**
 * Returns the last notice message from PostgreSQL server
 * @link http://www.php.net/manual/en/function.pg-last-notice.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @return string A string containing the last notice on the 
 *   given connection, or false on error.
 */
function pg_last_notice ($connection) {}

/**
 * Send a NULL-terminated string to PostgreSQL backend
 * @link http://www.php.net/manual/en/function.pg-put-line.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param data string <p>
 *       A line of text to be sent directly to the PostgreSQL backend.  A NULL
 *       terminator is added automatically.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_put_line ($connection = null, $data) {}

/**
 * Sync with PostgreSQL backend
 * @link http://www.php.net/manual/en/function.pg-end-copy.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_end_copy ($connection = null) {}

/**
 * Copy a table to an array
 * @link http://www.php.net/manual/en/function.pg-copy-to.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table from which to copy the data into rows.
 *      </p>
 * @param delimiter string[optional] <p>
 *       The token that separates values for each field in each element of
 *       rows.  Default is TAB.
 *      </p>
 * @param null_as string[optional] <p>
 *       How SQL NULL values are represented in the
 *       rows.  Default is \N ("\\N").
 *      </p>
 * @return array An array with one element for each line of COPY data.
 *    It returns false on failure.
 */
function pg_copy_to ($connection, $table_name, $delimiter = null, $null_as = null) {}

/**
 * Insert records into a table from an array
 * @link http://www.php.net/manual/en/function.pg-copy-from.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table into which to copy the rows.
 *      </p>
 * @param rows array <p>
 *       An array of data to be copied into table_name.
 *       Each value in rows becomes a row in table_name.
 *       Each value in rows should be a delimited string of the values
 *       to insert into each field.  Values should be linefeed terminated.
 *      </p>
 * @param delimiter string[optional] <p>
 *       The token that separates values for each field in each element of
 *       rows.  Default is TAB.
 *      </p>
 * @param null_as string[optional] <p>
 *       How SQL NULL values are represented in the
 *       rows.  Default is \N ("\\N").
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_copy_from ($connection, $table_name, array $rows, $delimiter = null, $null_as = null) {}

/**
 * Enable tracing a PostgreSQL connection
 * @link http://www.php.net/manual/en/function.pg-trace.php
 * @param pathname string <p>
 *       An optional file access mode, same as for fopen.
 *      </p>
 * @param mode string[optional] 
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_trace ($pathname, $mode = null, $connection = null) {}

/**
 * Disable tracing of a PostgreSQL connection
 * @link http://www.php.net/manual/en/function.pg-untrace.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return bool Always returns true.
 */
function pg_untrace ($connection = null) {}

/**
 * Create a large object
 * @link http://www.php.net/manual/en/function.pg-lo-create.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param object_id mixed[optional] <p>
 *       If an object_id is given the function
 *       will try to create a large object with this id, else a free
 *       object id is assigned by the server. The parameter
 *       was added in PHP 5.3 and relies on functionality that first
 *       appeared in PostgreSQL 8.1.
 *      </p>
 * @return int A large object OID or false on error.
 */
function pg_lo_create ($connection = null, $object_id = null) {}

/**
 * Delete a large object
 * @link http://www.php.net/manual/en/function.pg-lo-unlink.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param oid int <p>
 *       The OID of the large object in the database.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_lo_unlink ($connection, $oid) {}

/**
 * Open a large object
 * @link http://www.php.net/manual/en/function.pg-lo-open.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param oid int <p>
 *       The OID of the large object in the database.
 *      </p>
 * @param mode string <p>
 *       Can be either "r" for read-only, "w" for write only or "rw" for read and 
 *       write.
 *      </p>
 * @return resource A large object resource or false on error.
 */
function pg_lo_open ($connection, $oid, $mode) {}

/**
 * Close a large object
 * @link http://www.php.net/manual/en/function.pg-lo-close.php
 * @param large_object resource 
 * @return bool Returns true on success or false on failure.
 */
function pg_lo_close ($large_object) {}

/**
 * Read a large object
 * @link http://www.php.net/manual/en/function.pg-lo-read.php
 * @param large_object resource <p>
 *       PostgreSQL large object (LOB) resource, returned by pg_lo_open.
 *      </p>
 * @param len int[optional] <p>
 *       An optional maximum number of bytes to return.
 *      </p>
 * @return string A string containing len bytes from the
 *   large object, or false on error.
 */
function pg_lo_read ($large_object, $len = null) {}

/**
 * Write to a large object
 * @link http://www.php.net/manual/en/function.pg-lo-write.php
 * @param large_object resource <p>
 *       PostgreSQL large object (LOB) resource, returned by pg_lo_open.
 *      </p>
 * @param data string <p>
 *       The data to be written to the large object.  If len is
 *       specified and is less than the length of data, only
 *       len bytes will be written.
 *      </p>
 * @param len int[optional] <p>
 *       An optional maximum number of bytes to write.  Must be greater than zero
 *       and no greater than the length of data.  Defaults to
 *       the length of data.
 *      </p>
 * @return int The number of bytes written to the large object, or false on error.
 */
function pg_lo_write ($large_object, $data, $len = null) {}

/**
 * Reads an entire large object and send straight to browser
 * @link http://www.php.net/manual/en/function.pg-lo-read-all.php
 * @param large_object resource <p>
 *       PostgreSQL large object (LOB) resource, returned by pg_lo_open.
 *      </p>
 * @return int Number of bytes read or false on error.
 */
function pg_lo_read_all ($large_object) {}

/**
 * Import a large object from file
 * @link http://www.php.net/manual/en/function.pg-lo-import.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param pathname string <p>
 *       The full path and file name of the file on the client
 *       filesystem from which to read the large object data.
 *      </p>
 * @param object_id mixed[optional] <p>
 *       If an object_id is given the function
 *       will try to create a large object with this id, else a free
 *       object id is assigned by the server. The parameter
 *       was added in PHP 5.3 and relies on functionality that first
 *       appeared in PostgreSQL 8.1.
 *      </p>
 * @return int The OID of the newly created large object, or
 *   false on failure.
 */
function pg_lo_import ($connection = null, $pathname, $object_id = null) {}

/**
 * Export a large object to file
 * @link http://www.php.net/manual/en/function.pg-lo-export.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param oid int <p>
 *       The OID of the large object in the database.
 *      </p>
 * @param pathname string <p>
 *       The full path and file name of the file in which to write the
 *       large object on the client filesystem.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_lo_export ($connection = null, $oid, $pathname) {}

/**
 * Seeks position within a large object
 * @link http://www.php.net/manual/en/function.pg-lo-seek.php
 * @param large_object resource <p>
 *       PostgreSQL large object (LOB) resource, returned by pg_lo_open.
 *      </p>
 * @param offset int <p>
 *       The number of bytes to seek.
 *      </p>
 * @param whence int[optional] <p>
 *       One of the constants PGSQL_SEEK_SET (seek from object start), 
 *       PGSQL_SEEK_CUR (seek from current position)
 *       or PGSQL_SEEK_END (seek from object end) .
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function pg_lo_seek ($large_object, $offset, $whence = null) {}

/**
 * Returns current seek position a of large object
 * @link http://www.php.net/manual/en/function.pg-lo-tell.php
 * @param large_object resource <p>
 *       PostgreSQL large object (LOB) resource, returned by pg_lo_open.
 *      </p>
 * @return int The current seek offset (in number of bytes) from the beginning of the large
 *   object.  If there is an error, the return value is negative.
 */
function pg_lo_tell ($large_object) {}

/**
 * Escape a string for insertion into a text field
 * @link http://www.php.net/manual/en/function.pg-escape-string.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param data string <p>
 *       A string containing text to be escaped.
 *      </p>
 * @return string A string containing the escaped data.
 */
function pg_escape_string ($connection = null, $data) {}

/**
 * Escape a string for insertion into a bytea field
 * @link http://www.php.net/manual/en/function.pg-escape-bytea.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param data string <p>
 *       A string containing text or binary data to be inserted into a bytea
 *       column.
 *      </p>
 * @return string A string containing the escaped data.
 */
function pg_escape_bytea ($connection = null, $data) {}

/**
 * Unescape binary for bytea type
 * @link http://www.php.net/manual/en/function.pg-unescape-bytea.php
 * @param data string <p>
 *       A string containing PostgreSQL bytea data to be converted into
 *       a PHP binary string.
 *      </p>
 * @return string A string containing the unescaped data.
 */
function pg_unescape_bytea ($data) {}

/**
 * Determines the verbosity of messages returned by <function>pg_last_error</function> 
   and <function>pg_result_error</function>.
 * @link http://www.php.net/manual/en/function.pg-set-error-verbosity.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param verbosity int <p>
 *       The required verbosity: PGSQL_ERRORS_TERSE,
 *       PGSQL_ERRORS_DEFAULT
 *       or PGSQL_ERRORS_VERBOSE.
 *      </p>
 * @return int The previous verbosity level: PGSQL_ERRORS_TERSE,
 *   PGSQL_ERRORS_DEFAULT
 *   or PGSQL_ERRORS_VERBOSE.
 */
function pg_set_error_verbosity ($connection = null, $verbosity) {}

/**
 * Gets the client encoding
 * @link http://www.php.net/manual/en/function.pg-client-encoding.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @return string The client encoding, or false on error.
 */
function pg_client_encoding ($connection = null) {}

/**
 * Set the client encoding
 * @link http://www.php.net/manual/en/function.pg-set-client-encoding.php
 * @param connection resource[optional] <p>
 *       PostgreSQL database connection resource.  When 
 *       connection is not present, the default connection 
 *       is used. The default connection is the last connection made by 
 *       pg_connect or pg_pconnect.
 *      </p>
 * @param encoding string <p>
 *       The required client encoding.  One of SQL_ASCII, EUC_JP, 
 *       EUC_CN, EUC_KR, EUC_TW, 
 *       UNICODE, MULE_INTERNAL, LATINX (X=1...9), 
 *       KOI8, WIN, ALT, SJIS, 
 *       BIG5 or WIN1250.
 *      </p>
 *      <p>
 *       The exact list of available encodings depends on your PostgreSQL version, so check your
 *       PostgreSQL manual for a more specific list.
 *      </p>
 * @return int 0 on success or -1 on error.
 */
function pg_set_client_encoding ($connection = null, $encoding) {}

/**
 * Get meta data for table
 * @link http://www.php.net/manual/en/function.pg-meta-data.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       The name of the table.
 *      </p>
 * @return array An array of the table definition, or false on error.
 */
function pg_meta_data ($connection, $table_name) {}

/**
 * Convert associative array values into suitable for SQL statement
 * @link http://www.php.net/manual/en/function.pg-convert.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table against which to convert types.
 *      </p>
 * @param assoc_array array <p>
 *       Data to be converted.
 *      </p>
 * @param options int[optional] <p>
 *       Any number of PGSQL_CONV_IGNORE_DEFAULT, 
 *       PGSQL_CONV_FORCE_NULL or
 *       PGSQL_CONV_IGNORE_NOT_NULL, combined.
 *      </p>
 * @return array An array of converted values, or false on error.
 */
function pg_convert ($connection, $table_name, array $assoc_array, $options = null) {}

/**
 * Insert array into table
 * @link http://www.php.net/manual/en/function.pg-insert.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table into which to insert rows.  The table table_name must at least 
 *       have as many columns as assoc_array has elements.
 *      </p>
 * @param assoc_array array <p>
 *       An array whose keys are field names in the table table_name,
 *       and whose values are the values of those fields that are to be inserted.
 *      </p>
 * @param options int[optional] <p>
 *       Any number of PGSQL_CONV_OPTS, 
 *       PGSQL_DML_NO_CONV,
 *       PGSQL_DML_EXEC,
 *       PGSQL_DML_ASYNC or
 *       PGSQL_DML_STRING combined. If PGSQL_DML_STRING is part of the
 *       options then query string is returned.
 *      </p>
 * @return mixed Returns true on success or false on failure.  Returns string if PGSQL_DML_STRING is passed
 *   via options.
 */
function pg_insert ($connection, $table_name, array $assoc_array, $options = null) {}

/**
 * Update table
 * @link http://www.php.net/manual/en/function.pg-update.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table into which to update rows.
 *      </p>
 * @param data array <p>
 *       An array whose keys are field names in the table table_name,
 *       and whose values are what matched rows are to be updated to.
 *      </p>
 * @param condition array <p>
 *       An array whose keys are field names in the table table_name,
 *       and whose values are the conditions that a row must meet to be updated.
 *      </p>
 * @param options int[optional] <p>
 *       Any number of PGSQL_CONV_OPTS, 
 *       PGSQL_DML_NO_CONV,
 *       PGSQL_DML_EXEC or
 *       PGSQL_DML_STRING combined. If PGSQL_DML_STRING is part of the
 *       options then query string is returned.
 *      </p>
 * @return mixed Returns true on success or false on failure.  Returns string if PGSQL_DML_STRING is passed
 *   via options.
 */
function pg_update ($connection, $table_name, array $data, array $condition, $options = null) {}

/**
 * Deletes records
 * @link http://www.php.net/manual/en/function.pg-delete.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table from which to delete rows.
 *      </p>
 * @param assoc_array array <p>
 *       An array whose keys are field names in the table table_name,
 *       and whose values are the values of those fields that are to be deleted.
 *      </p>
 * @param options int[optional] <p>
 *       Any number of PGSQL_CONV_FORCE_NULL, 
 *       PGSQL_DML_NO_CONV,
 *       PGSQL_DML_EXEC or
 *       PGSQL_DML_STRING combined. If PGSQL_DML_STRING is part of the
 *       options then query string is returned.
 *      </p>
 * @return mixed Returns true on success or false on failure.  Returns string if PGSQL_DML_STRING is passed
 *   via options.
 */
function pg_delete ($connection, $table_name, array $assoc_array, $options = null) {}

/**
 * Select records
 * @link http://www.php.net/manual/en/function.pg-select.php
 * @param connection resource <p>
 *       PostgreSQL database connection resource.
 *      </p>
 * @param table_name string <p>
 *       Name of the table from which to select rows.
 *      </p>
 * @param assoc_array array <p>
 *       An array whose keys are field names in the table table_name,
 *       and whose values are the conditions that a row must meet to be retrieved.
 *      </p>
 * @param options int[optional] <p>
 *       Any number of PGSQL_CONV_FORCE_NULL, 
 *       PGSQL_DML_NO_CONV,
 *       PGSQL_DML_EXEC,
 *       PGSQL_DML_ASYNC or
 *       PGSQL_DML_STRING combined. If PGSQL_DML_STRING is part of the
 *       options then query string is returned.
 *      </p>
 * @return mixed Returns true on success or false on failure.  Returns string if PGSQL_DML_STRING is passed
 *   via options.
 */
function pg_select ($connection, $table_name, array $assoc_array, $options = null) {}

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
 * Passed to pg_connect to force the creation of a new connection,
 *      rather than re-using an existing identical connection.
 * @link http://www.php.net/manual/en/pgsql.constants.php
 */
define ('PGSQL_CONNECT_FORCE_NEW', 2);

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
define ('PGSQL_DML_NO_CONV', 256);
define ('PGSQL_DML_EXEC', 512);
define ('PGSQL_DML_ASYNC', 1024);
define ('PGSQL_DML_STRING', 2048);

// End of pgsql v.
?>
