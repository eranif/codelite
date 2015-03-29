
/**
 * @brief The JavaScript Array global object is a constructor for arrays, which are high-level, list-like objects.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array
 */
function Array() {

	/**
	 * @brief The length property represents an unsigned, 32-bit integer that specifies the number of elements in an array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/length
	 */
	this.length = '';

	/**
	 * @brief The Array.from() method creates a new Array instance from an array-like or iterable object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/from
	 */
	this.from = function() {};

	/**
	 * @brief The Array.isArray() method returns true if an object is an array, false if it is not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/isArray
	 * @param obj - The object to be checked.
	 */
	this.isArray = function(obj) {};

	/**
	 * @brief The Array.observe() method is used for asynchronously observing changes to Arrays, similar to Object.observe() for objects. It provides a stream of changes in order of occurrence. It's equivalent to Object.observe() invoked with the accept type list [&quot;add&quot;, &quot;update&quot;, &quot;delete&quot;, &quot;splice&quot;].
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/observe
	 * @param arr - The array to be observed.
	 * @param callback - The function called each time changes are made, with the following argument:   changes  An array of objects each representing a change. The properties of these change objects are:     name: The name of the property which was changed.   object: The changed array after the change was made.   type: A string indicating the type of change taking place. One of "add", "update", "delete", or "splice".   oldValue: Only for "update" and "delete" types. The value before the change.   index: Only for the "splice" type. The index at which the change occurred.   removed: Only for the "splice" type. An array of the removed elements.   addedCount: Only for the "splice" type. The number of elements added.
	 */
	this.observe = function(arr, callback) {};

	/**
	 * @brief The Array.of() method creates a new Array instance with a variable number of arguments, regardless of number or type of the arguments.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/of
	 * @param elementN - Elements of which to create the array.
	 */
	this.of = function(elementN) {};

}

/**
 * @brief The concat() method returns a new array comprised of the array on which it is called joined with the array(s) and/or value(s) provided as arguments.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/concat
 * @param valueN - Arrays and/or values to concatenate into a new array. See the description below for details.
 */
Array.prototype.concat = function(valueN) {};

/**
 * @brief The copyWithin() method copies the sequence of array elements within the array to the position starting at target. The copy is taken from the index positions of the second and third arguments start and end. The end argument is optional and defaults to the length of the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/copyWithin
 * @param target - Target start index position where to copy the elements to.
 * @param start - Source start index position where to start copying elements from.
 * @param end - Optional. Source end index position where to end copying elements from.
 */
Array.prototype.copyWithin = function(target, start, end) {};

/**
 * @brief The entries() method returns a new Array Iterator object that contains the key/value pairs for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/entries
 */
Array.prototype.entries = function() {};

/**
 * @brief The every() method tests whether all elements in the array pass the test implemented by the provided function.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/every
 * @param callback - Function to test for each element, taking three arguments:         currentValue       The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array every was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
Array.prototype.every = function(callback, index, array, thisArg) {};

/**
 * @brief The fill() method fills all the elements of an array from a start index to an end index with a static value.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/fill
 * @param value - Value to fill an array.
 * @param start - Optional. Start index.
 * @param end - Optional. End index.
 */
Array.prototype.fill = function(value, start, end) {};

/**
 * @brief The filter() method creates a new array with all elements that pass the test implemented by the provided function.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/filter
 * @param callback - Function to test each element of the array. Invoked with arguments (element, index, array). Return true to keep the element, false otherwise.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
Array.prototype.filter = function(callback, thisArg) {};

/**
 * @brief The find() method returns a value in the array, if an element in the array satisfies the provided testing function. Otherwise undefined is returned.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/find
 * @param callback - Function to execute on each value in the array, taking three arguments:         element       The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array find was called upon.
 * @param thisArg - Optional. Object to use as this when executing callback.
 */
Array.prototype.find = function(callback, index, array, thisArg) {};

/**
 * @brief The findIndex() method returns an index in the array, if an element in the array satisfies the provided testing function. Otherwise -1 is returned.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/findIndex
 * @param callback - Function to execute on each value in the array, taking three arguments:   element  The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array findIndex was called upon.
 * @param thisArg - Optional. Object to use as this when executing callback.
 */
Array.prototype.findIndex = function(callback, index, array, thisArg) {};

/**
 * @brief The forEach() method executes a provided function once per array element.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/forEach
 * @param callback - Function that produces an element of the new Array, taking three arguments:   currentValue  The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array forEach() was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
Array.prototype.forEach = function(callback, index, array, thisArg) {};

/**
 * @brief The includes() method determines whether an array includes a certain element, returning true or false as appropriate.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/includes
 * @param searchElement - The element to search for.
 * @param fromIndex - Optional. The position in this array at which to begin searching for searchElement; defaults to 0.
 */
Array.prototype.includes = function(searchElement, fromIndex) {};

/**
 * @brief The indexOf() method returns the first index at which a given element can be found in the array, or -1 if it is not present.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/indexOf
 * @param searchElement - Element to locate in the array.
 * @param fromIndex - The index to start the search at. If the index is greater than or equal to the array's length, -1 is returned, which means the array will not be searched. If the provided index value is a negative number, it is taken as the offset from the end of the array. Note: if the provided index is negative, the array is still searched from front to back. If the calculated index is less than 0, then the whole array will be searched. Default: 0 (entire array is searched).
 */
