<?php

// Start of Core v.5.3.8-ZS5.5.0

class stdClass  {
}

interface Traversable  {
}

interface IteratorAggregate extends Traversable {

	abstract public function getIterator () {}

}

interface Iterator extends Traversable {

	abstract public function current () {}

	abstract public function next () {}

	abstract public function key () {}

	abstract public function valid () {}

	abstract public function rewind () {}

}

interface ArrayAccess  {

	/**
	 * @param offset
	 */
	abstract public function offsetExists ($offset) {}

	/**
	 * @param offset
	 */
	abstract public function offsetGet ($offset) {}

	/**
	 * @param offset
	 * @param value
	 */
	abstract public function offsetSet ($offset, $value) {}

	/**
	 * @param offset
	 */
	abstract public function offsetUnset ($offset) {}

}

interface Serializable  {

	abstract public function serialize () {}

	/**
	 * @param serialized
	 */
	abstract public function unserialize ($serialized) {}

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

}

/**
 * Gets the version of the current Zend engine
 * @link http://www.php.net/manual/en/function.zend-version.php
 * @return string the Zend Engine version number, as a string.
 */
function zend_version () {}

/**
 * Returns the number of arguments passed to the function
 * @link http://www.php.net/manual/en/function.func-num-args.php
 * @return int the number of arguments passed into the current user-defined
 *   function.
 */
function func_num_args () {}

/**
 * Return an item from the argument list
 * @link http://www.php.net/manual/en/function.func-get-arg.php
 * @param arg_num int <p>
 *       The argument offset. Function arguments are counted starting from
 *       zero.
 *      </p>
 * @return mixed the specified argument, or false on error.
 */
function func_get_arg ($arg_num) {}

/**
 * Returns an array comprising a function's argument list
 * @link http://www.php.net/manual/en/function.func-get-args.php
 * @return array an array in which each element is a copy of the corresponding
 *   member of the current user-defined function's argument list.
 */
function func_get_args () {}

/**
 * Get string length
 * @link http://www.php.net/manual/en/function.strlen.php
 * @param string string <p>
 *       The string being measured for length.
 *      </p>
 * @return int The length of the string on success, 
 *   and 0 if the string is empty.
 */
function strlen ($string) {}

/**
 * Binary safe string comparison
 * @link http://www.php.net/manual/en/function.strcmp.php
 * @param str1 string <p>
 *       The first string.
 *      </p>
 * @param str2 string <p>
 *       The second string.
 *      </p>
 * @return int < 0 if str1 is less than
 *   str2; > 0 if str1
 *   is greater than str2, and 0 if they are
 *   equal.
 */
function strcmp ($str1, $str2) {}

/**
 * Binary safe string comparison of the first n characters
 * @link http://www.php.net/manual/en/function.strncmp.php
 * @param str1 string <p>
 *       The first string.
 *      </p>
 * @param str2 string <p>
 *       The second string.
 *      </p>
 * @param len int <p>
 *       Number of characters to use in the comparison.
 *      </p>
 * @return int < 0 if str1 is less than
 *   str2; > 0 if str1
 *   is greater than str2, and 0 if they are
 *   equal.
 */
function strncmp ($str1, $str2, $len) {}

/**
 * Binary safe case-insensitive string comparison
 * @link http://www.php.net/manual/en/function.strcasecmp.php
 * @param str1 string <p>
 *       The first string
 *      </p>
 * @param str2 string <p>
 *       The second string
 *      </p>
 * @return int < 0 if str1 is less than
 *   str2; > 0 if str1
 *   is greater than str2, and 0 if they are
 *   equal.
 */
function strcasecmp ($str1, $str2) {}

/**
 * Binary safe case-insensitive string comparison of the first n characters
 * @link http://www.php.net/manual/en/function.strncasecmp.php
 * @param str1 string <p>
 *       The first string.
 *      </p>
 * @param str2 string <p>
 *       The second string.
 *      </p>
 * @param len int <p>
 *       The length of strings to be used in the comparison.
 *      </p>
 * @return int < 0 if str1 is less than
 *   str2; > 0 if str1 is
 *   greater than str2, and 0 if they are equal.
 */
