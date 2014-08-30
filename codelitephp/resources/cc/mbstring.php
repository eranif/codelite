<?php

// Start of mbstring v.

/**
 * Perform case folding on a string
 * @link http://www.php.net/manual/en/function.mb-convert-case.php
 * @param str string <p>
 *       The string being converted.
 *      </p>
 * @param mode int <p>
 *       The mode of the conversion. It can be one of 
 *       MB_CASE_UPPER, 
 *       MB_CASE_LOWER, or 
 *       MB_CASE_TITLE.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string A case folded version of string converted in the
 *   way specified by mode.
 */
function mb_convert_case ($str, $mode, $encoding = null) {}

/**
 * Make a string uppercase
 * @link http://www.php.net/manual/en/function.mb-strtoupper.php
 * @param str string <p>
 *       The string being uppercased.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string str with all alphabetic characters converted to uppercase.
 */
function mb_strtoupper ($str, $encoding = null) {}

/**
 * Make a string lowercase
 * @link http://www.php.net/manual/en/function.mb-strtolower.php
 * @param str string <p>
 *       The string being lowercased.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string str with all alphabetic characters converted to lowercase.
 */
function mb_strtolower ($str, $encoding = null) {}

/**
 * Set/Get current language
 * @link http://www.php.net/manual/en/function.mb-language.php
 * @param language string[optional] <p>
 *       Used for encoding
 *       e-mail messages. Valid languages are "Japanese",
 *       "ja","English","en" and "uni"
 *       (UTF-8). mb_send_mail uses this setting to
 *       encode e-mail.
 *      </p>
 *      <p> 
 *       Language and its setting is ISO-2022-JP/Base64 for
 *       Japanese, UTF-8/Base64 for uni, ISO-8859-1/quoted printable for
 *       English.
 *      </p>
 * @return mixed If language is set and
 *   language is valid, it returns
 *   true. Otherwise, it returns false. 
 *   When language is omitted, it returns the language
 *   name as a string. If no language is set previously, it then returns
 *   false.
 */
function mb_language ($language = null) {}

/**
 * Set/Get internal character encoding
 * @link http://www.php.net/manual/en/function.mb-internal-encoding.php
 * @param encoding string[optional] <p>
 *       encoding is the character encoding name 
 *       used for the HTTP input character encoding conversion, HTTP output 
 *       character encoding conversion, and the default character encoding 
 *       for string functions defined by the mbstring module.
 *      </p>
 * @return mixed If encoding is set, then 
 *   Returns true on success or false on failure.
 *   If encoding is omitted, then 
 *   the current character encoding name is returned.
 */
function mb_internal_encoding ($encoding = null) {}

/**
 * Detect HTTP input character encoding
 * @link http://www.php.net/manual/en/function.mb-http-input.php
 * @param type string[optional] <p>
 *       Input string specifies the input type. 
 *       "G" for GET, "P" for POST, "C" for COOKIE, "S" for string, "L" for list, and
 *       "I" for the whole list (will return array). 
 *       If type is omitted, it returns the last input type processed. 
 *      </p>
 * @return mixed The character encoding name, as per the type.
 *   If mb_http_input does not process specified
 *   HTTP input, it returns false.
 */
function mb_http_input ($type = null) {}

/**
 * Set/Get HTTP output character encoding
 * @link http://www.php.net/manual/en/function.mb-http-output.php
 * @param encoding string[optional] <p>
 *       If encoding is set,
 *       mb_http_output sets the HTTP output character
 *       encoding to encoding.
 *      </p>
 *      <p>
 *       If encoding is omitted,
 *       mb_http_output returns the current HTTP output
 *       character encoding.
 *      </p>
 * @return mixed If encoding is omitted,
 *   mb_http_output returns the current HTTP output
 *   character encoding. Otherwise, 
 *   Returns true on success or false on failure.
 */
function mb_http_output ($encoding = null) {}

