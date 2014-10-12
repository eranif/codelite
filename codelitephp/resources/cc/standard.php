<?php

// Start of standard v.5.6.0

class __PHP_Incomplete_Class  {
}

class php_user_filter  {
	public $filtername;
	public $params;


	/**
	 * @param in
	 * @param out
	 * @param consumed
	 * @param closing
	 */
	public function filter ($in, $out, &$consumed, $closing) {}

	public function onCreate () {}

	public function onClose () {}

}

class Directory  {

	/**
	 * @param dir_handle[optional]
	 */
	public function close ($dir_handle) {}

	/**
	 * @param dir_handle[optional]
	 */
	public function rewind ($dir_handle) {}

	/**
	 * @param dir_handle[optional]
	 */
	public function read ($dir_handle) {}

}

/**
 * @param const_name
 */
function constant ($const_name) {}

/**
 * @param data
 */
function bin2hex ($data) {}

/**
 * @param data
 */
function hex2bin ($data) {}

/**
 * @param seconds
 */
function sleep ($seconds) {}

/**
 * @param micro_seconds
 */
function usleep ($micro_seconds) {}

/**
 * @param seconds
 * @param nanoseconds
 */
function time_nanosleep ($seconds, $nanoseconds) {}

/**
 * @param timestamp
 */
function time_sleep_until ($timestamp) {}

/**
 * @param timestamp
 * @param format
 */
function strptime ($timestamp, $format) {}

function flush () {}

/**
 * @param str
 * @param width[optional]
 * @param break[optional]
 * @param cut[optional]
 */
function wordwrap ($str, $width, $break, $cut) {}

/**
 * @param string
 * @param quote_style[optional]
 * @param charset[optional]
 * @param double_encode[optional]
 */
function htmlspecialchars ($string, $quote_style, $charset, $double_encode) {}

/**
 * @param string
 * @param quote_style[optional]
 * @param charset[optional]
 * @param double_encode[optional]
 */
function htmlentities ($string, $quote_style, $charset, $double_encode) {}

/**
 * @param string
 * @param quote_style[optional]
 * @param charset[optional]
 */
function html_entity_decode ($string, $quote_style, $charset) {}

/**
 * @param string
 * @param quote_style[optional]
 */
function htmlspecialchars_decode ($string, $quote_style) {}

/**
 * @param table[optional]
 * @param quote_style[optional]
 */
function get_html_translation_table ($table, $quote_style) {}

/**
 * @param str
 * @param raw_output[optional]
 */
function sha1 ($str, $raw_output) {}

/**
 * @param filename
 * @param raw_output[optional]
 */
function sha1_file ($filename, $raw_output) {}

/**
 * @param str
 * @param raw_output[optional]
 */
function md5 ($str, $raw_output) {}

/**
 * @param filename
 * @param raw_output[optional]
 */
function md5_file ($filename, $raw_output) {}

/**
 * @param str
 */
function crc32 ($str) {}

/**
 * @param iptcdata
 */
function iptcparse ($iptcdata) {}

/**
 * @param iptcdata
 * @param jpeg_file_name
 * @param spool[optional]
 */
function iptcembed ($iptcdata, $jpeg_file_name, $spool) {}

/**
 * @param imagefile
 * @param info[optional]
 */
function getimagesize ($imagefile, &$info) {}

/**
 * @param imagefile
 * @param info[optional]
 */
function getimagesizefromstring ($imagefile, &$info) {}

/**
 * @param imagetype
 */
function image_type_to_mime_type ($imagetype) {}

/**
 * @param imagetype
 * @param include_dot[optional]
 */
function image_type_to_extension ($imagetype, $include_dot) {}

/**
 * @param what[optional]
 */
function phpinfo ($what) {}

/**
 * @param extension[optional]
 */
function phpversion ($extension) {}

/**
 * @param flag[optional]
 */
function phpcredits ($flag) {}

function php_sapi_name () {}

function php_uname () {}

function php_ini_scanned_files () {}

function php_ini_loaded_file () {}

/**
 * @param s1
 * @param s2
 */
function strnatcmp ($s1, $s2) {}

/**
 * @param s1
 * @param s2
 */
function strnatcasecmp ($s1, $s2) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 * @param length[optional]
 */
function substr_count ($haystack, $needle, $offset, $length) {}

/**
 * @param str
 * @param mask
 * @param start[optional]
 * @param len[optional]
 */
function strspn ($str, $mask, $start, $len) {}

/**
 * @param str
 * @param mask
 * @param start[optional]
 * @param len[optional]
 */
function strcspn ($str, $mask, $start, $len) {}

/**
 * @param str
 * @param token
 */
function strtok ($str, $token) {}

/**
 * @param str
 */
function strtoupper ($str) {}

/**
 * @param str
 */
function strtolower ($str) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function strpos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function stripos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function strrpos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function strripos ($haystack, $needle, $offset) {}

/**
 * @param str
 */
function strrev ($str) {}

/**
 * @param str
 * @param max_chars_per_line[optional]
 */
function hebrev ($str, $max_chars_per_line) {}

/**
 * @param str
 * @param max_chars_per_line[optional]
 */
function hebrevc ($str, $max_chars_per_line) {}

/**
 * @param str
 * @param is_xhtml[optional]
 */
function nl2br ($str, $is_xhtml) {}

/**
 * @param path
 * @param suffix[optional]
 */
function basename ($path, $suffix) {}

/**
 * @param path
 */
function dirname ($path) {}

/**
 * @param path
 * @param options[optional]
 */
function pathinfo ($path, $options) {}

/**
 * @param str
 */
function stripslashes ($str) {}

/**
 * @param str
 */
function stripcslashes ($str) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 */
function strstr ($haystack, $needle, $part) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 */
function stristr ($haystack, $needle, $part) {}

/**
 * @param haystack
 * @param needle
 */
function strrchr ($haystack, $needle) {}

/**
 * @param str
 */
function str_shuffle ($str) {}

/**
 * @param str
 * @param format[optional]
 * @param charlist[optional]
 */
function str_word_count ($str, $format, $charlist) {}

/**
 * @param str
 * @param split_length[optional]
 */
function str_split ($str, $split_length) {}

/**
 * @param haystack
 * @param char_list[optional]
 */
function strpbrk ($haystack, $char_list) {}

/**
 * @param main_str
 * @param str
 * @param offset
 * @param length[optional]
 * @param case_sensitivity[optional]
 */
function substr_compare ($main_str, $str, $offset, $length, $case_sensitivity) {}

/**
 * @param str1
 * @param str2
 */
function strcoll ($str1, $str2) {}

/**
 * @param format
 * @param value
 */
function money_format ($format, $value) {}

/**
 * @param str
 * @param start
 * @param length[optional]
 */
function substr ($str, $start, $length) {}

/**
 * @param str
 * @param replace
 * @param start
 * @param length[optional]
 */
function substr_replace ($str, $replace, $start, $length) {}

/**
 * @param str
 */
function quotemeta ($str) {}

/**
 * @param str
 */
function ucfirst ($str) {}

/**
 * @param str
 */
function lcfirst ($str) {}

/**
 * @param str
 * @param delimiters[optional]
 */
function ucwords ($str, $delimiters) {}

/**
 * @param str
 * @param from
 * @param to[optional]
 */
function strtr ($str, $from, $to) {}

/**
 * @param str
 */
function addslashes ($str) {}

/**
 * @param str
 * @param charlist
 */
function addcslashes ($str, $charlist) {}

/**
 * @param str
 * @param character_mask[optional]
 */
function rtrim ($str, $character_mask) {}

/**
 * @param search
 * @param replace
 * @param subject
 * @param replace_count[optional]
 */
function str_replace ($search, $replace, $subject, &$replace_count) {}

/**
 * @param search
 * @param replace
 * @param subject
 * @param replace_count[optional]
 */
function str_ireplace ($search, $replace, $subject, &$replace_count) {}

/**
 * @param input
 * @param mult
 */
function str_repeat ($input, $mult) {}

/**
 * @param input
 * @param mode[optional]
 */
function count_chars ($input, $mode) {}

/**
 * @param str
 * @param chunklen[optional]
 * @param ending[optional]
 */
function chunk_split ($str, $chunklen, $ending) {}

/**
 * @param str
 * @param character_mask[optional]
 */
function trim ($str, $character_mask) {}

/**
 * @param str
 * @param character_mask[optional]
 */
function ltrim ($str, $character_mask) {}

/**
 * @param str
 * @param allowable_tags[optional]
 */
function strip_tags ($str, $allowable_tags) {}

/**
 * @param str1
 * @param str2
 * @param percent[optional]
 */
function similar_text ($str1, $str2, &$percent) {}

/**
 * @param separator
 * @param str
 * @param limit[optional]
 */
function explode ($separator, $str, $limit) {}

/**
 * @param glue
 * @param pieces
 */
function implode ($glue, $pieces) {}

/**
 * @param glue
 * @param pieces
 */
function join ($glue, $pieces) {}

/**
 * @param category
 * @param locales
 */
function setlocale ($category, $locales) {}

function localeconv () {}

/**
 * @param item
 */
function nl_langinfo ($item) {}

/**
 * @param str
 */
function soundex ($str) {}

/**
 * @param str1
 * @param str2
 * @param cost_ins
 * @param cost_rep
 * @param cost_del
 */
function levenshtein ($str1, $str2, $cost_ins, $cost_rep, $cost_del) {}

/**
 * @param codepoint
 */
function chr ($codepoint) {}

/**
 * @param character
 */
function ord ($character) {}

/**
 * @param encoded_string
 * @param result[optional]
 */
function parse_str ($encoded_string, &$result) {}