function strncasecmp ($str1, $str2, $len) {}

/**
 * Return the current key and value pair from an array and advance the array cursor
 * @link http://www.php.net/manual/en/function.each.php
 * @param array array <p>
 *       The input array.
 *      </p>
 * @return array the current key and value pair from the array
 *   array. This pair is returned in a four-element
 *   array, with the keys 0, 1,
 *   key, and value. Elements
 *   0 and key contain the key name of
 *   the array element, and 1 and value
 *   contain the data.
 *  </p>
 *  <p>
 *   If the internal pointer for the array points past the end of the
 *   array contents, each returns
 *   false.
 */
function each (array &$array) {}

/**
 * Sets which PHP errors are reported
 * @link http://www.php.net/manual/en/function.error-reporting.php
 * @param level int[optional] <p>
 *       The new error_reporting
 *       level. It takes on either a bitmask, or named constants. Using named 
 *       constants is strongly encouraged to ensure compatibility for future 
 *       versions. As error levels are added, the range of integers increases, 
 *       so older integer-based error levels will not always behave as expected.
 *      </p>
 *      <p>
 *       The available error level constants and the actual
 *       meanings of these error levels are described in the
 *       predefined constants.
 *      </p>
 * @return int the old error_reporting
 *   level or the current level if no level parameter is
 *   given.
 */
function error_reporting ($level = null) {}

/**
 * Defines a named constant
 * @link http://www.php.net/manual/en/function.define.php
 * @param name string <p>
 *       The name of the constant.
 *      </p>
 * @param value mixed <p>
 *       The value of the constant; only scalar and null values are allowed. 
 *       Scalar values are integer, 
 *       float, string or boolean values. It is 
 *       possible to define resource constants, however it is not recommended 
 *       and may cause unpredictable behavior.
 *      </p>
 * @param case_insensitive bool[optional] <p>
 *       If set to true, the constant will be defined case-insensitive. 
 *       The default behavior is case-sensitive; i.e. 
 *       CONSTANT and Constant represent
 *       different values.
 *      </p>
 *      
 *       <p>
 *        Case-insensitive constants are stored as lower-case.
 *       </p>
 * @return bool Returns true on success or false on failure.
 */
function define ($name, $value, $case_insensitive = null) {}

/**
 * Checks whether a given named constant exists
 * @link http://www.php.net/manual/en/function.defined.php
 * @param name string <p>
 *       The constant name.
 *      </p>
 * @return bool true if the named constant given by name
 *   has been defined, false otherwise.
 */
function defined ($name) {}

/**
 * Returns the name of the class of an object
 * @link http://www.php.net/manual/en/function.get-class.php
 * @param object object[optional] <p>
 *       The tested object. This parameter may be omitted when inside a class.
 *      </p>
 * @return string the name of the class of which object is an
 *   instance. Returns false if object is not an 
 *   object.
 *  </p>
 *  <p>
 *   If object is omitted when inside a class, the
 *   name of that class is returned.
 */
function get_class ($object = null) {}

/**
 * the "Late Static Binding" class name
 * @link http://www.php.net/manual/en/function.get-called-class.php
 * @return string the class name.  Returns false if called from outside a class.
 */
function get_called_class () {}

/**
 * Retrieves the parent class name for object or class
 * @link http://www.php.net/manual/en/function.get-parent-class.php
 * @param object mixed[optional] <p>
 *       The tested object or class name
 *      </p>
 * @return string the name of the parent class of the class of which
 *   object is an instance or the name.
 *  </p>
 *  
 *   <p>
 *    If the object does not have a parent false will be returned.
 *   </p>
 *  
 *  <p>
 *   If called without parameter outside object, this function returns false.
 */
function get_parent_class ($object = null) {}

