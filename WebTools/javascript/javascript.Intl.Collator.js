
/**
 * @brief The Intl.Collator object is a constructor for collators, objects that enable language sensitive string comparison.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Collator
 */
function Intl.Collator() {

	/**
	 * @brief The Intl.Collator.prototype.compare property returns a getter function that compares two strings according to the sort order of this Collator object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Collator/compare
	 */
	this.compare = '';

	/**
	 * @brief The Intl.Collator.supportedLocalesOf() method returns an array containing those of the provided locales that are supported in collation without having to fall back to the runtime's default locale.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Collator/supportedLocalesOf
	 * @param locales - A string with a BCP 47 language tag, or an array of such strings. For the general form of the locales argument, see the Intl page.
	 * @param options - Optional. An object that may have the following property:     localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
	 */
	this.supportedLocalesOf = function(locales, options) {};

}

/**
 * @brief The Intl.Collator.prototype.resolvedOptions() method returns a new object with properties reflecting the locale and collation options computed during initialization of this Collator object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Collator/resolvedOptions
 */
Intl.Collator.prototype.resolvedOptions = function() {};

