
/**
 * @brief The Clients interface of the Service Workers API represents a container for a list of Client objects.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Clients
 */
function Clients() {

	/**
	 * @brief The matchAll() method of the Clients interface returns a Promise for a list of service worker clients. Include options to return all service worker clients whose origin is the same as the associated service worker's origin. If options are not included, the method returns only the service worker clients controlled by the service worker.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Clients/matchAll
	 * @param options - An options object allowing you to set options for the matching operation. Available options are:
 
  includeUncontrolled: A Boolean — if set to true, the matching operation will return all service worker clients who share the same origin as the current service worker. Otherwise, it returns only the service worker clients controlled by the current service worker. The default is false.
  type: Sets the type of clients you want matched. Available values are window, worker, sharedworker, and all. The default is all.
	 */
	this.matchAll = function(options) {};

	/**
	 * @brief The openWindow() method of the Clients interface opens a service worker WindowClient in a new browser window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Clients/openWindow
	 * @param url - A USVString representing the URL of the client you want to open in the window.
	 */
	this.openWindow = function(url) {};

	/**
	 * @brief The claim() method of the of the Clients interface allows an active Service Worker to set itself as the active worker for a client page when the worker and the page are in the same scope. This triggers a oncontrollerchange event on any client pages within the Service Worker's scope.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Clients/claim
	 */
	this.claim = function() {};

}