/**
 * @param string
 * @param delimiter[optional]
 * @param enclosure[optional]
 * @param escape[optional]
 */
function str_getcsv ($string, $delimiter, $enclosure, $escape) {}

/**
 * @param input
 * @param pad_length
 * @param pad_string[optional]
 * @param pad_type[optional]
 */
function str_pad ($input, $pad_length, $pad_string, $pad_type) {}

/**
 * @param str
 * @param character_mask[optional]
 */
function chop ($str, $character_mask) {}

/**
 * @param haystack
 * @param needle
 * @param part[optional]
 */
function strchr ($haystack, $needle, $part) {}

/**
 * @param format
 * @param args
 */
function sprintf ($format, $args) {}

/**
 * @param format
 * @param args[optional]
 */
function printf ($format, $args) {}

/**
 * @param format
 * @param args
 */
function vprintf ($format, $args) {}

/**
 * @param format
 * @param args
 */
function vsprintf ($format, $args) {}

/**
 * @param stream
 * @param format
 * @param args[optional]
 */
function fprintf ($stream, $format, $args) {}

/**
 * @param stream
 * @param format
 * @param args
 */
function vfprintf ($stream, $format, $args) {}

/**
 * @param str
 * @param format
 * @param vars[optional]
 */
function sscanf ($str, $format, &$vars) {}

/**
 * @param stream
 * @param format
 * @param vars[optional]
 */
function fscanf ($stream, $format, &$vars) {}

/**
 * @param url
 * @param component[optional]
 */
function parse_url ($url, $component) {}

/**
 * @param str
 */
function urlencode ($str) {}

/**
 * @param str
 */
function urldecode ($str) {}

/**
 * @param str
 */
function rawurlencode ($str) {}

/**
 * @param str
 */
function rawurldecode ($str) {}

/**
 * @param formdata
 * @param prefix[optional]
 * @param arg_separator[optional]
 * @param enc_type[optional]
 */
function http_build_query ($formdata, $prefix, $arg_separator, $enc_type) {}

/**
 * @param filename
 */
function readlink ($filename) {}

/**
 * @param filename
 */
function linkinfo ($filename) {}

/**
 * @param target
 * @param link
 */
function symlink ($target, $link) {}

/**
 * @param target
 * @param link
 */
function link ($target, $link) {}

/**
 * @param filename
 * @param context[optional]
 */
function unlink ($filename, $context) {}

/**
 * @param command
 * @param output[optional]
 * @param return_value[optional]
 */
function exec ($command, &$output, &$return_value) {}

/**
 * @param command
 * @param return_value[optional]
 */
function system ($command, &$return_value) {}

/**
 * @param command
 */
function escapeshellcmd ($command) {}

/**
 * @param arg
 */
function escapeshellarg ($arg) {}

/**
 * @param command
 * @param return_value[optional]
 */
function passthru ($command, &$return_value) {}

/**
 * @param cmd
 */
function shell_exec ($cmd) {}

/**
 * @param command
 * @param descriptorspec
 * @param pipes
 * @param cwd[optional]
 * @param env[optional]
 * @param other_options[optional]
 */
function proc_open ($command, $descriptorspec, &$pipes, $cwd, $env, $other_options) {}

/**
 * @param process
 */
function proc_close ($process) {}

/**
 * @param process
 * @param signal[optional]
 */
function proc_terminate ($process, $signal) {}

/**
 * @param process
 */
function proc_get_status ($process) {}

/**
 * @param priority
 */
function proc_nice ($priority) {}

/**
 * @param min[optional]
 * @param max[optional]
 */
function rand ($min, $max) {}

/**
 * @param seed[optional]
 */
function srand ($seed) {}

function getrandmax () {}

/**
 * @param min[optional]
 * @param max[optional]
 */
function mt_rand ($min, $max) {}

/**
 * @param seed[optional]
 */
function mt_srand ($seed) {}

function mt_getrandmax () {}

/**
 * @param service
 * @param protocol
 */
function getservbyname ($service, $protocol) {}

/**
 * @param port
 * @param protocol
 */
function getservbyport ($port, $protocol) {}

/**
 * @param name
 */
function getprotobyname ($name) {}

/**
 * @param proto
 */
function getprotobynumber ($proto) {}

function getmyuid () {}

function getmygid () {}

function getmypid () {}

function getmyinode () {}

function getlastmod () {}

/**
 * @param str
 * @param strict[optional]
 */
function base64_decode ($str, $strict) {}

/**
 * @param str
 */
function base64_encode ($str) {}

/**
 * @param password
 * @param algo
 * @param options[optional]
 */
function password_hash ($password, $algo, $options) {}

/**
 * @param hash
 */
function password_get_info ($hash) {}

/**
 * @param hash
 * @param algo
 * @param options[optional]
 */
function password_needs_rehash ($hash, $algo, $options) {}

/**
 * @param password
 * @param hash
 */
function password_verify ($password, $hash) {}

/**
 * @param data
 */
function convert_uuencode ($data) {}

/**
 * @param data
 */
function convert_uudecode ($data) {}

/**
 * @param number
 */
function abs ($number) {}

/**
 * @param number
 */
function ceil ($number) {}

/**
 * @param number
 */
function floor ($number) {}

/**
 * @param number
 * @param precision[optional]
 * @param mode[optional]
 */
function round ($number, $precision, $mode) {}

/**
 * @param number
 */
function sin ($number) {}

/**
 * @param number
 */
function cos ($number) {}

/**
 * @param number
 */
function tan ($number) {}

/**
 * @param number
 */
function asin ($number) {}

/**
 * @param number
 */
function acos ($number) {}

/**
 * @param number
 */
function atan ($number) {}

/**
 * @param number
 */
function atanh ($number) {}

/**
 * @param y
 * @param x
 */
function atan2 ($y, $x) {}

/**
 * @param number
 */
function sinh ($number) {}

/**
 * @param number
 */
function cosh ($number) {}

/**
 * @param number
 */
function tanh ($number) {}

/**
 * @param number
 */
function asinh ($number) {}

/**
 * @param number
 */
function acosh ($number) {}

/**
 * @param number
 */
function expm1 ($number) {}

/**
 * @param number
 */
function log1p ($number) {}

function pi () {}

/**
 * @param val
 */
function is_finite ($val) {}

/**
 * @param val
 */
function is_nan ($val) {}

/**
 * @param val
 */
function is_infinite ($val) {}

/**
 * @param base
 * @param exponent
 */
function pow ($base, $exponent) {}

/**
 * @param number
 */
function exp ($number) {}

/**
 * @param number
 * @param base[optional]
 */
function log ($number, $base) {}

/**
 * @param number
 */
function log10 ($number) {}

/**
 * @param number
 */
function sqrt ($number) {}

/**
 * @param num1
 * @param num2
 */
function hypot ($num1, $num2) {}

/**
 * @param number
 */
function deg2rad ($number) {}

/**
 * @param number
 */
function rad2deg ($number) {}

/**
 * @param binary_number
 */
function bindec ($binary_number) {}

/**
 * @param hexadecimal_number
 */
function hexdec ($hexadecimal_number) {}

/**
 * @param octal_number
 */
function octdec ($octal_number) {}

/**
 * @param decimal_number
 */
function decbin ($decimal_number) {}

/**
 * @param decimal_number
 */
function decoct ($decimal_number) {}

/**
 * @param decimal_number
 */
function dechex ($decimal_number) {}

/**
 * @param number
 * @param frombase
 * @param tobase
 */
function base_convert ($number, $frombase, $tobase) {}

/**
 * @param number
 * @param num_decimal_places[optional]
 * @param dec_separator[optional]
 * @param thousands_separator[optional]
 */
function number_format ($number, $num_decimal_places, $dec_separator, $thousands_separator) {}

/**
 * @param x
 * @param y
 */
function fmod ($x, $y) {}

/**
 * @param in_addr
 */
function inet_ntop ($in_addr) {}

/**
 * @param ip_address
 */
function inet_pton ($ip_address) {}

/**
 * @param ip_address
 */
function ip2long ($ip_address) {}

/**
 * @param proper_address
 */
function long2ip ($proper_address) {}

/**
 * @param varname
 */
function getenv ($varname) {}

/**
 * @param setting
 */
function putenv ($setting) {}

/**
 * @param options
 * @param opts[optional]
 */
function getopt ($options, $opts) {}

function sys_getloadavg () {}

/**
 * @param get_as_float[optional]
 */
function microtime ($get_as_float) {}

/**
 * @param get_as_float[optional]
 */
function gettimeofday ($get_as_float) {}

/**
 * @param who[optional]
 */
function getrusage ($who) {}

/**
 * @param prefix[optional]
 * @param more_entropy[optional]
 */
function uniqid ($prefix, $more_entropy) {}

/**
 * @param str
 */
function quoted_printable_decode ($str) {}

/**
 * @param str
 */
function quoted_printable_encode ($str) {}

/**
 * @param str
 * @param from
 * @param to
 */
function convert_cyr_string ($str, $from, $to) {}

function get_current_user () {}

/**
 * @param seconds
 */
function set_time_limit ($seconds) {}

/**
 * @param callback
 */
function header_register_callback ($callback) {}

/**
 * @param option_name
 */
function get_cfg_var ($option_name) {}

/**
 * @param new_setting
 */
function magic_quotes_runtime ($new_setting) {}

/**
 * @param new_setting
 */
function set_magic_quotes_runtime ($new_setting) {}

function get_magic_quotes_gpc () {}

function get_magic_quotes_runtime () {}

/**
 * @param message
 * @param message_type[optional]
 * @param destination[optional]
 * @param extra_headers[optional]
 */
function error_log ($message, $message_type, $destination, $extra_headers) {}

