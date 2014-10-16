<?php

// Start of session v.

interface SessionHandlerInterface  {

	/**
	 * @param save_path
	 * @param session_name
	 */
	abstract public function open ($save_path, $session_name) ;

	abstract public function close () ;

	/**
	 * @param key
	 */
	abstract public function read ($key) ;

	/**
	 * @param key
	 * @param val
	 */
	abstract public function write ($key, $val) ;

	/**
	 * @param key
	 */
	abstract public function destroy ($key) ;

	/**
	 * @param maxlifetime
	 */
	abstract public function gc ($maxlifetime) ;

}

interface SessionIdInterface  {

	abstract public function create_sid () ;

}

class SessionHandler implements SessionHandlerInterface, SessionIdInterface {

	/**
	 * @param save_path
	 * @param session_name
	 */
	public function open ($save_path, $session_name) {}

	public function close () {}

	/**
	 * @param key
	 */
	public function read ($key) {}

	/**
	 * @param key
	 * @param val
	 */
	public function write ($key, $val) {}

	/**
	 * @param key
	 */
	public function destroy ($key) {}

	/**
	 * @param maxlifetime
	 */
	public function gc ($maxlifetime) {}

	public function create_sid () {}

}

/**
 * @param name[optional]
 */
function session_name ($name) {}

/**
 * @param module[optional]
 */
function session_module_name ($module) {}

/**
 * @param path[optional]
 */
function session_save_path ($path) {}

/**
 * @param id[optional]
 */
function session_id ($id) {}

/**
 * @param delete_old_session[optional]
 */
function session_regenerate_id ($delete_old_session) {}

/**
 * @param data
 */
function session_decode ($data) {}

function session_encode () {}

function session_start () {}

function session_destroy () {}

function session_unset () {}

/**
 * @param open
 * @param close[optional]
 * @param read[optional]
 * @param write[optional]
 * @param destroy[optional]
 * @param gc[optional]
 * @param create_sid[optional]
 */
function session_set_save_handler ($open, $close, $read, $write, $destroy, $gc, $create_sid) {}

/**
 * @param cache_limiter[optional]
 */
function session_cache_limiter ($cache_limiter) {}

/**
 * @param new_cache_expire[optional]
 */
function session_cache_expire ($new_cache_expire) {}

/**
 * @param lifetime
 * @param path[optional]
 * @param domain[optional]
 * @param secure[optional]
 * @param httponly[optional]
 */
function session_set_cookie_params ($lifetime, $path, $domain, $secure, $httponly) {}

function session_get_cookie_params () {}

function session_write_close () {}

function session_abort () {}

function session_reset () {}

function session_status () {}

function session_register_shutdown () {}

function session_commit () {}


/**
 * Since PHP 5.4.0. Return value of session_status if sessions are disabled.
 * @link http://www.php.net/manual/en/session.constants.php
 */
define ('PHP_SESSION_DISABLED', 0);

/**
 * Since PHP 5.4.0. Return value of session_status if sessions are enabled,
 *     but no session exists.
 * @link http://www.php.net/manual/en/session.constants.php
 */
define ('PHP_SESSION_NONE', 1);

/**
 * Since PHP 5.4.0. Return value of session_status if sessions are enabled,
 *     and a session exists.
 * @link http://www.php.net/manual/en/session.constants.php
 */
define ('PHP_SESSION_ACTIVE', 2);

// End of session v.
?>
