
/**
 * @brief The Number JavaScript object is a wrapper object allowing you to work with numerical values. A Number object is created using the Number() constructor.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number
 */
function Number() {

	/**
	 * @brief The Number.EPSILON property represents the difference between one and the smallest value greater than one that can be represented as a Number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/EPSILON
	 */
	this.EPSILON = '';

	/**
	 * @brief The Number.MAX_SAFE_INTEGER constant represents the maximum safe integer in JavaScript (253 - 1).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MAX_SAFE_INTEGER
	 */
	this.MAX_SAFE_INTEGER = '';

	/**
	 * @brief The Number.MAX_VALUE property represents the maximum numeric value representable in JavaScript.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MAX_VALUE
	 */
	this.MAX_VALUE = '';

	/**
	 * @brief The Number.MIN_SAFE_INTEGER constant represents the minimum safe integer in JavaScript (-(253 - 1)).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MIN_SAFE_INTEGER
	 */
	this.MIN_SAFE_INTEGER = '';

	/**
	 * @brief The Number.MIN_VALUE property represents the smallest positive numeric value representable in JavaScript.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MIN_VALUE
	 */
	this.MIN_VALUE = '';

	/**
	 * @brief The Number.NEGATIVE_INFINITY property represents the negative Infinity value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/NEGATIVE_INFINITY
	 */
	this.NEGATIVE_INFINITY = '';

	/**
	 * @brief The Number.NaN property represents Not-A-Number. Equivalent of NaN.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/NaN
	 */
	this.NaN = '';

	/**
	 * @brief The Number.POSITIVE_INFINITY property represents the positive Infinity value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/POSITIVE_INFINITY
	 */
	this.POSITIVE_INFINITY = '';

	/**
	 * @brief The Number.isFinite() method determines whether the passed value is a finite number.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/isFinite
	 * @param value - The value to be tested for finiteness.
	 */
	this.isFinite = function(value) {};

	/**
	 * @brief The Number.isInteger() method determines whether the passed value is an integer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/isInteger
	 * @param value - The value to be tested for being an integer.
	 */
	this.isInteger = function(value) {};

	/**
	 * @brief The Number.isNaN() method determines whether the passed value is NaN. More robust version of the original global isNaN().
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/isNaN
	 * @param value - The value to be tested for NaN.
	 */
	this.isNaN = function(value) {};

	/**
	 * @brief The Number.isSafeInteger() method determines whether the provided value is a number that is a safe integer. A safe integer is an integer that
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/isSafeInteger
	 * @param testValue - The value to be tested for being a safe integer.
	 */
	this.isSafeInteger = function(testValue) {};

	/**
	 * @brief The Number.parseFloat() method parses a string argument and returns a floating point number. This method behaves identically to the global function parseFloat() and is part of ECMAScript 6 (its purpose is modularization of globals).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/parseFloat
	 * @param string - A string that represents the value you want to parse.
	 */
	this.parseFloat = function(string) {};

	/**
	 * @brief The Number.parseInt() method parses a string argument and returns an integer of the specified radix or base.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/parseInt
	 * @param string - The value to parse. If string is not a string, then it is converted to one. Leading whitespace in the string is ignored.
	 * @param radix - An integer between 2 and 36 that represents the radix (the base in mathematical numeral systems) of the above mentioned string. Specify 10 for the decimal numeral system commonly used by humans. Always specify this parameter to eliminate reader confusion and to guarantee predictable behavior. Different implementations produce different results when a radix is not specified.
	 */
	this.parseInt = function(string, radix) {};

	/**
	 * @brief The Number.toInteger() method used to evaluate the passed value and convert it to an integer, but its implementation has been removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toInteger
	 * @param number - The value to be converted to an integer.
	 */
	this.toInteger = function(number) {};

}

/**
 * @brief The toExponential() method returns a string representing the Number object in exponential notation
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toExponential
 * @param fractionDigits - Optional. An integer specifying the number of digits after the decimal point. Defaults to as many digits as necessary to specify the number.
 */
Number.prototype.toExponential = function(fractionDigits) {};

/**
 * @brief The toFixed() method formats a number using fixed-point notation.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toFixed
 * @param digits - Optional. The number of digits to appear after the decimal point; this may be a value between 0 and 20, inclusive, and implementations may optionally support a larger range of values. If this argument is omitted, it is treated as 0.
 */
Number.prototype.toFixed = function(digits) {};

/**
 * @brief The toLocaleString() method returns a string with a language sensitive representation of this number.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toLocaleString
 * @param locales - A string with a BCP 47 language tag, or an array of such strings. For the general form and interpretation of the locales argument, see the Intl page. The following Unicode extension key is allowed:  nu   The numbering system to be used. Possible values include: "arab", "arabext", "bali", "beng", "deva", "fullwide", "gujr", "guru", "hanidec", "khmr", "knda", "laoo", "latn", "limb", "mlym", "mong", "mymr", "orya", "tamldec", "telu", "thai", "tibt".
 * @param options - An object with some or all of the following properties:  localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
 * @param style - The formatting style to use. Possible values are "decimal" for plain number formatting, "currency" for currency formatting, and "percent" for percent formatting; the default is "decimal".
 * @param currency - The currency to use in currency formatting. Possible values are the ISO 4217 currency codes, such as "USD" for the US dollar, "EUR" for the euro, or "CNY" for the Chinese RMB — see the Current currency &amp; funds code list. There is no default value; if the style is "currency", the currency property must be provided.
 * @param currencyDisplay - How to display the currency in currency formatting. Possible values are "symbol" to use a localized currency symbol such as €, "code" to use the ISO currency code, "name" to use a localized currency name such as "dollar"; the default is "symbol".
 * @param useGrouping - Whether to use grouping separators, such as thousands separators or thousand/lakh/crore separators. Possible values are true and false; the default is true.
 * @param minimumIntegerDigits - The minimum number of integer digits to use. Possible values are from 1 to 21; the default is 1.
 * @param minimumFractionDigits - The minimum number of fraction digits to use. Possible values are from 0 to 20; the default for plain number and percent formatting is 0; the default for currency formatting is the number of minor unit digits provided by the ISO 4217 currency code list (2 if the list doesn't provide that information).
 * @param maximumFractionDigits - The maximum number of fraction digits to use. Possible values are from 0 to 20; the default for plain number formatting is the larger of minimumFractionDigits and 3; the default for currency formatting is the larger of minimumFractionDigits and the number of minor unit digits provided by the ISO 4217 currency code list (2 if the list doesn't provide that information); the default for percent formatting is the larger of minimumFractionDigits and 0.
 * @param minimumSignificantDigits - The minimum number of significant digits to use. Possible values are from 1 to 21; the default is 1.
 * @param maximumSignificantDigits - The maximum number of significant digits to use. Possible values are from 1 to 21; the default is minimumSignificantDigits.
 */
Number.prototype.toLocaleString = function(locales, options, style, currency, currencyDisplay, useGrouping, minimumIntegerDigits, minimumFractionDigits, maximumFractionDigits, minimumSignificantDigits, maximumSignificantDigits) {};

/**
 * @brief The toPrecision() method returns a string representing the Number object to the specified precision.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toPrecision
 * @param precision - Optional. An integer specifying the number of significant digits.
 */
Number.prototype.toPrecision = function(precision) {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toSource
 */
Number.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the specified Number object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/toString
 * @param radix - Optional. An integer between 2 and 36 specifying the base to use for representing numeric values.
 */
Number.prototype.toString = function(radix) {};

/**
 * @brief The valueOf() method returns the wrapped primitive value of a Number object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/valueOf
 */
Number.prototype.valueOf = function() {};

