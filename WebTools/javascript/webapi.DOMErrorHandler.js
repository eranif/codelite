
/**
 * @brief Set as "error-handler" parameter in DOMConfiguration.setParameter . Implementation may provide a default handler. DOMError.relatedData will contain closest node to where error occurred or contain the Document node if it is unable to be determined. Document mutations from within the error handler result in implementation-dependent behavior. If there are to be multiple errors, the sequence and numbers of the errors passed to the error handler are also implementation dependent. The application using the DOM implementation implements this interface:
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMErrorHandler
 */
function DOMErrorHandler() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMErrorHandler/handleError
	 */
	this.handleError() = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMErrorHandler/handleError
	 */
	this.handleError = function() {};

}