/**
 * Checks if the class method exists
 * @link http://www.php.net/manual/en/function.method-exists.php
 * @param object mixed <p>
 *       An object instance or a class name
 *      </p>
 * @param method_name string <p>
 *       The method name
 *      </p>
 * @return bool true if the method given by method_name
 *   has been defined for the given object, false 
 *   otherwise.
 */
function method_exists ($object, $method_name) {}

/**
 * Checks if the object or class has a property
 * @link http://www.php.net/manual/en/function.property-exists.php
 * @param class mixed <p>
 *        The class name or an object of the class to test for
 *       </p>
 * @param property string <p>
 *        The name of the property
 *       </p>
 * @return bool true if the property exists, false if it doesn't exist or
 *   &null; in case of an error.
 */
function property_exists ($class, $property) {}

/**
 * Checks if the class has been defined
 * @link http://www.php.net/manual/en/function.class-exists.php
 * @param class_name string <p>
 *       The class name. The name is matched in a case-insensitive manner.
 *      </p>
 * @param autoload bool[optional] <p>
 *       Whether or not to call &link.autoload; by default.
 *      </p>
 * @return bool true if class_name is a defined class,
 *   false otherwise.
 */
function class_exists ($class_name, $autoload = null) {}

/**
 * Checks if the interface has been defined
 * @link http://www.php.net/manual/en/function.interface-exists.php
 * @param interface_name string <p>
 *       The interface name
 *      </p>
 * @param autoload bool[optional] <p>
 *       Whether to call &link.autoload; or not by default.
 *      </p>
 * @return bool true if the interface given by 
 *   interface_name has been defined, false otherwise.
 */
function interface_exists ($interface_name, $autoload = null) {}

/**
 * Return &true; if the given function has been defined
 * @link http://www.php.net/manual/en/function.function-exists.php
 * @param function_name string <p>
 *       The function name, as a string.
 *      </p>
 * @return bool true if function_name exists and is a
 *   function, false otherwise.
 *  </p>
 *  
 *   <p>
 *    This function will return false for constructs, such as 
 *    include_once and echo.
 */
function function_exists ($function_name) {}

/**
 * Creates an alias for a class
 * @link http://www.php.net/manual/en/function.class-alias.php
 * @param original string[optional] <p>
 *       The original class.
 *      </p>
 * @param alias string[optional] <p>
 *       The alias name for the class.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function class_alias ($original = null, $alias = null) {}

/**
 * Returns an array with the names of included or required files
 * @link http://www.php.net/manual/en/function.get-included-files.php
 * @return array an array of the names of all files.
 *  </p>
 *  <p>
 *   The script originally called is considered an "included file," so it will
 *   be listed together with the files referenced by 
 *   include and family.
 *  </p>
 *  <p>
 *   Files that are included or required multiple times only show up once in
 *   the returned array.
 */
function get_included_files () {}

/**
 * &Alias; <function>get_included_files</function>
 * @link http://www.php.net/manual/en/function.get-required-files.php
 */
function get_required_files () {}

/**
 * Checks if the object has this class as one of its parents
 * @link http://www.php.net/manual/en/function.is-subclass-of.php
 * @param object mixed <p>
 *       A class name or an object instance
 *      </p>
 * @param class_name string <p>
 *       The class name
 *      </p>
 * @return bool This function returns true if the object object,
 *   belongs to a class which is a subclass of 
 *   class_name, false otherwise.
 */
function is_subclass_of ($object, $class_name) {}

/**
 * Checks if the object is of this class or has this class as one of its parents
 * @link http://www.php.net/manual/en/function.is-a.php
 * @param object object <p>
 *       The tested object
 *      </p>
 * @param class_name string <p>
 *       The class name
 *      </p>
 * @return bool true if the object is of this class or has this class as one of
 *   its parents, false otherwise.
 */
function is_a ($object, $class_name) {}

/**
 * Get the default properties of the class
 * @link http://www.php.net/manual/en/function.get-class-vars.php
 * @param class_name string <p>
 *       The class name
 *      </p>
 * @return array an associative array of declared properties visible from the
 *   current scope, with their default value.
 *   The resulting array elements are in the form of 
 *   varname => value.
 */
