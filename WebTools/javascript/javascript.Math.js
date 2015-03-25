
/**
 * @brief Math is a built-in object that has properties and methods for mathematical constants and functions. Not a function object.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math
 */
function Math() {

	/**
	 * @brief The Math.E property represents the base of natural logarithms, e, approximately 2.718.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/E
	 */
	this.E = '';

	/**
	 * @brief The Math.LN10 property represents the natural logarithm of 10, approximately 2.302:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/LN10
	 */
	this.LN10 = '';

	/**
	 * @brief The Math.LN2 property represents the natural logarithm of 2, approximately 0.693:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/LN2
	 */
	this.LN2 = '';

	/**
	 * @brief The Math.LOG10E property represents the base 10 logarithm of e, approximately 0.434:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/LOG10E
	 */
	this.LOG10E = '';

	/**
	 * @brief The Math.LOG2E property represents the base 2 logarithm of e, approximately 1.442:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/LOG2E
	 */
	this.LOG2E = '';

	/**
	 * @brief The Math.PI property represents the ratio of the circumference of a circle to its diameter, approximately 3.14159:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/PI
	 */
	this.PI = '';

	/**
	 * @brief The Math.SQRT1_2 property represents the square root of 1/2 which is approximately 0.707:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/SQRT1_2
	 */
	this.SQRT1_2 = '';

	/**
	 * @brief The Math.SQRT2 property represents the square root of 2, approximately 1.414:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/SQRT2
	 */
	this.SQRT2 = '';

	/**
	 * @brief The Math.abs() function returns the absolute value of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/abs
	 * @param x - A number.
	 */
	this.abs = function(x) {};

	/**
	 * @brief The Math.acos() function returns the arccosine (in radians) of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/acos
	 * @param x - A number.
	 */
	this.acos = function(x) {};

	/**
	 * @brief The Math.acosh() function returns the hyperbolic arc-cosine of a number
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/acosh
	 * @param x - A number.
	 */
	this.acosh = function(x) {};

	/**
	 * @brief The Math.asin() function returns the arcsine (in radians) of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/asin
	 * @param x - A number.
	 */
	this.asin = function(x) {};

	/**
	 * @brief The Math.asinh() function returns the hyperbolic arcsine of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/asinh
	 * @param x - A number.
	 */
	this.asinh = function(x) {};

	/**
	 * @brief The Math.atan() function returns the arctangent (in radians) of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/atan
	 * @param x - A number.
	 */
	this.atan = function(x) {};

	/**
	 * @brief The Math.atan2() function returns the arctangent of the quotient of its arguments.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/atan2
	 * @param y - First number.
	 * @param x - Second number.
	 */
	this.atan2 = function(y, x) {};

	/**
	 * @brief The Math.atanh() function returns the hyperbolic arctangent of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/atanh
	 * @param x - A number.
	 */
	this.atanh = function(x) {};

	/**
	 * @brief The Math.cbrt() function returns the cube root of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/cbrt
	 * @param x - A number.
	 */
	this.cbrt = function(x) {};

	/**
	 * @brief The Math.clz32() function returns the number of leading zero bits in the 32-bit binary representation of a number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/ceil
	 * @param x - A number.
	 */
	this.clz32 = function(x) {};

	/**
	 * @brief The Math.cos() function returns the cosine of a number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/cos
	 * @param x - A number given in unit of radians.
	 */
	this.cos = function(x) {};

	/**
	 * @brief The Math.cosh() function returns the hyperbolic cosine of a number, that can be expressed using the constant e:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/cosh
	 * @param x - A number.
	 */
	this.cosh = function(x) {};

	/**
	 * @brief The Math.exp() function returns ex, where x is the argument, and e is Euler's constant, the base of the natural logarithms.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/exp
	 * @param x - A number.
	 */
	this.exp = function(x) {};

	/**
	 * @brief The Math.expm1() function returns ex - 1, where x is the argument, and e the base of the natural logarithms.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/expm1
	 * @param x - A number.
	 */
	this.expm1 = function(x) {};

	/**
	 * @brief The Math.fround() function returns the nearest single precision float representation of a number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/floor
	 * @param x - A number.
	 */
	this.fround = function(x) {};

	/**
	 * @brief The Math.hypot() function returns the square root of the sum of squares of its arguments, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/hypot
	 * @param value1, value2, ... - Numbers.
	 */
	this.) {};

	/**
	 * @brief The Math.imul() function returns the result of the C-like 32-bit multiplication of the two parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/imul
	 * @param a - First number.
	 * @param b - Second number.
	 */
	this.imul = function(a, b) {};

	/**
	 * @brief The Math.log() function returns the natural logarithm (base e) of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/log
	 * @param x - A number.
	 */
	this.log = function(x) {};

	/**
	 * @brief The Math.log10() function returns the base 10 logarithm of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/log10
	 * @param x - A number.
	 */
	this.log10 = function(x) {};

	/**
	 * @brief The Math.log1p() function returns the natural logarithm (base e) of 1 + a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/log1p
	 * @param x - A number.
	 */
	this.log1p = function(x) {};

	/**
	 * @brief The Math.log2() function returns the base 2 logarithm of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/log2
	 * @param x - A number.
	 */
	this.log2 = function(x) {};

	/**
	 * @brief The Math.max() function returns the largest of zero or more numbers.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/max
	 * @param value1, value2, ... - Numbers.
	 */
	this.) {};

	/**
	 * @brief The Math.min() function returns the smallest of zero or more numbers.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/min
	 * @param value1, value2, ... - Numbers.
	 */
	this.) {};

	/**
	 * @brief The Math.pow() function returns the base to the exponent Power, that is, baseexponent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/pow
	 * @param base - The base number.
	 * @param exponent - The exponent used to raise the base.
	 */
	this.pow = function(base, exponent) {};

	/**
	 * @brief The Math.random() function returns a floating-point, pseudo-random number in the range [0, 1) that is, from 0 (inclusive) up to but not including 1 (exclusive), which you can then scale to your desired range.  The implementation selects the initial seed to the random number generation algorithm; it cannot be chosen or reset by the user.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/random
	 */
	this.random = function() {};

	/**
	 * @brief The Math.round() function returns the value of a number rounded to the nearest integer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/round
	 * @param x - A number.
	 */
	this.round = function(x) {};

	/**
	 * @brief The Math.sign() function returns the sign of a number, indicating whether the number is positive, negative or zero.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/sign
	 * @param x - A number.
	 */
	this.sign = function(x) {};

	/**
	 * @brief The Math.sin() function returns the sine of a number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/sin
	 * @param x - A number (given in radians).
	 */
	this.sin = function(x) {};

	/**
	 * @brief The Math.sinh() function returns the hyperbolic sine of a number, that can be expressed using the constant e:
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/sinh
	 * @param x - A number.
	 */
	this.sinh = function(x) {};

	/**
	 * @brief The Math.sqrt() function returns the square root of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/sqrt
	 * @param x - A number.
	 */
	this.sqrt = function(x) {};

	/**
	 * @brief The Math.tan() function returns the tangent of a number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/tan
	 * @param x - A number representing an angle in radians.
	 */
	this.tan = function(x) {};

	/**
	 * @brief The Math.tanh() function returns the hyperbolic tangent of a number, that is
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/tanh
	 * @param x - A number.
	 */
	this.tanh = function(x) {};

	/**
	 * @brief The Math.trunc() method returns the integral part of a number by removing any fractional digits.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/trunc
	 * @param x - A number.
	 */
	this.trunc = function(x) {};

}

