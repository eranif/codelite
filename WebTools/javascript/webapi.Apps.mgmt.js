
/**
 * @brief Provides support for controlling Open Web apps in a browser. Currently implemented as window.navigator.mozApps.mgmt. The apps management API is privileged. It is intended to grant access to trusted pages, also called &quot;dashboards&quot;. The management API exposes functions that let dashboards manage and launch apps on a user's behalf. Additionally, the API exposes functions for app sync, which lets the dashboard display the logged-in state of the user and allows the user to sign up or register for an account to synchronize apps across devices.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Apps.mgmt
 */
function Apps.mgmt() {

	/**
	 * @brief Lists all installed apps in the user's repository.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Apps.mgmt.getAll
	 */
	this.getAll = function() {};

	/**
	 * @brief Enable privileged pages to listen for events that indicate an app has been installed or uninstalled, and perform an action in response.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Apps.mgmt.addEventListener
	 */
	this.addEventListener = function() {};

	/**
	 * @brief removeEventListener is used to remove something added by addEventListener:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Apps.mgmt.removeEventListener
	 */
	this.removeEventListener = function() {};

}