function error_get_last () {}

/**
 * @param function_name
 * @param parameters[optional]
 */
function call_user_func ($function_name, $parameters) {}

/**
 * @param function_name
 * @param parameters
 */
function call_user_func_array ($function_name, $parameters) {}

/**
 * @param method_name
 * @param object
 * @param parameters[optional]
 */
function call_user_method ($method_name, &$object, $parameters) {}

/**
 * @param method_name
 * @param object
 * @param params
 */
function call_user_method_array ($method_name, &$object, $params) {}

/**
 * @param function_name
 * @param parameters[optional]
 */
function forward_static_call ($function_name, $parameters) {}

/**
 * @param function_name
 * @param parameters
 */
function forward_static_call_array ($function_name, $parameters) {}

/**
 * @param var
 */
function serialize ($var) {}

/**
 * @param variable_representation
 */
function unserialize ($variable_representation) {}

/**
 * @param vars
 */
function var_dump ($vars) {}

/**
 * @param var
 * @param return[optional]
 */
function var_export ($var, $return) {}

/**
 * @param vars
 */
function debug_zval_dump ($vars) {}

/**
 * @param var
 * @param return[optional]
 */
function print_r ($var, $return) {}

/**
 * @param real_usage[optional]
 */
function memory_get_usage ($real_usage) {}

/**
 * @param real_usage[optional]
 */
function memory_get_peak_usage ($real_usage) {}

/**
 * @param function_name
 * @param parameters[optional]
 */
function register_shutdown_function ($function_name, $parameters) {}

/**
 * @param function_name
 * @param parameters[optional]
 */
function register_tick_function ($function_name, $parameters) {}

/**
 * @param function_name
 */
function unregister_tick_function ($function_name) {}

/**
 * @param file_name
 * @param return[optional]
 */
function highlight_file ($file_name, $return) {}

/**
 * @param file_name
 * @param return[optional]
 */
function show_source ($file_name, $return) {}

/**
 * @param string
 * @param return[optional]
 */
function highlight_string ($string, $return) {}

/**
 * @param file_name
 */
function php_strip_whitespace ($file_name) {}

/**
 * @param varname
 */
function ini_get ($varname) {}

/**
 * @param extension[optional]
 */
function ini_get_all ($extension) {}

/**
 * @param varname
 * @param newvalue
 */
function ini_set ($varname, $newvalue) {}

/**
 * @param varname
 * @param newvalue
 */
function ini_alter ($varname, $newvalue) {}

/**
 * @param varname
 */
function ini_restore ($varname) {}

function get_include_path () {}

/**
 * @param new_include_path
 */
function set_include_path ($new_include_path) {}

function restore_include_path () {}

/**
 * @param name
 * @param value[optional]
 * @param expires[optional]
 * @param path[optional]
 * @param domain[optional]
 * @param secure[optional]
 */
function setcookie ($name, $value, $expires, $path, $domain, $secure) {}

/**
 * @param name
 * @param value[optional]
 * @param expires[optional]
 * @param path[optional]
 * @param domain[optional]
 * @param secure[optional]
 */
function setrawcookie ($name, $value, $expires, $path, $domain, $secure) {}

/**
 * @param header
 * @param replace[optional]
 * @param http_response_code[optional]
 */
function header ($header, $replace, $http_response_code) {}

/**
 * @param name[optional]
 */
function header_remove ($name) {}

/**
 * @param file[optional]
 * @param line[optional]
 */
function headers_sent (&$file, &$line) {}

function headers_list () {}

/**
 * @param response_code[optional]
 */
function http_response_code ($response_code) {}

function connection_aborted () {}

function connection_status () {}

/**
 * @param value[optional]
 */
function ignore_user_abort ($value) {}

/**
 * @param filename
 * @param process_sections[optional]
 * @param scanner_mode[optional]
 */
function parse_ini_file ($filename, $process_sections, $scanner_mode) {}

/**
 * @param ini_string
 * @param process_sections[optional]
 * @param scanner_mode[optional]
 */
function parse_ini_string ($ini_string, $process_sections, $scanner_mode) {}

/**
 * @param path
 */
function is_uploaded_file ($path) {}

/**
 * @param path
 * @param new_path
 */
function move_uploaded_file ($path, $new_path) {}

/**
 * @param ip_address
 */
function gethostbyaddr ($ip_address) {}

/**
 * @param hostname
 */
function gethostbyname ($hostname) {}

/**
 * @param hostname
 */
function gethostbynamel ($hostname) {}

function gethostname () {}

/**
 * @param host
 * @param type[optional]
 */
function dns_check_record ($host, $type) {}

/**
 * @param host
 * @param type[optional]
 */
function checkdnsrr ($host, $type) {}

/**
 * @param hostname
 * @param mxhosts
 * @param weight[optional]
 */
function dns_get_mx ($hostname, &$mxhosts, &$weight) {}

/**
 * @param hostname
 * @param mxhosts
 * @param weight[optional]
 */
function getmxrr ($hostname, &$mxhosts, &$weight) {}

/**
 * @param hostname
 * @param type[optional]
 * @param authns[optional]
 * @param addtl[optional]
 * @param raw[optional]
 */
function dns_get_record ($hostname, $typearray , &$authns = nullarray , &$addtl = null, $raw) {}

/**
 * @param var
 * @param base[optional]
 */
function intval ($var, $base) {}

/**
 * @param var
 */
function floatval ($var) {}

/**
 * @param var
 */
function doubleval ($var) {}

/**
 * @param var
 */
function strval ($var) {}

/**
 * @param var
 */
function boolval ($var) {}

/**
 * @param var
 */
function gettype ($var) {}

/**
 * @param var
 * @param type
 */
function settype (&$var, $type) {}

/**
 * @param var
 */
function is_null ($var) {}

/**
 * @param var
 */
function is_resource ($var) {}

/**
 * @param var
 */
function is_bool ($var) {}

/**
 * @param var
 */
function is_long ($var) {}

/**
 * @param var
 */
function is_float ($var) {}

/**
 * @param var
 */
function is_int ($var) {}

/**
 * @param var
 */
function is_integer ($var) {}

/**
 * @param var
 */
function is_double ($var) {}

/**
 * @param var
 */
function is_real ($var) {}

/**
 * @param value
 */
function is_numeric ($value) {}

/**
 * @param var
 */
function is_string ($var) {}

/**
 * @param var
 */
function is_array ($var) {}

/**
 * @param var
 */
function is_object ($var) {}

/**
 * @param value
 */
function is_scalar ($value) {}

/**
 * @param var
 * @param syntax_only[optional]
 * @param callable_name[optional]
 */
function is_callable ($var, $syntax_only, &$callable_name) {}

/**
 * @param fp
 */
function pclose ($fp) {}

/**
 * @param command
 * @param mode
 */
function popen ($command, $mode) {}

/**
 * @param filename
 * @param flags[optional]
 * @param context[optional]
 */
function readfile ($filename, $flags, $context) {}

/**
 * @param fp
 */
function rewind ($fp) {}

/**
 * @param dirname
 * @param context[optional]
 */
function rmdir ($dirname, $context) {}

/**
 * @param mask[optional]
 */
function umask ($mask) {}

/**
 * @param fp
 */
function fclose ($fp) {}

/**
 * @param fp
 */
function feof ($fp) {}

/**
 * @param fp
 */
function fgetc ($fp) {}

/**
 * @param fp
 * @param length[optional]
 */
function fgets ($fp, $length) {}

/**
 * @param fp
 * @param length[optional]
 * @param allowable_tags[optional]
 */
function fgetss ($fp, $length, $allowable_tags) {}

/**
 * @param fp
 * @param length
 */
function fread ($fp, $length) {}

/**
 * @param filename
 * @param mode
 * @param use_include_path[optional]
 * @param context[optional]
 */
function fopen ($filename, $mode, $use_include_path, $context) {}

/**
 * @param fp
 */
function fpassthru ($fp) {}

/**
 * @param fp
 * @param size
 */
function ftruncate ($fp, $size) {}

/**
 * @param fp
 */
function fstat ($fp) {}

/**
 * @param fp
 * @param offset
 * @param whence[optional]
 */
function fseek ($fp, $offset, $whence) {}

/**
 * @param fp
 */
function ftell ($fp) {}

/**
 * @param fp
 */
function fflush ($fp) {}

/**
 * @param fp
 * @param str
 * @param length[optional]
 */
function fwrite ($fp, $str, $length) {}

/**
 * @param fp
 * @param str
 * @param length[optional]
 */
function fputs ($fp, $str, $length) {}

/**
 * @param pathname
 * @param mode[optional]
 * @param recursive[optional]
 * @param context[optional]
 */
function mkdir ($pathname, $mode, $recursive, $context) {}

/**
 * @param old_name
 * @param new_name
 * @param context[optional]
 */
function rename ($old_name, $new_name, $context) {}

/**
 * @param source_file
 * @param destination_file
 * @param context[optional]
 */
function copy ($source_file, $destination_file, $context) {}

/**
 * @param dir
 * @param prefix
 */
function tempnam ($dir, $prefix) {}

function tmpfile () {}

/**
 * @param filename
 * @param flags[optional]
 * @param context[optional]
 */
function file ($filename, $flags, $context) {}

/**
 * @param filename
 * @param flags[optional]
 * @param context[optional]
 * @param offset[optional]
 * @param maxlen[optional]
 */
function file_get_contents ($filename, $flags, $context, $offset, $maxlen) {}

/**
 * @param filename
 * @param data
 * @param flags[optional]
 * @param context[optional]
 */
function file_put_contents ($filename, $data, $flags, $context) {}

