
/**
 * @brief The MozSettingsEvent represents a settingchange event, providing information about a change to the value of a setting on the device. It extends the DOM Event interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSettingsEvent
 */
function MozSettingsEvent() {

	/**
	 * @brief The settingName property of the MozSettingsEvent specifies the exact name of the setting that has changed. For an up-to-date list of possible setting names, take a look at the Gaia source code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSettingsEvent/settingName
	 */
	this.settingName = '';

	/**
	 * @brief The settingValue property of the MozSettingsEvent object returns the setting's new value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSettingsEvent/settingValue
	 */
	this.settingValue = '';

}

