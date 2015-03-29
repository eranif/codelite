
/**
 * @brief The Body mixin of the Fetch API represents the body of the response/request, allowing you to declare what its content type is and how it should be handled.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body
 */
function Body() {

	/**
	 * @brief The arrayBuffer() method of the Body mixin takes a Response stream and reads it to completion. It returns a promise that resolves with an ArrayBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body/arrayBuffer
	 */
	this.arrayBuffer = function() {};

	/**
	 * @brief The blob() method of the Body mixin takes a Response stream and reads it to completion. It returns a promise that resolves with a Blob.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body/blob
	 */
	this.blob = function() {};

	/**
	 * @brief The formData() method of the Body mixin takes a Response stream and reads it to completion. It returns a promise that resolves with a FormData object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body/formData
	 */
	this.formData = function() {};

	/**
	 * @brief The json() method of the Body mixin takes a Response stream and reads it to completion. It returns a promise that resolves with an object literal containing the JSON data.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body/json
	 */
	this.json = function() {};

	/**
	 * @brief The text() method of the Body mixin takes a Response stream and reads it to completion. It returns a promise that resolves with a USVString object (text).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Body/text
	 */
	this.text = function() {};

}

