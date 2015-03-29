
/**
 * @brief The parameter passed into the ServiceWorkerGlobalScope.onfetch handler, FetchEvent represents a fetch action that is displatched on the ServiceWorkerGlobalScope of a ServiceWorker. It contains information about the request and resulting response, and provides the FetchEvent.respondWith() method, which allows us to provide an arbitrary response back to the controlled page.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/FetchEvent
 */
function FetchEvent() {

	/**
	 * @brief The respondWith() method of the FetchEvent interface is intended for containing code that generates custom responses to the requests coming from the controlled page. This code will resolve by returning a Response or a network error  to Fetch.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/FetchEvent/respondWith
	 */
	this.respondWith = function() {};

}

