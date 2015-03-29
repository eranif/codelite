
/**
 * @brief The Headers interface of the Fetch API allows you to perform various actions on HTTP request and response headers. These actions include retrieving, setting, adding to, and removing. A Headers object has an associated header list, which is initially empty and consists of zero or more name and value pairs.  You can add to this using methods like append() (see Examples.) In all methods of this interface, header names are matched by case-insensitive byte sequence.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers
 */
function Headers() {

	/**
	 * @brief The append() method of the Headers interface appends a new value onto an existing header inside a Headers object, or adds the header if it does not already exist.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/append
	 * @param name - The name of the HTTP header you want to add to the Headers object.
	 * @param value - The value of the HTTP header you want to add.
	 */
	this.append = function(name, value) {};

	/**
	 * @brief The delete() method of the Headers interface deletes a header from the current Headers object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/delete
	 * @param name - The name of the HTTP header you want to delete from the Headers object.
	 */
	this.delete = function(name) {};

	/**
	 * @brief The get() method of the Headers interface returns the first value of a given header from within a Headers object. If the requested header doesn't exist in the Headers object, the call returns null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/get
	 * @param name - The name of the HTTP header whose value you want to retrieve from the Headers object. If the given name is not the name of an HTTP header, this method throws a TypeError.
	 */
	this.get = function(name) {};

	/**
	 * @brief The getAll() method of the Headers interface returns an array of all the values of a header within a Headers object with a given name. If the requested header doesn't exist in the Headers object, it returns an empty array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/getAll
	 * @param name - The name of the HTTP header whose values you want to retrieve from the Headers object. If the given name is not the name of an HTTP header, this method throws a TypeError.
	 */
	this.getAll = function(name) {};

	/**
	 * @brief The has() method of the Headers interface returns a boolean stating whether a Headers object contains a certain header.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/has
	 * @param name - The name of the HTTP header you want to test for. If the given name is not the name of an HTTP header, this method throws a TypeError.
	 */
	this.has = function(name) {};

	/**
	 * @brief The set() method of the Headers interface sets a new value for an existing header inside a Headers object, or adds the header if it does not already exist.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Headers/set
	 * @param name - The name of the HTTP header you want to set to a new value. If the given name is not the name of an HTTP header, this method throws a TypeError.
	 * @param value - The new value you want to set.
	 */
	this.set = function(name, value) {};

}

