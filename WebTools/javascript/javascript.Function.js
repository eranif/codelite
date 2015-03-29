
/**
 * @brief The Function constructor creates a new Function object. In JavaScript every function is actually a Function object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function
 */
function Function() {

	/**
	 * @brief The function.arguments property refers to an an array-like object corresponding to the arguments passed to a function. Use the simple variable arguments instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/arguments
	 */
	this.arguments = '';

	/**
	 * @brief The arity property used to return the number of arguments expected by the function, however, it no longer exists and has been replaced by the Function.prototype.length property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/arity
	 */
	this.arity = '';

	/**
	 * @brief The function.caller property returns the function that invoked the specified function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/caller
	 */
	this.caller = '';

	/**
	 * @brief The function.displayName property returns the display name of the function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/displayName
	 */
	this.displayName = '';

	/**
	 * @brief The length property specifies the number of arguments expected by the function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/length
	 */
	this.length = '';

	/**
	 * @brief The function.name property returns the name of the function.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/name
	 */
	this.name = '';

}

/**
 * @brief The apply() method calls a function with a given this value and arguments provided as an array (or an array-like object).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/apply
 * @param thisArg - The value of this provided for the call to fun. Note that this may not be the actual value seen by the method: if the method is a function in non-strict mode code, null and undefined will be replaced with the global object, and primitive values will be boxed.
 * @param argsArray - An array-like object, specifying the arguments with which fun should be called, or null or undefined if no arguments should be provided to the function. Starting with ECMAScript 5 these arguments can be a generic array-like object instead of an array. See below for browser compatibility information.
 */
Function.prototype.apply = function(thisArg, argsArray) {};

/**
 * @brief The bind() method creates a new function that, when called, has its this keyword set to the provided value, with a given sequence of arguments preceding any provided when the new function is called.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/bind
 * @param thisArg - The value to be passed as the this parameter to the target function when the bound function is called. The value is ignored if the bound function is constructed using the new operator.
 * @param arg1, arg2, ... - Arguments to prepend to arguments provided to the bound function when invoking the target function.
 */
Function.prototype.bind = function(thisArg, arg1, arg2, ...) {};

/**
 * @brief The call() method calls a function with a given this value and arguments provided individually.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/call
 * @param thisArg - The value of this provided for the call to fun. Note that this may not be the actual value seen by the method: if the method is a function in non-strict mode code, null and undefined will be replaced with the global object, and primitive values will be boxed.
 * @param arg1, arg2, ... - Arguments for the object.
 */
Function.prototype.call = function(thisArg, arg1, arg2, ...) {};

/**
 * @brief The isGenerator() method determines whether or not a function is a generator.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/isGenerator
 */
Function.prototype.isGenerator = function() {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/toSource
 */
Function.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the source code of the function.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function/toString
 * @param indentation   Obsolete since Gecko 17 - The amount of spaces to indent the string representation of the source code. If indentation is less than or equal to -1, most unnecessary spaces are removed.
 */
Function.prototype.toString = function(indentation   Obsolete since Gecko 17) {};

