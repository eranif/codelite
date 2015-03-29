
/**
 * @brief Provides access to the device's settings.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsManager
 */
function SettingsManager() {

	/**
	 * @brief The onsettingchange property lets you set an event handler to be called for all setting changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsManager/onsettingchange
	 */
	this.onsettingchange = '';

	/**
	 * @brief This method returns a SettingsLock object. This object is used to access and modify all the settings asynchronously and safely.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsManager/createLock
	 */
	this.createLock = function() {};

	/**
	 * @brief Sets up a callback function to be notified when a specific setting's value changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsManager/addObserver
	 * @param settingName - A string specifying the name of the setting to observe. The exact list of possible strings is device-dependent. Each Gaia build can have its own list of settings. For an up-to-date list of those strings, take a look at the Gaia source code.
	 * @param callback - The function to be called each time the value of the setting is changed. This function will receive as input a MozSettingsEvent providing details about the change that occurred.
	 */
	this.addObserver = function(settingName, callback) {};

	/**
	 * @brief This method is used to remove a callback function that has been added with SettingsManager.addObserver().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsManager/removeObserver
	 * @param settingName - The string representing the setting to stop to observe. The exact list of possible strings is device dependent. Each Gaia build can have its own list of settings. For an up to date list of those strings, take a look at the Gaia source code.
	 * @param callback - The function to be removed from the callback stack.
	 */
	this.removeObserver = function(settingName, callback) {};

}