/**
 * @param read_streams
 * @param write_streams
 * @param except_streams
 * @param tv_sec
 * @param tv_usec[optional]
 */
function stream_select (&$read_streams, &$write_streams, &$except_streams, $tv_sec, $tv_usec) {}

/**
 * @param options[optional]
 * @param params[optional]
 */
function stream_context_create ($options, $params) {}

/**
 * @param stream_or_context
 * @param options
 */
function stream_context_set_params ($stream_or_context, $options) {}

/**
 * @param stream_or_context
 */
function stream_context_get_params ($stream_or_context) {}

/**
 * @param stream_or_context
 * @param wrappername
 * @param optionname
 * @param value
 */
function stream_context_set_option ($stream_or_context, $wrappername, $optionname, $value) {}

/**
 * @param stream_or_context
 */
function stream_context_get_options ($stream_or_context) {}

/**
 * @param options[optional]
 */
function stream_context_get_default ($options) {}

/**
 * @param options
 */
function stream_context_set_default ($options) {}

/**
 * @param stream
 * @param filtername
 * @param read_write[optional]
 * @param filterparams[optional]
 */
function stream_filter_prepend ($stream, $filtername, $read_write, $filterparams) {}

/**
 * @param stream
 * @param filtername
 * @param read_write[optional]
 * @param filterparams[optional]
 */
function stream_filter_append ($stream, $filtername, $read_write, $filterparams) {}

/**
 * @param stream_filter
 */
function stream_filter_remove ($stream_filter) {}

/**
 * @param remoteaddress
 * @param errcode[optional]
 * @param errstring[optional]
 * @param timeout[optional]
 * @param flags[optional]
 * @param context[optional]
 */
function stream_socket_client ($remoteaddress, &$errcode, &$errstring, $timeout, $flags, $context) {}

/**
 * @param localaddress
 * @param errcode[optional]
 * @param errstring[optional]
 * @param flags[optional]
 * @param context[optional]
 */
function stream_socket_server ($localaddress, &$errcode, &$errstring, $flags, $context) {}

/**
 * @param serverstream
 * @param timeout[optional]
 * @param peername[optional]
 */
function stream_socket_accept ($serverstream, $timeout, &$peername) {}

/**
 * @param stream
 * @param want_peer
 */
function stream_socket_get_name ($stream, $want_peer) {}

/**
 * @param stream
 * @param amount
 * @param flags[optional]
 * @param remote_addr[optional]
 */
function stream_socket_recvfrom ($stream, $amount, $flags, &$remote_addr) {}

/**
 * @param stream
 * @param data
 * @param flags[optional]
 * @param target_addr[optional]
 */
function stream_socket_sendto ($stream, $data, $flags, $target_addr) {}

/**
 * @param stream
 * @param enable
 * @param cryptokind[optional]
 * @param sessionstream[optional]
 */
function stream_socket_enable_crypto ($stream, $enable, $cryptokind, $sessionstream) {}

/**
 * @param stream
 * @param how
 */
function stream_socket_shutdown ($stream, $how) {}

/**
 * @param domain
 * @param type
 * @param protocol
 */
function stream_socket_pair ($domain, $type, $protocol) {}

/**
 * @param source
 * @param dest
 * @param maxlen[optional]
 * @param pos[optional]
 */
function stream_copy_to_stream ($source, $dest, $maxlen, $pos) {}

/**
 * @param source
 * @param maxlen[optional]
 * @param offset[optional]
 */
function stream_get_contents ($source, $maxlen, $offset) {}

/**
 * @param stream
 */
function stream_supports_lock ($stream) {}

/**
 * @param fp
 * @param length[optional]
 * @param delimiter[optional]
 * @param enclosure[optional]
 * @param escape[optional]
 */
function fgetcsv ($fp, $length, $delimiter, $enclosure, $escape) {}

/**
 * @param fp
 * @param fields
 * @param delimiter[optional]
 * @param enclosure[optional]
 */
function fputcsv ($fp, $fields, $delimiter, $enclosure) {}

/**
 * @param fp
 * @param operation
 * @param wouldblock[optional]
 */
function flock ($fp, $operation, &$wouldblock) {}

/**
 * @param filename
 * @param use_include_path[optional]
 */
function get_meta_tags ($filename, $use_include_path) {}

/**
 * @param fp
 * @param buffer
 */
function stream_set_read_buffer ($fp, $buffer) {}

/**
 * @param fp
 * @param buffer
 */
function stream_set_write_buffer ($fp, $buffer) {}

/**
 * @param fp
 * @param buffer
 */
function set_file_buffer ($fp, $buffer) {}

/**
 * @param fp
 * @param chunk_size
 */
function stream_set_chunk_size ($fp, $chunk_size) {}

/**
 * @param socket
 * @param mode
 */
function set_socket_blocking ($socket, $mode) {}

/**
 * @param socket
 * @param mode
 */
function stream_set_blocking ($socket, $mode) {}

/**
 * @param socket
 * @param mode
 */
function socket_set_blocking ($socket, $mode) {}

/**
 * @param fp
 */
function stream_get_meta_data ($fp) {}

/**
 * @param stream
 * @param maxlen
 * @param ending[optional]
 */
function stream_get_line ($stream, $maxlen, $ending) {}

/**
 * @param protocol
 * @param classname
 * @param flags[optional]
 */
function stream_wrapper_register ($protocol, $classname, $flags) {}

/**
 * @param protocol
 * @param classname
 * @param flags[optional]
 */
function stream_register_wrapper ($protocol, $classname, $flags) {}

/**
 * @param protocol
 */
function stream_wrapper_unregister ($protocol) {}

/**
 * @param protocol
 */
function stream_wrapper_restore ($protocol) {}

function stream_get_wrappers () {}

function stream_get_transports () {}

/**
 * @param filename
 */
function stream_resolve_include_path ($filename) {}

/**
 * @param stream
 */
function stream_is_local ($stream) {}

/**
 * @param url
 * @param format[optional]
 */
function get_headers ($url, $format) {}

/**
 * @param stream
 * @param seconds
 * @param microseconds
 */
function stream_set_timeout ($stream, $seconds, $microseconds) {}

/**
 * @param stream
 * @param seconds
 * @param microseconds
 */
function socket_set_timeout ($stream, $seconds, $microseconds) {}

/**
 * @param fp
 */
function socket_get_status ($fp) {}

/**
 * @param path
 */
function realpath ($path) {}

/**
 * @param pattern
 * @param filename
 * @param flags[optional]
 */
function fnmatch ($pattern, $filename, $flags) {}

/**
 * @param hostname
 * @param port
 * @param errno[optional]
 * @param errstr[optional]
 * @param timeout[optional]
 */
function fsockopen ($hostname, $port, &$errno, &$errstr, $timeout) {}

/**
 * @param hostname
 * @param port
 * @param errno[optional]
 * @param errstr[optional]
 * @param timeout[optional]
 */
function pfsockopen ($hostname, $port, &$errno, &$errstr, $timeout) {}

/**
 * @param format
 * @param args
 */
function pack ($format, $args) {}

/**
 * @param format
 * @param input
 */
function unpack ($format, $input) {}

/**
 * @param browser_name[optional]
 * @param return_array[optional]
 */
function get_browser ($browser_name, $return_array) {}

/**
 * @param str
 * @param salt[optional]
 */
function crypt ($str, $salt) {}

/**
 * @param path
 * @param context[optional]
 */
function opendir ($path, $context) {}

/**
 * @param dir_handle[optional]
 */
function closedir ($dir_handle) {}

/**
 * @param directory
 */
function chdir ($directory) {}

function getcwd () {}

/**
 * @param dir_handle[optional]
 */
function rewinddir ($dir_handle) {}

/**
 * @param dir_handle[optional]
 */
function readdir ($dir_handle) {}

/**
 * @param directory
 * @param context[optional]
 */
function dir ($directory, $context) {}

/**
 * @param dir
 * @param sorting_order[optional]
 * @param context[optional]
 */
function scandir ($dir, $sorting_order, $context) {}

/**
 * @param pattern
 * @param flags[optional]
 */
function glob ($pattern, $flags) {}

/**
 * @param filename
 */
function fileatime ($filename) {}

/**
 * @param filename
 */
function filectime ($filename) {}

/**
 * @param filename
 */
function filegroup ($filename) {}

/**
 * @param filename
 */
function fileinode ($filename) {}

/**
 * @param filename
 */
function filemtime ($filename) {}

/**
 * @param filename
 */
function fileowner ($filename) {}

/**
 * @param filename
 */
function fileperms ($filename) {}

/**
 * @param filename
 */
function filesize ($filename) {}

/**
 * @param filename
 */
function filetype ($filename) {}

/**
 * @param filename
 */
function file_exists ($filename) {}

/**
 * @param filename
 */
function is_writable ($filename) {}

/**
 * @param filename
 */
function is_writeable ($filename) {}

/**
 * @param filename
 */
function is_readable ($filename) {}

/**
 * @param filename
 */
function is_executable ($filename) {}

/**
 * @param filename
 */
function is_file ($filename) {}

/**
 * @param filename
 */
function is_dir ($filename) {}

/**
 * @param filename
 */
function is_link ($filename) {}

/**
 * @param filename
 */
function stat ($filename) {}

/**
 * @param filename
 */
function lstat ($filename) {}

/**
 * @param filename
 * @param user
 */
function chown ($filename, $user) {}

/**
 * @param filename
 * @param group
 */
function chgrp ($filename, $group) {}

/**
 * @param filename
 * @param user
 */
function lchown ($filename, $user) {}

/**
 * @param filename
 * @param group
 */
function lchgrp ($filename, $group) {}

/**
 * @param filename
 * @param mode
 */
