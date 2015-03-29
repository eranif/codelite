
/**
 * @brief XMLHttpRequest Level 2 adds support for the new FormData interface. FormData objects provide a way to easily construct a set of key/value pairs representing form fields and their values, which can then be easily sent using the XMLHttpRequest send() method.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData
 */
function FormData() {

	/**
	 * @brief The append() method of the FormData interface appends a new value onto an existing key inside a FormData object, or adds the key if it does not already exist.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/append
	 * @param name - The name of the field whose data is contained in value.
	 * @param value - The field's value. In the two parameter version this is a USVString, or if it is not, it is converted to a string. In the three parameter version this can be a Blob, File, or a USVString, Again, if none of these are specified the value is converted to a string.
	 * @param filename Optional - The filename reported to the server (a USVString), when a Blob or File is passed as the second parameter. The default filename for Blob objects is "blob".
	 */
	this.ormData.append

	/**
	 * @brief The delete() method of the FormData interface deletes a key/value pair from a FormData object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/delete
	 * @param name - The name of the key you want to delete.
	 */
	this.ormData.delete

	/**
	 * @brief The get() method of the FormData interface returns the first value associated with a given key from within a FormData object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/get
	 * @param name - A USVString representing the name of the key you want to retrieve.
	 */
	this.ormData.get

	/**
	 * @brief The getAll() method of the FormData interface returns the first value associated with a given key from within a FormData object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/getAll
	 * @param name - A USVString representing the name of the key you want to retrieve.
	 */
	this.ormData.getAll

	/**
	 * @brief The has() method of the FormData interface returns a boolean stating whether a FormData object contains a certain key/value pair.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/has
	 * @param name - A USVString representing the name of the key you want to test for.
	 */
	this.ormData.has

	/**
	 * @brief The append() method of the FormData interface Sets a new value for an existing key inside a FormData object, or adds the key/value if it does not already exist.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FormData/set
	 */
	this.ormData.set

}

