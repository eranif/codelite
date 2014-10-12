<?php

// Start of mbstring v.

/**
 * @param sourcestring
 * @param mode
 * @param encoding[optional]
 */
function mb_convert_case ($sourcestring, $mode, $encoding) {}

/**
 * @param sourcestring
 * @param encoding[optional]
 */
function mb_strtoupper ($sourcestring, $encoding) {}

/**
 * @param sourcestring
 * @param encoding[optional]
 */
function mb_strtolower ($sourcestring, $encoding) {}

/**
 * @param language[optional]
 */
function mb_language ($language) {}

/**
 * @param encoding[optional]
 */
function mb_internal_encoding ($encoding) {}

/**
 * @param type[optional]
 */
function mb_http_input ($type) {}

/**
 * @param encoding[optional]
 */
function mb_http_output ($encoding) {}

/**
 * @param encoding[optional]
 */
function mb_detect_order ($encoding) {}

/**
 * @param substchar[optional]
 */
function mb_substitute_character ($substchar) {}

/**
 * @param encoded_string
 * @param result[optional]
 */
function mb_parse_str ($encoded_string, &$result) {}

/**
 * @param contents
 * @param status
 */
function mb_output_handler ($contents, $status) {}

/**
 * @param encoding
 */
function mb_preferred_mime_name ($encoding) {}

/**
 * @param str
 * @param encoding[optional]
 */
