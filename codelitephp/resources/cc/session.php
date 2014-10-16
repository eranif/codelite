<?php

// Start of session v.

interface SessionHandlerInterface  {

	/**
	 * Initialize session
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.open.php
	 * @param save_path string <p>
	 *      The path where to store/retrieve the session.
	 *     </p>
	 * @param name string <p>
	 *      The session name.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	abstract public function open ($save_path, $name) ;

	/**
	 * Close the session
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.close.php
	 * @return bool &returns.session.storage.retval;
	 */
	abstract public function close () ;

	/**
	 * Read session data
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.read.php
	 * @param session_id string <p>
	 *      The session id.
	 *     </p>
	 * @return string an encoded string of the read data. If nothing was read, it must return an empty string. Note this value is returned internally to PHP for processing.
	 */
	abstract public function read ($session_id) ;

	/**
	 * Write session data
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.write.php
	 * @param session_id string <p>
	 *      The session id.
	 *     </p>
	 * @param session_data string <p>
	 *      The encoded session data.  This data is the result of the PHP internally encoding the $_SESSION superglobal to a serialized
	 *      string and passing it as this parameter.  Please note sessions use an alternative serialization method.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	abstract public function write ($session_id, $session_data) ;

	/**
	 * Destroy a session
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.destroy.php
	 * @param session_id string <p>
	 *       The session ID being destroyed.
	 *      </p>
	 * @return bool &returns.session.storage.retval;
	 */
	abstract public function destroy ($session_id) ;

	/**
	 * Cleanup old sessions
	 * @link http://www.php.net/manual/en/sessionhandlerinterface.gc.php
	 * @param maxlifetime string <p>
	 *      Sessions that have not updated for the last maxlifetime seconds will be removed.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	abstract public function gc ($maxlifetime) ;

}

interface SessionIdInterface  {

	abstract public function create_sid () ;

}

class SessionHandler implements SessionHandlerInterface, SessionIdInterface {

	/**
	 * Initialize session
	 * @link http://www.php.net/manual/en/sessionhandler.open.php
	 * @param save_path string <p>
	 *      The path where to store/retrieve the session.
	 *     </p>
	 * @param session_id string <p>
	 *      The session id.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	public function open ($save_path, $session_id) {}

	/**
	 * Close the session
	 * @link http://www.php.net/manual/en/sessionhandler.close.php
	 * @return bool &returns.session.storage.retval;
	 */
	public function close () {}

	/**
	 * Read session data
	 * @link http://www.php.net/manual/en/sessionhandler.read.php
	 * @param session_id string <p>
	 *      The session id to read data for.
	 *     </p>
	 * @return string an encoded string of the read data. If nothing was read, it must return an empty string. Note this value is returned internally to PHP for processing.
	 */
	public function read ($session_id) {}

	/**
	 * Write session data
	 * @link http://www.php.net/manual/en/sessionhandler.write.php
	 * @param session_id string <p>
	 *      The session id.
	 *     </p>
	 * @param session_data string <p>
	 *      The encoded session data.  This data is the result of the PHP internally encoding the $_SESSION superglobal to a serialized
	 *      string and passing it as this parameter.  Please note sessions use an alternative serialization method.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	public function write ($session_id, $session_data) {}

	/**
	 * Destroy a session
	 * @link http://www.php.net/manual/en/sessionhandler.destroy.php
	 * @param session_id string <p>
	 *       The session ID being destroyed.
	 *      </p>
	 * @return bool &returns.session.storage.retval;
	 */
	public function destroy ($session_id) {}

	/**
	 * Cleanup old sessions
	 * @link http://www.php.net/manual/en/sessionhandler.gc.php
	 * @param maxlifetime int <p>
	 *      Sessions that have not updated for the last maxlifetime seconds will be removed.
	 *     </p>
	 * @return bool &returns.session.storage.retval;
	 */
	public function gc ($maxlifetime) {}

	/**
	 * Return a new session ID
	 * @link http://www.php.net/manual/en/sessionhandler.create-sid.php
	 * @return string A session ID valid for the default session handler.
	 */
	public function create_sid () {}

}

