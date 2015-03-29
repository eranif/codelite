
/**
 * @brief The CacheStorage interface of the ServiceWorker API represents the storage for Cache objects. It provides a master directory of all the named caches that a ServiceWorker can access and maintains a mapping of string names to corresponding Cache objects.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage
 */
function CacheStorage() {

	/**
	 * @brief The match() method of the CacheStorage interface checks if a given Request is a key in any of the Cache objects that the CacheStorage object tracks and returns a Promise that resolves to the matching Response.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage/match
	 * @param request - The Request you are looking for a match for in the CacheStorage.
	 * @param options Optional - An options object allowing you to set specific control options for the matching done in the match operation. The available options are:
 
  ignoreSearch: A Boolean that specifies whether the matching process should ignore the query string in the url.  If set to true, the ?value=bar part of http://foo.com/?value=bar would be ignored when performing a match. It defaults to false.
  ignoreMethod: A Boolean that, when set to true, prevents matching operations from validating the Request http method (normally only GET and HEAD are allowed.) It defaults to false.
  ignoreVary: A Boolean that when set to true tells the matching operation not to perform VARY header matching — i.e. if the URL matches you will get a match regardless of the Response object having a VARY header or not. It defaults to false.
  cacheName: A DOMString that represents a specific cache to search within.
 

 
 Note: If a cacheName is set in the options object and the cache does not exist, the promise will reject (cache.match() just resolves undefined if a matching response is not found.)
	 */
	this.match = function(request, options Optional) {};

	/**
	 * @brief The has() method of the CacheStorage interface returns a Promise that resolves to true if a Cache object matching the cacheName exists.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage/has
	 * @param cacheName - A DOMString representing the name of the Cache object you are looking for in the CacheStorage.
	 */
	this.has = function(cacheName) {};

	/**
	 * @brief The open() method of the CacheStorage interface returns a Promise that resolves to the Cache object matching the cacheName.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage/open
	 * @param cacheName - The name of the cache you want to open.
	 */
	this.open = function(cacheName) {};

	/**
	 * @brief The delete() method of the CacheStorage interface finds the Cache object matching the cacheName, and if found, deletes the Cache object and returns a Promise that resolves to true. If no Cache object is found, it returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage/delete
	 * @param cacheName - The name of the cache you want to delete.
	 */
	this.delete = function(cacheName) {};

	/**
	 * @brief The keys() method of the CacheStorage interface returns a Promise that will resolve with an array containing strings corresponding to all of the named Cache objects tracked by the CacheStorage. Use this method to iterate over a list of all the Cache objects.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CacheStorage/keys
	 */
	this.keys = function() {};

}

