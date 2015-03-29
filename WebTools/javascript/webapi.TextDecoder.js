
/**
 * @brief The TextDecoder interface represents a decoder for a specific method, that is a specific character encoding, like utf-8, iso-8859-2, koi8, cp1261, gbk, ... A decoder takes a stream of bytes as input and emits a stream of code points. For a more scalable, non-native library, see StringView – a C-like representation of strings based on typed arrays.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TextDecoder
 */
function TextDecoder() {

	/**
	 * @brief The TextDecoder.decode method returns a DOMString containing the text, given in parameters, decoded with the specific method for that TextDecoder object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TextDecoder/decode
	 * @param buffer Optional - Is an ArrayBufferView containing the text to decode.
	 * @param options Optional - Is a TextDecodeOptions dictionary with the property:
	 */
	this.decode = function(buffer Optional, options Optional) {};

}