function chmod ($filename, $mode) {}

/**
 * @param filename
 * @param time[optional]
 * @param atime[optional]
 */
function touch ($filename, $time, $atime) {}

/**
 * @param clear_realpath_cache[optional]
 * @param filename[optional]
 */
function clearstatcache ($clear_realpath_cache, $filename) {}

/**
 * @param path
 */
function disk_total_space ($path) {}

/**
 * @param path
 */
function disk_free_space ($path) {}

/**
 * @param path
 */
function diskfreespace ($path) {}

function realpath_cache_size () {}

function realpath_cache_get () {}

/**
 * @param to
 * @param subject
 * @param message
 * @param additional_headers[optional]
 * @param additional_parameters[optional]
 */
function mail ($to, $subject, $message, $additional_headers, $additional_parameters) {}

/**
 * @param addr
 */
function ezmlm_hash ($addr) {}

/**
 * @param ident
 * @param option
 * @param facility
 */
function openlog ($ident, $option, $facility) {}

/**
 * @param priority
 * @param message
 */
function syslog ($priority, $message) {}

function closelog () {}

function lcg_value () {}

/**
 * @param text
 * @param phones[optional]
 */
function metaphone ($text, $phones) {}

/**
 * @param user_function[optional]
 * @param chunk_size[optional]
 * @param flags[optional]
 */
function ob_start ($user_function, $chunk_size, $flags) {}

function ob_flush () {}

function ob_clean () {}

function ob_end_flush () {}

function ob_end_clean () {}

function ob_get_flush () {}

function ob_get_clean () {}

function ob_get_length () {}

function ob_get_level () {}

/**
 * @param full_status[optional]
 */
function ob_get_status ($full_status) {}

function ob_get_contents () {}

/**
 * @param flag[optional]
 */
function ob_implicit_flush ($flag) {}

