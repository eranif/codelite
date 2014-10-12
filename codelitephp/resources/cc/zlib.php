<?php

// Start of zlib v.2.0

/**
 * @param filename
 * @param use_include_path[optional]
 */
function readgzfile ($filename, $use_include_path) {}

/**
 * @param fp
 */
function gzrewind ($fp) {}

/**
 * @param fp
 */
function gzclose ($fp) {}

/**
 * @param fp
 */
function gzeof ($fp) {}

/**
 * @param fp
 */
function gzgetc ($fp) {}

/**
 * @param fp
 * @param length[optional]
 */
function gzgets ($fp, $length) {}

/**
 * @param fp
 * @param length[optional]
 * @param allowable_tags[optional]
 */
function gzgetss ($fp, $length, $allowable_tags) {}

/**
 * @param fp
 * @param length
 */
function gzread ($fp, $length) {}

/**
 * @param filename
 * @param mode
 * @param use_include_path[optional]
 */
function gzopen ($filename, $mode, $use_include_path) {}

/**
 * @param fp
 */
function gzpassthru ($fp) {}

/**
 * @param fp
 * @param offset
 * @param whence[optional]
 */
function gzseek ($fp, $offset, $whence) {}

/**
 * @param fp
 */
function gztell ($fp) {}

/**
 * @param fp
 * @param str
 * @param length[optional]
 */
function gzwrite ($fp, $str, $length) {}

/**
 * @param fp
 * @param str
 * @param length[optional]
 */
function gzputs ($fp, $str, $length) {}

/**
 * @param filename
 * @param use_include_path[optional]
 */
function gzfile ($filename, $use_include_path) {}

/**
 * @param data
 * @param level[optional]
 * @param encoding[optional]
 */
function gzcompress ($data, $level, $encoding) {}

/**
 * @param data
 * @param max_decoded_len[optional]
 */
function gzuncompress ($data, $max_decoded_len) {}

/**
 * @param data
 * @param level[optional]
 * @param encoding[optional]
 */
function gzdeflate ($data, $level, $encoding) {}

/**
 * @param data
 * @param max_decoded_len[optional]
 */
function gzinflate ($data, $max_decoded_len) {}

/**
 * @param data
 * @param level[optional]
 * @param encoding[optional]
 */
function gzencode ($data, $level, $encoding) {}

/**
 * @param data
 * @param max_decoded_len[optional]
 */
function gzdecode ($data, $max_decoded_len) {}

/**
 * @param data
 * @param encoding
 * @param level[optional]
 */
function zlib_encode ($data, $encoding, $level) {}

/**
 * @param data
 * @param max_decoded_len[optional]
 */
function zlib_decode ($data, $max_decoded_len) {}

function zlib_get_coding_type () {}

/**
 * @param data
 * @param flags
 */
function ob_gzhandler ($data, $flags) {}

define ('FORCE_GZIP', 31);
define ('FORCE_DEFLATE', 15);
define ('ZLIB_ENCODING_RAW', -15);
define ('ZLIB_ENCODING_GZIP', 31);
define ('ZLIB_ENCODING_DEFLATE', 15);

// End of zlib v.2.0
?>
