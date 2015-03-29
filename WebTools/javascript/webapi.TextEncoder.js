
/**
 * @brief The TextEncoder interface represents an encoder for a specific method, that is a specific character encoding, like utf-8, iso-8859-2, koi8, cp1261, gbk, ... An encoder takes a stream of code points as input and emits a stream of bytes. For a more scalable, non-native library, see StringView – a C-like representation of strings based on typed arrays.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TextEncoder
 */
function TextEncoder() {

	/**
	 * @brief The TextEncoder.encode method returns a Uint8Array containing the text given in parameters encoded with the specific method for that TextEncoder object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TextEncoder/encode
	 * @param buffer - Is a DOMString containing the text to encode.
	 * @param options Optional - Is a TextEncodeOptions dictionary with the property:
	 */
	this.encode = function(buffer, options Optional) {};

}

