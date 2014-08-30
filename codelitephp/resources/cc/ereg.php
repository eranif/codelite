<?php

// Start of ereg v.

/**
 * Regular expression match
 * @link http://www.php.net/manual/en/function.ereg.php
 * @param pattern string <p>
 *       Case sensitive regular expression.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @param regs array[optional] <p>
 *       If matches are found for parenthesized substrings of
 *       pattern and the function is called with the
 *       third argument regs, the matches will be stored
 *       in the elements of the array regs. 
 *      </p>
 *      <p>
 *       $regs[1] will contain the substring which starts at
 *       the first left parenthesis; $regs[2] will contain
 *       the substring starting at the second, and so on.
 *       $regs[0] will contain a copy of the complete string
 *       matched.
 *      </p>
 * @return int the length of the matched string if a match for
 *   pattern was found in string,
 *   or false if no matches were found or an error occurred.
 *  </p>
 *  <p>
 *   If the optional parameter regs was not passed or
 *   the length of the matched string is 0, this function returns 1.
 */
function ereg ($pattern, $string, array &$regs = null) {}

/**
 * Replace regular expression
 * @link http://www.php.net/manual/en/function.ereg-replace.php
 * @param pattern string <p>
 *       A POSIX extended regular expression.
 *      </p>
 * @param replacement string <p>
 *       If pattern contains parenthesized substrings,
 *       replacement may contain substrings of the form
 *       \\digit, which will be
 *       replaced by the text matching the digit'th parenthesized substring; 
 *       \\0 will produce the entire contents of string.
 *       Up to nine substrings may be used. Parentheses may be nested, in which
 *       case they are counted by the opening parenthesis.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @return string The modified string is returned. If no matches are found in 
 *   string, then it will be returned unchanged.
 */
function ereg_replace ($pattern, $replacement, $string) {}

/**
 * Case insensitive regular expression match
 * @link http://www.php.net/manual/en/function.eregi.php
 * @param pattern string <p>
 *       Case insensitive regular expression.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @param regs array[optional] <p>
 *       If matches are found for parenthesized substrings of
 *       pattern and the function is called with the
 *       third argument regs, the matches will be stored
 *       in the elements of the array regs. 
 *      </p>
 *      <p>
 *       $regs[1] will contain the substring which starts at the first left
 *       parenthesis; $regs[2] will contain the substring starting at the
 *       second, and so on. $regs[0] will contain a copy of the complete string
 *       matched.
 *      </p>
 * @return int the length of the matched string if a match for
 *   pattern was found in string,
 *   or false if no matches were found or an error occurred.
 *  </p>
 *  <p>
 *   If the optional parameter regs was not passed or
 *   the length of the matched string is 0, this function returns 1.
 */
function eregi ($pattern, $string, array &$regs = null) {}

/**
 * Replace regular expression case insensitive
 * @link http://www.php.net/manual/en/function.eregi-replace.php
 * @param pattern string <p>
 *       A POSIX extended regular expression.
 *      </p>
 * @param replacement string <p>
 *       If pattern contains parenthesized substrings,
 *       replacement may contain substrings of the form
 *       \\digit, which will be
 *       replaced by the text matching the digit'th parenthesized substring; 
 *       \\0 will produce the entire contents of string.
 *       Up to nine substrings may be used. Parentheses may be nested, in which
 *       case they are counted by the opening parenthesis.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @return string The modified string is returned. If no matches are found in 
 *   string, then it will be returned unchanged.
 */
function eregi_replace ($pattern, $replacement, $string) {}

/**
 * Split string into array by regular expression
 * @link http://www.php.net/manual/en/function.split.php
 * @param pattern string <p>
 *       Case sensitive regular expression.
 *      </p>
 *      <p>
 *       If you want to split on any of the characters which are considered
 *       special by regular expressions, you'll need to escape them first. If
 *       you think split (or any other regex function, for
 *       that matter) is doing something weird, please read the file
 *       regex.7, included in the 
 *       regex/ subdirectory of the PHP distribution. It's
 *       in manpage format, so you'll want to do something along the lines of
 *       man /usr/local/src/regex/regex.7 in order to read it.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @param limit int[optional] <p>
 *       If limit is set, the returned array will
 *       contain a maximum of limit elements with the
 *       last element containing the whole rest of
 *       string.
 *      </p>
 * @return array an array of strings, each of which is a substring of
 *   string formed by splitting it on boundaries formed
 *   by the case-sensitive regular expression pattern.
 *  </p>
 *  <p>
 *   If there are n occurrences of
 *   pattern, the returned array will contain
 *   n+1 items. For example, if
 *   there is no occurrence of pattern, an array with
 *   only one element will be returned. Of course, this is also true if
 *   string is empty. If an error occurs,
 *   split returns false.
 */
function split ($pattern, $string, $limit = null) {}

/**
 * Split string into array by regular expression case insensitive
 * @link http://www.php.net/manual/en/function.spliti.php
 * @param pattern string <p>
 *       Case insensitive regular expression.
 *      </p>
 *      <p>
 *       If you want to split on any of the characters which are considered
 *       special by regular expressions, you'll need to escape them first. If
 *       you think spliti (or any other regex function, for
 *       that matter) is doing something weird, please read the file
 *       regex.7, included in the 
 *       regex/ subdirectory of the PHP distribution. It's
 *       in manpage format, so you'll want to do something along the lines of
 *       man /usr/local/src/regex/regex.7 in order to read it.
 *      </p>
 * @param string string <p>
 *       The input string.
 *      </p>
 * @param limit int[optional] <p>
 *       If limit is set, the returned array will
 *       contain a maximum of limit elements with the
 *       last element containing the whole rest of
 *       string.
 *      </p>
 * @return array an array of strings, each of which is a substring of
 *   string formed by splitting it on boundaries formed
 *   by the case insensitive regular expression pattern.
 *  </p>
 *  <p>
 *   If there are n occurrences of
 *   pattern, the returned array will contain
 *   n+1 items. For example, if
 *   there is no occurrence of pattern, an array with
 *   only one element will be returned. Of course, this is also true if
 *   string is empty. If an error occurs,
 *   spliti returns false.
 */
function spliti ($pattern, $string, $limit = null) {}

/**
 * Make regular expression for case insensitive match
 * @link http://www.php.net/manual/en/function.sql-regcase.php
 * @param string string <p>
 *       The input string.
 *      </p>
 * @return string a valid regular expression which will match
 *   string, ignoring case. This expression is
 *   string with each alphabetic character converted to
 *   a bracket expression; this bracket expression contains that character's
 *   uppercase and lowercase form.  Other characters remain unchanged.
 */
function sql_regcase ($string) {}

// End of ereg v.
?>
