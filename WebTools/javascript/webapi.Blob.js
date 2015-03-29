
/**
 * @brief A Blob object represents a file-like object of immutable, raw data. Blobs represent data that isn't necessarily in a JavaScript-native format. The File interface is based on Blob, inheriting blob functionality and expanding it to support files on the user's system.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Blob
 */
function Blob() {

	/**
	 * @brief The slice is used to create a new Blob object containing the data in the specified range of bytes of the source Blob.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Blob/slice
	 * @param start Optional - An index into the Blob indicating the first byte to copy into the new Blob. If you specify a negative value, it's treated as an offset from the end of the string toward the beginning. For example, -10 would be the 10th from last byte in the Blob. The default value is 0. If you specify a value for start that is larger than the size of the source Blob, the returned Blob has size 0 and contains no data.
	 * @param end Optional - An index into the Blob indicating where to stop copying into the new Blob. The byte exactly at this index is not included. If you specify a negative value, it's treated as an offset from the end of the string toward the beginning. For example, -10 would be the 10th from last byte in the Blob. The default value is size.
	 * @param contentType Optional - The content type to assign to the new Blob; this will be the value of its type property. The default value is an empty string.
	 */
	this.slice = function(start Optional, end Optional, contentType Optional) {};

}

