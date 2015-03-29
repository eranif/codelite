
/**
 * @brief The Client interface of the ServiceWorker API represents the scope of a service worker client. A service worker client is either a document in a browser context or a SharedWorker, which is controlled by an active worker. Unless specified otherwise here, the service worker client is a document.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Client
 */
function Client() {

	/**
	 * @brief 
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Client/postMessage
	 * @param message - The message to send to the service worker.
	 * @param transfer Optional - A transferable object such as, for example, a reference to a port.
	 */
	this.postMessage = function(message, transfer Optional) {};

}

