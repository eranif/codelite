<?php

// Start of zlib v.1.1

/**
 * Output a gz-file
 * @link http://www.php.net/manual/en/function.readgzfile.php
 * @param filename string <p>
 *       The file name. This file will be opened from the filesystem and its
 *       contents written to standard output.
 *      </p>
 * @param use_include_path int[optional] <p>
 *       You can set this optional parameter to 1, if you
 *       want to search for the file in the include_path too.
 *      </p>
 * @return int the number of (uncompressed) bytes read from the file. If
 *   an error occurs, false is returned and unless the function was
 *   called as @readgzfile, an error message is
 *   printed.
 */
function readgzfile ($filename, $use_include_path = null) {}

/**
 * Rewind the position of a gz-file pointer
 * @link http://www.php.net/manual/en/function.gzrewind.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function gzrewind ($zp) {}

/**
 * Close an open gz-file pointer
 * @link http://www.php.net/manual/en/function.gzclose.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function gzclose ($zp) {}

/**
 * Test for <acronym>EOF</acronym> on a gz-file pointer
 * @link http://www.php.net/manual/en/function.gzeof.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return int true if the gz-file pointer is at EOF or an error occurs;
 *   otherwise returns false.
 */
function gzeof ($zp) {}

/**
 * Get character from gz-file pointer
 * @link http://www.php.net/manual/en/function.gzgetc.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return string The uncompressed character or false on EOF (unlike gzeof).
 */
function gzgetc ($zp) {}

/**
 * Get line from file pointer
 * @link http://www.php.net/manual/en/function.gzgets.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @param length int <p>
 *       The length of data to get.
 *      </p>
 * @return string The uncompressed string, or false on error.
 */
function gzgets ($zp, $length) {}

/**
 * Get line from gz-file pointer and strip HTML tags
 * @link http://www.php.net/manual/en/function.gzgetss.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @param length int <p>
 *       The length of data to get.
 *      </p>
 * @param allowable_tags string[optional] <p>
 *       You can use this optional parameter to specify tags which should not 
 *       be stripped.
 *      </p>
 * @return string The uncompressed and striped string, or false on error.
 */
function gzgetss ($zp, $length, $allowable_tags = null) {}

/**
 * Binary-safe gz-file read
 * @link http://www.php.net/manual/en/function.gzread.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @param length int <p>
 *       The number of bytes to read.
 *      </p>
 * @return string The data that have been read.
 */
function gzread ($zp, $length) {}

/**
 * Open gz-file
 * @link http://www.php.net/manual/en/function.gzopen.php
 * @param filename string <p>
 *       The file name.
 *      </p>
 * @param mode string <p>
 *       As in fopen (rb or 
 *       wb) but can also include a compression level 
 *       (wb9) or a strategy: f for
 *       filtered data as in wb6f, h for
 *       Huffman only compression as in wb1h.
 *       (See the description of deflateInit2
 *       in zlib.h for 
 *       more information about the strategy parameter.)
 *      </p>
 * @param use_include_path int[optional] <p>
 *       You can set this optional parameter to 1, if you
 *       want to search for the file in the include_path too.
 *      </p>
 * @return resource a file pointer to the file opened, after that, everything you read
 *   from this file descriptor will be transparently decompressed and what you 
 *   write gets compressed.
 *  </p>
 *  <p>
 *   If the open fails, the function returns false.
 */
function gzopen ($filename, $mode, $use_include_path = null) {}

/**
 * Output all remaining data on a gz-file pointer
 * @link http://www.php.net/manual/en/function.gzpassthru.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return int The number of uncompressed characters read from gz
 *   and passed through to the input, or false on error.
 */
function gzpassthru ($zp) {}

/**
 * Seek on a gz-file pointer
 * @link http://www.php.net/manual/en/function.gzseek.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @param offset int <p>
 *       The seeked offset.
 *      </p>
 * @param whence int[optional] <p>
 *       whence values are:
 *       
 *        SEEK_SET - Set position equal to offset bytes.
 *        SEEK_CUR - Set position to current location plus offset.
 *       
 *      </p>
 *      <p>
 *       If whence is not specified, it is assumed to be
 *       SEEK_SET.
 *      </p>
 * @return int Upon success, returns 0; otherwise, returns -1. Note that seeking
 *   past EOF is not considered an error.
 */
function gzseek ($zp, $offset, $whence = null) {}

/**
 * Tell gz-file pointer read/write position
 * @link http://www.php.net/manual/en/function.gztell.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @return int The position of the file pointer or false if an error occurs.
 */
function gztell ($zp) {}

