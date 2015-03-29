
/**
 * @brief The CSS interface holds useful CSS-related methods. No object with this interface are implemented: it contains only static methods and therefore is a utilitarian interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CSS
 */
function CSS() {

	/**
	 * @brief The CSS.supports() static methods returns a Boolean value indicating if the browser supports a given CSS feature, or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CSS/supports
	 */
	this.supports() = '';

	/**
	 * @brief The CSS.supports() static methods returns a Boolean value indicating if the browser supports a given CSS feature, or not.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CSS/supports
	 * @param propertyName - A DOMString containing the name of the CSS property to check.
	 * @param value - A DOMString containing the value of the CSS property to check.
	 * @param supportCondition - A DOMString containing the condition to check.
	 */
	this.supports = function(propertyName, value, supportCondition) {};

}

