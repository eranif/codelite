<?php

// Start of iconv v.

/**
 * @param in_charset
 * @param out_charset
 * @param str
 */
function iconv ($in_charset, $out_charset, $str) {}

/**
 * @param type[optional]
 */
function iconv_get_encoding ($type) {}

/**
 * @param type
 * @param charset
 */
function iconv_set_encoding ($type, $charset) {}

/**
 * @param str
 * @param charset[optional]
 */
function iconv_strlen ($str, $charset) {}

/**
 * @param str
 * @param offset
 * @param length[optional]
 * @param charset[optional]
 */
function iconv_substr ($str, $offset, $length, $charset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param charset[optional]
 */
function iconv_strpos ($haystack, $needle, $offset, $charset) {}

/**
 * @param haystack
 * @param needle
 * @param charset[optional]
 */
function iconv_strrpos ($haystack, $needle, $charset) {}

/**
 * @param field_name
 * @param field_value
 * @param preference[optional]
 */
function iconv_mime_encode ($field_name, $field_value, $preference) {}

/**
 * @param encoded_string
 * @param mode[optional]
 * @param charset[optional]
 */
function iconv_mime_decode ($encoded_string, $mode, $charset) {}

/**
 * @param headers
 * @param mode[optional]
 * @param charset[optional]
 */
function iconv_mime_decode_headers ($headers, $mode, $charset) {}


/**
 * string
 * @link http://www.php.net/manual/en/iconv.constants.php
 */
define ('ICONV_IMPL', "glibc");

/**
 * string
 * @link http://www.php.net/manual/en/iconv.constants.php
 */
define ('ICONV_VERSION', 2.19);

/**
 * integer
 * @link http://www.php.net/manual/en/iconv.constants.php
 */
define ('ICONV_MIME_DECODE_STRICT', 1);

/**
 * integer
 * @link http://www.php.net/manual/en/iconv.constants.php
 */
define ('ICONV_MIME_DECODE_CONTINUE_ON_ERROR', 2);

// End of iconv v.
?>
