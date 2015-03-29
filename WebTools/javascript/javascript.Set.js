
/**
 * @brief The Set object lets you store unique values of any type, whether primitive values or object references.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set
 */
function Set() {

	/**
	 * @brief The size accessor property returns the number of elements in a Set object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/size
	 */
	this.size = '';

}

/**
 * @brief The add() method appends a new element with a specified value to the end of a Set object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/add
 * @param value - Required. The value of the element to add to the Set object.
 */
Set.prototype.add = function(value) {};

/**
 * @brief The clear() method removes all elements from a Set object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/clear
 */
Set.prototype.clear = function() {};

/**
 * @brief The delete() method removes the specified element from a Set object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/delete
 * @param value - Required. The value of the element to remove from the Set object.
 */
Set.prototype.delete = function(value) {};

/**
 * @brief The entries() method returns a new Iterator object that contains an array of [value, value] for each element in the Set object, in insertion order. For Set objects there is no key like in Map objects. However, to keep the API similar to the Map object, each entry has the same value for its key and value here, so that an array [value, value] is returned.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/entries
 */
Set.prototype.entries = function() {};

/**
 * @brief The forEach() method executes a provided function once per each value in the Set object, in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/forEach
 * @param callback - Function to execute for each element.
 * @param thisArg - Value to use as this when executing callback.
 */
Set.prototype.forEach = function(callback, thisArg) {};

/**
 * @brief The has() method returns a boolean indicating whether an element with the specified value exists in a Set object or not.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/has
 * @param value - Required. The value to test for presence in the Set object.
 */
Set.prototype.has = function(value) {};

/**
 * @brief The values() method returns a new Iterator object that contains the values for each element in the Set object in insertion order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/values
 */
Set.prototype.values = function() {};

/**
 * @brief The initial value of the @@iterator property is the same function object as the initial value of the values property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set/@@iterator
 */
Set.prototype['@@iterator'] = function() {};

