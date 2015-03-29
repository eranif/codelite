
/**
 * @brief The SharedWorkerGlobalScope object (the SharedWorker global scope) is accessible through the self keyword. Some additional global functions, namespaces objects, and constructors, not typically associated with the worker global scope, but available on it, are listed in the JavaScript Reference. See also: Functions available to workers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/SharedWorkerGlobalScope
 */
function SharedWorkerGlobalScope() {

	/**
	 * @brief The close() method of the WorkerGlobalScope interface discards any tasks queued in the WorkerGlobalScope's event loop, effectively closing this particular scope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/close
	 */
	this.close = function() {};

	/**
	 * @brief The dump() method of the WorkerGlobalScope interface allows you to write a message to stdout — i.e. in your terminal, in Firefox only. This is the same as Firefox's window.dump, but for workers.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/dump
	 */
	this.dump = function() {};

	/**
	 * @brief The importScripts() method of the WorkerGlobalScope interface imports one or more scripts into the worker's scope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/importScripts
	 */
	this.importScripts = function() {};

}

