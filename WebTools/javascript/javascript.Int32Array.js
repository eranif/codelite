
/**
 * @brief The Int32Array typed array represents an array of twos-complement 32-bit signed integers in the platform byte order. If control over byte order is needed, use DataView instead. The contents are initialized to 0. Once established, you can reference elements in the array using the object's methods, or using standard array index syntax (that is, using bracket notation).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Int32Array
 */
function Int32Array() {

	/**
	 * @brief The TypedArray.BYTES_PER_ELEMENT property represents the size in bytes of each element in an typed array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/BYTES_PER_ELEMENT
	 */
	this.BYTES_PER_ELEMENT = '';

	/**
	 * @brief The TypedArray.name property represents a string value of the typed array constructor name.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/name
	 */
	this.name = '';

	/**
	 * @brief The buffer accessor property represents the ArrayBuffer referenced by a TypedArray at construction time.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/buffer
	 */
	this.buffer = '';

	/**
	 * @brief The byteLength accessor property represents the length (in bytes) of a typed array from the start of its ArrayBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/byteLength
	 */
	this.byteLength = '';

	/**
	 * @brief The byteOffset accessor property represents the offset (in bytes) of a typed array from the start of its ArrayBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/byteOffset
	 */
	this.byteOffset = '';

	/**
	 * @brief The length accessor property represents the length (in elements) of a typed array.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/length
	 */
	this.length = '';

	/**
	 * @brief The %TypedArray%.from() method creates a new typed array from an array-like or iterable object. This method is nearly the same as Array.from().
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/from
	 */
	this.from = function() {};

	/**
	 * @brief The %TypedArray%.of() method creates a new new typed array with a variable number of arguments. This method is nearly the same as Array.of().
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/of
	 * @param elementN - Elements of which to create the typed array.
	 */
	this.of = function(elementN) {};

}

/**
 * @brief The copyWithin() method copies the sequence of array elements within the array to the position starting at target. The copy is taken from the index positions of the second and third arguments start and end. The end argument is optional and defaults to the length of the array. This method has the same algorithm as Array.prototype.copyWithin. TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/copyWithin
 * @param target - Target start index position where to copy the elements to.
 * @param start - Source start index position where to start copying elements from.
 * @param end Optional - Optional. Source end index position where to end copying elements from.
 */
TypedArray.prototype.copyWithin = function(target, start, end Optional) {};

/**
 * @brief The entries() method returns a new Array Iterator object that contains the key/value pairs for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/entries
 */
TypedArray.prototype.entries = function() {};

/**
 * @brief The every() method tests whether all elements in the typed array pass the test implemented by the provided function. This method has the same algorithm as Array.prototype.every(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/every
 * @param callback - Function to test for each element, taking three arguments:         currentValue       The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The typed array every was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
TypedArray.prototype.every = function(callback, index, array, thisArg) {};

/**
 * @brief The fill() method fills all the elements of a typed array from a start index to an end index with a static value. This method has the same algorithm as Array.prototype.fill(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/fill
 * @param value - Value to fill the typed array with.
 * @param start - Optional. Start index. Defaults to 0.
 * @param end - Optional. End index. Defaults to 0.
 */
TypedArray.prototype.fill = function(value, start, end) {};

/**
 * @brief The find() method returns a value in the typed array, if an element satisfies the provided testing function. Otherwise undefined is returned. TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/find
 * @param callback - Function to execute on each value in the typed array, taking three arguments:         element       The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The array find was called upon.
 * @param thisArg - Optional. Object to use as this when executing callback.
 */
TypedArray.prototype.find = function(callback, index, array, thisArg) {};

/**
 * @brief The findIndex() method returns an index in the typed array, if an element in the typed array satisfies the provided testing function. Otherwise -1 is returned.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/findIndex
 * @param callback - Function to execute on each value in the typed array, taking three arguments:         element       The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The typed array findIndex was called upon.
 * @param thisArg - Optional. Object to use as this when executing callback.
 */
TypedArray.prototype.findIndex = function(callback, index, array, thisArg) {};

/**
 * @brief The includes() method determines whether a typed array includes a certain element, returning true or false as appropriate. This method has the same algorithm as Array.prototype.includes(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/includes
 * @param searchElement - The element to search for.
 * @param fromIndex - Optional. The position in this array at which to begin searching for searchElement; defaults to 0.
 */
TypedArray.prototype.includes = function(searchElement, fromIndex) {};

/**
 * @brief The indexOf() method returns the first index at which a given element can be found in the typed array, or -1 if it is not present. This method has the same algorithm as Array.prototype.indexOf(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/indexOf
 * @param searchElement - Element to locate in the typed array.
 * @param fromIndex - The index to start the search at. If the index is greater than or equal to the typed array's length, -1 is returned, which means the typed array will not be searched. If the provided index value is a negative number, it is taken as the offset from the end of the typed array. Note: if the provided index is negative, the typed array is still searched from front to back. If the calculated index is less than 0, then the whole typed array will be searched. Default: 0 (entire typed array is searched).
 */
TypedArray.prototype.indexOf = function(searchElement, fromIndex) {};

