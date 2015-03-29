
/**
 * @brief The ServiceWorkerContainer interface of the ServiceWorker API provides an object representing the service worker as an overall unit in the network ecosystem, including facilities to register, unregister and update service workers, and access the state of service workers and their registrations.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerContainer
 */
function ServiceWorkerContainer() {

	/**
	 * @brief The register() method of the ServiceWorkerContainer interface creates or updates a ServiceWorkerRegistration for the given scriptURL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerContainer/register
	 * @param scriptURL - The URL of the service worker script.
	 * @param options Optional - An options object to provide options upon registration. Currently available options are:
 
  scope: A USVString representing a URL that defines a service worker's registration scope; what range of URLs a service worker can control. This is usually a relative URL, and it defaults to '/' when not specified.
	 */
	this.register = function(scriptURL, options Optional) {};

	/**
	 * @brief The getRegistration() method of the ServiceWorkerContainer interface gets a ServiceWorkerRegistration object whose scope URL matches the provided document URL.  If the method can't return a ServiceWorkerRegistration, it returns a Promise.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerContainer/getRegistration
	 * @param scope Optional - A unique identifier for a service worker registration — the scope URL of the registration object you want to return. This is usually a relative URL.
	 */
	this.getRegistration = function(scope Optional) {};

	/**
	 * @brief The getRegistrations() method of the ServiceWorkerContainer interface returns all ServiceWorkerRegistrations associated with a ServiceWorkerContainer in an array.  If the method can't return ServiceWorkerRegistrations, it returns a Promise.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ServiceWorkerContainer/getRegistrations
	 */
	this.getRegistrations = function() {};

}