function get_class_vars ($class_name) {}

/**
 * Gets the properties of the given object
 * @link http://www.php.net/manual/en/function.get-object-vars.php
 * @param object object <p>
 *       An object instance.
 *      </p>
 * @return array an associative array of defined object accessible non-static properties 
 *   for the specified object in scope. If a property have 
 *   not been assigned a value, it will be returned with a &null; value.
 */
function get_object_vars ($object) {}

/**
 * Gets the class methods' names
 * @link http://www.php.net/manual/en/function.get-class-methods.php
 * @param class_name mixed <p>
 *       The class name or an object instance
 *      </p>
 * @return array an array of method names defined for the class specified by
 *   class_name. In case of an error, it returns &null;.
 */
function get_class_methods ($class_name) {}

/**
 * Generates a user-level error/warning/notice message
 * @link http://www.php.net/manual/en/function.trigger-error.php
 * @param error_msg string <p>
 *       The designated error message for this error. It's limited to 1024 
 *       characters in length. Any additional characters beyond 1024 will be 
 *       truncated.
 *      </p>
 * @param error_type int[optional] <p>
 *       The designated error type for this error. It only works with the E_USER
 *       family of constants, and will default to E_USER_NOTICE.
 *      </p>
 * @return bool This function returns false if wrong error_type is
 *   specified, true otherwise.
 */
function trigger_error ($error_msg, $error_type = null) {}

/**
 * Alias of <function>trigger_error</function>
 * @link http://www.php.net/manual/en/function.user-error.php
 * @param message
 * @param error_type[optional]
 */
function user_error ($message, $error_type) {}

/**
 * Sets a user-defined error handler function
 * @link http://www.php.net/manual/en/function.set-error-handler.php
 * @param error_handler callback <p>
 *       The user function needs to accept two parameters: the error code, and a
 *       string describing the error. Then there are three optional parameters 
 *       that may be supplied: the filename in which the error occurred, the
 *       line number in which the error occurred, and the context in which the
 *       error occurred (an array that points to the active symbol table at the
 *       point the error occurred).  The function can be shown as:
 *      </p>
 *      <p>
 *       
 *        handler
 *        interrno
 *        stringerrstr
 *        stringerrfile
 *        interrline
 *        arrayerrcontext
 *       
 *       
 *        
 *         errno
 *         
 *          
 *           The first parameter, errno, contains the
 *           level of the error raised, as an integer.
 * @param error_types int[optional] <p>
 *       Can be used to mask the triggering of the
 *       error_handler function just like the error_reporting ini setting 
 *       controls which errors are shown. Without this mask set the
 *       error_handler will be called for every error
 *       regardless to the setting of the error_reporting setting.
 *      </p>
 * @return mixed a string containing the previously defined error handler (if any). If
 *   the built-in error handler is used &null; is returned. &null; is also returned
 *   in case of an error such as an invalid callback. If the previous error handler
 *   was a class method, this function will return an indexed array with the class
 *   and the method name.
 */
function set_error_handler ($error_handler, $error_types = null) {}

/**
 * Restores the previous error handler function
 * @link http://www.php.net/manual/en/function.restore-error-handler.php
 * @return bool This function always returns true.
 */
function restore_error_handler () {}

/**
 * Sets a user-defined exception handler function
 * @link http://www.php.net/manual/en/function.set-exception-handler.php
 * @param exception_handler callback <p>
 *       Name of the function to be called when an uncaught exception occurs.
 *       This function must be defined before calling
 *       set_exception_handler. This handler function
 *       needs to accept one parameter, which will be the exception object that
 *       was thrown.
 *      </p>
 * @return callback the name of the previously defined exception handler, or &null; on error. If
 *   no previous handler was defined, &null; is also returned.
 */
function set_exception_handler ($exception_handler) {}

/**
 * Restores the previously defined exception handler function
 * @link http://www.php.net/manual/en/function.restore-exception-handler.php
 * @return bool This function always returns true.
 */
