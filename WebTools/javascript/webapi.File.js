
/**
 * @brief The File interface provides information about files and allows to access their content.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/File
 */
function File() {

	/**
	 * @brief The getAsBinary method allows to access the file's data in raw binary format.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/File/getAsBinary
	 */
	this.getAsBinary = function() {};

	/**
	 * @brief The getAsDataURL provides a data: URL that encodes the entire contents of the referenced file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/File/getAsDataURL
	 */
	this.getAsDataURL = function() {};

	/**
	 * @brief The getAsText method provides the file's data interpreted as text using a given encoding.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/File/getAsText
	 * @param encoding - A string indicating the encoding to use for the returned data. If this string is empty, UTF-8 is assumed.
	 */
	this.ile.getAsText(string encoding)

}

