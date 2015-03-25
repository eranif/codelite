
/**
 * @brief The Worker interface of the Web Workers API represents a background task that can be easily created and can send messages back to its creator. Creating a worker is as simple as calling the Worker() constructor and specifying a script to be run in the worker thread.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/Worker
 */
function Worker() {

	/**
	 * @brief The AbstractWorker.onerror property of the AbstractWorker interface represents an EventHandler, that is a function to be called when the error event occurs and bubbles through the Worker.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AbstractWorker/onerror
	 */
	this.onerror = '';

	/**
	 * @brief The onmessage property of the Worker interface represents an EventHandler, that is a function to be called when the message event occurs. These events are of type MessageEvent and will be called when the worker's parent receives a message (i.e. from the DedicatedWorkerGlobalScope.postMessage method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Worker/onmessage
	 */
	this.onmessage = '';

	/**
	 * @brief The postMessage() method of the Worker interface sends a message to the worker's inner scope. This accepts a single parameter, which is the data to send to the worker. The data may be any value or JavaScript object handled by the structured clone algorithm, which includes cyclical references.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Worker/postMessage
	 * @param aMessage - The object to deliver to the worker; this will be in the data field in the event delivered to the DedicatedWorkerGlobalScope.onmessage handler. This may be any value or JavaScript object handled by the structured clone algorithm, which includes cyclical references.
	 * @param transferList Optional - An optional array of Transferable objects to transfer ownership of. If the ownership of an object is transferred, it becomes unusable (neutered) in the context it was sent from and it becomes available only to the worker it was sent to.
	 */
	this.postMessage = function(aMessage, transferList Optional) {};

	/**
	 * @brief The terminate() method of the Worker interface immediately terminates the Worker. This does not offer the worker an opportunity to finish its operations; it is simply stopped at once.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Worker/terminate
	 */
	this.terminate = function() {};

}

