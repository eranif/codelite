
/**
 * @brief The Crypto interface represents basic cryptography features available in the current context. It allows access to a cryptographically strong random number generator and to cryptographic primitives.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Crypto
 */
function Crypto() {

	/**
	 * @brief The RandomSource.getRandomValues() method lets you get cryptographically random values. The array given as the parameter is filled with random numbers (random in its cryptographic meaning).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RandomSource/getRandomValues
	 * @param typedArray - Is an integer-based TypedArray, that is an Int8Array, a Uint8Array, a Uint16Array, an Int32Array, or a Uint32Array. All elements in the array are going to be overridden with random numbers.
	 */
	this.getRandomValues = function(typedArray) {};

}

