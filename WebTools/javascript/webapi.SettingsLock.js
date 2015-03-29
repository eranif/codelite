
/**
 * @brief Each call to SettingsManager.createLock() create a new SettingsLock object.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsLock
 */
function SettingsLock() {

	/**
	 * @brief The closed property indicates whether or not the settings lock is closed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsLock/closed
	 */
	this.closed = '';

	/**
	 * @brief This method is changes the value of one or more given settings.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsLock/set
	 * @param settings - An object containing a set of key/value pairs where each key represents the string name of a given setting. The exact list of possible strings is device dependent. Each Gaia build can have its own list of settings. For an up-to-date list of those strings, take a look at the Gaia source code.
	 */
	this.set = function(settings) {};

	/**
	 * @brief This method fetches the value of a specified setting.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsLock/get
	 * @param settingName - The string name of the setting whose value you wish to retreive. The exact list of possible strings is device dependent. Each Gaia build can have its own list of settings. For an up-to-date list of those strings, take a look at the Gaia source code.
	 */
	this.get = function(settingName) {};

	/**
	 * @brief This method clears the queue of SettingsLock objects.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SettingsLock/clear
	 */
	this.clear = function() {};

}