function restore_exception_handler () {}

/**
 * Returns an array with the name of the defined classes
 * @link http://www.php.net/manual/en/function.get-declared-classes.php
 * @return array an array of the names of the declared classes in the current
 *   script.
 *  </p>
 *  
 *   <p>
 *    Note that depending on what extensions you have compiled or
 *    loaded into PHP, additional classes could be present. This means that
 *    you will not be able to define your own classes using these
 *    names. There is a list of predefined classes in the Predefined Classes section of
 *    the appendices.
 */
function get_declared_classes () {}

/**
 * Returns an array of all declared interfaces
 * @link http://www.php.net/manual/en/function.get-declared-interfaces.php
 * @return array an array of the names of the declared interfaces in the current
 *   script.
 */
function get_declared_interfaces () {}

/**
 * Returns an array of all defined functions
 * @link http://www.php.net/manual/en/function.get-defined-functions.php
 * @return array an multidimensional array containing a list of all defined
 *   functions, both built-in (internal) and user-defined. The internal
 *   functions will be accessible via $arr["internal"], and
 *   the user defined ones using $arr["user"] (see example
 *   below).
 */
function get_defined_functions () {}

/**
 * Returns an array of all defined variables
 * @link http://www.php.net/manual/en/function.get-defined-vars.php
 * @return array A multidimensional array with all the variables.
 */
function get_defined_vars () {}

/**
 * Create an anonymous (lambda-style) function
 * @link http://www.php.net/manual/en/function.create-function.php
 * @param args string <p>
 *       The function arguments.
 *      </p>
 * @param code string <p>
 *       The function code.
 *      </p>
 * @return string a unique function name as a string, or false on error.
 */
function create_function ($args, $code) {}

/**
 * Returns the resource type
 * @link http://www.php.net/manual/en/function.get-resource-type.php
 * @param handle resource <p>
 *       The evaluated resource handle.
 *      </p>
 * @return string If the given handle is a resource, this function
 *   will return a string representing its type. If the type is not identified
 *   by this function, the return value will be the string 
 *   Unknown.
 *  </p>
 *  <p>
 *   This function will return false and generate an error if 
 *   handle is not a resource.
 */
function get_resource_type ($handle) {}

/**
 * Returns an array with the names of all modules compiled and loaded
 * @link http://www.php.net/manual/en/function.get-loaded-extensions.php
 * @param zend_extensions bool[optional] <p>
 *       Only return Zend extensions, if not then regular extensions, like 
 *       mysqli are listed. Defaults to false (return regular extensions).
 *      </p>
 * @return array an indexed array of all the modules names.
 */
function get_loaded_extensions ($zend_extensions = null) {}

/**
 * Find out whether an extension is loaded
 * @link http://www.php.net/manual/en/function.extension-loaded.php
 * @param name string <p>
 *       The extension name.
 *      </p>
 *      <p>
 *       You can see the names of various extensions by using
 *       phpinfo or if you're using the
 *       CGI or CLI version of
 *       PHP you can use the -m switch to
 *       list all available extensions:
 *       
 *       
 *      </p>
 * @return bool true if the extension identified by name
 *   is loaded, false otherwise.
 */
function extension_loaded ($name) {}

/**
 * Returns an array with the names of the functions of a module
 * @link http://www.php.net/manual/en/function.get-extension-funcs.php
 * @param module_name string <p>
 *       The module name.
 *      </p>
 *      
 *       <p>
 *        This parameter must be in lowercase.
 *       </p>
 * @return array an array with all the functions, or false if 
 *   module_name is not a valid extension.
 */
function get_extension_funcs ($module_name) {}

