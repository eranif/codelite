
/**
 * @brief The CanvasPattern interface represents an opaque object describing a pattern, based on a image, a canvas or a video, created by the CanvasRenderingContext2D.createPattern() method.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasPattern
 */
function CanvasPattern() {

	/**
	 * @brief The CanvasPattern.setTransform() method uses an SVGMatrix object as the pattern's transformation matrix and invokes it on the pattern.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasPattern/setTransform
	 * @param matrix - An SVGMatrix to use as the pattern's transformation matrix.
	 */
	this.setTransform = function(matrix) {};

}

