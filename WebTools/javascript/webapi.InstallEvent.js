
/**
 * @brief The parameter passed into the oninstall handler, the InstallEvent interface represents an install action that is dispatched on the ServiceWorkerGlobalScope of a ServiceWorker. As a child of ExtendableEvent, it ensures that functional events such as FetchEvent are not dispatched during installation.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/InstallEvent
 */
function InstallEvent() {

	/**
	 * @brief The ExtendableEvent.waitUntil() method extends the lifetime of the event. When called in an EventHandler associated to the install event, it delays treating the installing worker as installed until the passed Promise resolves successfully. This is primarily used to ensure that a service worker is not considered installed until all of the core caches it depends on are populated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ExtendableEvent/waitUntil
	 */
	this.waitUntil = function() {};

}