/**
 * Set/Get character encoding detection order
 * @link http://www.php.net/manual/en/function.mb-detect-order.php
 * @param encoding_list mixed[optional] <p>
 *       encoding_list is an array or 
 *       comma separated list of character encoding. See supported encodings.
 *      </p>
 *      <p>
 *       If encoding_list is omitted, it returns
 *       the current character encoding detection order as array.
 *      </p>
 *      <p>
 *       This setting affects mb_detect_encoding and
 *       mb_send_mail.
 *      </p>
 *      <p>
 *       mbstring currently implements the following
 *       encoding detection filters. If there is an invalid byte sequence
 *       for the following encodings, encoding detection will fail.
 *      </p>
 *      
 *       UTF-8, UTF-7,
 *       ASCII,
 *       EUC-JP,SJIS,
 *       eucJP-win, SJIS-win,
 *       JIS, ISO-2022-JP 
 *      
 *      <p>
 *       For ISO-8859-*, mbstring
 *       always detects as ISO-8859-*.
 *      </p>
 *      <p>
 *       For UTF-16, UTF-32,
 *       UCS2 and UCS4, encoding
 *       detection will fail always.
 *      </p>
 *      <p>
 *       
 *        Useless detect order example
 *        
 *        
 *       
 *      </p>
 * @return mixed Returns true on success or false on failure.
 */
function mb_detect_order ($encoding_list = null) {}

/**
 * Set/Get substitution character
 * @link http://www.php.net/manual/en/function.mb-substitute-character.php
 * @param substrchar mixed[optional] <p>
 *       Specify the Unicode value as an integer, 
 *       or as one of the following strings:
 *       
 *        
 *         
 *          "none" : no output
 * @return mixed If substchar is set, it returns true for success, 
 *   otherwise returns false. 
 *   If substchar is not set, it returns the Unicode value, 
 *   or "none" or "long".
 */
function mb_substitute_character ($substrchar = null) {}

/**
 * Parse GET/POST/COOKIE data and set global variable
 * @link http://www.php.net/manual/en/function.mb-parse-str.php
 * @param encoded_string string <p>
 *       The URL encoded data.
 *      </p>
 * @param result array[optional] <p>
 *       An array containing decoded and character encoded converted values.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function mb_parse_str ($encoded_string, array &$result = null) {}

/**
 * Callback function converts character encoding in output buffer
 * @link http://www.php.net/manual/en/function.mb-output-handler.php
 * @param contents string <p>
 *       The contents of the output buffer.
 *      </p>
 * @param status int <p>
 *       The status of the output buffer.
 *      </p>
 * @return string The converted string.
 */
function mb_output_handler ($contents, $status) {}

/**
 * Get MIME charset string
 * @link http://www.php.net/manual/en/function.mb-preferred-mime-name.php
 * @param encoding string <p>
 *       The encoding being checked.
 *      </p>
 * @return string The MIME charset string for character encoding
 *   encoding.
 */
function mb_preferred_mime_name ($encoding) {}

/**
 * Get string length
 * @link http://www.php.net/manual/en/function.mb-strlen.php
 * @param str string <p>
 *       The string being checked for length.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return int the number of characters in
 *   string str having character encoding
 *   encoding. A multi-byte character is
 *   counted as 1.
 */
function mb_strlen ($str, $encoding = null) {}

/**
 * Find position of first occurrence of string in a string
 * @link http://www.php.net/manual/en/function.mb-strpos.php
 * @param haystack string <p>
 *       The string being checked.
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack. In contrast
 *       with strpos, numeric values are not applied
 *       as the ordinal value of a character.
 *      </p>
 * @param offset int[optional] <p>
 *       The search offset. If it is not specified, 0 is used.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return int the numeric position of
 *   the first occurrence of needle in the
 *   haystack string. If
 *   needle is not found, it returns false.
 */
function mb_strpos ($haystack, $needle, $offset = null, $encoding = null) {}

/**
 * Find position of last occurrence of a string in a string
 * @link http://www.php.net/manual/en/function.mb-strrpos.php
 * @param haystack string <p>
 *       The string being checked, for the last occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack.
 *      </p>
 * @param offset int[optional] May be specified to begin searching an arbitrary number of characters into
 *       the string.  Negative values will stop searching at an arbitrary point
 *       prior to the end of the string.
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return int the numeric position of
 *   the last occurrence of needle in the
 *   haystack string. If
 *   needle is not found, it returns false.
 */
