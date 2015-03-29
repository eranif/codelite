
/**
 * @brief The LockedFile interface provides tools to deal with a given file with all the necessary locks.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile
 */
function LockedFile() {

	/**
	 * @brief The fileHandle property gives access to the FileHandle object that produced the LockedFile object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/fileHandle
	 */
	this.location = '';

	/**
	 * @brief The getMetadata method allows to retrieve some metadata about the locked file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/getMetadata
	 * @param param Optional - An object used to request specific metadata. Each key is a boolean where true means the metadata is expected and where false means it is not expected. Note that if the key is undefined, it is considered as if it were true. The following metadata are supported:
	 */
	this.getMetadata = function(param Optional) {};

	/**
	 * @brief The readAsArrayBuffer method is used to read the content of the LockedFile object and provide the result of that reading as an ArrayBuffer. In many ways, it performs like the FileReader.readAsArrayBuffer() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/readAsArrayBuffer
	 * @param size - A number representing the number of bytes to read in the file.
	 */
	this.readAsArrayBuffer = function(size) {};

	/**
	 * @brief The readAsText method is used to read the content of the LockedFile object and provide the result of that reading as a string. In many ways, it performs like the FileReader.readAsText() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/readAsText
	 * @param size - A number representing the number of bytes to read in the file.
	 * @param encoding Optional - A string indicating the encoding to use for the returned data. By default, UTF-8 is assumed if this parameter is not specified.
	 */
	this.readAsText = function(size, encoding Optional) {};

	/**
	 * @brief The write method is used to write some data within the file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/write
	 * @param data - The data to write into the file. It can be a string or an ArrayBuffer.
	 */
	this.write = function(data) {};

	/**
	 * @brief The append method is used to write some data at the end of the file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/append
	 * @param data - The data to write into the file. It can be a string or an ArrayBuffer.
	 */
	this.append = function(data) {};

	/**
	 * @brief The truncate method is used to remove some data within the file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/truncate
	 * @param start Optional - A number representing the index where to start the operation.
	 */
	this.truncate = function(start Optional) {};

	/**
	 * @brief The flush method is used to ensure any change made to a file is properly written on disk.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/flush
	 */
	this.flush = function() {};

	/**
	 * @brief The abort method is used to release the lock on the LockedFile object, making it inactive: its active property is set to false and all ongoing operations are canceled.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/LockedFile/abort
	 */
	this.abort = function() {};

}

