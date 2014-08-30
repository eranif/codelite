<?php

// Start of readline v.

/**
 * Reads a line
 * @link http://www.php.net/manual/en/function.readline.php
 * @param prompt string[optional] <p>
 *       You may specify a string with which to prompt the user.
 *      </p>
 * @return string a single string from the user. The line returned has the ending
 *   newline removed.
 */
function readline ($prompt = null) {}

/**
 * Gets/sets various internal readline variables
 * @link http://www.php.net/manual/en/function.readline-info.php
 * @param varname string[optional] <p>
 *       A variable name.
 *      </p>
 * @param newvalue string[optional] <p>
 *       If provided, this will be the new value of the setting.
 *      </p>
 * @return mixed If called with no parameters, this function returns an array of
 *   values for all the setting readline uses.  The elements will
 *   be indexed by the following values: done, end, erase_empty_line,
 *   library_version, line_buffer, mark, pending_input, point, prompt,
 *   readline_name, and terminal_name.
 *  </p>
 *  <p>
 *   If called with one or two parameters, the old value is returned.
 */
function readline_info ($varname = null, $newvalue = null) {}

/**
 * Adds a line to the history
 * @link http://www.php.net/manual/en/function.readline-add-history.php
 * @param line string <p>
 *       The line to be added in the history.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function readline_add_history ($line) {}

/**
 * Clears the history
 * @link http://www.php.net/manual/en/function.readline-clear-history.php
 * @return bool Returns true on success or false on failure.
 */
function readline_clear_history () {}

/**
 * Lists the history
 * @link http://www.php.net/manual/en/function.readline-list-history.php
 * @return array an array of the entire command line history.  The elements are
 *   indexed by integers starting at zero.
 */
function readline_list_history () {}

/**
 * Reads the history
 * @link http://www.php.net/manual/en/function.readline-read-history.php
 * @param filename string[optional] <p>
 *       Path to the filename containing the command history.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function readline_read_history ($filename = null) {}

/**
 * Writes the history
 * @link http://www.php.net/manual/en/function.readline-write-history.php
 * @param filename string[optional] <p>
 *       Path to the saved file.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function readline_write_history ($filename = null) {}

/**
 * Registers a completion function
 * @link http://www.php.net/manual/en/function.readline-completion-function.php
 * @param function callback <p>
 *       You must supply the name of an existing function which accepts a
 *       partial command line and returns an array of possible matches.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function readline_completion_function ($function) {}

/**
 * Initializes the readline callback interface and terminal, prints the prompt and returns immediately
 * @link http://www.php.net/manual/en/function.readline-callback-handler-install.php
 * @param prompt string <p>
 *       The prompt message.
 *      </p>
 * @param callback callback <p>
 *       The callback function takes one parameter; the
 *       user input returned.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function readline_callback_handler_install ($prompt, $callback) {}

/**
 * Reads a character and informs the readline callback interface when a line is received
 * @link http://www.php.net/manual/en/function.readline-callback-read-char.php
 * @return void 
 */
function readline_callback_read_char () {}

/**
 * Removes a previously installed callback handler and restores terminal settings
 * @link http://www.php.net/manual/en/function.readline-callback-handler-remove.php
 * @return bool true if a previously installed callback handler was removed, or
 *   false if one could not be found.
 */
function readline_callback_handler_remove () {}

/**
 * Redraws the display
 * @link http://www.php.net/manual/en/function.readline-redisplay.php
 * @return void 
 */
function readline_redisplay () {}

/**
 * Inform readline that the cursor has moved to a new line
 * @link http://www.php.net/manual/en/function.readline-on-new-line.php
 * @return void 
 */
function readline_on_new_line () {}

// End of readline v.
?>