function mb_strrpos ($haystack, $needle, $offset = null, $encoding = null) {}

/**
 * Finds position of first occurrence of a string within another, case insensitive
 * @link http://www.php.net/manual/en/function.mb-stripos.php
 * @param haystack string <p>
 *       The string from which to get the position of the first occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param offset int[optional] <p>
 *       The position in haystack
 *       to start searching
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return int Return the numeric position of the first occurrence of
 *   needle in the haystack
 *   string, or false if needle is not found.
 */
function mb_stripos ($haystack, $needle, $offset = null, $encoding = null) {}

/**
 * Finds position of last occurrence of a string within another, case insensitive
 * @link http://www.php.net/manual/en/function.mb-strripos.php
 * @param haystack string <p>
 *       The string from which to get the position of the last occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param offset int[optional] <p>
 *       The position in haystack
 *       to start searching
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return int Return the numeric position of
 *   the last occurrence of needle in the
 *   haystack string, or false
 *   if needle is not found.
 */
function mb_strripos ($haystack, $needle, $offset = null, $encoding = null) {}

/**
 * Finds first occurrence of a string within another
 * @link http://www.php.net/manual/en/function.mb-strstr.php
 * @param haystack string <p>
 *       The string from which to get the first occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param part bool[optional] <p>
 *       Determines which portion of haystack
 *       this function returns. 
 *       If set to true, it returns all of  haystack
 *       from the beginning to the first occurrence of needle.
 *       If set to false, it returns all of haystack
 *       from the first occurrence of needle to the end,
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return string the portion of haystack,
 *   or false if needle is not found.
 */
function mb_strstr ($haystack, $needle, $part = null, $encoding = null) {}

/**
 * Finds the last occurrence of a character in a string within another
 * @link http://www.php.net/manual/en/function.mb-strrchr.php
 * @param haystack string <p>
 *       The string from which to get the last occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param part bool[optional] <p>
 *       Determines which portion of haystack
 *       this function returns. 
 *       If set to true, it returns all of haystack
 *       from the beginning to the last occurrence of needle.
 *       If set to false, it returns all of haystack
 *       from the last occurrence of needle to the end,
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return string the portion of haystack.
 *   or false if needle is not found.
 */
function mb_strrchr ($haystack, $needle, $part = null, $encoding = null) {}

/**
 * Finds first occurrence of a string within another, case insensitive
 * @link http://www.php.net/manual/en/function.mb-stristr.php
 * @param haystack string <p>
 *       The string from which to get the first occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param part bool[optional] <p>
 *       Determines which portion of haystack
 *       this function returns. 
 *       If set to true, it returns all of  haystack
 *       from the beginning to the first occurrence of needle.
 *       If set to false, it returns all of haystack
 *       from the first occurrence of needle to the end,
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return string the portion of haystack,
 *   or false if needle is not found.
 */
function mb_stristr ($haystack, $needle, $part = null, $encoding = null) {}

/**
 * Finds the last occurrence of a character in a string within another, case insensitive
 * @link http://www.php.net/manual/en/function.mb-strrichr.php
 * @param haystack string <p>
 *       The string from which to get the last occurrence
 *       of needle
 *      </p>
 * @param needle string <p>
 *       The string to find in haystack
 *      </p>
 * @param part bool[optional] <p>
 *       Determines which portion of haystack
 *       this function returns. 
 *       If set to true, it returns all of haystack
 *       from the beginning to the last occurrence of needle.
 *       If set to false, it returns all of haystack
 *       from the last occurrence of needle to the end,
 *      </p>
 * @param encoding string[optional] <p>
 *       Character encoding name to use.
 *       If it is omitted, internal character encoding is used.
 *      </p>
 * @return string the portion of haystack.
 *   or false if needle is not found.
 */
