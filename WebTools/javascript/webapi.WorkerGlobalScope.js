
/**
 * @brief The WorkerGlobalScope interface of the Web Workers API is an interface representing the scope of any worker. Workers have no browsing context; this scope contains the information usually conveyed by Window objects — in this case event handlers, the console or the associated WorkerNavigator object. Each WorkerGlobalScope has its own event loop.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope
 */
function WorkerGlobalScope() {

	/**
	 * @brief The close() method of the WorkerGlobalScope interface discards any tasks queued in the WorkerGlobalScope's event loop, effectively closing this particular scope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/close
	 */
	this.close = function() {};

	/**
	 * @brief The importScripts() method of the WorkerGlobalScope interface imports one or more scripts into the worker's scope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope/importScripts
	 */
	this.importScripts = function() {};

}

