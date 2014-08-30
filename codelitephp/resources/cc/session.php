<?php

// Start of session v.

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
 * @return bool Returns true on success or false on failure.
 */
function session_regenerate_id ($delete_old_session = null) {}

/**
 * Decodes session data from a string
 * @link http://www.php.net/manual/en/function.session-decode.php
 * @param data string <p>
 *       The encoded data to be stored.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function session_decode ($data) {}

/**
 * Register one or more global variables with the current session
 * @link http://www.php.net/manual/en/function.session-register.php
 * @param name mixed <p>
 *       A string holding the name of a variable or an array consisting of
 *       variable names or other arrays.
 *      </p>
 * @param _ mixed[optional] 
 * @return bool Returns true on success or false on failure.
 */
function session_register ($name, $_ = null) {}

/**
 * Unregister a global variable from the current session
 * @link http://www.php.net/manual/en/function.session-unregister.php
 * @param name string <p>
 *       The variable name.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function session_unregister ($name) {}

/**
 * Find out whether a global variable is registered in a session
 * @link http://www.php.net/manual/en/function.session-is-registered.php
 * @param name string <p>
 *       The variable name.
 *      </p>
 * @return bool session_is_registered returns true if there is a
 *   global variable with the name name registered in
 *   the current session, false otherwise.
 */
function session_is_registered ($name) {}

/**
 * Encodes the current session data as a string
 * @link http://www.php.net/manual/en/function.session-encode.php
 * @return string the contents of the current session encoded.
 */
function session_encode () {}

/**
 * Initialize session data
 * @link http://www.php.net/manual/en/function.session-start.php
 * @return bool This function returns true if a session was successfully started,
 *   otherwise false.
 */
function session_start () {}

/**
 * Destroys all data registered to a session
 * @link http://www.php.net/manual/en/function.session-destroy.php
 * @return bool Returns true on success or false on failure.
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
 * @param open callback <p>
 *       Open function, this works like a constructor in classes and is 
 *       executed when the session is being opened. The open function 
 *       expects two parameters, where the first is the save path and 
 *       the second is the session name.
 *      </p>
 * @param close callback <p>
 *       Close function, this works like a destructor in classes and is 
 *       executed when the session operation is done.
 *      </p>
 * @param read callback <p>
 *       Read function must return string value always to make save handler
 *       work as expected. Return empty string if there is no data to read.
 *       Return values from other handlers are converted to boolean expression.
 *       true for success, false for failure.
 *      </p>
 * @param write callback <p>
 *       Write function that is called when session data is to be saved. This
 *       function expects two parameters: an identifier and the data associated
 *       with it.
 *       
 *        <p>
 *         The "write" handler is not executed until after the output stream is
 *         closed.  Thus, output from debugging statements in the "write"
 *         handler will never be seen in the browser.  If debugging output is
 *         necessary, it is suggested that the debug output be written to a
 *         file instead.
 *        </p>
 *       
 *      </p>
 * @param destroy callback <p>
 *       The destroy handler, this is executed when a session is destroyed with 
 *       session_destroy and takes the session id as its 
 *       only parameter.
 *      </p>
 * @param gc callback <p>
 *       The garbage collector, this is executed when the session garbage collector 
 *       is executed and takes the max session lifetime as its only parameter.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function session_set_save_handler ($open, $close, $read, $write, $destroy, $gc) {}

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
 * &Alias; <function>session_write_close</function>
 * @link http://www.php.net/manual/en/function.session-commit.php
 */
function session_commit () {}

// End of session v.
?>