function mb_strrichr ($haystack, $needle, $part = null, $encoding = null) {}

/**
 * Count the number of substring occurrences
 * @link http://www.php.net/manual/en/function.mb-substr-count.php
 * @param haystack string <p>
 *       The string being checked.
 *      </p>
 * @param needle string <p>
 *       The string being found.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return int The number of times the
 *   needle substring occurs in the
 *   haystack string.
 */
function mb_substr_count ($haystack, $needle, $encoding = null) {}

/**
 * Get part of string
 * @link http://www.php.net/manual/en/function.mb-substr.php
 * @param str string <p>
 *       The string to extract the substring from.
 *      </p>
 * @param start int <p>
 *       Position of first character to use from str.
 *      </p>
 * @param length int[optional] <p>
 *       Maximum number of characters to use from str.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string mb_substr returns the portion of
 *   str specified by the
 *   start and
 *   length parameters.
 */
function mb_substr ($str, $start, $length = null, $encoding = null) {}

/**
 * Get part of string
 * @link http://www.php.net/manual/en/function.mb-strcut.php
 * @param str string <p>
 *       The string being cut.
 *      </p>
 * @param start int <p>
 *       Starting position in bytes.
 *      </p>
 * @param length int[optional] <p>
 *       Length in bytes.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string mb_strcut returns the portion of
 *   str specified by the
 *   start and
 *   length parameters.
 */
function mb_strcut ($str, $start, $length = null, $encoding = null) {}

/**
 * Return width of string
 * @link http://www.php.net/manual/en/function.mb-strwidth.php
 * @param str string <p>
 *       The string being decoded.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return int The width of string str.
 */
function mb_strwidth ($str, $encoding = null) {}

/**
 * Get truncated string with specified width
 * @link http://www.php.net/manual/en/function.mb-strimwidth.php
 * @param str string <p>
 *       The string being decoded.
 *      </p>
 * @param start int <p>
 *       The start position offset. Number of
 *       characters from the beginning of string. (First character is 0)
 *      </p>
 * @param width int <p>
 *       The width of the desired trim.
 *      </p>
 * @param trimmarker string[optional] <p>
 *       A string that is added to the end of string 
 *       when string is truncated.
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string The truncated string. If trimmarker is set,
 *   trimmarker is appended to the return value.
 */
function mb_strimwidth ($str, $start, $width, $trimmarker = null, $encoding = null) {}

/**
 * Convert character encoding
 * @link http://www.php.net/manual/en/function.mb-convert-encoding.php
 * @param str string <p>
 *       The string being encoded.
 *      </p>
 * @param to_encoding string <p>
 *       The type of encoding that str is being converted to.
 *      </p>
 * @param from_encoding mixed[optional] <p>
 *       Is specified by character code names before conversion. It is either
 *       an array, or a comma separated enumerated list.
 *       If from_encoding is not specified, the internal 
 *       encoding will be used.
 *       
 *      </p>
 *      <p>
 *      See supported
 *      encodings.
 *      </p>
 * @return string The encoded string.
 */
function mb_convert_encoding ($str, $to_encoding, $from_encoding = null) {}

/**
 * Detect character encoding
 * @link http://www.php.net/manual/en/function.mb-detect-encoding.php
 * @param str string <p>
 *       The string being detected.
 *      </p>
 * @param encoding_list mixed[optional] <p>
 *       encoding_list is list of character
 *       encoding. Encoding order may be specified by array or comma
 *       separated list string.
 *      </p>
 *      <p>
 *       If encoding_list is omitted,
 *       detect_order is used.
 *      </p>
 * @param strict bool[optional] <p>
 *       strict specifies whether to use
 *       the strict encoding detection or not.
 *       Default is false.
 *      </p>
 * @return string The detected character encoding or false if the encoding cannot be
 *   detected from the given string.
 */
function mb_detect_encoding ($str, $encoding_list = null, $strict = null) {}

/**
 * Returns an array of all supported encodings
 * @link http://www.php.net/manual/en/function.mb-list-encodings.php
 * @return array a numerically indexed array.
 */
