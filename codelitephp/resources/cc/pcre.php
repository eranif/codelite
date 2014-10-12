<?php

// Start of pcre v.

/**
 * @param pattern
 * @param subject
 * @param subpatterns[optional]
 * @param flags[optional]
 * @param offset[optional]
 */
function preg_match ($pattern, $subject, &$subpatterns, $flags, $offset) {}

/**
 * @param pattern
 * @param subject
 * @param subpatterns[optional]
 * @param flags[optional]
 * @param offset[optional]
 */
function preg_match_all ($pattern, $subject, &$subpatterns, $flags, $offset) {}

/**
 * @param regex
 * @param replace
 * @param subject
 * @param limit[optional]
 * @param count[optional]
 */
function preg_replace ($regex, $replace, $subject, $limit, &$count) {}

/**
 * @param regex
 * @param callback
 * @param subject
 * @param limit[optional]
 * @param count[optional]
 */
function preg_replace_callback ($regex, $callback, $subject, $limit, &$count) {}

/**
 * @param regex
 * @param replace
 * @param subject
 * @param limit[optional]
 * @param count[optional]
 */
function preg_filter ($regex, $replace, $subject, $limit, &$count) {}

/**
 * @param pattern
 * @param subject
 * @param limit[optional]
 * @param flags[optional]
 */
function preg_split ($pattern, $subject, $limit, $flags) {}

/**
 * @param str
 * @param delim_char[optional]
 */
function preg_quote ($str, $delim_char) {}

/**
 * @param regex
 * @param input
 * @param flags[optional]
 */
function preg_grep ($regex, $input, $flags) {}

function preg_last_error () {}


/**
 * Orders results so that $matches[0] is an array of full pattern
 *      matches, $matches[1] is an array of strings matched by the first
 *      parenthesized subpattern, and so on. This flag is only used with
 *      preg_match_all.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_PATTERN_ORDER', 1);

/**
 * Orders results so that $matches[0] is an array of first set of
 *      matches, $matches[1] is an array of second set of matches, and so
 *      on. This flag is only used with preg_match_all.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_SET_ORDER', 2);

/**
 * See the description of
 *      PREG_SPLIT_OFFSET_CAPTURE.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_OFFSET_CAPTURE', 256);

/**
 * This flag tells preg_split to return only non-empty
 *      pieces.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_SPLIT_NO_EMPTY', 1);

/**
 * This flag tells preg_split to capture
 *      parenthesized expression in the delimiter pattern as well.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_SPLIT_DELIM_CAPTURE', 2);

/**
 * If this flag is set, for every occurring match the appendant string
 *      offset will also be returned. Note that this changes the return
 *      values in an array where every element is an array consisting of the
 *      matched string at offset 0 and its string offset within subject at
 *      offset 1. This flag is only used for preg_split.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_SPLIT_OFFSET_CAPTURE', 4);
define ('PREG_GREP_INVERT', 1);

/**
 * Returned by preg_last_error if there were no
 *      errors.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_NO_ERROR', 0);

/**
 * Returned by preg_last_error if there was an
 *      internal PCRE error.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_INTERNAL_ERROR', 1);

/**
 * Returned by preg_last_error if backtrack limit was exhausted.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_BACKTRACK_LIMIT_ERROR', 2);

/**
 * Returned by preg_last_error if recursion limit was exhausted.
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_RECURSION_LIMIT_ERROR', 3);

/**
 * Returned by preg_last_error if the last error was
 *      caused by malformed UTF-8 data (only when running a regex in UTF-8 mode).
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_BAD_UTF8_ERROR', 4);

/**
 * Returned by preg_last_error if the offset didn't
 *      correspond to the begin of a valid UTF-8 code point (only when running
 *      a regex in UTF-8
 *      mode).
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PREG_BAD_UTF8_OFFSET_ERROR', 5);

/**
 * PCRE version and release date (e.g. "7.0 18-Dec-2006").
 * @link http://www.php.net/manual/en/pcre.constants.php
 */
define ('PCRE_VERSION', "8.34 2013-12-15");

// End of pcre v.
?>
