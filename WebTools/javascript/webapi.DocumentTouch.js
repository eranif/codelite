
/**
 * @brief The DocumentTouch interface used to provides convenience methods for creating Touch and TouchList objects, but has been removed. The methods live on the Document interface now.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentTouch
 */
function DocumentTouch() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentTouch/createTouch
	 * @param view - The window in which the touch occurred.
	 * @param target - The EventTarget for the touch.
	 * @param identifier - The value for Touch.identifier.
	 * @param pageX - The value for Touch.pageX.
	 * @param pageY - The value for Touch.pageY.
	 * @param screenX - The value for Touch.screenX.
	 * @param screenY - The value for Touch.screenY.
	 * @param clientX - The value for Touch.clientX.
	 * @param clientY - The value for Touch.clientY.
	 * @param radiusX - The value for Touch.radiusX.
	 * @param radiusY - The value for Touch.radiusY.
	 * @param rotationAngle - The value for Touch.rotationAngle.
	 * @param force - The value for Touch.force.
	 */
	this.createTouch = function(view, target, identifier, pageX, pageY, screenX, screenY, clientX, clientY, radiusX, radiusY, rotationAngle, force) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DocumentTouch/createTouchList
	 * @param touches - Either a single Touch object or an array of Touch objects.
	 */
	this.createTouchList = function(touches) {};

}