function mb_list_encodings () {}

/**
 * Get aliases of a known encoding type
 * @link http://www.php.net/manual/en/function.mb-encoding-aliases.php
 * @param encoding string <p>
 *      The encoding type being checked, for aliases.
 *     </p>
 * @return array a numerically indexed array of encoding aliases on success,
 *   &return.falseforfailure;
 */
function mb_encoding_aliases ($encoding) {}

/**
 * Convert "kana" one from another ("zen-kaku", "han-kaku" and more)
 * @link http://www.php.net/manual/en/function.mb-convert-kana.php
 * @param str string <p>
 *       The string being converted.
 *      </p>
 * @param option string[optional] <p>
 *       The conversion option.
 *      </p>
 *      <p>
 *       Specify with a combination of following options.
 *       <table>
 *        Applicable Conversion Options
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Meaning</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>r</td>
 *           <td>
 *            Convert "zen-kaku" alphabets to "han-kaku"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>R</td>
 *           <td>
 *            Convert "han-kaku" alphabets to "zen-kaku"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>n</td>
 *           <td>
 *            Convert "zen-kaku" numbers to "han-kaku"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>N</td>
 *           <td>
 *            Convert "han-kaku" numbers to "zen-kaku"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>a</td>
 *           <td>
 *            Convert "zen-kaku" alphabets and numbers to "han-kaku"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>A</td>
 *           <td>
 *            Convert "han-kaku" alphabets and numbers to "zen-kaku"
 *            (Characters included in "a", "A" options are
 *            U+0021 - U+007E excluding U+0022, U+0027, U+005C, U+007E)
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>s</td>
 *           <td>
 *            Convert "zen-kaku" space to "han-kaku" (U+3000 -> U+0020)
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>S</td>
 *           <td>
 *            Convert "han-kaku" space to "zen-kaku" (U+0020 -> U+3000)
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>k</td>
 *           <td>
 *            Convert "zen-kaku kata-kana" to "han-kaku kata-kana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>K</td>
 *           <td>
 *            Convert "han-kaku kata-kana" to "zen-kaku kata-kana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>h</td>
 *           <td>
 *            Convert "zen-kaku hira-gana" to "han-kaku kata-kana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>H</td>
 *           <td>
 *            Convert "han-kaku kata-kana" to "zen-kaku hira-gana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>c</td>
 *           <td>
 *           Convert "zen-kaku kata-kana" to "zen-kaku hira-gana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>C</td>
 *           <td>
 *            Convert "zen-kaku hira-gana" to "zen-kaku kata-kana"
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>V</td>
 *           <td>
 *            Collapse voiced sound notation and convert them into a character. Use with "K","H"
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return string The converted string.
 */
function mb_convert_kana ($str, $option = null, $encoding = null) {}

/**
 * Encode string for MIME header
 * @link http://www.php.net/manual/en/function.mb-encode-mimeheader.php
 * @param str string <p>
 *       The string being encoded.
 *      </p>
 * @param charset string[optional] <p>
 *       charset specifies the name of the character set
 *       in which str is represented in. The default value
 *       is determined by the current NLS setting (mbstring.language).
 *       mb_internal_encoding should be set to same encoding.
 *      </p>
 * @param transfer_encoding string[optional] <p>
 *       transfer_encoding specifies the scheme of MIME
 *       encoding. It should be either "B" (Base64) or
 *       "Q" (Quoted-Printable). Falls back to
 *       "B" if not given.
 *      </p>
 * @param linefeed string[optional] <p>
 *       linefeed specifies the EOL (end-of-line) marker
 *       with which mb_encode_mimeheader performs
 *       line-folding (a RFC term,
 *       the act of breaking a line longer than a certain length into multiple
 *       lines. The length is currently hard-coded to 74 characters).
 *       Falls back to "\r\n" (CRLF) if not given.
 *      </p>
 * @param indent int[optional] <p>
 *       Indentation of the first line (number of characters in the header
 *       before str).
 *      </p>
 * @return string A converted version of the string represented in ASCII.
 */
