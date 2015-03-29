
/**
 * @brief The ExtendableEvent interface extends the lifetime of the install and activate events dispatched on the global scope as part of the service worker lifecycle. This ensures that any functional events (like FetchEvent) are not dispatched until it upgrades database schemas and deletes the outdated cache entries.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ExtendableEvent
 */
function ExtendableEvent() {

	/**
	 * @brief The ExtendableEvent.waitUntil() method extends the lifetime of the event. When called in an EventHandler associated to the install event, it delays treating the installing worker as installed until the passed Promise resolves successfully. This is primarily used to ensure that a service worker is not considered installed until all of the core caches it depends on are populated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ExtendableEvent/waitUntil
	 */
	this.waitUntil() = '';

	/**
	 * @brief The ExtendableEvent.waitUntil() method extends the lifetime of the event. When called in an EventHandler associated to the install event, it delays treating the installing worker as installed until the passed Promise resolves successfully. This is primarily used to ensure that a service worker is not considered installed until all of the core caches it depends on are populated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ExtendableEvent/waitUntil
	 */
	this.waitUntil = function() {};

}

