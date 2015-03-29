
/**
 * @brief RandomSource represents a source of cryptographically secure random numbers. It is available via the Crypto object of the global object: Window.crypto on Web pages, WorkerGlobalScope.crypto in workers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/RandomSource
 */
function RandomSource() {

	/**
	 * @brief The RandomSource.getRandomValues() method lets you get cryptographically random values. The array given as the parameter is filled with random numbers (random in its cryptographic meaning).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RandomSource/getRandomValues
	 * @param typedArray - Is an integer-based TypedArray, that is an Int8Array, a Uint8Array, a Uint16Array, an Int32Array, or a Uint32Array. All elements in the array are going to be overridden with random numbers.
	 */
	this.getRandomValues = function(typedArray) {};

}