function mb_encode_mimeheader ($str, $charset = null, $transfer_encoding = null, $linefeed = null, $indent = null) {}

/**
 * Decode string in MIME header field
 * @link http://www.php.net/manual/en/function.mb-decode-mimeheader.php
 * @param str string <p>
 *       The string being decoded.
 *      </p>
 * @return string The decoded string in internal character encoding.
 */
function mb_decode_mimeheader ($str) {}

/**
 * Convert character code in variable(s)
 * @link http://www.php.net/manual/en/function.mb-convert-variables.php
 * @param to_encoding string <p>
 *       The encoding that the string is being converted to.
 *      </p>
 * @param from_encoding mixed <p>
 *       from_encoding is specified as an array 
 *       or comma separated string, it tries to detect encoding from
 *       from-coding. When from_encoding 
 *       is omitted, detect_order is used.
 *      </p>
 * @param vars mixed <p>
 *       vars is the reference to the
 *       variable being converted. String, Array and Object are accepted.
 *       mb_convert_variables assumes all parameters
 *       have the same encoding.
 *      </p>
 * @param _ mixed[optional] 
 * @return string The character encoding before conversion for success, 
 *   or false for failure.
 */
function mb_convert_variables ($to_encoding, $from_encoding, &$vars, &$_ = null) {}

/**
 * Encode character to HTML numeric string reference
 * @link http://www.php.net/manual/en/function.mb-encode-numericentity.php
 * @param str string <p>
 *       The string being encoded.
 *      </p>
 * @param convmap array <p>
 *       convmap is array specifies code area to
 *       convert.
 *      </p>
 * @param encoding string &mbstring.encoding.parameter;
 * @return string The converted string.
 */
function mb_encode_numericentity ($str, array $convmap, $encoding) {}

/**
 * Decode HTML numeric string reference to character
 * @link http://www.php.net/manual/en/function.mb-decode-numericentity.php
 * @param str string <p>
 *       The string being decoded.
 *      </p>
 * @param convmap array <p>
 *       convmap is an array that specifies 
 *       the code area to convert.
 *      </p>
 * @param encoding string &mbstring.encoding.parameter;
 * @return string The converted string.
 */
function mb_decode_numericentity ($str, array $convmap, $encoding) {}

/**
 * Send encoded mail
 * @link http://www.php.net/manual/en/function.mb-send-mail.php
 * @param to string <p>
 *       The mail addresses being sent to. Multiple
 *       recipients may be specified by putting a comma between each
 *       address in to. 
 *       This parameter is not automatically encoded.
 *      </p>
 * @param subject string <p>
 *       The subject of the mail.
 *      </p>
 * @param message string <p>
 *       The message of the mail.
 *      </p>
 * @param additional_headers string[optional] <p>
 *       additional_headers is inserted at
 *       the end of the header. This is typically used to add extra
 *       headers.  Multiple extra headers are separated with a
 *       newline ("\n").
 *      </p>
 * @param additional_parameter string[optional] <p>
 *       additional_parameter is a MTA command line
 *       parameter. It is useful when setting the correct Return-Path
 *       header when using sendmail.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function mb_send_mail ($to, $subject, $message, $additional_headers = null, $additional_parameter = null) {}

/**
 * Get internal settings of mbstring
 * @link http://www.php.net/manual/en/function.mb-get-info.php
 * @param type string[optional] <p>
 *       If type isn't specified or is specified to
 *       "all", an array having the elements "internal_encoding",
 *       "http_output", "http_input", "func_overload", "mail_charset",
 *       "mail_header_encoding", "mail_body_encoding" will be returned. 
 *      </p>
 *      <p>
 *       If type is specified as "http_output",
 *       "http_input", "internal_encoding", "func_overload",
 *       the specified setting parameter will be returned.
 *      </p>
 * @return mixed An array of type information if type 
 *   is not specified, otherwise a specific type.
 */
function mb_get_info ($type = null) {}

