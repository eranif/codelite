
/**
 * @brief A Touch object represents a single point of contact between the user and a touch-sensitive interface device (which may be, for example, a touchscreen or a trackpad).
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch
 */
function Touch() {

	/**
	 * @brief Returns a value uniquely identifying this point of contact with the touch surface. This value remains consistent for every event involving this finger's (or stylus's) movement on the surface until it is lifted off the surface.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/identifier
	 */
	this.identifier = '';

	/**
	 * @brief Returns the X coordinate of the touch point relative to the screen, not including any scroll offset.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/screenX
	 */
	this.pageX = '';

	/**
	 * @brief Returns the Y coordinate of the touch point relative to the viewport, including any scroll offset.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/pageY
	 */
	this.pageY = '';

	/**
	 * @brief Returns the X radius of the ellipse that most closely circumscribes the area of contact with the screen. The value is in pixels of the same scale as screenX.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/radiusX
	 */
	this.radiusX = '';

	/**
	 * @brief Returns the Y radius of the ellipse that most closely circumscribes the area of contact with the screen. The value is in pixels of the same scale as screenY.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/radiusY
	 */
	this.radiusY = '';

	/**
	 * @brief Returns the rotation angle, in degrees, of the contact area ellipse defined by Touch.radiusX and Touch.radiusY. The value may be between 0 and 90. Together, these three values describe an ellipse that approximates the size and shape of the area of contact between the user and the screen. This may be a large ellipse representing the contact between a fingertip and the screen or a small one representing the tip of a stylus, for example.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/rotationAngle
	 */
	this.rotationAngle = '';

	/**
	 * @brief Returns the amount of pressure the user is applying to the touch surface for this Touch.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/force
	 */
	this.force = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Touch/target
	 */
	this.target = '';

}

