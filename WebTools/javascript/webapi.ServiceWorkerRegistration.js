
/**
 * @brief The ServiceWorkerRegistion interface of the ServiceWorker API represents the service worker registration. You register a service worker to control one or more pages that share the same origin.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerRegistration
 */
function ServiceWorkerRegistration() {

	/**
	 * @brief The update method of the ServiceWorkerRegistration interface allows you to ping the server for an updated service worker script. If you don't explicitly call this, the UA will do this automatically once every 24 hours.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerRegistration/update
	 */
	this.update = function() {};

	/**
	 * @brief The unregister method of the ServiceWorkerRegistration interface unregisters the service worker registration and returns a Promise. The promise will resolve to false if no registration was found, otherwise it resolves to true irrespective of whether unregistration happened or not (it may not unregister if someone else just called ServiceWorkerContainer.register with the same scope.) The service worker will finish any ongoing operations before it is unregistered.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerRegistration/unregister
	 */
	this.unregister = function() {};

}