/**
 * Check if the string is valid for the specified encoding
 * @link http://www.php.net/manual/en/function.mb-check-encoding.php
 * @param var string[optional] <p>
 *       The byte stream to check. If it is omitted, this function checks
 *       all the input from the beginning of the request.
 *      </p>
 * @param encoding string[optional] <p>
 *       The expected encoding.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function mb_check_encoding ($var = null, $encoding = null) {}

/**
 * Returns current encoding for multibyte regex as string
 * @link http://www.php.net/manual/en/function.mb-regex-encoding.php
 * @param encoding string[optional] &mbstring.encoding.parameter;
 * @return mixed 
 */
function mb_regex_encoding ($encoding = null) {}

/**
 * Set/Get the default options for mbregex functions
 * @link http://www.php.net/manual/en/function.mb-regex-set-options.php
 * @param options string[optional] <p>
 *       The options to set. This is a a string where each 
 *       character is an option. To set a mode, the mode 
 *       character must be the last one set, however there 
 *       can only be set one mode but multiple options.
 *      </p>
 *      <table>
 *       Regex options
 *       
 *        
 *         <tr valign="top">
 *          <td>Option</td>
 *          <td>Meaning</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>i</td>
 *          <td>Ambiguity match on</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>x</td>
 *          <td>Enables extended pattern form</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>m</td>
 *          <td>'.' matches with newlines</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>s</td>
 *          <td>'^' -> '\A', '$' -> '\Z'</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>p</td>
 *          <td>Same as both the m and s options</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>l</td>
 *          <td>Finds longest matches</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>n</td>
 *          <td>Ignores empty matches</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>e</td>
 *          <td>eval resulting code</td>
 *         </tr>
 *        
 *       
 *      </table>
 *      <table>
 *       Regex syntax modes
 *       
 *        
 *         <tr valign="top">
 *          <td>Mode</td>
 *          <td>Meaning</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>j</td>
 *          <td>Java (Sun java.util.regex)</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>u</td>
 *          <td>GNU regex</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>g</td>
 *          <td>grep</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>c</td>
 *          <td>Emacs</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>r</td>
 *          <td>Ruby</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>z</td>
 *          <td>Perl</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>b</td>
 *          <td>POSIX Basic regex</td>
 *         </tr>
 *         <tr valign="top">
 *          <td>d</td>
 *          <td>POSIX Extended regex</td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return string The previous options. If options is omitted, 
 *   it returns the string that describes the current options.
 */
function mb_regex_set_options ($options = null) {}

/**
 * Regular expression match with multibyte support
 * @link http://www.php.net/manual/en/function.mb-ereg.php
 * @param pattern string <p>
 *       The search pattern.
 *      </p>
 * @param string string <p>
 *       The search string.
 *      </p>
 * @param regs array[optional] <p>
 *       Contains a substring of the matched string.
 *      </p>
 * @return int 
 */
function mb_ereg ($pattern, $string, array $regs = null) {}

/**
 * Regular expression match ignoring case with multibyte support
 * @link http://www.php.net/manual/en/function.mb-eregi.php
 * @param pattern string <p>
 *       The regular expression pattern.
 *      </p>
 * @param string string <p>
 *       The string being searched.
 *      </p>
 * @param regs array[optional] <p>
 *       Contains a substring of the matched string.
 *      </p>
 * @return int 
 */
function mb_eregi ($pattern, $string, array $regs = null) {}

/**
 * Replace regular expression with multibyte support
 * @link http://www.php.net/manual/en/function.mb-ereg-replace.php
 * @param pattern string <p>
 *       The regular expression pattern.
 *      </p>
 *      <p>
 *       Multibyte characters may be used in pattern.
 *      </p>
 * @param replacement string <p>
 *       The replacement text.
 *      </p>
 * @param string string <p>
 *       The string being checked.
 *      </p>
 * @param option string[optional] Matching condition can be set by option
 *       parameter. If i is specified for this
 *       parameter, the case will be ignored. If x is
 *       specified, white space will be ignored. If m
 *       is specified, match will be executed in multiline mode and line
 *       break will be included in '.'. If p is
 *       specified, match will be executed in POSIX mode, line break 
 *       will be considered as normal character. If e
 *       is specified, replacement string will be
 *       evaluated as PHP expression.
 * @return string The resultant string on success, or false on error.
 */
