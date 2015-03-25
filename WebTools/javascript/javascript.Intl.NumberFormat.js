
/**
 * @brief The Intl.NumberFormat object is a constructor for objects that enable language sensitive number formatting.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/NumberFormat
 */
function Intl.NumberFormat() {

	/**
	 * @brief The Intl.NumberFormat.prototype.format property returns a getter function that formats a number according to the locale and formatting options of this NumberFormat object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/NumberFormat/format
	 */
	this.format = '';

	/**
	 * @brief The Intl.NumberFormat.supportedLocalesOf() method returns an array containing those of the provided locales that are supported in number formatting without having to fall back to the runtime's default locale.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/NumberFormat/supportedLocalesOf
	 * @param locales - A string with a BCP 47 language tag, or an array of such strings. For the general form of the locales argument, see the Intl page.
	 * @param options - Optional. An object that may have the following property:     localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
	 */
	this.supportedLocalesOf = function(locales, options) {};

}

/**
 * @brief The Intl.NumberFormat.prototype.resolvedOptions() method returns a new object with properties reflecting the locale and number formatting options computed during initialization of this NumberFormat object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/NumberFormat/resolvedOptions
 */
Intl.NumberFormat.prototype.resolvedOptions = function() {};

