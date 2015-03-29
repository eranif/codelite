
/**
 * @brief The FileRequest interface extends the DOMRequest interface to provide some extra properties necessary for the LockedFile objects.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileRequest
 */
function FileRequest() {

	/**
	 * @brief The lockedFile property represents the LockedFile object from which the request was started.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FileRequest/lockedFile
	 */
	this.onprogress = '';

}

