
/**
 * @brief The Screen interface represents a screen, usually the one on which the current window is being rendered.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen
 */
function Screen() {

	/**
	 * @brief Specifies the y-coordinate of the first pixel that is not allocated to permanent or semipermanent user interface features.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/availTop
	 */
	this.availTop = '';

	/**
	 * @brief Returns the first available pixel available from the left side of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/availLeft
	 */
	this.availLeft = '';

	/**
	 * @brief Returns the amount of vertical space available to the window on the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/availHeight
	 */
	this.availHeight = '';

	/**
	 * @brief Returns the amount of horizontal space in pixels available to the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/availWidth
	 */
	this.availWidth = '';

	/**
	 * @brief Returns the color depth of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/colorDepth
	 */
	this.colorDepth = '';

	/**
	 * @brief Returns the height of the screen in pixels.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/height
	 */
	this.height = '';

	/**
	 * @brief Returns the distance in pixels from the left side of the main screen to the left side of the current screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/left
	 */
	this.left = '';

	/**
	 * @brief The Screen.orientation property give the current orientation of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/orientation
	 */
	this.orientation = '';

	/**
	 * @brief Returns the bit depth of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/pixelDepth
	 */
	this.pixelDepth = '';

	/**
	 * @brief Returns the distance in pixels from the top side of the current screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/top
	 */
	this.top = '';

	/**
	 * @brief Returns the width of the screen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/width
	 */
	this.width = '';

	/**
	 * @brief The lockOrientation method locks the screen into the specified orientation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/lockOrientation
	 * @param orientation - The orientation into which to lock the screen. This is either a string or an array of strings. Passing several strings lets the screen rotate only in the selected orientations.
	 * @param portrait-primary - It represents the orientation of the screen when it is in its primary portrait mode. A screen is considered in its primary portrait mode if the device is held in its normal position and that position is in portrait, or if the normal position of the device is in landscape and the device held turned by 90° clockwise. The normal position is device dependant.
	 * @param portrait-secondary - It represents the orientation of the screen when it is in its secondary portrait mode. A screen is considered in its secondary portrait mode if the device is held 180° from its normal position and that position is in portrait, or if the normal position of the device is in landscape and the device held is turned by 90° anticlockwise. The normal position is device dependant.
	 * @param landscape-primary - It represents the orientation of the screen when it is in its primary landscape mode. A screen is considered in its primary landscape mode if the device is held in its normal position and that position is in landscape, or if the normal position of the device is in portrait and the device held is turned by 90° clockwise. The normal position is device dependant.
	 * @param landscape-secondary - It represents the orientation of the screen when it is in its secondary landscape mode. A screen is considered in its secondary landscape mode if the device held is 180° from its normal position and that position is in landscape, or if the normal position of the device is in portrait and the device held is turned by 90° anticlockwise. The normal position is device dependant.
	 * @param portrait - It represents both portrait-primary and portrait-secondary.
	 * @param landscape - It represents both landscape-primary and landscape-secondary.
	 * @param default - It represents either portrait-primary and landscape-primary depends on natural orientation of devices. For example, if the panel resolution is 1280*800, default will make it landscape, if the resolution is 800*1280, default will make it to portrait.
	 */
	this.creen.lockOrientation

	/**
	 * @brief The Screen.unlockOrientation method removes all the previous screen locks set by the page/app.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Screen/unlockOrientation
	 */
	this.creen.unlockOrientation

}

