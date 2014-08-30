<?php

// Start of filter v.0.11.0

/**
 * Gets a specific external variable by name and optionally filters it
 * @link http://www.php.net/manual/en/function.filter-input.php
 * @param type int <p>
 *       One of INPUT_GET, INPUT_POST,
 *       INPUT_COOKIE, INPUT_SERVER, or
 *       INPUT_ENV.
 *      </p>
 * @param variable_name string <p>
 *       Name of a variable to get.
 *      </p>
 * @param filter int[optional] <p>
 *       The ID of the filter to apply. The 
 *       manual page lists the available filters.
 *      </p>
 * @param options mixed[optional] <p>
 *       Associative array of options or bitwise disjunction of flags. If filter
 *       accepts options, flags can be provided in "flags" field of array.
 *      </p>
 * @return mixed Value of the requested variable on success, false if the filter fails,
 *   or &null; if the variable_name variable is not set.
 *   If the flag FILTER_NULL_ON_FAILURE is used, it
 *   returns false if the variable is not set and &null; if the filter fails.
 */
function filter_input ($type, $variable_name, $filter = null, $options = null) {}

/**
 * Filters a variable with a specified filter
 * @link http://www.php.net/manual/en/function.filter-var.php
 * @param variable mixed <p>
 *       Value to filter.
 *      </p>
 * @param filter int[optional] <p>
 *       The ID of the filter to apply. The 
 *       manual page lists the available filters.
 *      </p>
 * @param options mixed[optional] <p>
 *       Associative array of options or bitwise disjunction of flags. If filter
 *       accepts options, flags can be provided in "flags" field of array. For
 *       the "callback" filter, callback type should be passed.  The
 *       callback must accept one argument, the value to be filtered, and return
 *       the value after filtering/sanitizing it.
 *      </p>
 *      <p>
 *       
 *         array(
 *        'default' => 3, // value to return if the filter fails
 *        // other options here
 *        'min_range' => 0
 *    ),
 *    'flags' => FILTER_FLAG_ALLOW_OCTAL,
 * );
 * $var = filter_var('0755', FILTER_VALIDATE_INT, $options);
 * // for filter that only accept flags, you can pass them directly
 * $var = filter_var('oops', FILTER_VALIDATE_BOOLEAN, FILTER_NULL_ON_FAILURE);
 * // for filter that only accept flags, you can also pass as an array
 * $var = filter_var('oops', FILTER_VALIDATE_BOOLEAN,
 *                  array('flags' => FILTER_NULL_ON_FAILURE));
 * // callback filter
 * function foo($value)
 * {
 *    $ret = new stdClass;
 *    $ret->value = filter_var($value, FILTER_VALIDATE_BOOLEAN,
 *                             array('flags' => FILTER_NULL_ON_FAILURE));
 *    return $ret;
 * }
 * $var = filter_var('yes', FILTER_CALLBACK, array('options' => 'foo'));
 * ?>
 *        ]]>
 *       
 *      </p>
 * @return mixed the filtered data, or false if the filter fails.
 */
function filter_var ($variable, $filter = null, $options = null) {}

/**
 * Gets external variables and optionally filters them
 * @link http://www.php.net/manual/en/function.filter-input-array.php
 * @param type int <p>
 *       One of INPUT_GET, INPUT_POST,
 *       INPUT_COOKIE, INPUT_SERVER, or
 *       INPUT_ENV.
 *      </p>
 * @param definition mixed[optional] <p>
 *       An array defining the arguments. A valid key is a string
 *       containing a variable name and a valid value is either a filter type, or an array
 *       optionally specifying the filter, flags and options. If the value is an
 *       array, valid keys are filter which specifies the
 *       filter type,
 *       flags which specifies any flags that apply to the
 *       filter, and options which specifies any options that
 *       apply to the filter. See the example below for a better understanding.
 *      </p>
 *      <p>
 *       This parameter can be also an integer holding a filter constant. Then all values in the
 *       input array are filtered by this filter.
 *      </p>
 * @return mixed An array containing the values of the requested variables on success, or false 
 *   on failure. An array value will be false if the filter fails, or &null; if 
 *   the variable is not set. Or if the flag FILTER_NULL_ON_FAILURE
 *   is used, it returns false if the variable is not set and &null; if the filter 
 *   fails.
 */
function filter_input_array ($type, $definition = null) {}

/**
 * Gets multiple variables and optionally filters them
 * @link http://www.php.net/manual/en/function.filter-var-array.php
 * @param data array <p>
 *       An array with string keys containing the data to filter.
 *      </p>
 * @param definition mixed[optional] <p>
 *       An array defining the arguments. A valid key is a string
 *       containing a variable name and a valid value is either a
 *       filter type, or an
 *       array optionally specifying the filter, flags and options.
 *       If the value is an array, valid keys are filter
 *       which specifies the filter type,
 *       flags which specifies any flags that apply to the
 *       filter, and options which specifies any options that
 *       apply to the filter. See the example below for a better understanding.
 *      </p>
 *      <p>
 *       This parameter can be also an integer holding a filter constant. Then all values in the
 *       input array are filtered by this filter.
 *      </p>
 * @return mixed An array containing the values of the requested variables on success, or false 
 *   on failure. An array value will be false if the filter fails, or &null; if 
 *   the variable is not set.
 */
function filter_var_array (array $data, $definition = null) {}

