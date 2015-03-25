
/**
 * @brief The ArrayBuffer object is used to represent a generic, fixed-length raw binary data buffer. You can not directly manipulate the contents of an ArrayBuffer; instead, you create one of the typed array objects or a DataView object which represents the buffer in a specific format, and use that to read and write the contents of the buffer.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
 */
function ArrayBuffer() {

	/**
	 * @brief The byteLength accessor property represents the length of an ArrayBuffer in bytes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer/byteLength
	 */
	this.byteLength = '';

	/**
	 * @brief The ArrayBuffer.isView() method returns true if arg is a view one of the ArrayBuffer views, such as typed array objects or a DataView; false otherwise.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer/isView
	 * @param arg - The argument to be checked.
	 */
	this.isView = function(arg) {};

	/**
	 * @brief The static ArrayBuffer.transfer() method returns a new ArrayBuffer whose contents are taken from the oldBuffer's data and then is either truncated or zero-extended by newByteLength. If newByteLength is undefined, the byteLength of the oldBuffer is used. This operation leaves oldBuffer in a detached state.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer/transfer
	 * @param oldBuffer - An ArrayBuffer object from which to transfer from.
	 * @param newByteLength - The byte length of the new ArrayBuffer object.
	 */
	this.transfer = function(oldBuffer, newByteLength) {};

}

/**
 * @brief The slice() method returns a new ArrayBuffer whose contents are a copy of this ArrayBuffer's bytes from begin, inclusive, up to end, exclusive.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer/slice
 * @param begin - Zero-based byte index at which to begin slicing.
 * @param end - Byte index to end slicing. If end is unspecified, the new ArrayBuffer contains all bytes from begin to the end of this ArrayBuffer. The range specified by the begin and end values is clamped to the valid index range for the current array. If the computed length of the new ArrayBuffer would be negative, it is clamped to zero.
 */
ArrayBuffer.prototype.slice = function(begin, end) {};