/**
 * Returns an associative array with the names of all the constants and their values
 * @link http://www.php.net/manual/en/function.get-defined-constants.php
 * @param categorize bool[optional] <p>
 *       Causing this function to return a multi-dimensional
 *       array with categories in the keys of the first dimension and constants
 *       and their values in the second dimension.
 *       
 *        
 * ]]>
 *        
 *        &example.outputs.similar;
 *        
 * Array
 *        (
 *            [E_ERROR] => 1
 *            [E_WARNING] => 2
 *            [E_PARSE] => 4
 *            [E_NOTICE] => 8
 *            [E_CORE_ERROR] => 16
 *            [E_CORE_WARNING] => 32
 *            [E_COMPILE_ERROR] => 64
 *            [E_COMPILE_WARNING] => 128
 *            [E_USER_ERROR] => 256
 *            [E_USER_WARNING] => 512
 *            [E_USER_NOTICE] => 1024
 *            [E_ALL] => 2047
 *            [TRUE] => 1
 *        )
 *    [pcre] => Array
 *        (
 *            [PREG_PATTERN_ORDER] => 1
 *            [PREG_SET_ORDER] => 2
 *            [PREG_OFFSET_CAPTURE] => 256
 *            [PREG_SPLIT_NO_EMPTY] => 1
 *            [PREG_SPLIT_DELIM_CAPTURE] => 2
 *            [PREG_SPLIT_OFFSET_CAPTURE] => 4
 *            [PREG_GREP_INVERT] => 1
 *        )
 *    [user] => Array
 *        (
 *            [MY_CONSTANT] => 1
 *        )
 * )
 * ]]>
 *        
 *       
 *      </p>
 * @return array 
 */
function get_defined_constants ($categorize = null) {}

/**
 * Generates a backtrace
 * @link http://www.php.net/manual/en/function.debug-backtrace.php
 * @param options int[optional] <p>
 *       As of 5.3.6, this parameter is a bitmask for the following options:
 *       <table>
 *        debug_backtrace options
 *        
 *         
 *          <tr valign="top">
 *           <td>DEBUG_BACKTRACE_PROVIDE_OBJECT</td>
 *           <td>
 *            Whether or not to populate the "object" index.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>DEBUG_BACKTRACE_IGNORE_ARGS</td>
 *           <td>
 *            Whether or not to omit the "args" index, and thus all the function/method arguments,
 *            to save memory.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *       Before 5.3.6, the only values recognized are true or false, which are the same as 
 *       setting or not setting the DEBUG_BACKTRACE_PROVIDE_OBJECT option respectively.
 *      </p>
 * @param limit int[optional] <p>
 *       As of 5.4.0, this parameter can be used to limit the number of stack frames returned.
 *       By default (limit=0) it returns all stack frames.
 *      </p>
 * @return array an array of associative arrays. The possible returned elements
 *   are as follows:
 *  </p>
 *  <p>
 *   <table>
 *    Possible returned elements from debug_backtrace
 *    
 *     
 *      <tr valign="top">
 *       <td>&Name;</td>
 *       <td>&Type;</td>
 *       <td>&Description;</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>function</td>
 *       <td>string</td>
 *       <td>
 *        The current function name.  See also
 *        __FUNCTION__.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>line</td>
 *       <td>integer</td>
 *       <td>
 *        The current line number.  See also
 *        __LINE__.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>file</td>
 *       <td>string</td>
 *       <td>
 *        The current file name.  See also
 *        __FILE__.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>class</td>
 *       <td>string</td>
 *       <td>
 *        The current class name.  See also
 *        __CLASS__
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>object</td>
 *       <td>object</td>
 *       <td>
 *        The current object.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>type</td>
 *       <td>string</td>
 *       <td>
 *        The current call type. If a method call, "->" is returned. If a static
 *        method call, "::" is returned. If a function call, nothing is returned.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>args</td>
 *       <td>array</td>
 *       <td>
 *        If inside a function, this lists the functions arguments.  If
 *        inside an included file, this lists the included file name(s).
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function debug_backtrace ($options = null, $limit = null) {}

/**
 * Prints a backtrace
 * @link http://www.php.net/manual/en/function.debug-print-backtrace.php
 * @param options int[optional] <p>
 *       As of 5.3.6, this parameter is a bitmask for the following options:
 *       <table>
 *        debug_print_backtrace options
 *        
 *         
 *          <tr valign="top">
 *           <td>DEBUG_BACKTRACE_IGNORE_ARGS</td>
 *           <td>
 *            Whether or not to omit the "args" index, and thus all the function/method arguments,
 *            to save memory.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @param limit int[optional] <p>
 *       As of 5.4.0, this parameter can be used to limit the number of stack frames printed.
 *       By default (limit=0) it prints all stack frames.
 *      </p>
 * @return void 
 */
