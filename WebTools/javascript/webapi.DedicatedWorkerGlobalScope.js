
/**
 * @brief The DedicatedWorkerGlobalScope object (the Worker global scope) is accessible through the self keyword. Some additional global functions, namespaces objects, and constructors, not typically associated with the worker global scope, but available on it, are listed in the JavaScript Reference. See also: Functions available to workers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DedicatedWorkerGlobalScope
 */
function DedicatedWorkerGlobalScope() {

	/**
	 * @brief The self read-only property of the WorkerGlobalScope interface returns a reference to the WorkerGlobalScope itself. Most of the time it is a specific scope like DedicatedWorkerGlobalScope,  SharedWorkerGlobalScope, or ServiceWorkerGlobalScope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/self
	 */
	this.self = '';

	/**
	 * @brief The postMessage() method of the DedicatedWorkerGlobalScope interface sends a message to the main thread that spawned it. This accepts a single parameter, which is the data to send to the worker. The data may be any value or JavaScript object handled by the structured clone algorithm, which includes cyclical references.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DedicatedWorkerGlobalScope/postMessage
	 * @param aMessage - The object to deliver to the main thread; this will be in the data field in the event delivered to the Worker.onmessage handler. This may be any value or JavaScript object handled by the structured clone algorithm, which includes cyclical references.
	 * @param transferList Optional - An optional array of Transferable objects to transfer ownership of. If the ownership of an object is transferred, it becomes unusable (neutered) in the context it was sent from and it becomes available only to the main thread it was sent to.
	 */
	this.edicatedWorkerGlobalScope.postMessage

}