function mb_ereg_replace ($pattern, $replacement, $string, $option = null) {}

/**
 * Replace regular expression with multibyte support ignoring case
 * @link http://www.php.net/manual/en/function.mb-eregi-replace.php
 * @param pattern string <p>
 *       The regular expression pattern.  Multibyte characters may be used. The case will be ignored.
 *      </p>
 * @param replace string <p>
 *       The replacement text.
 *      </p>
 * @param string string <p>
 *       The searched string.
 *      </p>
 * @param option string[optional] option has the same meaning as in
 *       mb_ereg_replace.
 * @return string The resultant string or false on error.
 */
function mb_eregi_replace ($pattern, $replace, $string, $option = null) {}

/**
 * Split multibyte string using regular expression
 * @link http://www.php.net/manual/en/function.mb-split.php
 * @param pattern string <p>
 *       The regular expression pattern.
 *      </p>
 * @param string string <p>
 *       The string being split.
 *      </p>
 * @param limit int[optional] If optional parameter limit is specified, 
 *       it will be split in limit elements as
 *       maximum.
 * @return array The result as an array.
 */
function mb_split ($pattern, $string, $limit = null) {}

/**
 * Regular expression match for multibyte string
 * @link http://www.php.net/manual/en/function.mb-ereg-match.php
 * @param pattern string <p>
 *       The regular expression pattern.
 *      </p>
 * @param string string <p>
 *       The string being evaluated.
 *      </p>
 * @param option string[optional] <p>
 *       
 *      </p>
 * @return bool 
 */
function mb_ereg_match ($pattern, $string, $option = null) {}

/**
 * Multibyte regular expression match for predefined multibyte string
 * @link http://www.php.net/manual/en/function.mb-ereg-search.php
 * @param pattern string[optional] <p>
 *       The search pattern.
 *      </p>
 * @param option string[optional] <p>
 *       The search option.
 *      </p>
 * @return bool 
 */
function mb_ereg_search ($pattern = null, $option = null) {}

/**
 * Returns position and length of a matched part of the multibyte regular expression for a predefined multibyte string
 * @link http://www.php.net/manual/en/function.mb-ereg-search-pos.php
 * @param pattern string[optional] <p>
 *       The search pattern.
 *      </p>
 * @param option string[optional] <p>
 *       The search option.
 *      </p>
 * @return array 
 */
function mb_ereg_search_pos ($pattern = null, $option = null) {}

/**
 * Returns the matched part of a multibyte regular expression
 * @link http://www.php.net/manual/en/function.mb-ereg-search-regs.php
 * @param pattern string[optional] <p>
 *       The search pattern.
 *      </p>
 * @param option string[optional] <p>
 *       The search option.
 *      </p>
 * @return array 
 */
function mb_ereg_search_regs ($pattern = null, $option = null) {}

/**
 * Setup string and regular expression for a multibyte regular expression match
 * @link http://www.php.net/manual/en/function.mb-ereg-search-init.php
 * @param string string <p>
 *       The search string.
 *      </p>
 * @param pattern string[optional] <p>
 *       The search pattern.
 *      </p>
 * @param option string[optional] <p>
 *       The search option.
 *      </p>
 * @return bool 
 */
function mb_ereg_search_init ($string, $pattern = null, $option = null) {}

/**
 * Retrieve the result from the last multibyte regular expression match
 * @link http://www.php.net/manual/en/function.mb-ereg-search-getregs.php
 * @return array 
 */
function mb_ereg_search_getregs () {}

/**
 * Returns start point for next regular expression match
 * @link http://www.php.net/manual/en/function.mb-ereg-search-getpos.php
 * @return int 
 */
function mb_ereg_search_getpos () {}

/**
 * Set start point of next regular expression match
 * @link http://www.php.net/manual/en/function.mb-ereg-search-setpos.php
 * @param position int <p>
 *       The position to set.
 *      </p>
 * @return bool 
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
