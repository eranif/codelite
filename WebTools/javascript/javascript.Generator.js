
/**
 * @brief The Generator object is returned by a generator function and it conforms to both the iterator and the Iterable protocol.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Generator
 */
function Generator() {

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
 * @brief The next() method returns an object with two properties done and value. You can also provide a parameter to the next method to send a value to the generator.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Generator/next
 * @param value - The value to send to the generator.
 */
Generator.prototype.next = function(value) {};

/**
 * @brief The return() method returns the given value and finishes the generator.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Generator/return
 * @param value - The value to return.
 */
Generator.prototype.return = function(value) {};

/**
 * @brief The throw() method throws an error to a generator.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Generator/throw
 * @param exception - The exception to throw. For debugging purposes, it is useful to make it an instanceof Error.
 */
Generator.prototype.throw = function(exception) {};

