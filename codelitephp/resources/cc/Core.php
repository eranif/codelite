<?php

// Start of Core v.5.6.0

class stdClass  {
}

interface Traversable  {
}

interface IteratorAggregate extends Traversable {

	abstract public function getIterator () ;

}

interface Iterator extends Traversable {

	abstract public function current () ;

	abstract public function next () ;

	abstract public function key () ;

	abstract public function valid () ;

	abstract public function rewind () ;

}

interface ArrayAccess  {

	/**
	 * @param offset
	 */
	abstract public function offsetExists ($offset) ;

	/**
	 * @param offset
	 */
	abstract public function offsetGet ($offset) ;

	/**
	 * @param offset
	 * @param value
	 */
	abstract public function offsetSet ($offset, $value) ;

	/**
	 * @param offset
	 */
	abstract public function offsetUnset ($offset) ;

}

interface Serializable  {

	abstract public function serialize () ;

	/**
	 * @param serialized
	 */
	abstract public function unserialize ($serialized) ;

}

class Exception  {
	protected $message;
	private $string;
	protected $code;
	protected $file;
	protected $line;
	private $trace;
	private $previous;


	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

class ErrorException extends Exception  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;
	protected $severity;


	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param severity[optional]
	 * @param filename[optional]
	 * @param lineno[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $severity, $filename, $lineno, $previous) {}

	final public function getSeverity () {}