/**
 * Get and/or set the current session name
 * @link http://www.php.net/manual/en/function.session-name.php
 * @param name string[optional] <p>
 *       The session name references the name of the session, which is 
 *       used in cookies and URLs (e.g. PHPSESSID). It
 *       should contain only alphanumeric characters; it should be short and
 *       descriptive (i.e. for users with enabled cookie warnings).
 *       If name is specified, the name of the current
 *       session is changed to its value.
 *      </p>
 *      <p>
 *       
 *        <p>
 *         The session name can't consist of digits only, at least one letter
 *         must be present. Otherwise a new session id is generated every time.
 *        </p>
 *       
 *      </p>
 * @return string the name of the current session.
 */
function session_name ($name = null) {}

/**
 * Get and/or set the current session module
 * @link http://www.php.net/manual/en/function.session-module-name.php
 * @param module string[optional] <p>
 *       If module is specified, that module will be
 *       used instead.
 *      </p>
 * @return string the name of the current session module.
 */
function session_module_name ($module = null) {}

/**
 * Get and/or set the current session save path
 * @link http://www.php.net/manual/en/function.session-save-path.php
 * @param path string[optional] <p>
 *       Session data path. If specified, the path to which data is saved will
 *       be changed. session_save_path needs to be called
 *       before session_start for that purpose.
 *      </p>
 *      <p>
 *       
 *        <p>
 *         On some operating systems, you may want to specify a path on a
 *         filesystem that handles lots of small files efficiently. For example,
 *         on Linux, reiserfs may provide better performance than ext2fs.
 *        </p>
 *       
 *      </p>
 * @return string the path of the current directory used for data storage.
 */
function session_save_path ($path = null) {}

/**
 * Get and/or set the current session id
 * @link http://www.php.net/manual/en/function.session-id.php
 * @param id string[optional] <p>
 *        If id is specified, it will replace the current
 *        session id. session_id needs to be called before
 *        session_start for that purpose. Depending on the
 *        session handler, not all characters are allowed within the session id.
 *        For example, the file session handler only allows characters in the
 *        range a-z A-Z 0-9 , (comma) and - (minus)!
 *       </p>
 *       
 *        
 *         When using session cookies, specifying an id
 *         for session_id will always send a new cookie
 *         when session_start is called, regardless if the
 *         current session id is identical to the one being set.
 * @return string session_id returns the session id for the current
 *   session or the empty string ("") if there is no current
 *   session (no current session id exists).
 */
function session_id ($id = null) {}

/**
 * Update the current session id with a newly generated one
 * @link http://www.php.net/manual/en/function.session-regenerate-id.php
 * @param delete_old_session bool[optional] <p>
 *        Whether to delete the old associated session file or not.
 *       </p>
 * @return bool Returns true on success, false on failure.
 */
function session_regenerate_id ($delete_old_session = null) {}

/**
 * Decodes session data from a session encoded string
 * @link http://www.php.net/manual/en/function.session-decode.php
 * @param data string <p>
 *       The encoded data to be stored.
 *      </p>
 * @return bool Returns true on success, false on failure.
 */
function session_decode ($data) {}

/**
 * Encodes the current session data as a session encoded string
 * @link http://www.php.net/manual/en/function.session-encode.php
 * @return string the contents of the current session encoded.
 */
function session_encode () {}

/**
 * Start new or resume existing session
 * @link http://www.php.net/manual/en/function.session-start.php
 * @return bool This function returns true if a session was successfully started,
 *   otherwise false.
 */
function session_start () {}

/**
 * Destroys all data registered to a session
 * @link http://www.php.net/manual/en/function.session-destroy.php
 * @return bool Returns true on success, false on failure.
 */
function session_destroy () {}

/**
 * Free all session variables
 * @link http://www.php.net/manual/en/function.session-unset.php
 * @return void 
 */
function session_unset () {}

/**
 * Sets user-level session storage functions
 * @link http://www.php.net/manual/en/function.session-set-save-handler.php
 * @param open callable 
 * @param close callable 
 * @param read callable 
 * @param write callable 
 * @param destroy callable 
 * @param gc callable 
 * @param create_sid callable[optional] 
 * @return bool Returns true on success, false on failure.
 */
function session_set_save_handler ($open, $close, $read, $write, $destroy, $gc, $create_sid = null) {}