/**
 * Binary-safe gz-file write
 * @link http://www.php.net/manual/en/function.gzwrite.php
 * @param zp resource <p>
 *       The gz-file pointer. It must be valid, and must point to a file
 *       successfully opened by gzopen.
 *      </p>
 * @param string string <p>
 *       The string to write.
 *      </p>
 * @param length int[optional] <p>
 *       The number of uncompressed bytes to write. If supplied, writing will 
 *       stop after length (uncompressed) bytes have been
 *       written or the end of string is reached,
 *       whichever comes first.
 *      </p>
 *      
 *       <p>
 *        Note that if the length argument is given,
 *        then the magic_quotes_runtime
 *        configuration option will be ignored and no slashes will be
 *        stripped from string.
 *       </p>
 * @return int the number of (uncompressed) bytes written to the given gz-file 
 *   stream.
 */
function gzwrite ($zp, $string, $length = null) {}

/**
 * &Alias; <function>gzwrite</function>
 * @link http://www.php.net/manual/en/function.gzputs.php
 * @param fp
 * @param str
 * @param length[optional]
 */
function gzputs ($fp, $str, $length) {}

/**
 * Read entire gz-file into an array
 * @link http://www.php.net/manual/en/function.gzfile.php
 * @param filename string <p>
 *       The file name.
 *      </p>
 * @param use_include_path int[optional] <p>
 *       You can set this optional parameter to 1, if you
 *       want to search for the file in the include_path too.
 *      </p>
 * @return array An array containing the file, one line per cell.
 */
function gzfile ($filename, $use_include_path = null) {}

/**
 * Compress a string
 * @link http://www.php.net/manual/en/function.gzcompress.php
 * @param data string <p>
 *       The data to compress.
 *      </p>
 * @param level int[optional] <p>
 *       The level of compression. Can be given as 0 for no compression up to 9
 *       for maximum compression.
 *      </p>
 * @return string The compressed string or false if an error occurred.
 */
function gzcompress ($data, $level = null) {}

/**
 * Uncompress a compressed string
 * @link http://www.php.net/manual/en/function.gzuncompress.php
 * @param data string <p>
 *       The data compressed by gzcompress.
 *      </p>
 * @param length int[optional] <p>
 *       The maximum length of data to decode.
 *      </p>
 * @return string The original uncompressed data or false on error.
 *  </p>
 *  <p>
 *   The function will return an error if the uncompressed data is more than
 *   32768 times the length of the compressed input data 
 *   or more than the optional parameter length.
 */
function gzuncompress ($data, $length = null) {}

/**
 * Deflate a string
 * @link http://www.php.net/manual/en/function.gzdeflate.php
 * @param data string <p>
 *       The data to deflate.
 *      </p>
 * @param level int[optional] <p>
 *       The level of compression. Can be given as 0 for no compression up to 9
 *       for maximum compression. If not given, the default compression level will
 *       be the default compression level of the zlib library.
 *      </p>
 * @return string The deflated string or false if an error occurred.
 */
function gzdeflate ($data, $level = null) {}

/**
 * Inflate a deflated string
 * @link http://www.php.net/manual/en/function.gzinflate.php
 * @param data string <p>
 *       The data compressed by gzdeflate.
 *      </p>
 * @param length int[optional] <p>
 *       The maximum length of data to decode.
 *      </p>
 * @return string The original uncompressed data or false on error.
 *  </p>
 *  <p>
 *   The function will return an error if the uncompressed data is more than
 *   32768 times the length of the compressed input data 
 *   or more than the optional parameter length.
 */
function gzinflate ($data, $length = null) {}

/**
 * Create a gzip compressed string
 * @link http://www.php.net/manual/en/function.gzencode.php
 * @param data string <p>
 *       The data to encode.
 *      </p>
 * @param level int[optional] <p>
 *       The level of compression. Can be given as 0 for no compression up to 9
 *       for maximum compression. If not given, the default compression level will
 *       be the default compression level of the zlib library.
 *      </p>
 * @param encoding_mode int[optional] <p>
 *       The encoding mode. Can be FORCE_GZIP (the default)
 *       or FORCE_DEFLATE. 
 *      </p>
 *      <p>
 *       If you use FORCE_DEFLATE, you get a standard zlib
 *       deflated string (inclusive zlib headers) after the gzip file header but
 *       without the trailing crc32 checksum.
 *      </p>
 * @return string The encoded string, or false if an error occurred.
 */
function gzencode ($data, $level = null, $encoding_mode = null) {}

/**
 * ob_start callback function to gzip output buffer
 * @link http://www.php.net/manual/en/function.ob-gzhandler.php
 * @param buffer string <p>
 *      </p>
 * @param mode int <p>
 *      </p>
 * @return string 
 */
function ob_gzhandler ($buffer, $mode) {}

/**
 * Returns the coding type used for output compression
 * @link http://www.php.net/manual/en/function.zlib-get-coding-type.php
 * @return string Possible return values are gzip, deflate,
 *   or false.
 */
function zlib_get_coding_type () {}

define ('FORCE_GZIP', 1);
define ('FORCE_DEFLATE', 2);

// End of zlib v.1.1
?>