function mb_strlen ($str, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param encoding[optional]
 */
function mb_strpos ($haystack, $needle, $offset, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param encoding[optional]
 */
function mb_strrpos ($haystack, $needle, $offset, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param encoding[optional]
 */
function mb_stripos ($haystack, $needle, $offset, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param encoding[optional]
 */
function mb_strripos ($haystack, $needle, $offset, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 * @param encoding[optional]
 */
function mb_strstr ($haystack, $needle, $part, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 * @param encoding[optional]
 */
function mb_strrchr ($haystack, $needle, $part, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 * @param encoding[optional]
 */
function mb_stristr ($haystack, $needle, $part, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 * @param encoding[optional]
 */
function mb_strrichr ($haystack, $needle, $part, $encoding) {}

/**
 * @param haystack
 * @param needle
 * @param encoding[optional]
 */
function mb_substr_count ($haystack, $needle, $encoding) {}

/**
 * @param str
 * @param start
 * @param length[optional]
 * @param encoding[optional]
 */
function mb_substr ($str, $start, $length, $encoding) {}

/**
 * @param str
 * @param start
 * @param length[optional]
 * @param encoding[optional]
 */
function mb_strcut ($str, $start, $length, $encoding) {}

/**
 * @param str
 * @param encoding[optional]
 */
function mb_strwidth ($str, $encoding) {}

/**
 * @param str
 * @param start
 * @param width
 * @param trimmarker[optional]
 * @param encoding[optional]
 */
function mb_strimwidth ($str, $start, $width, $trimmarker, $encoding) {}

/**
 * @param str
 * @param to
 * @param from[optional]
 */
function mb_convert_encoding ($str, $to, $from) {}

/**
 * @param str
 * @param encoding_list[optional]
 * @param strict[optional]
 */
function mb_detect_encoding ($str, $encoding_list, $strict) {}

function mb_list_encodings () {}

/**
 * @param encoding
 */
function mb_encoding_aliases ($encoding) {}

/**
 * @param str
 * @param option[optional]
 * @param encoding[optional]
 */
function mb_convert_kana ($str, $option, $encoding) {}

/**
 * @param str
 * @param charset[optional]
 * @param transfer[optional]
 * @param linefeed[optional]
 * @param indent[optional]
 */
function mb_encode_mimeheader ($str, $charset, $transfer, $linefeed, $indent) {}

/**
 * @param string
 */
function mb_decode_mimeheader ($string) {}

/**
 * @param to
 * @param from
 * @param vars
 */
function mb_convert_variables ($to, $from, &$vars) {}

/**
 * @param string
 * @param convmap
 * @param encoding[optional]
 * @param is_hex[optional]
 */
function mb_encode_numericentity ($string, $convmap, $encoding, $is_hex) {}

/**
 * @param string
 * @param convmap
 * @param encoding[optional]
 */
function mb_decode_numericentity ($string, $convmap, $encoding) {}

/**
 * @param to
 * @param subject
 * @param message
 * @param additional_headers[optional]
 * @param additional_parameters[optional]
 */
function mb_send_mail ($to, $subject, $message, $additional_headers, $additional_parameters) {}

/**
 * @param type[optional]
 */
function mb_get_info ($type) {}

/**
 * @param var[optional]
 * @param encoding[optional]
 */
function mb_check_encoding ($var, $encoding) {}

/**
 * @param encoding[optional]
 */
function mb_regex_encoding ($encoding) {}

/**
 * @param options[optional]
 */
function mb_regex_set_options ($options) {}

/**
 * @param pattern
 * @param string
 * @param registers[optional]
 */
function mb_ereg ($pattern, $string, &$registers) {}

/**
 * @param pattern
 * @param string
 * @param registers[optional]
 */
function mb_eregi ($pattern, $string, &$registers) {}

/**
 * @param pattern
 * @param replacement
 * @param string
 * @param option[optional]
 */
function mb_ereg_replace ($pattern, $replacement, $string, $option) {}

/**
 * @param pattern
 * @param replacement
 * @param string
 */
function mb_eregi_replace ($pattern, $replacement, $string) {}

/**
 * @param pattern
 * @param callback
 * @param string
 * @param option[optional]
 */
function mb_ereg_replace_callback ($pattern, $callback, $string, $option) {}

/**
 * @param pattern
 * @param string
 * @param limit[optional]
 */
function mb_split ($pattern, $string, $limit) {}

/**
 * @param pattern
 * @param string
 * @param option[optional]
 */
function mb_ereg_match ($pattern, $string, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mb_ereg_search ($pattern, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mb_ereg_search_pos ($pattern, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mb_ereg_search_regs ($pattern, $option) {}

/**
 * @param string
 * @param pattern[optional]
 * @param option[optional]
 */
function mb_ereg_search_init ($string, $pattern, $option) {}

function mb_ereg_search_getregs () {}

function mb_ereg_search_getpos () {}

/**
 * @param position
 */
function mb_ereg_search_setpos ($position) {}

/**
 * @param encoding[optional]
 */
function mbregex_encoding ($encoding) {}

/**
 * @param pattern
 * @param string
 * @param registers[optional]
 */
function mbereg ($pattern, $string, &$registers) {}

/**
 * @param pattern
 * @param string
 * @param registers[optional]
 */
function mberegi ($pattern, $string, &$registers) {}

/**
 * @param pattern
 * @param replacement
 * @param string
 * @param option[optional]
 */
function mbereg_replace ($pattern, $replacement, $string, $option) {}

/**
 * @param pattern
 * @param replacement
 * @param string
 */
function mberegi_replace ($pattern, $replacement, $string) {}

/**
 * @param pattern
 * @param string
 * @param limit[optional]
 */
function mbsplit ($pattern, $string, $limit) {}

/**
 * @param pattern
 * @param string
 * @param option[optional]
 */
function mbereg_match ($pattern, $string, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mbereg_search ($pattern, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mbereg_search_pos ($pattern, $option) {}

/**
 * @param pattern[optional]
 * @param option[optional]
 */
function mbereg_search_regs ($pattern, $option) {}

/**
 * @param string
 * @param pattern[optional]
 * @param option[optional]
 */
function mbereg_search_init ($string, $pattern, $option) {}

function mbereg_search_getregs () {}

function mbereg_search_getpos () {}

/**
 * @param position
 */
function mbereg_search_setpos ($position) {}

define ('MB_OVERLOAD_MAIL', 1);
define ('MB_OVERLOAD_STRING', 2);
define ('MB_OVERLOAD_REGEX', 4);
define ('MB_CASE_UPPER', 0);
define ('MB_CASE_LOWER', 1);
define ('MB_CASE_TITLE', 2);

// End of mbstring v.
?>
