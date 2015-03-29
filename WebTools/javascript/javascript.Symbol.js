
/**
 * @brief A symbol is a unique and immutable data type and may be used as an identifier for object properties. The symbol object is an implicit object wrapper for the symbol primitive data type.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol
 */
function Symbol() {

	/**
	 * @brief The Symbol.for(key) method searches for existing symbols in a runtime-wide symbol registry with the given key and returns it if found. Otherwise a new symbol gets created in the global symbol registry with this key.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol/for
	 * @param key - String, required. The key for the symbol (and also used for the description of the symbol).
	 */
	this.for = function(key) {};

	/**
	 * @brief The Symbol.keyFor(sym) method retrieves a shared symbol key from the global symbol registry for the given symbol.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol/keyFor
	 * @param sym - Symbol, required. The symbol to find a key for.
	 */
	this.keyFor = function(sym) {};

}

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol/toSource
 */
Symbol.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the specified Symbol object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol/toString
 */
Symbol.prototype.toString = function() {};

/**
 * @brief The valueOf() method returns the primitive value of a Symbol object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol/valueOf
 */
Symbol.prototype.valueOf = function() {};

