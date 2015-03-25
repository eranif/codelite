
/**
 * @brief The Map object is a simple key/value map. Any value (both objects and primitive values) may be used as either a key or a value.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map
 */
function Map() {

	/**
	 * @brief The size accessor property returns the number of elements in a Map object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/size
	 */
	this.size = '';

}

/**
 * @brief The clear() method removes all elements from a Map object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/clear
 */
Map.prototype.clear = function() {};

/**
 * @brief The delete() method removes the specified element from a Map object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/delete
 * @param key - Required. The key of the element to remove from the Map object.
 */
Map.prototype.delete = function(key) {};

/**
 * @brief The entries() method returns a new Iterator object that contains the [key, value] pairs for each element in the Map object in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/entries
 */
Map.prototype.entries = function() {};

/**
 * @brief The forEach() method executes a provided function once per each key/value pair in the Map object, in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/forEach
 * @param callback - Function to execute for each element.
 * @param thisArg - Value to use as this when executing callback.
 */
Map.prototype.forEach = function(callback, thisArg) {};

/**
 * @brief The get() method returns a specified element from a Map object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/get
 * @param key - Required. The key of the element to return from the Map object.
 */
Map.prototype.get = function(key) {};

/**
 * @brief The has() method returns a boolean indicating whether an element with the specified key exists or not.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/has
 * @param key - Required. The key of the element to test for presence in the Map object.
 */
Map.prototype.has = function(key) {};

/**
 * @brief The keys() method returns a new Iterator object that contains the keys for each element in the Map object in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/keys
 */
Map.prototype.keys = function() {};

/**
 * @brief The set() method adds a new element with a specified key and value to a Map object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/set
 * @param key - Required. The key of the element to add to the Map object.
 * @param value - Required. The value of the element to add to the Map object.
 */
Map.prototype.set = function(key, value) {};

/**
 * @brief The values() method returns a new Iterator object that contains the values for each element in the Map object in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/values
 */
Map.prototype.values = function() {};

/**
 * @brief The initial value of the @@iterator property is the same function object as the initial value of the entries property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map/@@iterator
 */
Map.prototype['@@iterator'] = function() {};