/**
 * @brief The join() method joins all elements of an array into a string. This method has the same algorithm as Array.prototype.join(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/join
 * @param separator - Optional. Specifies a string to separate each element. The separator is converted to a string if necessary. If omitted, the typed array elements are separated with a comma (",").
 */
TypedArray.prototype.join = function(separator) {};

/**
 * @brief The keys() method returns a new Array Iterator object that contains the keys for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/keys
 */
TypedArray.prototype.keys = function() {};

/**
 * @brief The lastIndexOf() method returns the last index at which a given element can be found in the typed array, or -1 if it is not present. The typed array is searched backwards, starting at fromIndex. This method has the same algorithm as Array.prototype.lastIndexOf(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/lastIndexOf
 * @param searchElement - Element to locate in the typed array.
 * @param fromIndex - Optional. The index at which to start searching backwards. Defaults to the typed array's length, i.e. the whole typed array will be searched. If the index is greater than or equal to the length of the typed array, the whole typed array will be searched. If negative, it is taken as the offset from the end of the typed array. Note that even when the index is negative, the typed array is still searched from back to front. If the calculated index is less than 0, -1 is returned, i.e. the typed array will not be searched.
 */
TypedArray.prototype.lastIndexOf = function(searchElement, fromIndex) {};

/**
 * @brief The move() method used to copy the sequence of array elements within the array to the position starting at target. However, this non-standard method has been replaced with the standard TypedArray.prototype.copyWithin() method. TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/move
 * @param start - Source start index position where to start copying elements from.
 * @param end - Source end index position where to end copying elements from.
 * @param target - Target start index position where to copy the elements to.
 */
TypedArray.prototype.move = function(start, end, target) {};

/**
 * @brief The reduce() method applies a function against an accumulator and each value of the typed array (from left-to-right) has to reduce it to a single value. This method has the same algorithm as Array.prototype.reduce(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/reduce
 * @param callback - Function to execute on each value in the typed array, taking four arguments:   previousValue  The value previously returned in the last invocation of the callback, or initialValue, if supplied (see below).
 * @param currentValue - The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The typed array reduce was called upon.
 * @param initialValue - Optional. Object to use as the first argument to the first call of the callback.
 */
TypedArray.prototype.reduce = function(callback, currentValue, index, array, initialValue) {};

/**
 * @brief The reduceRight() method applies a function against an accumulator and each value of the typed array (from right-to-left) has to reduce it to a single value. This method has the same algorithm as Array.prototype.reduceRight(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/reduceRight
 * @param callback - Function to execute on each value in the typed array, taking four arguments:   previousValue  The value previously returned in the last invocation of the callback, or initialValue, if supplied (see below).
 * @param currentValue - The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The typed array reduce was called upon.
 * @param initialValue - Optional. Object to use as the first argument to the first call of the callback.
 */
TypedArray.prototype.reduceRight = function(callback, currentValue, index, array, initialValue) {};

/**
 * @brief The reverse() method reverses a typed array in place. The first typed array element becomes the last and the last becomes the first. This method has the same algorithm as Array.prototype.reverse(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/reverse
 */
TypedArray.prototype.reverse = function() {};

/**
 * @brief The set() method stores multiple values in the typed array, reading input values from a specified array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/set
 * @param array - The array from which to copy values. All values from the source array are copied into the target array, unless the length of the source array plus the offset exceeds the length of the target array, in which case an exception is thrown.
 * @param typedarray - If the source array is a typed array, the two arrays may share the same underlying ArrayBuffer; the browser will intelligently copy the source range of the buffer to the destination range.
 * @param offset Optional - The offset into the target array at which to begin writing values from the source array. If you omit this value, 0 is assumed (that is, the source array will overwrite values in the target array starting at index 0).
 */
TypedArray.prototype.set = function(array, typedarray, offset Optional) {};

/**
 * @brief The some() method tests whether some element in the typed array passes the test implemented by the provided function. This method has the same algorithm as Array.prototype.some(). TypedArray is one of the typed array types here.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/some
 * @param callback - Function to test for each element, taking three arguments:         currentValue       The current element being processed in the typed array.
 * @param index - The index of the current element being processed in the typed array.
 * @param array - The typed array every was called upon.
 * @param thisArg - Optional. Value to use as this when executing callback.
 */
TypedArray.prototype.some = function(callback, index, array, thisArg) {};

/**
 * @brief The subarray() method returns a new TypedArray on the same ArrayBuffer store and with the same element types as for this TypedArray object. The begin offset is inclusive and the end offset is exclusive. TypedArray is one of the typed array types.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/subarray
 * @param begin Optional - Element to begin at. The offset is inclusive.
 * @param end Optional - Element to end at. The offset is exclusive. If not specified, all elements from the one specified by begin to the end of the array are included in the new view.
 */
TypedArray.prototype.subarray = function(begin Optional, end Optional) {};

/**
 * @brief The values() method returns a new Array Iterator object that contains the values for each index in the array.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/values
 */
TypedArray.prototype.values = function() {};

/**
 * @brief The initial value of the @@iterator property is the same function object as the initial value of the values property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray/@@iterator
 */
TypedArray.prototype['@@iterator'] = function() {};