/**
 * Get and/or set the current cache limiter
 * @link http://www.php.net/manual/en/function.session-cache-limiter.php
 * @param cache_limiter string[optional] <p>
 *       If cache_limiter is specified, the name of the
 *       current cache limiter is changed to the new value.
 *      </p>
 *      <table>
 *       Possible values
 *       
 *        
 *         <tr valign="top">
 *          <td>Value</td>
 *          <td>Headers sent</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>public</td>
 *          <td>
 *           
 *           
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>private_no_expire</td>
 *          <td>
 *           
 *           
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>private</td>
 *          <td>
 *           
 *           
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>nocache</td>
 *          <td>
 *           
 *           
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return string the name of the current cache limiter.
 */
function session_cache_limiter ($cache_limiter = null) {}

/**
 * Return current cache expire
 * @link http://www.php.net/manual/en/function.session-cache-expire.php
 * @param new_cache_expire string[optional] <p>
 *       If new_cache_expire is given, the current cache
 *       expire is replaced with new_cache_expire.
 *      </p>
 *      <p>
 *       
 *        
 *         Setting new_cache_expire is of value only, if
 *         session.cache_limiter is set to a value
 *         different from nocache.
 *        
 *       
 *      </p>
 * @return int the current setting of session.cache_expire.
 *   The value returned should be read in minutes, defaults to 180.
 */
function session_cache_expire ($new_cache_expire = null) {}

/**
 * Set the session cookie parameters
 * @link http://www.php.net/manual/en/function.session-set-cookie-params.php
 * @param lifetime int <p>
 *       Lifetime of the
 *       session cookie, defined in seconds.
 *      </p>
 * @param path string[optional] <p>
 *       Path on the domain where
 *       the cookie will work. Use a single slash ('/') for all paths on the
 *       domain.
 *      </p>
 * @param domain string[optional] <p>
 *       Cookie domain, for
 *       example 'www.php.net'. To make cookies visible on all subdomains then
 *       the domain must be prefixed with a dot like '.php.net'.
 *      </p>
 * @param secure bool[optional] <p>
 *       If true cookie will only be sent over
 *       secure connections.
 *      </p>
 * @param httponly bool[optional] <p>
 *       If set to true then PHP will attempt to send the
 *       httponly
 *       flag when setting the session cookie.
 *      </p>
 * @return void 
 */
function session_set_cookie_params ($lifetime, $path = null, $domain = null, $secure = null, $httponly = null) {}

/**
 * Get the session cookie parameters
 * @link http://www.php.net/manual/en/function.session-get-cookie-params.php
 * @return array an array with the current session cookie information, the array
 *   contains the following items:
 *   
 *    
 *     
 *      "lifetime" -  The
 *      lifetime of the cookie in seconds.
 *     
 *    
 *    
 *     
 *      "path" -  The path where
 *      information is stored.
 *     
 *    
 *    
 *     
 *      "domain" -  The domain
 *      of the cookie.
 *     
 *    
 *    
 *     
 *      "secure" -  The cookie
 *      should only be sent over secure connections.
 *     
 *    
 *    
 *     
 *      "httponly" - The
 *      cookie can only be accessed through the HTTP protocol.
 */
function session_get_cookie_params () {}

/**
 * Write session data and end session
 * @link http://www.php.net/manual/en/function.session-write-close.php
 * @return void 
 */
function session_write_close () {}

/**
 * Discard session array changes and finish session
 * @link http://www.php.net/manual/en/function.session-abort.php
 * @return bool This function returns true if a session was successfully reinitialized,
 *   otherwise false.
 */
function session_abort () {}

/**
 * Re-initialize session array with original values
 * @link http://www.php.net/manual/en/function.session-reset.php
 * @return bool This function returns true if a session was successfully reinitialized,
 *   otherwise false.
 */
function session_reset () {}

/**
 * Returns the current session status
 * @link http://www.php.net/manual/en/function.session-status.php
 */
function session_status () {}

/**
 * Session shutdown function
 * @link http://www.php.net/manual/en/function.session-register-shutdown.php
 * @return void 
 */
function session_register_shutdown () {}

/**
 * &Alias; <function>session_write_close</function>
 * @link http://www.php.net/manual/en/function.session-commit.php
 */
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