function ob_list_handlers () {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function ksort (&$arg, $sort_flags) {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function krsort (&$arg, $sort_flags) {}

/**
 * @param arg
 */
function natsort (&$arg) {}

/**
 * @param arg
 */
function natcasesort (&$arg) {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function asort (&$arg, $sort_flags) {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function arsort (&$arg, $sort_flags) {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function sort (&$arg, $sort_flags) {}

/**
 * @param arg
 * @param sort_flags[optional]
 */
function rsort (&$arg, $sort_flags) {}

/**
 * @param arg
 * @param cmp_function
 */
function usort (&$arg, $cmp_function) {}

/**
 * @param arg
 * @param cmp_function
 */
function uasort (&$arg, $cmp_function) {}

/**
 * @param arg
 * @param cmp_function
 */
function uksort (&$arg, $cmp_function) {}

/**
 * @param arg
 */
function shuffle (&$arg) {}

/**
 * @param input
 * @param funcname
 * @param userdata[optional]
 */
function array_walk (&$input, $funcname, $userdata) {}

/**
 * @param input
 * @param funcname
 * @param userdata[optional]
 */
function array_walk_recursive (&$input, $funcname, $userdata) {}

/**
 * @param var
 * @param mode[optional]
 */
function count ($var, $mode) {}

/**
 * @param arg
 */
function end (&$arg) {}

/**
 * @param arg
 */
function prev (&$arg) {}

/**
 * @param arg
 */
function next (&$arg) {}

/**
 * @param arg
 */
function reset (&$arg) {}

/**
 * @param arg
 */
function current (&$arg) {}

/**
 * @param arg
 */
function key (&$arg) {}

/**
 * @param args
 */
function min ($args) {}

/**
 * @param args
 */
function max ($args) {}

/**
 * @param needle
 * @param haystack
 * @param strict[optional]
 */
function in_array ($needle, $haystack, $strict) {}

/**
 * @param needle
 * @param haystack
 * @param strict[optional]
 */
function array_search ($needle, $haystack, $strict) {}

/**
 * @param arg
 * @param extract_type[optional]
 * @param prefix[optional]
 */
function extract (&$arg, $extract_type, $prefix) {}

/**
 * @param var_names
 */
function compact ($var_names) {}

/**
 * @param start_key
 * @param num
 * @param val
 */
function array_fill ($start_key, $num, $val) {}

/**
 * @param keys
 * @param val
 */
function array_fill_keys ($keys, $val) {}

/**
 * @param low
 * @param high
 * @param step[optional]
 */
function range ($low, $high, $step) {}

/**
 * @param arr1
 * @param SORT_ASC_or_SORT_DESC[optional]
 * @param SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING[optional]
 * @param arr2[optional]
 * @param SORT_ASC_or_SORT_DESC[optional]
 * @param SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING[optional]
 * @param more_array_and_sort_options[optional]
 */
function array_multisort (&$arr1, &$SORT_ASC_or_SORT_DESC, &$SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING, &$arr2, &$SORT_ASC_or_SORT_DESC, &$SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING, &$more_array_and_sort_options) {}

/**
 * @param stack
 * @param vars
 */
function array_push (&$stack, $vars) {}

/**
 * @param stack
 */
function array_pop (&$stack) {}

/**
 * @param stack
 */
function array_shift (&$stack) {}

/**
 * @param stack
 * @param vars
 */
function array_unshift (&$stack, $vars) {}

/**
 * @param arg
 * @param offset
 * @param length[optional]
 * @param replacement[optional]
 */
function array_splice (&$arg, $offset, $length, $replacement) {}

/**
 * @param arg
 * @param offset
 * @param length[optional]
 * @param preserve_keys[optional]
 */
function array_slice ($arg, $offset, $length, $preserve_keys) {}

/**
 * @param arr1
 * @param arrays
 */
function array_merge ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arrays
 */
function array_merge_recursive ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arrays
 */
function array_replace ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arrays
 */
function array_replace_recursive ($arr1, $arrays) {}

/**
 * @param arg
 * @param search_value[optional]
 * @param strict[optional]
 */
function array_keys ($arg, $search_value, $strict) {}

/**
 * @param arg
 */
function array_values ($arg) {}

/**
 * @param arg
 */
function array_count_values ($arg) {}

/**
 * @param arg
 * @param column_key
 * @param index_key[optional]
 */
function array_column ($arg, $column_key, $index_key) {}

/**
 * @param input
 * @param preserve_keys[optional]
 */
function array_reverse ($input, $preserve_keys) {}

/**
 * @param arg
 * @param callback
 * @param initial[optional]
 */
function array_reduce ($arg, $callback, $initial) {}

/**
 * @param arg
 * @param pad_size
 * @param pad_value
 */
function array_pad ($arg, $pad_size, $pad_value) {}

/**
 * @param arg
 */
function array_flip ($arg) {}

/**
 * @param input
 * @param case[optional]
 */
function array_change_key_case ($input, $case) {}

/**
 * @param arg
 * @param num_req[optional]
 */
function array_rand ($arg, $num_req) {}

/**
 * @param arg
 */
function array_unique ($arg) {}

/**
 * @param arr1
 * @param arrays
 */
function array_intersect ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arrays
 */
function array_intersect_key ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_key_compare_func
 */
function array_intersect_ukey ($arr1, $arr2, $callback_key_compare_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_compare_func
 */
function array_uintersect ($arr1, $arr2, $callback_data_compare_func) {}

/**
 * @param arr1
 * @param arrays
 */
function array_intersect_assoc ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_compare_func
 */
function array_uintersect_assoc ($arr1, $arr2, $callback_data_compare_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_key_compare_func
 */
function array_intersect_uassoc ($arr1, $arr2, $callback_key_compare_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_compare_func
 * @param callback_key_compare_func
 */
function array_uintersect_uassoc ($arr1, $arr2, $callback_data_compare_func, $callback_key_compare_func) {}

/**
 * @param arr1
 * @param arrays
 */
function array_diff ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arrays
 */
function array_diff_key ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_key_comp_func
 */
function array_diff_ukey ($arr1, $arr2, $callback_key_comp_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_comp_func
 */
function array_udiff ($arr1, $arr2, $callback_data_comp_func) {}

/**
 * @param arr1
 * @param arrays
 */
function array_diff_assoc ($arr1, $arrays) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_key_comp_func
 */
function array_udiff_assoc ($arr1, $arr2, $callback_key_comp_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_comp_func
 */
function array_diff_uassoc ($arr1, $arr2, $callback_data_comp_func) {}

/**
 * @param arr1
 * @param arr2
 * @param callback_data_comp_func
 * @param callback_key_comp_func
 */
function array_udiff_uassoc ($arr1, $arr2, $callback_data_comp_func, $callback_key_comp_func) {}

/**
 * @param arg
 */
function array_sum ($arg) {}

/**
 * @param arg
 */
function array_product ($arg) {}

/**
 * @param arg
 * @param callback[optional]
 * @param use_keys[optional]
 */
function array_filter ($arg, $callback, $use_keys) {}

/**
 * @param callback
 * @param arrays
 */
function array_map ($callback, $arrays) {}

/**
 * @param arg
 * @param size
 * @param preserve_keys[optional]
 */
function array_chunk ($arg, $size, $preserve_keys) {}

/**
 * @param keys
 * @param values
 */
function array_combine ($keys, $values) {}

/**
 * @param key
 * @param search
 */
function array_key_exists ($key, $search) {}

/**
 * @param arg
 */
function pos (&$arg) {}

/**
 * @param var
 * @param mode[optional]
 */
function sizeof ($var, $mode) {}

/**
 * @param key
 * @param search
 */
function key_exists ($key, $search) {}

/**
 * @param assertion
 */
function assert ($assertion) {}

/**
 * @param what
 * @param value[optional]
 */
function assert_options ($what, $value) {}

/**
 * @param ver1
 * @param ver2
 * @param oper[optional]
 */
function version_compare ($ver1, $ver2, $oper) {}

/**
 * @param pathname
 * @param proj
 */
function ftok ($pathname, $proj) {}

/**
 * @param str
 */
function str_rot13 ($str) {}

function stream_get_filters () {}

/**
 * @param filtername
 * @param classname
 */
function stream_filter_register ($filtername, $classname) {}

/**
 * @param brigade
 */
function stream_bucket_make_writeable ($brigade) {}

/**
 * @param brigade
 * @param bucket
 */
function stream_bucket_prepend ($brigade, $bucket) {}

/**
 * @param brigade
 * @param bucket
 */
function stream_bucket_append ($brigade, $bucket) {}

/**
 * @param stream
 * @param buffer
 */
function stream_bucket_new ($stream, $buffer) {}

/**
 * @param name
 * @param value
 */
function output_add_rewrite_var ($name, $value) {}

function output_reset_rewrite_vars () {}

function sys_get_temp_dir () {}

/**
 * @param extension_filename
 */
function dl ($extension_filename) {}

/**
 * @param title
 */
function cli_set_process_title ($title) {}

function cli_get_process_title () {}

define ('CONNECTION_ABORTED', 1);
define ('CONNECTION_NORMAL', 0);
define ('CONNECTION_TIMEOUT', 2);
define ('INI_USER', 1);
define ('INI_PERDIR', 2);
define ('INI_SYSTEM', 4);
define ('INI_ALL', 7);

/**
 * Normal INI scanner mode (since PHP 5.3).
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('INI_SCANNER_NORMAL', 0);

/**
 * Raw INI scanner mode (since PHP 5.3).
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('INI_SCANNER_RAW', 1);
define ('PHP_URL_SCHEME', 0);
define ('PHP_URL_HOST', 1);
define ('PHP_URL_PORT', 2);
define ('PHP_URL_USER', 3);
define ('PHP_URL_PASS', 4);
define ('PHP_URL_PATH', 5);
define ('PHP_URL_QUERY', 6);
define ('PHP_URL_FRAGMENT', 7);
define ('PHP_QUERY_RFC1738', 1);
define ('PHP_QUERY_RFC3986', 2);
define ('M_E', 2.718281828459);
define ('M_LOG2E', 1.442695040889);
define ('M_LOG10E', 0.43429448190325);
define ('M_LN2', 0.69314718055995);
define ('M_LN10', 2.302585092994);

/**
 * Round halves up
 * @link http://www.php.net/manual/en/math.constants.php
 */
define ('M_PI', 3.1415926535898);
define ('M_PI_2', 1.5707963267949);
define ('M_PI_4', 0.78539816339745);
define ('M_1_PI', 0.31830988618379);
define ('M_2_PI', 0.63661977236758);
define ('M_SQRTPI', 1.7724538509055);
define ('M_2_SQRTPI', 1.1283791670955);
define ('M_LNPI', 1.1447298858494);
define ('M_EULER', 0.57721566490153);
define ('M_SQRT2', 1.4142135623731);
define ('M_SQRT1_2', 0.70710678118655);
define ('M_SQRT3', 1.7320508075689);
define ('INF', INF);
define ('NAN', NAN);
define ('PHP_ROUND_HALF_UP', 1);

/**
 * Round halves down
 * @link http://www.php.net/manual/en/math.constants.php
 */
define ('PHP_ROUND_HALF_DOWN', 2);

/**
 * Round halves to even numbers
 * @link http://www.php.net/manual/en/math.constants.php
 */
define ('PHP_ROUND_HALF_EVEN', 3);

/**
 * Round halves to odd numbers
 * @link http://www.php.net/manual/en/math.constants.php
 */
define ('PHP_ROUND_HALF_ODD', 4);
define ('INFO_GENERAL', 1);

/**
 * PHP Credits.  See also phpcredits.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_CREDITS', 2);

/**
 * Current Local and Master values for PHP directives. See
 *      also ini_get.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_CONFIGURATION', 4);

/**
 * Loaded modules and their respective settings.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_MODULES', 8);

/**
 * Environment Variable information that's also available in
 *      $_ENV.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_ENVIRONMENT', 16);

/**
 * Shows all 
 *      predefined variables from EGPCS (Environment, GET,
 *      POST, Cookie, Server).
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_VARIABLES', 32);

/**
 * PHP License information.  See also the license faq.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_LICENSE', 64);

/**
 * Unused
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('INFO_ALL', 4294967295);

/**
 * A list of the core developers
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_GROUP', 1);

/**
 * General credits: Language design and concept, PHP
 *      authors and SAPI module.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_GENERAL', 2);

/**
 * A list of the server API modules for PHP, and their authors.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_SAPI', 4);

/**
 * A list of the extension modules for PHP, and their authors.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_MODULES', 8);

/**
 * The credits for the documentation team.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_DOCS', 16);

/**
 * Usually used in combination with the other flags. Indicates
 *      that a complete stand-alone HTML page needs to be
 *      printed including the information indicated by the other
 *      flags.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_FULLPAGE', 32);

/**
 * The credits for the quality assurance team.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_QA', 64);

/**
 * The configuration line, &php.ini; location, build date, Web
 *      Server, System and more.
 * @link http://www.php.net/manual/en/info.constants.php
 */
define ('CREDITS_ALL', 4294967295);
define ('HTML_SPECIALCHARS', 0);
define ('HTML_ENTITIES', 1);
define ('ENT_COMPAT', 2);
define ('ENT_QUOTES', 3);
define ('ENT_NOQUOTES', 0);
define ('ENT_IGNORE', 4);
define ('ENT_SUBSTITUTE', 8);
define ('ENT_DISALLOWED', 128);
define ('ENT_HTML401', 0);
define ('ENT_XML1', 16);
define ('ENT_XHTML', 32);
define ('ENT_HTML5', 48);
define ('STR_PAD_LEFT', 0);
define ('STR_PAD_RIGHT', 1);
define ('STR_PAD_BOTH', 2);
define ('PATHINFO_DIRNAME', 1);
define ('PATHINFO_BASENAME', 2);
define ('PATHINFO_EXTENSION', 4);

/**
 * Since PHP 5.2.0.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('PATHINFO_FILENAME', 8);
define ('CHAR_MAX', 127);
define ('LC_CTYPE', 0);
define ('LC_NUMERIC', 1);
define ('LC_TIME', 2);
define ('LC_COLLATE', 3);
define ('LC_MONETARY', 4);
define ('LC_ALL', 6);
define ('LC_MESSAGES', 5);
define ('SEEK_SET', 0);
define ('SEEK_CUR', 1);
define ('SEEK_END', 2);
define ('LOCK_SH', 1);
define ('LOCK_EX', 2);
define ('LOCK_UN', 3);
define ('LOCK_NB', 4);

/**
 * A connection with an external resource has been established.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_CONNECT', 2);

/**
 * Additional authorization is required to access the specified resource.
 *       Typical issued with severity level of
 *       STREAM_NOTIFY_SEVERITY_ERR.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_AUTH_REQUIRED', 3);

/**
 * Authorization has been completed (with or without success).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_AUTH_RESULT', 10);

/**
 * The mime-type of resource has been identified,
 *       refer to message for a description of the
 *       discovered type.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_MIME_TYPE_IS', 4);

/**
 * The size of the resource has been discovered.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_FILE_SIZE_IS', 5);

/**
 * The external resource has redirected the stream to an alternate
 *       location.  Refer to message.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_REDIRECTED', 6);

/**
 * Indicates current progress of the stream transfer in
 *       bytes_transferred and possibly
 *       bytes_max as well.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_PROGRESS', 7);

/**
 * A generic error occurred on the stream, consult
 *       message and message_code
 *       for details.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_FAILURE', 9);

/**
 * There is no more data available on the stream.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_COMPLETED', 8);

/**
 * A remote address required for this stream has been resolved, or the resolution
 *       failed.  See severity for an indication of which happened.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_RESOLVE', 1);

/**
 * Normal, non-error related, notification.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_SEVERITY_INFO', 0);

/**
 * Non critical error condition.  Processing may continue.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_SEVERITY_WARN', 1);

/**
 * A critical error occurred.  Processing cannot continue.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_NOTIFY_SEVERITY_ERR', 2);

/**
 * Used with stream_filter_append and
 *       stream_filter_prepend to indicate
 *       that the specified filter should only be applied when
 *       reading
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_FILTER_READ', 1);

/**
 * Used with stream_filter_append and
 *       stream_filter_prepend to indicate
 *       that the specified filter should only be applied when
 *       writing
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_FILTER_WRITE', 2);

/**
 * This constant is equivalent to 
 *       STREAM_FILTER_READ | STREAM_FILTER_WRITE
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_FILTER_ALL', 3);

/**
 * Client socket opened with stream_socket_client
 *       should remain persistent between page loads.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_CLIENT_PERSISTENT', 1);

/**
 * Open client socket asynchronously. This option must be used
 *       together with the STREAM_CLIENT_CONNECT flag.
 *       Used with stream_socket_client.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_CLIENT_ASYNC_CONNECT', 2);

/**
 * Open client socket connection. Client sockets should always
 *       include this flag. Used with stream_socket_client.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_CLIENT_CONNECT', 4);
define ('STREAM_CRYPTO_METHOD_ANY_CLIENT', 63);
define ('STREAM_CRYPTO_METHOD_SSLv2_CLIENT', 3);
define ('STREAM_CRYPTO_METHOD_SSLv3_CLIENT', 5);
define ('STREAM_CRYPTO_METHOD_SSLv23_CLIENT', 7);
define ('STREAM_CRYPTO_METHOD_TLS_CLIENT', 57);
define ('STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT', 9);
define ('STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT', 17);
define ('STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT', 33);
define ('STREAM_CRYPTO_METHOD_ANY_SERVER', 62);
define ('STREAM_CRYPTO_METHOD_SSLv2_SERVER', 2);
define ('STREAM_CRYPTO_METHOD_SSLv3_SERVER', 4);
define ('STREAM_CRYPTO_METHOD_SSLv23_SERVER', 6);
define ('STREAM_CRYPTO_METHOD_TLS_SERVER', 56);
define ('STREAM_CRYPTO_METHOD_TLSv1_0_SERVER', 8);
define ('STREAM_CRYPTO_METHOD_TLSv1_1_SERVER', 16);
define ('STREAM_CRYPTO_METHOD_TLSv1_2_SERVER', 32);

/**
 * Used with stream_socket_shutdown to disable
 *       further receptions. Added in PHP 5.2.1.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SHUT_RD', 0);

/**
 * Used with stream_socket_shutdown to disable
 *       further transmissions. Added in PHP 5.2.1.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SHUT_WR', 1);

/**
 * Used with stream_socket_shutdown to disable
 *       further receptions and transmissions. Added in PHP 5.2.1.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SHUT_RDWR', 2);

/**
 * Internet Protocol Version 4 (IPv4).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_PF_INET', 2);

/**
 * Internet Protocol Version 6 (IPv6).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_PF_INET6', 10);

/**
 * Unix system internal protocols.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_PF_UNIX', 1);

/**
 * Provides a IP socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_IPPROTO_IP', 0);

/**
 * Provides a TCP socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_IPPROTO_TCP', 6);

/**
 * Provides a UDP socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_IPPROTO_UDP', 17);

/**
 * Provides a ICMP socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_IPPROTO_ICMP', 1);

/**
 * Provides a RAW socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_IPPROTO_RAW', 255);

/**
 * Provides sequenced, two-way byte streams with a transmission mechanism
 *       for out-of-band data (TCP, for example).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SOCK_STREAM', 1);

/**
 * Provides datagrams, which are connectionless messages (UDP, for
 *       example).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SOCK_DGRAM', 2);

/**
 * Provides a raw socket, which provides access to internal network
 *       protocols and interfaces. Usually this type of socket is just available
 *       to the root user.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SOCK_RAW', 3);

/**
 * Provides a sequenced packet stream socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SOCK_SEQPACKET', 5);

/**
 * Provides a RDM (Reliably-delivered messages) socket.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SOCK_RDM', 4);
define ('STREAM_PEEK', 2);
define ('STREAM_OOB', 1);

/**
 * Tells a stream created with stream_socket_server
 *       to bind to the specified target.  Server sockets should always include this flag.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SERVER_BIND', 4);

/**
 * Tells a stream created with stream_socket_server
 *       and bound using the STREAM_SERVER_BIND flag to start
 *       listening on the socket. Connection-orientated transports (such as TCP)
 *       must use this flag, otherwise the server socket will not be enabled.
 *       Using this flag for connect-less transports (such as UDP) is an error.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_SERVER_LISTEN', 8);

/**
 * Search for filename in
 *     include_path (since PHP 5).
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_USE_INCLUDE_PATH', 1);

/**
 * Strip EOL characters (since PHP 5).
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_IGNORE_NEW_LINES', 2);

/**
 * Skip empty lines (since PHP 5).
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_SKIP_EMPTY_LINES', 4);

/**
 * Append content to existing file.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_APPEND', 8);
define ('FILE_NO_DEFAULT_CONTEXT', 16);

/**
 * <p>
 *     Text mode (since PHP 5.2.7).
 *     
 *      <p>
 *       This constant has no effect, and is only available for 
 *       forward compatibility.
 *      </p>
 *     
 *    </p>
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_TEXT', 0);

/**
 * <p>
 *     Binary mode (since PHP 5.2.7).
 *     
 *      <p>
 *       This constant has no effect, and is only available for 
 *       forward compatibility.
 *      </p>
 *     
 *    </p>
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FILE_BINARY', 0);

/**
 * Disable backslash escaping.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FNM_NOESCAPE', 2);

/**
 * Slash in string only matches slash in the given pattern.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FNM_PATHNAME', 1);

/**
 * Leading period in string must be exactly matched by period in the given pattern.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FNM_PERIOD', 4);

/**
 * Caseless match. Part of the GNU extension.
 * @link http://www.php.net/manual/en/filesystem.constants.php
 */
define ('FNM_CASEFOLD', 16);

/**
 * Return Code indicating that the
 *       userspace filter returned buckets in $out.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_PASS_ON', 2);

/**
 * Return Code indicating that the
 *       userspace filter did not return buckets in $out
 *       (i.e. No data available).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_FEED_ME', 1);

/**
 * Return Code indicating that the
 *       userspace filter encountered an unrecoverable error
 *       (i.e. Invalid data received).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_ERR_FATAL', 0);

/**
 * Regular read/write.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_FLAG_NORMAL', 0);

/**
 * An incremental flush.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_FLAG_FLUSH_INC', 1);

/**
 * Final flush prior to closing.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('PSFS_FLAG_FLUSH_CLOSE', 2);

/**
 * <p>
 *      The default algorithm to use for hashing if no algorithm is provided.
 *      This may change in newer PHP releases when newer, stronger hashing
 *      algorithms are supported.
 *     </p>
 *     <p>
 *      It is worth noting that over time this constant can (and likely will)
 *      change. Therefore you should be aware that the length of the resulting
 *      hash can change. Therefore, if you use PASSWORD_DEFAULT
 *      you should store the resulting hash in a way that can store more than 60
 *      characters (255 is the recomended width).
 *     </p>
 *     <p>
 *      Values for this constant:
 *     </p>
 *     
 *      
 *       
 *        PHP 5.5.0 - PASSWORD_BCRYPT
 * @link http://www.php.net/manual/en/password.constants.php
 */
define ('PASSWORD_DEFAULT', 1);

/**
 * <p>
 *      PASSWORD_BCRYPT is used to create new password
 *      hashes using the CRYPT_BLOWFISH algorithm.
 *     </p>
 *     <p>
 *      This will always result in a hash using the "$2y$" crypt format, 
 *      which is always 60 characters wide.
 *     </p>
 *     <p>
 *      Supported Options:
 *     </p>
 *     
 *      
 *       <p>
 *        salt - to manually provide a salt to use when hashing the password.
 *        Note that this will override and prevent a salt from being automatically generated.
 *       </p>
 *       <p>
 *        If omitted, a random salt will be generated by password_hash for
 *        each password hashed. This is the intended mode of operation.
 *       </p>
 * @link http://www.php.net/manual/en/password.constants.php
 */
define ('PASSWORD_BCRYPT', 1);
define ('PASSWORD_BCRYPT_DEFAULT_COST', 10);
define ('ABDAY_1', 131072);
define ('ABDAY_2', 131073);
define ('ABDAY_3', 131074);
define ('ABDAY_4', 131075);
define ('ABDAY_5', 131076);
define ('ABDAY_6', 131077);
define ('ABDAY_7', 131078);
define ('DAY_1', 131079);
define ('DAY_2', 131080);
define ('DAY_3', 131081);
define ('DAY_4', 131082);
define ('DAY_5', 131083);
define ('DAY_6', 131084);
define ('DAY_7', 131085);
define ('ABMON_1', 131086);
define ('ABMON_2', 131087);
define ('ABMON_3', 131088);
define ('ABMON_4', 131089);
define ('ABMON_5', 131090);
define ('ABMON_6', 131091);
define ('ABMON_7', 131092);
define ('ABMON_8', 131093);
define ('ABMON_9', 131094);
define ('ABMON_10', 131095);
define ('ABMON_11', 131096);
define ('ABMON_12', 131097);
define ('MON_1', 131098);
define ('MON_2', 131099);
define ('MON_3', 131100);
define ('MON_4', 131101);
define ('MON_5', 131102);
define ('MON_6', 131103);
define ('MON_7', 131104);
define ('MON_8', 131105);
define ('MON_9', 131106);
define ('MON_10', 131107);
define ('MON_11', 131108);
define ('MON_12', 131109);
define ('AM_STR', 131110);
define ('PM_STR', 131111);
define ('D_T_FMT', 131112);
define ('D_FMT', 131113);
define ('T_FMT', 131114);
define ('T_FMT_AMPM', 131115);
define ('ERA', 131116);
define ('ERA_D_T_FMT', 131120);
define ('ERA_D_FMT', 131118);
define ('ERA_T_FMT', 131121);
define ('ALT_DIGITS', 131119);
define ('CRNCYSTR', 262159);
define ('RADIXCHAR', 65536);
define ('THOUSEP', 65537);
define ('YESEXPR', 327680);
define ('NOEXPR', 327681);
define ('CODESET', 14);
define ('CRYPT_SALT_LENGTH', 123);
define ('CRYPT_STD_DES', 1);
define ('CRYPT_EXT_DES', 1);
define ('CRYPT_MD5', 1);
define ('CRYPT_BLOWFISH', 1);
define ('CRYPT_SHA256', 1);
define ('CRYPT_SHA512', 1);
define ('DIRECTORY_SEPARATOR', "/");

/**
 * Available since PHP 4.3.0. Semicolon on Windows, colon otherwise.
 * @link http://www.php.net/manual/en/dir.constants.php
 */
define ('PATH_SEPARATOR', ":");

/**
 * Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/dir.constants.php
 */
define ('SCANDIR_SORT_ASCENDING', 0);

/**
 * Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/dir.constants.php
 */
define ('SCANDIR_SORT_DESCENDING', 1);

/**
 * Available since PHP 5.4.0.
 * @link http://www.php.net/manual/en/dir.constants.php
 */
define ('SCANDIR_SORT_NONE', 2);
define ('GLOB_BRACE', 1024);
define ('GLOB_MARK', 2);
define ('GLOB_NOSORT', 4);
define ('GLOB_NOCHECK', 16);
define ('GLOB_NOESCAPE', 64);
define ('GLOB_ERR', 1);
define ('GLOB_ONLYDIR', 8192);
define ('GLOB_AVAILABLE_FLAGS', 9303);

/**
 * system is unusable
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_EMERG', 0);

/**
 * action must be taken immediately
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_ALERT', 1);

/**
 * critical conditions
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_CRIT', 2);

/**
 * error conditions
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_ERR', 3);

/**
 * warning conditions
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_WARNING', 4);

/**
 * normal, but significant, condition
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_NOTICE', 5);

/**
 * informational message
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_INFO', 6);

/**
 * debug-level message
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_DEBUG', 7);

/**
 * kernel messages
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_KERN', 0);

/**
 * generic user-level messages
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_USER', 8);

/**
 * mail subsystem
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_MAIL', 16);

/**
 * other system daemons
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_DAEMON', 24);

/**
 * security/authorization messages (use LOG_AUTHPRIV instead
 *       in systems where that constant is defined)
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_AUTH', 32);

/**
 * messages generated internally by syslogd
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_SYSLOG', 40);

/**
 * line printer subsystem
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_LPR', 48);

/**
 * USENET news subsystem
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_NEWS', 56);

/**
 * UUCP subsystem
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_UUCP', 64);

/**
 * clock daemon (cron and at)
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_CRON', 72);

/**
 * security/authorization messages (private)
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_AUTHPRIV', 80);
define ('LOG_LOCAL0', 128);
define ('LOG_LOCAL1', 136);
define ('LOG_LOCAL2', 144);
define ('LOG_LOCAL3', 152);
define ('LOG_LOCAL4', 160);
define ('LOG_LOCAL5', 168);
define ('LOG_LOCAL6', 176);
define ('LOG_LOCAL7', 184);

/**
 * include PID with each message
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_PID', 1);

/**
 * if there is an error while sending data to the system logger,
 *       write directly to the system console
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_CONS', 2);

/**
 * (default) delay opening the connection until the first
 *       message is logged
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_ODELAY', 4);

/**
 * open the connection to the logger immediately
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_NDELAY', 8);
define ('LOG_NOWAIT', 16);

/**
 * print log message also to standard error
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('LOG_PERROR', 32);
define ('EXTR_OVERWRITE', 0);
define ('EXTR_SKIP', 1);
define ('EXTR_PREFIX_SAME', 2);
define ('EXTR_PREFIX_ALL', 3);
define ('EXTR_PREFIX_INVALID', 4);
define ('EXTR_PREFIX_IF_EXISTS', 5);
define ('EXTR_IF_EXISTS', 6);
define ('EXTR_REFS', 256);

/**
 * SORT_ASC is used with
 *     array_multisort to sort in ascending order.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_ASC', 4);

/**
 * SORT_DESC is used with
 *     array_multisort to sort in descending order.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_DESC', 3);

/**
 * SORT_REGULAR is used to compare items normally.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_REGULAR', 0);

/**
 * SORT_NUMERIC is used to compare items numerically.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_NUMERIC', 1);

/**
 * SORT_STRING is used to compare items as strings.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_STRING', 2);

/**
 * SORT_LOCALE_STRING is used to compare items as
 *    strings, based on the current locale. Added in PHP 4.4.0 and 5.0.2.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_LOCALE_STRING', 5);

/**
 * SORT_NATURAL is used to compare items as
 *    strings using "natural ordering" like natsort. Added in PHP 5.4.0.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_NATURAL', 6);

/**
 * SORT_FLAG_CASE can be combined
 *         (bitwise OR) with
 *         SORT_STRING or
 *         SORT_NATURAL to sort strings case-insensitively. Added in PHP 5.4.0.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('SORT_FLAG_CASE', 8);

/**
 * CASE_LOWER is used with
 *     array_change_key_case and is used to convert array
 *     keys to lower case. This is also the default case for
 *     array_change_key_case.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('CASE_LOWER', 0);

/**
 * CASE_UPPER is used with
 *     array_change_key_case and is used to convert array
 *     keys to upper case.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('CASE_UPPER', 1);
define ('COUNT_NORMAL', 0);
define ('COUNT_RECURSIVE', 1);

/**
 * ARRAY_FILTER_USE_BOTH is used with
 *     array_filter to pass both value and key to the given callback function.
 *     Added in PHP 5.6.0.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('ARRAY_FILTER_USE_BOTH', 1);

/**
 * ARRAY_FILTER_USE_KEY is used with
 *     array_filter to pass each key as the first argument to the given callback function.
 *     Added in PHP 5.6.0.
 * @link http://www.php.net/manual/en/array.constants.php
 */
define ('ARRAY_FILTER_USE_KEY', 2);
define ('ASSERT_ACTIVE', 1);
define ('ASSERT_CALLBACK', 2);
define ('ASSERT_BAIL', 3);
define ('ASSERT_WARNING', 4);
define ('ASSERT_QUIET_EVAL', 5);

/**
 * Flag indicating if the stream
 *       used the include path.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_USE_PATH', 1);
define ('STREAM_IGNORE_URL', 2);

/**
 * Flag indicating if the wrapper
 *       is responsible for raising errors using trigger_error 
 *       during opening of the stream.  If this flag is not set, you
 *       should not raise any errors.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_REPORT_ERRORS', 8);

/**
 * This flag is useful when your extension really must be able to randomly
 *       seek around in a stream.  Some streams may not be seekable in their
 *       native form, so this flag asks the streams API to check to see if the
 *       stream does support seeking.  If it does not, it will copy the stream
 *       into temporary storage (which may be a temporary file or a memory
 *       stream) which does support seeking.
 *       Please note that this flag is not useful when you want to seek the
 *       stream and write to it, because the stream you are accessing might
 *       not be bound to the actual resource you requested.
 *      
 *      
 *       
 *        If the requested resource is network based, this flag will cause the
 *        opener to block until the whole contents have been downloaded.
 * @link http://www.php.net/manual/en/internals2.ze1.streams.constants.php
 */
define ('STREAM_MUST_SEEK', 16);
define ('STREAM_URL_STAT_LINK', 1);
define ('STREAM_URL_STAT_QUIET', 2);
define ('STREAM_MKDIR_RECURSIVE', 1);
define ('STREAM_IS_URL', 1);
define ('STREAM_OPTION_BLOCKING', 1);
define ('STREAM_OPTION_READ_TIMEOUT', 4);
define ('STREAM_OPTION_READ_BUFFER', 2);
define ('STREAM_OPTION_WRITE_BUFFER', 3);
define ('STREAM_BUFFER_NONE', 0);
define ('STREAM_BUFFER_LINE', 1);
define ('STREAM_BUFFER_FULL', 2);

/**
 * Stream casting, when stream_cast is called 
 *       otherwise (see above).
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_CAST_AS_STREAM', 0);

/**
 * Stream casting, for when stream_select is 
 *       calling stream_cast.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_CAST_FOR_SELECT', 3);

/**
 * Used with stream_metadata, to specify touch call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_TOUCH', 1);

/**
 * Used with stream_metadata, to specify chown call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_OWNER', 3);

/**
 * Used with stream_metadata, to specify chown call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_OWNER_NAME', 2);

/**
 * Used with stream_metadata, to specify chgrp call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_GROUP', 5);

/**
 * Used with stream_metadata, to specify chgrp call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_GROUP_NAME', 4);

/**
 * Used with stream_metadata, to specify chmod call.
 * @link http://www.php.net/manual/en/stream.constants.php
 */
define ('STREAM_META_ACCESS', 6);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_GIF', 1);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JPEG', 2);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_PNG', 3);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_SWF', 4);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_PSD', 5);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_BMP', 6);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_TIFF_II', 7);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_TIFF_MM', 8);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JPC', 9);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JP2', 10);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JPX', 11);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JB2', 12);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_SWC', 13);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_IFF', 14);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_WBMP', 15);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_JPEG2000', 9);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_XBM', 16);

/**
 * Image type constant used by the
 *     image_type_to_mime_type and
 *     image_type_to_extension functions.
 *     (Available as of PHP 5.3.0)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMAGETYPE_ICO', 17);
define ('IMAGETYPE_UNKNOWN', 0);
define ('IMAGETYPE_COUNT', 18);

/**
 * IPv4 Address Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_A', 1);

/**
 * Authoritative Name Server Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_NS', 2);

/**
 * Alias (Canonical Name) Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_CNAME', 16);

/**
 * Start of Authority Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_SOA', 32);

/**
 * Pointer Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_PTR', 2048);

/**
 * Host Info Resource (See IANA's
 *       Operating System Names
 *        for the meaning of these values)
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_HINFO', 4096);

/**
 * Mail Exchanger Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_MX', 16384);

/**
 * Text Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_TXT', 32768);
define ('DNS_SRV', 33554432);
define ('DNS_NAPTR', 67108864);

/**
 * IPv6 Address Resource
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_AAAA', 134217728);
define ('DNS_A6', 16777216);

/**
 * Any Resource Record.  On most systems
 *        this returns all resource records, however
 *        it should not be counted upon for critical
 *        uses. Try DNS_ALL instead.
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_ANY', 268435456);

/**
 * Iteratively query the name server for
 *        each available record type.
 * @link http://www.php.net/manual/en/network.constants.php
 */
define ('DNS_ALL', 251713587);

// End of standard v.5.6.0
?>
