
/**
 * @brief 
 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager
 */
function PowerManager() {

	/**
	 * @brief The screenEnabled property allows to know if the device's screen is currently enabled (true).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/screenEnabled
	 */
	this.screenEnabled = '';

	/**
	 * @brief The screenBrightness property defines the brightness of the screen's backlight, on a scale from 0 (very dim) to 1 (full brightness).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/screenBrightness
	 */
	this.screenBrightness = '';

	/**
	 * @brief The cpuSleepAllowed property determines if the device's CPU will sleep after the screen is disabled. Setting this attribute to false will prevent the device from entering the suspend state.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/cpuSleepAllowed
	 */
	this.cpuSleepAllowed = '';

	/**
	 * @brief The addWakeLockListener method is used to register a handler to be called each time a resource changes its lock state.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/addWakeLockListener
	 * @param handler - It's a callback function that accepts two parameters: a string that contains the name of the resource and a string that figures the lock state for that resource (see getWakeLockState for more information about the various states).
	 */
	this.addWakeLockListener = function(handler) {};

	/**
	 * @brief The factoryReset method is used to reset the device to its factory state.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/factoryReset
	 */
	this.factoryReset = function() {};

	/**
	 * @brief The getWakeLockState method is used to retrieve the current lock state of a given resource.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/getWakeLockState
	 * @param resourceName - A string that represent the name of the resource to check.
	 */
	this.getWakeLockState = function(resourceName) {};

	/**
	 * @brief The powerOff method is used to shut off the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/powerOff
	 */
	this.powerOff = function() {};

	/**
	 * @brief The reboot method is used to completely shut down and boot the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/reboot
	 */
	this.reboot = function() {};

	/**
	 * @brief The removeWakeLockListener method is used to remove a handler previously set with addWakeLockListener.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PowerManager/removeWakeLockListener
	 * @param handler - A callback function previously set with addWakeLockListener.
	 */
	this.removeWakeLockListener = function(handler) {};

}

