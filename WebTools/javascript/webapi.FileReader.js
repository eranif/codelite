
/**
 * @brief The FileReader object lets web applications asynchronously read the contents of files (or raw data buffers) stored on the user's computer, using File or Blob objects to specify the file or data to read.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader
 */
function FileReader() {

	/**
	 * @brief The abort method is used to aborts the read operation. Upon return, the readyState will be DONE.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader/abort
	 */
	this.abort = function() {};

	/**
	 * @brief The readAsArrayBuffer method is used to start reading the contents of a specified Blob or File. When the read operation is finished, the readyState becomes DONE, and the loadend is triggered. At that time, the result attribute contains an ArrayBuffer representing the file's data.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsArrayBuffer
	 * @param blob - The Blob or File from which to read.
	 */
	this.readAsArrayBuffer = function(blob) {};

	/**
	 * @brief The readAsBinaryString method is used to start reading the contents of the specified Blob or File. When the read operation is finished, the readyState becomes DONE, and the loadend is triggered. At that time, the result attribute contains the raw binary data from the file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsBinaryString
	 * @param blob - The Blob or File from which to read.
	 */
	this.readAsBinaryString = function(blob) {};

	/**
	 * @brief The readAsDataURL method is used to read the contents of the specified Blob or File. When the read operation is finished, the readyState becomes DONE, and the loadend is triggered. At that time, the result attribute contains  the data as a URL representing the file's data as a base64 encoded string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsDataURL
	 * @param blob - The Blob or File from which to read.
	 */
	this.readAsDataURL = function(blob) {};

	/**
	 * @brief The readAsText method is used to read the contents of the specified Blob or File. When the read operation is complete, the readyState is changed to DONE, the loadend is triggered, and the result attribute contains the contents of the file as a text string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsText
	 * @param blob - The Blob or File from which to read.
	 * @param encoding Optional - A string specifying the encoding to use for the returned data. By default, UTF-8 is assumed if this parameter is not specified.
	 */
	this.readAsText = function(blob, encoding Optional) {};

}