function debug_print_backtrace ($options = null, $limit = null) {}

/**
 * Forces collection of any existing garbage cycles
 * @link http://www.php.net/manual/en/function.gc-collect-cycles.php
 * @return int number of collected cycles.
 */
function gc_collect_cycles () {}

/**
 * Returns status of the circular reference collector
 * @link http://www.php.net/manual/en/function.gc-enabled.php
 * @return bool true if the garbage collector is enabled, false otherwise.
 */
function gc_enabled () {}

/**
 * Activates the circular reference collector
 * @link http://www.php.net/manual/en/function.gc-enable.php
 * @return void 
 */
function gc_enable () {}

/**
 * Deactivates the circular reference collector
 * @link http://www.php.net/manual/en/function.gc-disable.php
 * @return void 
 */
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
 *      occured, but did not leave the Engine in an unstable state. If the error
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
 *      E_STRICT.
 * @link http://www.php.net/manual/en/errorfunc.constants.php
 */
define ('E_ALL', 30719);
define ('DEBUG_BACKTRACE_PROVIDE_OBJECT', 1);
define ('DEBUG_BACKTRACE_IGNORE_ARGS', 2);
define ('TRUE', true);
define ('FALSE', false);
define ('NULL', null);
define ('ZEND_THREAD_SAFE', false);
define ('ZEND_DEBUG_BUILD', false);
define ('PHP_VERSION', "5.3.8-ZS5.5.0");
define ('PHP_MAJOR_VERSION', 5);
define ('PHP_MINOR_VERSION', 3);
define ('PHP_RELEASE_VERSION', 8);
define ('PHP_EXTRA_VERSION', "-ZS5.5.0");
define ('PHP_VERSION_ID', 50308);
define ('PHP_ZTS', 0);
define ('PHP_DEBUG', 0);
define ('PHP_OS', "Linux");
define ('PHP_SAPI', "cli");
define ('DEFAULT_INCLUDE_PATH', ".:/usr/local/zend/share/pear");
define ('PEAR_INSTALL_DIR', "/usr/local/zend/share/pear");
define ('PEAR_EXTENSION_DIR', "/usr/local/zend/lib/php/20090626");
define ('PHP_EXTENSION_DIR', "/usr/local/zend/lib/php/20090626");
define ('PHP_PREFIX', "/usr/local/zend");
define ('PHP_BINDIR', "/usr/local/zend/bin");
define ('PHP_MANDIR', "/usr/local/zend/man");
define ('PHP_LIBDIR', "/usr/local/zend/lib/php");
define ('PHP_DATADIR', "/usr/local/zend/share/php");
define ('PHP_SYSCONFDIR', "/usr/local/zend/etc");
define ('PHP_LOCALSTATEDIR', "/usr/local/zend/var");
define ('PHP_CONFIG_FILE_PATH', "/usr/local/zend/etc");
define ('PHP_CONFIG_FILE_SCAN_DIR', "/usr/local/zend/etc/conf.d");
define ('PHP_SHLIB_SUFFIX', "so");
define ('PHP_EOL', "\n");
define ('PHP_MAXPATHLEN', 4096);
define ('PHP_INT_MAX', 2147483647);
define ('PHP_INT_SIZE', 4);
define ('ZEND_MULTIBYTE', 0);
define ('PHP_OUTPUT_HANDLER_START', 1);
define ('PHP_OUTPUT_HANDLER_CONT', 2);
define ('PHP_OUTPUT_HANDLER_END', 4);
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

// End of Core v.5.3.8-ZS5.5.0
?>
