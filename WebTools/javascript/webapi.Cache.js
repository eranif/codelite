
/**
 * @brief The Cache interface of the ServiceWorker API represents the storage for Request / Response object pairs that are cached as part of the ServiceWorker life cycle.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache
 */
function Cache() {

	/**
	 * @brief The match() method of the Cache interface returns a Promise that resolves to the Response associated with the first matching request in the Cache object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/match
	 * @param request - The Request you are attempting to find in the Cache.
	 * @param options Optional - An options object allowing you to set specific control options for the matching done in the match operation. The available options are:
 
  ignoreSearch: A Boolean that specifies whether the matching process should ignore the query string in the url.  If set to true, the ?value=bar part of http://foo.com/?value=bar would be ignored when performing a match. It defaults to false.
  ignoreMethod: A Boolean that, when set to true, prevents matching operations from validating the Request http method (normally only GET and HEAD are allowed.) It defaults to false.
  ignoreVary: A Boolean that when set to true tells the matching operation not to perform VARY header matching — i.e. if the URL matches you will get a match regardless of the Response object having a VARY header or not. It defaults to false.
  cacheName: A DOMString that represents a specific cache to search within. Note that this option is ignored by Cache.match().
	 */
	this.ache.match(request, options)

	/**
	 * @brief The matchAll() method of the Cache interface returns a Promise that resolves to an array of all matching requests in the Cache object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/matchAll
	 * @param request - The Request you are attempting to find in the Cache.
	 * @param options Optional - An options object allowing you to set specific control options for the matching done in the match operation. The available options are:
 
  ignoreSearch: A Boolean that specifies whether the matching process should ignore the query string in the url.  If set to true, the ?value=bar part of http://foo.com/?value=bar would be ignored when performing a match. It defaults to false.
  ignoreMethod: A Boolean that, when set to true, prevents matching operations from validating the Request http method (normally only GET and HEAD are allowed.) It defaults to false.
  ignoreVary: A Boolean that when set to true tells the matching operation not to perform VARY header matching — i.e. if the URL matches you will get a match regardless of the Response object having a VARY header or not. It defaults to false.
  cacheName: A DOMString that represents a specific cache to search within. Note that this option is ignored by Cache.matchAll().
	 */
	this.ache.matchAll(request, options)

	/**
	 * @brief The add() method of the Cache interface returns a Promise that resolves to a new Cache entry whose key is the request.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/add
	 * @param request - The Request you want to add to the cache.
	 */
	this.ache.add(request)

	/**
	 * @brief The add() method of the Cache interface returns a Promise that resolves to a new array of Cache entries whose keys are the requests.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/addAll
	 * @param requests - An array of Request objects you want to add to the cache.
	 */
	this.ache.addAll(requests)

	/**
	 * @brief The put() method of the Cache interface allows key/value pairs to be added to the current Cache object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/put
	 * @param request - The Request you want to add to the cache.
	 * @param response - The response you want to match up to the request.
	 */
	this.ache.put(request, response)

	/**
	 * @brief The delete() method of the Cache interface finds the Cache entry whose key is the request, and if found, deletes the Cache entry and returns a Promise that resolves to true. If no Cache entry is found, it returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/delete
	 * @param request - The Request you are looking to delete.
	 * @param options Optional - An options object allowing you to set specific control options for the matching done in the delete operation. The available options are:
 
  ignoreSearch: A Boolean that specifies whether the matching process should ignore the query string in the url.  If set to true, the ?value=bar part of http://foo.com/?value=bar would be ignored when performing a match. It defaults to false.
  ignoreMethod: A Boolean that, when set to true, prevents matching operations from validating the Request http method (normally only GET and HEAD are allowed.) It defaults to false.
  ignoreVary: A Boolean that when set to true tells the matching operation not to perform VARY header matching — i.e. if the URL matches you will get a match regardless of the Response object having a VARY header or not. It defaults to false.
  cacheName: A DOMString that represents a specific cache to search within. Note that this option is ignored by Cache.delete().
	 */
	this.ache.delete(request, options)

	/**
	 * @brief The keys() method of the Cache interface returns a Promise that resolves to an array of Cache keys.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Cache/keys
	 * @param request Optional - The Request want to return, if a specific key is desired.
	 * @param options Optional - An options object allowing you to set specific control options for the matching done in the keys operation. The available options are:
 
  ignoreSearch: A Boolean that specifies whether the matching process should ignore the query string in the url.  If set to true, the ?value=bar part of http://foo.com/?value=bar would be ignored when performing a match. It defaults to false.
  ignoreMethod: A Boolean that, when set to true, prevents matching operations from validating the Request http method (normally only GET and HEAD are allowed.) It defaults to false.
  ignoreVary: A Boolean that when set to true tells the matching operation not to perform VARY header matching — i.e. if the URL matches you will get a match regardless of the Response object having a VARY header or not. It defaults to false.
  cacheName: A DOMString that represents a specific cache to search within. Note that this option is ignored by Cache.keys().
	 */
	this.ache.keys(request, options)

}

