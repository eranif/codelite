
/**
 * @brief The Path2D interface of the Canvas 2D API is used to declare paths that are then later used on CanvasRenderingContext2D objects. The path methods of the CanvasRenderingContext2D interface are present on this interface as well and are allowing you to create paths that you can retain and replay as required on a canvas.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Path2D
 */
function Path2D() {

	/**
	 * @brief The Path2D.addPath() method of the Canvas 2D API adds to the path the path given by the argument.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Path2D/addPath
	 * @param path - A Path2D path to add.
	 * @param transform Optional - An SVGMatrix to be used as the transformation matrix for the path that is added.
	 */
	this.addPath = function(path, transform Optional) {};

	/**
	 * @brief The CanvasRenderingContext2D.closePath() method of the Canvas 2D API causes the point of the pen to move back to the start of the current sub-path. It tries to draw a straight line from the current point to the start. If the shape has already been closed or has only one point, this function does nothing.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/closePath
	 */
	this.closePath = function() {};

	/**
	 * @brief The CanvasRenderingContext2D.moveTo() method of the Canvas 2D API moves the starting point of a new sub-path to the (x, y) coordinates.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/moveTo
	 * @param x - The x axis of the point.
	 * @param y - The y axis of the point.
	 */
	this.moveTo = function(x, y) {};

	/**
	 * @brief The CanvasRenderingContext2D.lineTo() method of the Canvas 2D API connects the last point in the sub-path to the x, y coordinates with a straight line (but does not actually draw it).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineTo
	 * @param x - The x axis of the coordinate for the end of the line.
	 * @param y - The y axis of the coordinate for the end of the line.
	 */
	this.lineTo = function(x, y) {};

	/**
	 * @brief The CanvasRenderingContext2D.bezierCurveTo() method of the Canvas 2D API adds a cubic Bézier curve to the path. It requires three points. The first two points are control points and the third one is the end point. The starting point is the last point in the current path, which can be changed using moveTo() before creating the Bézier curve.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/bezierCurveTo
	 * @param cp1x - The x axis of the coordinate for the first control point.
	 * @param cp1y - The y axis of the coordinate for first control point.
	 * @param cp2x - The x axis of the coordinate for the second control point.
	 * @param cp2y - The y axis of the coordinate for the second control point.
	 * @param x - The x axis of the coordinate for the end point.
	 * @param y - The y axis of the coordinate for the end point.
	 */
	this.bezierCurveTo = function(cp1x, cp1y, cp2x, cp2y, x, y) {};

	/**
	 * @brief The CanvasRenderingContext2D.quadraticCurveTo() method of the Canvas 2D API adds a quadratic Bézier curve to the path. It requires two points. The first point is a control point and the second one is the end point. The starting point is the last point in the current path, which can be changed using moveTo() before creating the quadratic Bézier curve.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/quadraticCurveTo
	 * @param cpx - The x axis of the coordinate for the control point.
	 * @param cpy - The y axis of the coordinate for the control point.
	 * @param x - The x axis of the coordinate for the end point.
	 * @param y - The y axis of the coordinate for the end point.
	 */
	this.quadraticCurveTo = function(cpx, cpy, x, y) {};

	/**
	 * @brief The CanvasRenderingContext2D.arc() method of the Canvas 2D API adds an arc to the path which is centered at (x, y) position with radius r starting at startAngle and ending at endAngle going in the given direction by anticlockwise (defaulting to clockwise).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arc
	 * @param x - The x axis of the coordinate for the arc's center.
	 * @param y - The y axis of the coordinate for the arc's center.
	 * @param radius - The arc's radius.
	 * @param startAngle - The starting point, measured from the x axis, from which it will be drawn, expressed in radians.
	 * @param endAngle - The end arc's angle to which it will be drawn, expressed in radians.
	 * @param anticlockwise Optional - An optional Boolean which, if true, draws the arc anticlockwise (counter-clockwise), otherwise in a clockwise direction.
	 */
	this.arc = function(x, y, radius, startAngle, endAngle, anticlockwise Optional) {};

	/**
	 * @brief The CanvasRenderingContext2D.arcTo() method of the Canvas 2D API adds an arc to the path with the given control points and radius, connected to the previous point by a straight line.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arcTo
	 * @param x1 - The x axis of the coordinate for the first control point.
	 * @param y1 - The y axis of the coordinate for the first control point.
	 * @param x2 - The x axis of the coordinate for the second control point.
	 * @param y2 - The y axis of the coordinate for the second control point.
	 * @param radius - The arc's radius.
	 */
	this.arcTo = function(x1, y1, x2, y2, radius) {};

	/**
	 * @brief The CanvasRenderingContext2D.ellipse() method of the Canvas 2D API adds an ellipse to the path which is centered at (x, y) position with the radii radiusX and radiusY starting at startAngle and ending at endAngle going in the given direction by anticlockwise (defaulting to clockwise).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/ellipse
	 * @param x - The x axis of the coordinate for the ellipse's center.
	 * @param y - The y axis of the coordinate for the ellipse's center.
	 * @param radiusX - The ellipse's major-axis radius.
	 * @param radiusY - The ellipse's minor-axis radius.
	 * @param rotation - The rotation for this ellipse, expressed in degrees.
	 * @param startAngle - The starting point, measured from the x axis, from which it will be drawn, expressed in radians.
	 * @param endAngle - The end ellipse's angle to which it will be drawn, expressed in radians.
	 * @param anticlockwise Optional - An optional Boolean which, if true, draws the ellipse anticlockwise (counter-clockwise), otherwise in a clockwise direction.
	 */
	this.ellipse = function(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise Optional) {};

	/**
	 * @brief The CanvasRenderingContext2D.rect() method of the Canvas 2D API creates a path for a rectangle at position (x, y) with a size that is determined by width and height. Those four points are connected by straight lines and the sub-path is marked as closed, so that you can fill or stroke this rectangle.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/rect
	 * @param x - The x axis of the coordinate for the rectangle starting point.
	 * @param y - The y axis of the coordinate for the rectangle starting point.
	 * @param width - The rectangle's width.
	 * @param height - The rectangle's height.
	 */
	this.rect = function(x, y, width, height) {};

}

