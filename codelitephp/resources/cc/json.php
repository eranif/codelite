<?php

// Start of json v.1.2.1

interface JsonSerializable  {

	abstract public function jsonSerialize () {}

}

/**
 * @param value
 * @param options[optional]
 * @param depth[optional]
 */
function json_encode ($value, $options, $depth) {}

/**
 * @param json
 * @param assoc[optional]
 * @param depth[optional]
 * @param options[optional]
 */
function json_decode ($json, $assoc, $depth, $options) {}

function json_last_error () {}

function json_last_error_msg () {}


/**
 * All < and > are converted to \u003C and \u003E.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_HEX_TAG', 1);

/**
 * All &s are converted to \u0026.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_HEX_AMP', 2);

/**
 * All ' are converted to \u0027.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_HEX_APOS', 4);

/**
 * All " are converted to \u0022.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_HEX_QUOT', 8);

/**
 * Outputs an object rather than an array when a non-associative array is
 *     used. Especially useful when the recipient of the output is expecting
 *     an object and the array is empty.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_FORCE_OBJECT', 16);

/**
 * Encodes numeric strings as numbers.
 *     Available since PHP 5.3.3.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_NUMERIC_CHECK', 32);

/**
 * Don't escape /.
 *     Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_UNESCAPED_SLASHES', 64);

/**
 * Use whitespace in returned data to format it.
 *     Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_PRETTY_PRINT', 128);

/**
 * Encode multibyte Unicode characters literally (default is to escape as \uXXXX).
 *     Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_UNESCAPED_UNICODE', 256);
define ('JSON_PARTIAL_OUTPUT_ON_ERROR', 512);

/**
 * No error has occurred.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_NONE', 0);

/**
 * The maximum stack depth has been exceeded.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_DEPTH', 1);

/**
 * Occurs with underflow or with the modes mismatch.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_STATE_MISMATCH', 2);

/**
 * Control character error, possibly incorrectly encoded.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_CTRL_CHAR', 3);

/**
 * Syntax error.
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_SYNTAX', 4);

/**
 * Malformed UTF-8 characters, possibly incorrectly encoded. This 
 *     constant is available as of PHP 5.3.3.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_UTF8', 5);

/**
 * <p>
 *     The object or array passed to json_encode include
 *     recursive references and cannot be encoded.
 *     If the JSON_PARTIAL_OUTPUT_ON_ERROR option was
 *     given, &null; will be encoded in the place of the recursive reference.
 *    </p>
 *    <p>
 *     This constant is available as of PHP 5.5.0.
 *    </p>
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_RECURSION', 6);

/**
 * <p>
 *     The value passed to json_encode includes either
 *     NAN
 *     or INF.
 *     If the JSON_PARTIAL_OUTPUT_ON_ERROR option was
 *     given, 0 will be encoded in the place of these
 *     special numbers.
 *    </p>
 *    <p>
 *     This constant is available as of PHP 5.5.0.
 *    </p>
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_INF_OR_NAN', 7);

/**
 * <p>
 *     A value of an unsupported type was given to
 *     json_encode, such as a resource.
 *     If the JSON_PARTIAL_OUTPUT_ON_ERROR option was
 *     given, &null; will be encoded in the place of the unsupported value.
 *    </p>
 *    <p>
 *     This constant is available as of PHP 5.5.0.
 *    </p>
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_ERROR_UNSUPPORTED_TYPE', 8);
define ('JSON_OBJECT_AS_ARRAY', 1);

/**
 * Encodes large integers as their original string value.
 *     Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/json.constants.php
 */
define ('JSON_BIGINT_AS_STRING', 2);

// End of json v.1.2.1
?>
