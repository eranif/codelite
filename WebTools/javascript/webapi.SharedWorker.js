
/**
 * @brief The SharedWorker interface represents a specific kind of worker that can be accessed from several browsing contexts, such as several windows, iframes or even workers. They implement an interface different than dedicated workers and have a different global scope, SharedWorkerGlobalScope.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/SharedWorker
 */
function SharedWorker() {

	/**
	 * @brief The AbstractWorker.onerror property of the AbstractWorker interface represents an EventHandler, that is a function to be called when the error event occurs and bubbles through the Worker.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AbstractWorker/onerror
	 */
	this.onerror = '';

}

