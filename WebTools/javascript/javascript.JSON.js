
/**
 * @brief The JSON object contains methods for parsing JavaScript Object Notation (JSON) and converting values to JSON. It can't be called or constructed, and aside from its two method properties it has no interesting functionality of its own.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON
 */
function JSON() {

	/**
	 * @brief The __count__ property used to store the count of enumerable properties on the object, but it has been removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/count
	 */
	this.__count__ = '';

	/**
	 * @brief The __noSuchMethod__ property references a function to be executed when a non-existent method is called on an object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/noSuchMethod
	 */
	this.__noSuchMethod__ = '';

	/**
	 * @brief The __parent__ property used to point to an object's context, but it has been removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/Parent
	 */
	this.__parent__ = '';

	/**
	 * @brief The __proto__ property of Object.prototype is an accessor property (a getter function and a setter function) that exposes the internal [[Prototype]] (either an object or null) of the object through which it is accessed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/proto
	 */
	this.__proto__ = '';

	/**
	 * @brief Returns a reference to the Object function that created the instance's prototype. Note that the value of this property is a reference to the function itself, not a string containing the function's name. The value is only read-only for primitive values such as 1, true and &quot;test&quot;.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/constructor
	 */
	this.constructor = '';

	/**
	 * @brief The JSON.parse() method parses a string as JSON, optionally transforming the value produced by parsing.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON/parse
	 * @param text - The string to parse as JSON. See the JSON object for a description of JSON syntax.
	 * @param reviver Optional - If a function, prescribes how the value originally produced by parsing is transformed, before being returned.
	 */
	this.parse = function(text, reviver Optional) {};

	/**
	 * @brief The JSON.stringify() method converts a JavaScript value to a JSON string, optionally replacing values if a replacer function is specified, or optionally including only the specified properties if a replacer array is specified.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON/stringify
	 * @param value - The value to convert to a JSON string.
	 * @param replacer Optional - A function that alters the behavior of the stringification process, or an array of String and Number objects that serve as a whitelist for selecting the properties of the value object to be included in the JSON string. If this value is null or not provided, all properties of the object are included in the resulting JSON string.
	 * @param space Optional - A String or Number object that's used to insert white space into the output JSON string for readability purposes. If this is a Number, it indicates the number of space characters to use as white space; this number is capped at 10 if it's larger than that. Values less than 1 indicate that no space should be used. If this is a String, the string (or the first 10 characters of the string, if it's longer than that) is used as white space. If this parameter is not provided (or is null), no white space is used.
	 */
	this.stringify = function(value, replacer Optional, space Optional) {};

}

