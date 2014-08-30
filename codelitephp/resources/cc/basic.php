<?php

/**
 * Loads a PHP extension at runtime
 * @link http://www.php.net/manual/en/function.dl.php
 * @param library string <p>
 *       This parameter is only the filename of the
 *       extension to load which also depends on your platform. For example,
 *       the sockets extension (if compiled
 *       as a shared module, not the default!) would be called 
 *       sockets.so on Unix platforms whereas it is called
 *       php_sockets.dll on the Windows platform.
 *      </p>
 *      <p>
 *       The directory where the extension is loaded from depends on your
 *       platform:
 *      </p>
 *      <p>
 *       Windows - If not explicitly set in the &php.ini;, the extension is
 *       loaded from C:\php4\extensions\ (PHP 4) or 
 *       C:\php5\ (PHP 5) by default.
 *      </p>
 *      <p>
 *       Unix - If not explicitly set in the &php.ini;, the default extension
 *       directory depends on
 *       
 *        
 *         
 *          whether PHP has been built with --enable-debug
 *          or not
 * @return bool Returns true on success or false on failure. If the functionality of loading modules is not available
 *   or has been disabled (either by setting
 *   enable_dl off or by enabling &safemode;
 *   in &php.ini;) an E_ERROR is emitted
 *   and execution is stopped. If dl fails because the
 *   specified library couldn't be loaded, in addition to false an
 *   E_WARNING message is emitted.
 */
function dl ($library) {}


Notice: Undefined index: internal in /home/eran/doc-gen.php on line 97

/**
 * The full path and filename of the file.  If used inside an include,
 *         the name of the included file is returned.
 *         Since PHP 4.0.2, __FILE__ always contains an
 *         absolute path with symlinks resolved whereas in older versions it contained relative path
 *         under some circumstances.
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__FILE__', null);

/**
 * The current line number of the file.
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__LINE__', null);

/**
 * The class name. (Added in PHP 4.3.0)  As of PHP 5 this constant 
 *         returns the class name as it was declared (case-sensitive).  In PHP
 *         4 its value is always lowercased.  The class name includes the namespace
 *         it was declared in (e.g. Foo\Bar).
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__CLASS__', null);

/**
 * The function name. (Added in PHP 4.3.0)  As of PHP 5 this constant 
 *         returns the function name as it was declared (case-sensitive).  In
 *         PHP 4 its value is always lowercased.
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__FUNCTION__', null);

/**
 * The class method name. (Added in PHP 5.0.0)  The method name is
 *         returned as it was declared (case-sensitive).
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__METHOD__', null);

/**
 * The directory of the file.  If used inside an include,
 *         the directory of the included file is returned. This is equivalent
 *         to dirname(__FILE__). This directory name
 *         does not have a trailing slash unless it is the root directory.
 *         (Added in PHP 5.3.0.)
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__DIR__', null);

/**
 * The name of the current namespace (case-sensitive). This constant 
 *         is defined in compile-time (Added in PHP 5.3.0).
 * @link http://www.php.net/manual/en/language.constants.php
 */
define ('__NAMESPACE__', null);
?>
