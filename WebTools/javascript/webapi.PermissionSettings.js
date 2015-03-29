
/**
 * @brief The PermissionSettings interface provides ways to let the user manage all the permissions requested by all apps on a Firefox OS device.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings
 */
function PermissionSettings() {

	/**
	 * @brief The get method allows to retrieve the current status of a given permission for a given app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/get
	 */
	this.get() = '';

	/**
	 * @brief The isExplicit method allows to know if the setting is explicit (true) or implicit (false).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/isExplicit
	 */
	this.isExplicit() = '';

	/**
	 * @brief The remove method allows to simply reset a permission for an app which is running inside the browser app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/remove
	 */
	this.remove() = '';

	/**
	 * @brief The set method allows to change a given permission for a given app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/set
	 */
	this.set() = '';

	/**
	 * @brief The get method allows to retrieve the current status of a given permission for a given app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/get
	 * @param permission - A string with the name of the permission to retrieve.
	 * @param manifest - A string representing the URL of the manifest of the app to check, or for an app running inside the browser, the URL of the browser app manifest.
	 * @param origin - A string representing the domain name of the app.
	 * @param browserFlag - A boolean indicating if the app is running in the browser instead of as an installed app.
	 */
	this.get = function(permission, manifest, origin, browserFlag) {};

	/**
	 * @brief The isExplicit method allows to know if the setting is explicit (true) or implicit (false).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/isExplicit
	 * @param permission - A string with the name of the permission to retrieve.
	 * @param manifest - A string representing the URL of the manifest of the app to check, or for an app running inside the browser, the URL of the browser app manifest.
	 * @param origin - A string representing the domain name of the app.
	 * @param browserFlag - A boolean indicating if the app is running in the browser instead of as an installed app.
	 */
	this.isExplicit = function(permission, manifest, origin, browserFlag) {};

	/**
	 * @brief The remove method allows to simply reset a permission for an app which is running inside the browser app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/remove
	 * @param permission - A string with the name of the permission to retrieve.
	 * @param manifest - A string representing the URL of the manifest of the app to check, or for an app running inside the browser, the URL of the browser app manifest.
	 * @param origin - A string representing the domain name of the app.
	 */
	this.remove = function(permission, manifest, origin) {};

	/**
	 * @brief The set method allows to change a given permission for a given app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PermissionSettings/set
	 * @param permission - A string with the name of the permission to retrieve.
	 * @param value - A string representing the new value for the permission, one of deny, prompt, or allow.
	 * @param manifest - A string representing the URL of the manifest of the app to check, or for an app running inside the browser, the URL of the browser app manifest.
	 * @param origin - A string representing the domain name of the app.
	 * @param browserFlag - A boolean indicating if the app is running in the browser instead of as an installed app.
	 */
	this.set = function(permission, value, manifest, origin, browserFlag) {};

}

