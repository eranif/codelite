
/**
 * @brief The WorkerLocation interface defines the absolute location of the script executed by the Worker. Such an object is initialized for each worker and is available via the WorkerGlobalScope.location property obtained by calling window.self.location.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerLocation
 */
function WorkerLocation() {

	/**
	 * @brief The URLUtilsReadOnly.toString() stringifier method returns a DOMString containing the whole URL. It is a synonym for URLUtilsReadOnly.href.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/URLUtilsReadOnly/toString
	 */
	this.toString = function() {};

}

