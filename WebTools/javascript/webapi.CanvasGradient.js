
/**
 * @brief The CanvasGradient interface represents an opaque object describing a gradient. It is returned by the methods CanvasRenderingContext2D.createLinearGradient() or CanvasRenderingContext2D.createRadialGradient().
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient
 */
function CanvasGradient() {

	/**
	 * @brief The CanvasGradient.addColorStop() method adds a new stop, defined by an offset and a color, to the gradient. If the offset is not between 0 and 1, an INDEX_SIZE_ERR is raised, if the color can't be parsed as a CSS &lt;color>, a SYNTAX_ERR is raised.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient/addColorStop
	 */
	this.addColorStop() = '';

	/**
	 * @brief The CanvasGradient.addColorStop() method adds a new stop, defined by an offset and a color, to the gradient. If the offset is not between 0 and 1, an INDEX_SIZE_ERR is raised, if the color can't be parsed as a CSS &lt;color>, a SYNTAX_ERR is raised.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient/addColorStop
	 * @param offset - A number between 0 and 1. An INDEX_SIZE_ERR is raised, if the number is not in that range.
	 * @param color - A CSS &lt;color&gt;. A SYNTAX_ERR is raised, if the value can not be parsed as a CSS &lt;color&gt; value.
	 */
	this.addColorStop = function(offset, color) {};

}

