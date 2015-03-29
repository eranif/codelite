
/**
 * @brief The WorkerNavigator interface represents a subset of the Navigator interface allowed to be accessed from a Worker. Such an object is initialized for each worker and is available via the WorkerGlobalScope.navigator property obtained by calling window.self.navigator.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/WorkerNavigator
 */
function WorkerNavigator() {

	/**
	 * @brief The NavigatorID.taintEnabled() method always returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NavigatorID/taintEnabled
	 */
	this.taintEnabled = function() {};

}