/**
 * Returns a list of all supported filters
 * @link http://www.php.net/manual/en/function.filter-list.php
 * @return array an array of names of all supported filters, empty array if there
 *   are no such filters. Indexes of this array are not filter IDs, they can be
 *   obtained with filter_id from a name instead.
 */
function filter_list () {}

/**
 * Checks if variable of specified type exists
 * @link http://www.php.net/manual/en/function.filter-has-var.php
 * @param type int <p>
 *       One of INPUT_GET, INPUT_POST,
 *       INPUT_COOKIE, INPUT_SERVER, or
 *       INPUT_ENV.
 *      </p>
 * @param variable_name string <p>
 *       Name of a variable to check.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function filter_has_var ($type, $variable_name) {}

/**
 * Returns the filter ID belonging to a named filter
 * @link http://www.php.net/manual/en/function.filter-id.php
 * @param filtername string <p>
 *       Name of a filter to get.
 *      </p>
 * @return int ID of a filter on success or false if filter doesn't exist.
 */
function filter_id ($filtername) {}


/**
 * POST variables.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_POST', 0);

/**
 * GET variables.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_GET', 1);

/**
 * COOKIE variables.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_COOKIE', 2);

/**
 * ENV variables.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_ENV', 4);

/**
 * SERVER variables.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_SERVER', 5);

/**
 * SESSION variables.
 *    (not implemented yet)
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_SESSION', 6);

/**
 * REQUEST variables.
 *    (not implemented yet)
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('INPUT_REQUEST', 99);

/**
 * No flags.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_NONE', 0);

/**
 * Flag used to require scalar as input
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_REQUIRE_SCALAR', 33554432);

/**
 * Require an array as input.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_REQUIRE_ARRAY', 16777216);

/**
 * Always returns an array.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FORCE_ARRAY', 67108864);

/**
 * Use NULL instead of FALSE on failure.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_NULL_ON_FAILURE', 134217728);

/**
 * ID of "int" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_INT', 257);

/**
 * ID of "boolean" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_BOOLEAN', 258);

/**
 * ID of "float" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_FLOAT', 259);

/**
 * ID of "validate_regexp" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_REGEXP', 272);

/**
 * ID of "validate_url" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_URL', 273);

/**
 * ID of "validate_email" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_EMAIL', 274);

/**
 * ID of "validate_ip" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_VALIDATE_IP', 275);

/**
 * ID of default ("string") filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_DEFAULT', 516);

/**
 * ID of "unsafe_raw" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_UNSAFE_RAW', 516);

/**
 * ID of "string" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_STRING', 513);

/**
 * ID of "stripped" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_STRIPPED', 513);

/**
 * ID of "encoded" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_ENCODED', 514);

/**
 * ID of "special_chars" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_SPECIAL_CHARS', 515);
define ('FILTER_SANITIZE_FULL_SPECIAL_CHARS', 515);

/**
 * ID of "email" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_EMAIL', 517);

/**
 * ID of "url" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_URL', 518);

/**
 * ID of "number_int" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_NUMBER_INT', 519);

/**
 * ID of "number_float" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_NUMBER_FLOAT', 520);

/**
 * ID of "magic_quotes" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_SANITIZE_MAGIC_QUOTES', 521);

/**
 * ID of "callback" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_CALLBACK', 1024);

/**
 * Allow octal notation (0[0-7]+) in "int" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ALLOW_OCTAL', 1);

/**
 * Allow hex notation (0x[0-9a-fA-F]+) in "int" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ALLOW_HEX', 2);

/**
 * Strip characters with ASCII value less than 32.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_STRIP_LOW', 4);

/**
 * Strip characters with ASCII value greater than 127.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_STRIP_HIGH', 8);
define ('FILTER_FLAG_STRIP_BACKTICK', 512);

/**
 * Encode characters with ASCII value less than 32.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ENCODE_LOW', 16);

/**
 * Encode characters with ASCII value greater than 127.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ENCODE_HIGH', 32);

/**
 * Encode &.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ENCODE_AMP', 64);

/**
 * Don't encode ' and ".
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_NO_ENCODE_QUOTES', 128);

/**
 * (No use for now.)
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_EMPTY_STRING_NULL', 256);

/**
 * Allow fractional part in "number_float" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ALLOW_FRACTION', 4096);

/**
 * Allow thousand separator (,) in "number_float" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ALLOW_THOUSAND', 8192);

/**
 * Allow scientific notation (e, E) in
 *     "number_float" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_ALLOW_SCIENTIFIC', 16384);
define ('FILTER_FLAG_SCHEME_REQUIRED', 65536);
define ('FILTER_FLAG_HOST_REQUIRED', 131072);

/**
 * Require path in "validate_url" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_PATH_REQUIRED', 262144);

/**
 * Require query in "validate_url" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_QUERY_REQUIRED', 524288);

/**
 * Allow only IPv4 address in "validate_ip" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_IPV4', 1048576);

/**
 * Allow only IPv6 address in "validate_ip" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_IPV6', 2097152);

/**
 * Deny reserved addresses in "validate_ip" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_NO_RES_RANGE', 4194304);

/**
 * Deny private addresses in "validate_ip" filter.
 * @link http://www.php.net/manual/en/filter.constants.php
 */
define ('FILTER_FLAG_NO_PRIV_RANGE', 8388608);

// End of filter v.0.11.0
?>
