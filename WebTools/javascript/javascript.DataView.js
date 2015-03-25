
/**
 * @brief The DataView view provides a low-level interface for reading data from and writing it to an ArrayBuffer.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView
 */
function DataView() {

	/**
	 * @brief The buffer accessor property represents the ArrayBuffer referenced by the DataView at construction time.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/buffer
	 */
	this.buffer = '';

	/**
	 * @brief The byteLength accessor property represents the length (in bytes) of this view from the start of its ArrayBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/byteLength
	 */
	this.byteLength = '';

	/**
	 * @brief The byteOffset accessor property represents the offset (in bytes) of this view from the start of its ArrayBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/byteOffset
	 */
	this.byteOffset = '';

}

/**
 * @brief The getFloat32() method gets a signed 32-bit float (float) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getFloat32
 */
DataView.prototype.getFloat32 = function() {};

/**
 * @brief The getFloat64() method gets a signed 64-bit float (double) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getFloat64
 */
DataView.prototype.getFloat64 = function() {};

/**
 * @brief The getInt16() method gets a signed 16-bit integer (short) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getInt16
 */
DataView.prototype.getInt16 = function() {};

/**
 * @brief The getInt32() method gets a signed 32-bit integer (long) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getInt32
 */
DataView.prototype.getInt32 = function() {};

/**
 * @brief The getInt8() method gets a signed 8-bit integer (byte) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getInt8
 */
DataView.prototype.getInt8 = function() {};

/**
 * @brief The getUint16() method gets an unsigned 16-bit integer (unsigned short) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getUint16
 */
DataView.prototype.getUint16 = function() {};

/**
 * @brief The getUint32() method gets an unsigned 32-bit integer (unsigned long) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getUint32
 */
DataView.prototype.getUint32 = function() {};

/**
 * @brief The getUint8() method gets an unsigned 8-bit integer (unsigned byte) at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/getUint8
 */
DataView.prototype.getUint8 = function() {};

/**
 * @brief The setFloat32() method stores a signed 32-bit float (float) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setFloat32
 */
DataView.prototype.setFloat32 = function() {};

/**
 * @brief The setFloat64() method stores a signed 64-bit float (double) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setFloat64
 */
DataView.prototype.setFloat64 = function() {};

/**
 * @brief The setInt16() method stores a signed 16-bit integer (short) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setInt16
 */
DataView.prototype.setInt16 = function() {};

/**
 * @brief The setInt32() method stores a signed 32-bit integer (long) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setInt32
 */
DataView.prototype.setInt32 = function() {};

/**
 * @brief The setInt8() method stores a signed 8-bit integer (byte) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setInt8
 */
DataView.prototype.setInt8 = function() {};

/**
 * @brief The setUint16() method stores an unsigned 16-bit integer (unsigned short) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setUint16
 */
DataView.prototype.setUint16 = function() {};

/**
 * @brief The setUint32() method stores an unsigned 32-bit integer (unsigned long) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setUint32
 */
DataView.prototype.setUint32 = function() {};

/**
 * @brief The setUint8() method stores an unsigned 8-bit integer (byte) value at the specified byte offset from the start of the DataView.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setUint8
 */
DataView.prototype.setUint8 = function() {};

