
/**
 * @brief The&nbsp;Promise&nbsp;object is used for deferred and asynchronous computations. A&nbsp;Promise&nbsp;is in one of these&nbsp;states:
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise
 */
function Promise() {

	/**
	 * @brief The Promise.all(iterable) method returns a promise that resolves when all of the promises in the iterable argument have resolved.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise/all
	 * @param iterable - An iterable object, such as an Array. See iterable.
	 */
	this.all = function(iterable) {};

	/**
	 * @brief The Promise.race(iterable) method returns a promise that resolves or rejects as soon as one of the promises in the iterable resolves or rejects, with the value or reason from that promise.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise/race
	 * @param iterable - An iterable object, such as an Array. See iterable.
	 */
	this.race = function(iterable) {};

	/**
	 * @brief The Promise.reject(reason) method returns a Promise object that is rejected with the given reason.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise/reject
	 * @param reason - Reason why this Promise rejected.
	 */
	this.reject = function(reason) {};

}

/**
 * @brief The catch() method returns a Promise and deals with rejected cases only. It behaves the same as calling Promise.prototype.then(undefined, onRejected).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise/catch
 * @param onRejected - A Function called when the Promise is rejected. This function has one argument, the rejection reason.
 */
Promise.prototype.catch = function(onRejected) {};

/**
 * @brief The then() method returns a Promise. It takes two arguments, both are callback functions for the success and failure cases of the Promise.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise/then
 * @param onFulfilled - A Function called when the Promise is fulfilled. This function has one argument, the fulfillment value.
 * @param onRejected - A Function called when the Promise is rejected. This function has one argument, the rejection reason.
 */
Promise.prototype.then = function(onFulfilled, onRejected) {};

