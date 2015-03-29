
/**
 * @brief The PushManager interface provides a way to receive notifications from third-party servers as well as request URLs for push notifications. This interface has replaced functionality offered by the obsolete PushRegistrationManager.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager
 */
function PushManager() {

	/**
	 * @brief The PushManager.subscribe() method of the PushManager interface returns a Promise that resolves to a PushSubscription} with details of a new push subscription.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/subscribe
	 */
	this.subscribe() = '';

	/**
	 * @brief The PushManager.getSubscription() method of the PushManager interface returns a Promise that resolves to a PushSubscription} with details of the retrieved push subscription.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/getSubscription
	 */
	this.getSubscription() = '';

	/**
	 * @brief The PushManager.hasPermission() method of the PushManager interface returns a Promise that resolves to the PushPermissionStatus of the requesting webapp, which will be one of granted, denied, or default.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/hasPermission
	 */
	this.hasPermission() = '';

	/**
	 * @brief The PushManager.subscribe() method of the PushManager interface returns a Promise that resolves to a PushSubscription} with details of a new push subscription.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/subscribe
	 */
	this.subscribe = function() {};

	/**
	 * @brief The PushManager.getSubscription() method of the PushManager interface returns a Promise that resolves to a PushSubscription} with details of the retrieved push subscription.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/getSubscription
	 */
	this.getSubscription = function() {};

	/**
	 * @brief The PushManager.hasPermission() method of the PushManager interface returns a Promise that resolves to the PushPermissionStatus of the requesting webapp, which will be one of granted, denied, or default.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PushManager/hasPermission
	 */
	this.hasPermission = function() {};

}

