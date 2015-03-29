
/**
 * @brief The FileHandle interface provides access in read or write mode to a file, dealing with all the necessary locks.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileHandle
 */
function FileHandle() {

	/**
	 * @brief The open method returns a LockedFile object that allows to safely write in the file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileHandle/open
	 * @param mode - A string that specifies the writing mode for the file. It can be readonly or readwrite.
	 */
	this.open = function(mode) {};

	/**
	 * @brief The getFile method allows to retrieve a read-only snapshot of the handled file in the form of a File object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileHandle/getFile
	 */
	this.getFile = function() {};

}

