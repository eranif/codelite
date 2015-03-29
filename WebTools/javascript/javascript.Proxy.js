
/**
 * @brief The Proxy object is used to define custom behavior for fundamental operations (e.g. property lookup, assignment, enumeration, function invocation, etc).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Proxy
 */
function Proxy() {

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

	/**
	 * @brief The Proxy.revocable() method is used to create a revocable Proxy object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Proxy/revocable
	 */
	this.revocable = function() {};

}

