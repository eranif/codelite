<?php

// Start of filter v.0.11.0

/**
 * @param type
 * @param variable_name
 * @param filter[optional]
 * @param options[optional]
 */
function filter_input ($type, $variable_name, $filter, $options) {}

/**
 * @param variable
 * @param filter[optional]
 * @param options[optional]
 */
function filter_var ($variable, $filter, $options) {}

/**
 * @param type
 * @param definition[optional]
 * @param add_empty[optional]
 */
function filter_input_array ($type, $definition, $add_empty) {}

/**
 * @param data
 * @param definition[optional]
 * @param add_empty[optional]
 */
function filter_var_array ($data, $definition, $add_empty) {}

function filter_list () {}

/**
 * @param type
 * @param variable_name
 */
function filter_has_var ($type, $variable_name) {}

/**
 * @param filtername
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
define ('FILTER_VALIDATE_MAC', 276);

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
define ('FILTER_SANITIZE_FULL_SPECIAL_CHARS', 522);

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