	final private function __clone () {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

final class Closure  {

	private function __construct () {}

	/**
	 * @param closure
	 * @param newthis
	 * @param newscope[optional]
	 */
	public static function bind ($closure, $newthis, $newscope) {}

	/**
	 * @param newthis
	 * @param newscope[optional]
	 */
	public function bindTo ($newthis, $newscope) {}

}

final class Generator implements Iterator, Traversable {

	public function rewind () {}

	public function valid () {}

	public function current () {}

	public function key () {}

	public function next () {}

	/**
	 * @param value
	 */
	public function send ($value) {}

	/**
	 * @param exception
	 */
	public function throw ($exception) {}

	public function __wakeup () {}

}

function zend_version () {}

function func_num_args () {}

/**
 * @param arg_num
 */
function func_get_arg ($arg_num) {}

function func_get_args () {}

/**
 * @param str
 */
function strlen ($str) {}

/**
 * @param str1
 * @param str2
 */
function strcmp ($str1, $str2) {}

/**
 * @param str1
 * @param str2
 * @param len
 */
function strncmp ($str1, $str2, $len) {}

/**
 * @param str1
 * @param str2
 */
function strcasecmp ($str1, $str2) {}

/**
 * @param str1
 * @param str2
 * @param len
 */
function strncasecmp ($str1, $str2, $len) {}

/**
 * @param arr
 */
function each (&$arr) {}

/**
 * @param new_error_level[optional]
 */
function error_reporting ($new_error_level) {}

/**
 * @param constant_name
 * @param value
 * @param case_insensitive
 */
function define ($constant_name, $value, $case_insensitive) {}

/**
 * @param constant_name
 */
function defined ($constant_name) {}

/**
 * @param object[optional]
 */
function get_class ($object) {}

function get_called_class () {}

/**
 * @param object[optional]
 */
function get_parent_class ($object) {}

/**
 * @param object
 * @param method
 */
function method_exists ($object, $method) {}

/**
 * @param object_or_class
 * @param property_name
 */
function property_exists ($object_or_class, $property_name) {}

/**
 * @param classname
 * @param autoload[optional]
 */
function class_exists ($classname, $autoload) {}

/**
 * @param classname
 * @param autoload[optional]
 */
function interface_exists ($classname, $autoload) {}

/**
 * @param traitname
 * @param autoload[optional]
 */
function trait_exists ($traitname, $autoload) {}

/**
 * @param function_name
 */
function function_exists ($function_name) {}

/**
 * @param user_class_name
 * @param alias_name
 * @param autoload[optional]
 */
function class_alias ($user_class_name, $alias_name, $autoload) {}

function get_included_files () {}

function get_required_files () {}

/**
 * @param object
 * @param class_name
 * @param allow_string[optional]
 */
function is_subclass_of ($object, $class_name, $allow_string) {}

/**
 * @param object
 * @param class_name
 * @param allow_string[optional]
 */
function is_a ($object, $class_name, $allow_string) {}

/**
 * @param class_name
 */
function get_class_vars ($class_name) {}

/**
 * @param obj
 */
function get_object_vars ($obj) {}

/**
 * @param class
 */
function get_class_methods ($class) {}

/**
 * @param message
 * @param error_type[optional]
 */
function trigger_error ($message, $error_type) {}

/**
 * @param message
 * @param error_type[optional]
 */
function user_error ($message, $error_type) {}

/**
 * @param error_handler
 * @param error_types[optional]
 */
function set_error_handler ($error_handler, $error_types) {}

function restore_error_handler () {}

/**
 * @param exception_handler
 */
function set_exception_handler ($exception_handler) {}

function restore_exception_handler () {}

function get_declared_classes () {}

function get_declared_traits () {}

function get_declared_interfaces () {}

function get_defined_functions () {}

function get_defined_vars () {}

/**
 * @param args
 * @param code
 */
function create_function ($args, $code) {}

/**
 * @param res
 */
function get_resource_type ($res) {}

/**
 * @param zend_extensions[optional]
 */
function get_loaded_extensions ($zend_extensions) {}

/**
 * @param extension_name
 */
function extension_loaded ($extension_name) {}

/**
 * @param extension_name
 */
function get_extension_funcs ($extension_name) {}

/**
 * @param categorize[optional]
 */
function get_defined_constants ($categorize) {}

/**
 * @param options[optional]
 * @param limit[optional]
 */
function debug_backtrace ($options, $limit) {}

/**
 * @param options[optional]
 */
function debug_print_backtrace ($options) {}

function gc_collect_cycles () {}

function gc_enabled () {}

function gc_enable () {}

function gc_disable () {}


/**
 * Fatal run-time errors. These indicate errors that can not be
 *      recovered from, such as a memory allocation problem.
 *      Execution of the script is halted.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_ERROR', 1);

/**
 * Catchable fatal error. It indicates that a probably dangerous error
 *      occurred, but did not leave the Engine in an unstable state. If the error
 *      is not caught by a user defined handle (see also
 *      set_error_handler), the application aborts as it
 *      was an E_ERROR.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_RECOVERABLE_ERROR', 4096);

/**
 * Run-time warnings (non-fatal errors). Execution of the script is not
 *      halted.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_WARNING', 2);

/**
 * Compile-time parse errors. Parse errors should only be generated by
 *      the parser.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_PARSE', 4);

/**
 * Run-time notices. Indicate that the script encountered something that
 *      could indicate an error, but could also happen in the normal course of
 *      running a script.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_NOTICE', 8);

/**
 * Enable to have PHP suggest changes
 *      to your code which will ensure the best interoperability
 *      and forward compatibility of your code.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_STRICT', 2048);

/**
 * Run-time notices. Enable this to receive warnings about code
 *      that will not work in future versions.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_DEPRECATED', 8192);

/**
 * Fatal errors that occur during PHP's initial startup. This is like an
 *      E_ERROR, except it is generated by the core of PHP.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_CORE_ERROR', 16);

/**
 * Warnings (non-fatal errors) that occur during PHP's initial startup.
 *      This is like an E_WARNING, except it is generated
 *      by the core of PHP.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_CORE_WARNING', 32);

/**
 * Fatal compile-time errors. This is like an E_ERROR,
 *      except it is generated by the Zend Scripting Engine.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_COMPILE_ERROR', 64);

/**
 * Compile-time warnings (non-fatal errors). This is like an
 *      E_WARNING, except it is generated by the Zend
 *      Scripting Engine.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_COMPILE_WARNING', 128);

/**
 * User-generated error message. This is like an
 *      E_ERROR, except it is generated in PHP code by
 *      using the PHP function trigger_error.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_USER_ERROR', 256);

/**
 * User-generated warning message. This is like an
 *      E_WARNING, except it is generated in PHP code by
 *      using the PHP function trigger_error.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_USER_WARNING', 512);

/**
 * User-generated notice message. This is like an
 *      E_NOTICE, except it is generated in PHP code by
 *      using the PHP function trigger_error.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_USER_NOTICE', 1024);

/**
 * User-generated warning message. This is like an
 *      E_DEPRECATED, except it is generated in PHP code by
 *      using the PHP function trigger_error.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_USER_DEPRECATED', 16384);

/**
 * All errors and warnings, as supported, except of level
 *      E_STRICT prior to PHP 5.4.0.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_ALL', 32767);
define ('DEBUG_BACKTRACE_PROVIDE_OBJECT', 1);
define ('DEBUG_BACKTRACE_IGNORE_ARGS', 2);
define ('TRUE', true);
define ('FALSE', false);
define ('ZEND_THREAD_SAFE', false);
define ('ZEND_DEBUG_BUILD', false);
define ('NULL', null);
define ('PHP_VERSION', "5.6.0");
define ('PHP_MAJOR_VERSION', 5);
define ('PHP_MINOR_VERSION', 6);
define ('PHP_RELEASE_VERSION', 0);
define ('PHP_EXTRA_VERSION', "");
define ('PHP_VERSION_ID', 50600);
define ('PHP_ZTS', 0);
define ('PHP_DEBUG', 0);
define ('PHP_OS', "Linux");
define ('PHP_SAPI', "cli");
define ('DEFAULT_INCLUDE_PATH', ".:/usr/local/zend/share/pear");
define ('PEAR_INSTALL_DIR', "/usr/local/zend/share/pear");
define ('PEAR_EXTENSION_DIR', "/usr/local/zend/lib/php/20131226");
define ('PHP_EXTENSION_DIR', "/usr/local/zend/lib/php/20131226");
define ('PHP_PREFIX', "/usr/local/zend");
define ('PHP_BINDIR', "/usr/local/zend/bin");
define ('PHP_MANDIR', "/usr/local/zend/share/man");
define ('PHP_LIBDIR', "/usr/local/zend/lib/php");
define ('PHP_DATADIR', "${prefix}/share");
define ('PHP_SYSCONFDIR', "/usr/local/zend/etc");
define ('PHP_LOCALSTATEDIR', "/usr/local/zend/var");
define ('PHP_CONFIG_FILE_PATH', "/usr/local/zend/etc");
define ('PHP_CONFIG_FILE_SCAN_DIR', "/usr/local/zend/etc/conf.d");
define ('PHP_SHLIB_SUFFIX', "so");
define ('PHP_EOL', "\n");
define ('PHP_MAXPATHLEN', 4096);
define ('PHP_INT_MAX', 9223372036854775807);
define ('PHP_INT_SIZE', 8);
define ('PHP_BINARY', "/usr/local/zend/bin/php");

/**
 * <p>
 *     Indicates that output buffering has begun.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_START', 1);

/**
 * <p>
 *     Indicates that the output buffer is being flushed, and had data to output.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_WRITE', 0);

/**
 * <p>
 *     Indicates that the buffer has been flushed.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_FLUSH', 4);

/**
 * <p>
 *     Indicates that the output buffer has been cleaned.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_CLEAN', 2);

/**
 * <p>
 *     Indicates that this is the final output buffering operation.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_FINAL', 8);

/**
 * <p>
 *     Indicates that the buffer has been flushed, but output buffering will
 *     continue.
 *    </p>
 *    <p>
 *     As of PHP 5.4, this is an alias for
 *     PHP_OUTPUT_HANDLER_WRITE.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_CONT', 0);

/**
 * <p>
 *     Indicates that output buffering has ended.
 *    </p>
 *    <p>
 *     As of PHP 5.4, this is an alias for
 *     PHP_OUTPUT_HANDLER_FINAL.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_END', 8);

/**
 * <p>
 *     Controls whether an output buffer created by
 *     ob_start can be cleaned.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_CLEANABLE', 16);

/**
 * <p>
 *     Controls whether an output buffer created by
 *     ob_start can be flushed.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_FLUSHABLE', 32);

/**
 * <p>
 *     Controls whether an output buffer created by
 *     ob_start can be removed before the end of the script.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_REMOVABLE', 64);

/**
 * <p>
 *     The default set of output buffer flags; currently equivalent to
 *     PHP_OUTPUT_HANDLER_CLEANABLE |
 *     PHP_OUTPUT_HANDLER_FLUSHABLE |
 *     PHP_OUTPUT_HANDLER_REMOVABLE.
 *    </p>
 *    <p>
 *     Available since PHP 5.4.
 *    </p>
 * @link http://www.php.net/manual/en/outcontrol.constants.php
 */
define ('PHP_OUTPUT_HANDLER_STDFLAGS', 112);
define ('PHP_OUTPUT_HANDLER_STARTED', 4096);
define ('PHP_OUTPUT_HANDLER_DISABLED', 8192);
define ('UPLOAD_ERR_OK', 0);
define ('UPLOAD_ERR_INI_SIZE', 1);
define ('UPLOAD_ERR_FORM_SIZE', 2);
define ('UPLOAD_ERR_PARTIAL', 3);
define ('UPLOAD_ERR_NO_FILE', 4);
define ('UPLOAD_ERR_NO_TMP_DIR', 6);
define ('UPLOAD_ERR_CANT_WRITE', 7);
define ('UPLOAD_ERR_EXTENSION', 8);
define ('STDIN', "Resource id #1");
define ('STDOUT', "Resource id #2");
define ('STDERR', "Resource id #3");

// End of Core v.5.6.0
?>
