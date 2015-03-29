
/**
 * @brief Provides support for installing, managing, and controlling Open Web apps in a browser. Currently implemented as window.navigator.mozApps.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMApplicationsRegistry
 */
function DOMApplicationsRegistry() {

	/**
	 * @brief Gets information about the given app. This can be used to determine if the app is installed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMApplicationsRegistry/checkInstalled
	 * @param url - A string URL containing the location of the manifest of the app.
	 */
	this.checkInstalled = function(url) {};

	/**
	 * @brief Triggers the installation of an app. During the installation process, the app is validated and the user is prompted to approve the installation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMApplicationsRegistry/install
	 * @param url - A string URL containing the location of the manifest to be installed. In the case of self distribution (where the installing origin is the same as the app origin), the installing site may omit the origin part of the URL and provide an absolute path (beginning with /).
	 * @param receipts - (Optional) An array of one or more receipts. Example:
	 */
	this.install = function(url, receipts) {};

	/**
	 * @brief Returns information about the calling app, if any. You can use this to determine if an app is installed (i.e. if it is running in the browser, or in a separate app context.)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMApplicationsRegistry/getSelf
	 */
	this.getSelf = function() {};

	/**
	 * @brief Get a list of all installed apps from this origin. For example, if you call this on the Firefox Marketplace, you will get the list of apps installed by the Firefox Marketplace.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMApplicationsRegistry/getInstalled
	 */
	this.getInstalled = function() {};

}

