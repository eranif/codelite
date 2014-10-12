<?php

// Start of readline v.5.6.0

/**
 * @param prompt[optional]
 */
function readline ($prompt) {}

/**
 * @param varname[optional]
 * @param newvalue[optional]
 */
function readline_info ($varname, $newvalue) {}

/**
 * @param prompt
 */
function readline_add_history ($prompt) {}

function readline_clear_history () {}

function readline_list_history () {}

/**
 * @param filename[optional]
 */
function readline_read_history ($filename) {}

/**
 * @param filename[optional]
 */
function readline_write_history ($filename) {}

/**
 * @param funcname
 */
function readline_completion_function ($funcname) {}

/**
 * @param prompt
 * @param callback
 */
function readline_callback_handler_install ($prompt, $callback) {}

function readline_callback_read_char () {}

function readline_callback_handler_remove () {}

function readline_redisplay () {}

define ('READLINE_LIB', "readline");

// End of readline v.5.6.0
?>