Array.prototype.indexOf = function(searchElement, fromIndex) {};

/**
 * @brief The join() method joins all elements of an array into a string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/join
 * @param separator - Optional. Specifies a string to separate each element of the array. The separator is converted to a string if necessary. If omitted, the array elements are separated with a comma. If separator is an empty string all ellements are joined without any characters in between them.
 */
Array.prototype.join = function(separator) {};

/**
 * @brief The keys() method returns a new Array Iterator that contains the keys for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/keys
 */
Array.prototype.keys = function() {};

/**
 * @brief The lastIndexOf() method returns the last index at which a given element can be found in the array, or -1 if it is not present. The array is searched backwards, starting at fromIndex.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/lastIndexOf
 */
Array.prototype.lastIndexOf = function() {};

/**
 * @brief The map() method creates a new array with the results of calling a provided function on every element in this array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map
 * @param callback - Function that produces an element of the new Array, taking three arguments:   currentValue  The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array map was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
Array.prototype.map = function(callback, index, array, thisArg) {};

/**
 * @brief The pop() method removes the last element from an array and returns that element.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/pop
 */
Array.prototype.pop = function() {};

/**
 * @brief The push() method adds one or more elements to the end of an array and returns the new length of the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/push
 * @param elementN - The elements to add to the end of the array.
 */
Array.prototype.push = function(elementN) {};

/**
 * @brief The reduce() method applies a function against an accumulator and each value of the array (from left-to-right) has to reduce it to a single value.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/Reduce
 * @param callback - Function to execute on each value in the array, taking four arguments:   previousValue  The value previously returned in the last invocation of the callback, or initialValue, if supplied. (See below.)
 * @param currentValue - The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array reduce was called upon.
 * @param initialValue - Optional. Object to use as the first argument to the first call of the callback.
 */
Array.prototype.reduce = function(callback, currentValue, index, array, initialValue) {};

/**
 * @brief The reduceRight() method applies a function against an accumulator and each value of the array (from right-to-left) has to reduce it to a single value.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/ReduceRight
 * @param callback - Function to execute on each value in the array, taking four arguments:   previousValue  The value previously returned in the last invocation of the callback, or initialValue, if supplied. (See below.)
 * @param currentValue - The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array reduce was called upon.
 * @param initialValue - Optional. Object to use as the first argument to the first call of the callback.
 */
Array.prototype.reduceRight = function(callback, currentValue, index, array, initialValue) {};

/**
 * @brief The reverse() method reverses an array in place. The first array element becomes the last and the last becomes the first.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reverse
 */
Array.prototype.reverse = function() {};

/**
 * @brief The shift() method removes the first element from an array and returns that element. This method changes the length of the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/shift
 */
Array.prototype.shift = function() {};

/**
 * @brief The slice() method returns a shallow copy of a portion of an array into a new array object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/slice
 */
Array.prototype.slice = function() {};

/**
 * @brief The some() method tests whether some element in the array passes the test implemented by the provided function.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/some
 * @param callback - Function to test for each element, taking three arguments:     currentValue   The current element being processed in the array.
 * @param index - The index of the current element being processed in the array.
 * @param array - The array some() was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
Array.prototype.some = function(callback, index, array, thisArg) {};

/**
 * @brief The sort() method sorts the elements of an array in place and returns the array. The sort is not necessarily stable. The default sort order is according to string Unicode code points.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/sort
 * @param compareFunction - Optional. Specifies a function that defines the sort order. If omitted, the array is sorted according to each character's Unicode code point value, according to the string conversion of each element.
 */
Array.prototype.sort = function(compareFunction) {};

/**
 * @brief The splice() method changes the content of an array by removing existing elements and/or adding new elements.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/splice
 * @param start - Index at which to start changing the array. If greater than the length of the array, actual starting index will be set to the length of the array. If negative, will begin that many elements from the end.
 * @param deleteCount - An integer indicating the number of old array elements to remove. If deleteCount is 0, no elements are removed. In this case, you should specify at least one new element. If deleteCount is greater than the number of elements left in the array starting at start, then all of the elements through the end of the array will be deleted.
 * @param itemN - The element to add to the array. If you don't specify any elements, splice() will only remove elements from the array.
 */
Array.prototype.splice = function(start, deleteCount, itemN) {};

/**
 * @brief The toLocaleString() method returns a string representing the elements of the array. The elements are converted to Strings using their toLocaleString methods and these Strings are separated by a locale-specific String (such as a comma “,”).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/toLocaleString
 */
Array.prototype.toLocaleString = function() {};

/**
 * @brief The toSource() method returns a string representing the source code of the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/toSource
 */
Array.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the specified array and its elements.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/toString
 */
Array.prototype.toString = function() {};

/**
 * @brief The unshift() method adds one or more elements to the beginning of an array and returns the new length of the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/unshift
 * @param elementN - The elements to add to the front of the array.
 */
Array.prototype.unshift = function(elementN) {};

/**
 * @brief The values() method returns a new Array Iterator object that contains the values for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/values
 */
Array.prototype.values = function() {};

/**
 * @brief The initial value of the @@iterator property is the same function object as the initial value of the values() property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/@@iterator
 */
Array.prototype['@@iterator'] = function() {};

