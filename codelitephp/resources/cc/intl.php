<?php

// Start of intl v.1.1.0

class Collator  {
	const DEFAULT_VALUE = -1;
	const PRIMARY = 0;
	const SECONDARY = 1;
	const TERTIARY = 2;
	const DEFAULT_STRENGTH = 2;
	const QUATERNARY = 3;
	const IDENTICAL = 15;
	const OFF = 16;
	const ON = 17;
	const SHIFTED = 20;
	const NON_IGNORABLE = 21;
	const LOWER_FIRST = 24;
	const UPPER_FIRST = 25;
	const FRENCH_COLLATION = 0;
	const ALTERNATE_HANDLING = 1;
	const CASE_FIRST = 2;
	const CASE_LEVEL = 3;
	const NORMALIZATION_MODE = 4;
	const STRENGTH = 5;
	const HIRAGANA_QUATERNARY_MODE = 6;
	const NUMERIC_COLLATION = 7;
	const SORT_REGULAR = 0;
	const SORT_STRING = 1;
	const SORT_NUMERIC = 2;


	/**
	 * Create a collator
	 * @link http://www.php.net/manual/en/collator.construct.php
	 * @param arg1
	 */
	public function __construct ($arg1) {}

	/**
	 * Create a collator
	 * @link http://www.php.net/manual/en/collator.create.php
	 * @param locale string <p>
	 *       The locale containing the required collation rules. Special values for
	 *       locales can be passed in - if null is passed for the locale, the
	 *       default locale collation rules will be used. If empty string ("") or
	 *       "root" are passed, UCA rules will be used.
	 *      </p>
	 * @return Collator Return new instance of Collator object, or &null;
	 *   on error.
	 */
	public static function create ($locale) {}

	/**
	 * Compare two Unicode strings
	 * @link http://www.php.net/manual/en/collator.compare.php
	 * @param str1 string <p>
	 *       The first string to compare.
	 *      </p>
	 * @param str2 string <p>
	 *       The second string to compare.
	 *      </p>
	 * @return int Return comparison result:</p>
	 *  <p>
	 *   
	 *    
	 *     <p>
	 *      1 if str1 is greater than 
	 *      str2 ;
	 *     </p>
	 *    
	 *    
	 *     <p>
	 *      0 if str1 is equal to 
	 *      str2;
	 *     </p>
	 *    
	 *    
	 *     <p>
	 *      -1 if str1 is less than 
	 *      str2 .
	 *     </p>
	 *    
	 *   
	 *   On error
	 *   boolean
	 *   false
	 *   is returned.
	 */
	public function compare ($str1, $str2) {}

	/**
	 * Sort array using specified collator
	 * @link http://www.php.net/manual/en/collator.sort.php
	 * @param arr array <p>
	 *       Array of strings to sort.
	 *      </p>
	 * @param sort_flag int[optional] <p>
	 *       Optional sorting type, one of the following:
	 *      </p>
	 *      <p>
	 *       
	 *        
	 *         <p>
	 *          Collator::SORT_REGULAR
	 *          - compare items normally (don't change types)
	 *         </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function sort (array &$arr, $sort_flag = null) {}

	/**
	 * Sort array using specified collator and sort keys
	 * @link http://www.php.net/manual/en/collator.sortwithsortkeys.php
	 * @param arr array <p>Array of strings to sort</p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function sortWithSortKeys (array &$arr) {}

	/**
	 * Sort array maintaining index association
	 * @link http://www.php.net/manual/en/collator.asort.php
	 * @param arr array <p>Array of strings to sort.</p>
	 * @param sort_flag int[optional] <p>
	 *       Optional sorting type, one of the following:
	 *       
	 *        
	 *         <p>
	 *          Collator::SORT_REGULAR
	 *          - compare items normally (don't change types)
	 *         </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function asort (array &$arr, $sort_flag = null) {}

	/**
	 * Get collation attribute value
	 * @link http://www.php.net/manual/en/collator.getattribute.php
	 * @param attr int <p>
	 *       Attribute to get value for.
	 *      </p>
	 * @return int Attribute value, or boolean false on error.
	 */
	public function getAttribute ($attr) {}

	/**
	 * Set collation attribute
	 * @link http://www.php.net/manual/en/collator.setattribute.php
	 * @param attr int <p>Attribute.</p>
	 * @param val int <p>
	 *       Attribute value.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setAttribute ($attr, $val) {}

	/**
	 * Get current collation strength
	 * @link http://www.php.net/manual/en/collator.getstrength.php
	 * @return int current collation strength, or boolean false on error.
	 */
	public function getStrength () {}

	/**
	 * Set collation strength
	 * @link http://www.php.net/manual/en/collator.setstrength.php
	 * @param strength int <p>Strength to set.</p>
	 *      <p>
	 *       Possible values are:
	 *       
	 *        
	 *         <p>
	 *          Collator::PRIMARY
	 *         </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setStrength ($strength) {}

	/**
	 * Get the locale name of the collator
	 * @link http://www.php.net/manual/en/collator.getlocale.php
	 * @param type int[optional] <p>
	 *       You can choose between valid and actual locale (
	 *       Locale::VALID_LOCALE and 
	 *       Locale::ACTUAL_LOCALE,
	 *       respectively). The default is the actual locale.
	 *      </p>
	 * @return string Real locale name from which the collation data comes. If the collator was
	 *   instantiated from rules or an error occurred, returns
	 *   boolean false.
	 */
	public function getLocale ($type = null) {}

	/**
	 * Get collator's last error code
	 * @link http://www.php.net/manual/en/collator.geterrorcode.php
	 * @return int Error code returned by the last Collator API function call.
	 */
	public function getErrorCode () {}

	/**
	 * Get text for collator's last error code
	 * @link http://www.php.net/manual/en/collator.geterrormessage.php
	 * @return string Description of an error occurred in the last Collator API function call.
	 */
	public function getErrorMessage () {}

	/**
	 * Get sorting key for a string
	 * @link http://www.php.net/manual/en/collator.getsortkey.php
	 * @param str string <p>
	 *       The string to produce the key from.
	 *      </p>
	 * @return string the collation key for the string. Collation keys can be compared directly instead of strings.
	 */
	public function getSortKey ($str) {}

}

class NumberFormatter  {
	const PATTERN_DECIMAL = 0;
	const DECIMAL = 1;
	const CURRENCY = 2;
	const PERCENT = 3;
	const SCIENTIFIC = 4;
	const SPELLOUT = 5;
	const ORDINAL = 6;
	const DURATION = 7;
	const PATTERN_RULEBASED = 8;
	const IGNORE = 0;
	const DEFAULT_STYLE = 1;
	const ROUND_CEILING = 0;
	const ROUND_FLOOR = 1;
	const ROUND_DOWN = 2;
	const ROUND_UP = 3;
	const ROUND_HALFEVEN = 4;
	const ROUND_HALFDOWN = 5;
	const ROUND_HALFUP = 6;
	const PAD_BEFORE_PREFIX = 0;
	const PAD_AFTER_PREFIX = 1;
	const PAD_BEFORE_SUFFIX = 2;
	const PAD_AFTER_SUFFIX = 3;
	const PARSE_INT_ONLY = 0;
	const GROUPING_USED = 1;
	const DECIMAL_ALWAYS_SHOWN = 2;
	const MAX_INTEGER_DIGITS = 3;
	const MIN_INTEGER_DIGITS = 4;
	const INTEGER_DIGITS = 5;
	const MAX_FRACTION_DIGITS = 6;
	const MIN_FRACTION_DIGITS = 7;
	const FRACTION_DIGITS = 8;
	const MULTIPLIER = 9;
	const GROUPING_SIZE = 10;
	const ROUNDING_MODE = 11;
	const ROUNDING_INCREMENT = 12;
	const FORMAT_WIDTH = 13;
	const PADDING_POSITION = 14;
	const SECONDARY_GROUPING_SIZE = 15;
	const SIGNIFICANT_DIGITS_USED = 16;
	const MIN_SIGNIFICANT_DIGITS = 17;
	const MAX_SIGNIFICANT_DIGITS = 18;
	const LENIENT_PARSE = 19;
	const POSITIVE_PREFIX = 0;
	const POSITIVE_SUFFIX = 1;
	const NEGATIVE_PREFIX = 2;
	const NEGATIVE_SUFFIX = 3;
	const PADDING_CHARACTER = 4;
	const CURRENCY_CODE = 5;
	const DEFAULT_RULESET = 6;
	const PUBLIC_RULESETS = 7;
	const DECIMAL_SEPARATOR_SYMBOL = 0;
	const GROUPING_SEPARATOR_SYMBOL = 1;
	const PATTERN_SEPARATOR_SYMBOL = 2;
	const PERCENT_SYMBOL = 3;
	const ZERO_DIGIT_SYMBOL = 4;
	const DIGIT_SYMBOL = 5;
	const MINUS_SIGN_SYMBOL = 6;
	const PLUS_SIGN_SYMBOL = 7;
	const CURRENCY_SYMBOL = 8;
	const INTL_CURRENCY_SYMBOL = 9;
	const MONETARY_SEPARATOR_SYMBOL = 10;
	const EXPONENTIAL_SYMBOL = 11;
	const PERMILL_SYMBOL = 12;
	const PAD_ESCAPE_SYMBOL = 13;
	const INFINITY_SYMBOL = 14;
	const NAN_SYMBOL = 15;
	const SIGNIFICANT_DIGIT_SYMBOL = 16;
	const MONETARY_GROUPING_SEPARATOR_SYMBOL = 17;
	const TYPE_DEFAULT = 0;
	const TYPE_INT32 = 1;
	const TYPE_INT64 = 2;
	const TYPE_DOUBLE = 3;
	const TYPE_CURRENCY = 4;


	/**
	 * @param locale
	 * @param style
	 * @param pattern[optional]
	 */
	public function __construct ($locale, $style, $pattern) {}

	/**
	 * Create a number formatter
	 * @link http://www.php.net/manual/en/numberformatter.create.php
	 * @param locale string <p>
	 *       Locale in which the number would be formatted (locale name, e.g. en_CA).
	 *      </p>
	 * @param style int <p>
	 *       Style of the formatting, one of the
	 *       
	 *        format style constants. If
	 *       NumberFormatter::PATTERN_DECIMAL
	 *       or NumberFormatter::PATTERN_RULEBASED
	 *       is passed then the number format is opened using the given pattern,
	 *       which must conform to the syntax described in
	 *       ICU DecimalFormat
	 *       documentation or
	 *       ICU RuleBasedNumberFormat
	 *       documentation, respectively.
	 *      </p>
	 * @param pattern string[optional] <p>
	 *       Pattern string if the chosen style requires a pattern.
	 *      </p>
	 * @return NumberFormatter NumberFormatter object or false on error.
	 */
	public static function create ($locale, $style, $pattern = null) {}

	/**
	 * Format a number
	 * @link http://www.php.net/manual/en/numberformatter.format.php
	 * @param value number <p>
	 *       The value to format. Can be integer or double,
	 *       other values will be converted to a numeric value.
	 *      </p>
	 * @param type int[optional] <p>
	 *       The
	 *       
	 *        formatting type to use.
	 *      </p>
	 * @return string the string containing formatted value, or false on error.
	 */
	public function format ($value, $type = null) {}

	/**
	 * Parse a number
	 * @link http://www.php.net/manual/en/numberformatter.parse.php
	 * @param value string 
	 * @param type int[optional] <p>
	 *       The
	 *       
	 *        formatting type to use. By default,
	 *       NumberFormatter::TYPE_DOUBLE is used.
	 *      </p>
	 * @param position int[optional] <p>
	 *       Offset in the string at which to begin parsing. On return, this value
	 *       will hold the offset at which parsing ended.
	 *      </p>
	 * @return mixed The value of the parsed number or false on error.
	 */
	public function parse ($value, $type = null, &$position = null) {}

	/**
	 * Format a currency value
	 * @link http://www.php.net/manual/en/numberformatter.formatcurrency.php
	 * @param value float <p>
	 *       The numeric currency value.
	 *      </p>
	 * @param currency string <p>
	 *       The 3-letter ISO 4217 currency code indicating the currency to use.
	 *      </p>
	 * @return string String representing the formatted currency value.
	 */
	public function formatCurrency ($value, $currency) {}

	/**
	 * Parse a currency number
	 * @link http://www.php.net/manual/en/numberformatter.parsecurrency.php
	 * @param value string 
	 * @param currency string <p>
	 *       Parameter to receive the currency name (3-letter ISO 4217 currency
	 *       code).
	 *      </p>
	 * @param position int[optional] <p>
	 *       Offset in the string at which to begin parsing. On return, this value
	 *       will hold the offset at which parsing ended.
	 *      </p>
	 * @return float The parsed numeric value or false on error.
	 */
	public function parseCurrency ($value, &$currency, &$position = null) {}

	/**
	 * Set an attribute
	 * @link http://www.php.net/manual/en/numberformatter.setattribute.php
	 * @param attr int <p>
	 *       Attribute specifier - one of the
	 *       
	 *        numeric attribute constants.
	 *      </p>
	 * @param value int <p>
	 *       The attribute value.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setAttribute ($attr, $value) {}

	/**
	 * Get an attribute
	 * @link http://www.php.net/manual/en/numberformatter.getattribute.php
	 * @param attr int <p>
	 *       Attribute specifier - one of the
	 *       
	 *        numeric attribute constants.
	 *      </p>
	 * @return int Return attribute value on success, or false on error.
	 */
	public function getAttribute ($attr) {}

	/**
	 * Set a text attribute
	 * @link http://www.php.net/manual/en/numberformatter.settextattribute.php
	 * @param attr int <p>
	 *       Attribute specifier - one of the
	 *       text attribute
	 *       constants.
	 *      </p>
	 * @param value string <p>
	 *       Text for the attribute value.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setTextAttribute ($attr, $value) {}

	/**
	 * Get a text attribute
	 * @link http://www.php.net/manual/en/numberformatter.gettextattribute.php
	 * @param attr int <p>
	 *       Attribute specifier - one of the
	 *       
	 *        text attribute constants.
	 *      </p>
	 * @return string Return attribute value on success, or false on error.
	 */
	public function getTextAttribute ($attr) {}

	/**
	 * Set a symbol value
	 * @link http://www.php.net/manual/en/numberformatter.setsymbol.php
	 * @param attr int <p>
	 *       Symbol specifier, one of the
	 *       
	 *        format symbol constants.
	 *      </p>
	 * @param value string <p>
	 *       Text for the symbol.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setSymbol ($attr, $value) {}

	/**
	 * Get a symbol value
	 * @link http://www.php.net/manual/en/numberformatter.getsymbol.php
	 * @param attr int <p>
	 *       Symbol specifier, one of the
	 *       
	 *        format symbol constants.
	 *      </p>
	 * @return string The symbol string or false on error.
	 */
	public function getSymbol ($attr) {}

	/**
	 * Set formatter pattern
	 * @link http://www.php.net/manual/en/numberformatter.setpattern.php
	 * @param pattern string <p>
	 *       Pattern in syntax described in
	 *       ICU DecimalFormat
	 *       documentation.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setPattern ($pattern) {}

	/**
	 * Get formatter pattern
	 * @link http://www.php.net/manual/en/numberformatter.getpattern.php
	 * @return string Pattern string that is used by the formatter, or false if an error happens.
	 */
	public function getPattern () {}

	/**
	 * Get formatter locale
	 * @link http://www.php.net/manual/en/numberformatter.getlocale.php
	 * @param type int[optional] <p>
	 *       You can choose between valid and actual locale (
	 *       Locale::VALID_LOCALE,
	 *       Locale::ACTUAL_LOCALE,
	 *       respectively). The default is the actual locale.
	 *      </p>
	 * @return string The locale name used to create the formatter.
	 */
	public function getLocale ($type = null) {}

	/**
	 * Get formatter's last error code.
	 * @link http://www.php.net/manual/en/numberformatter.geterrorcode.php
	 * @return int error code from last formatter call.
	 */
	public function getErrorCode () {}

	/**
	 * Get formatter's last error message.
	 * @link http://www.php.net/manual/en/numberformatter.geterrormessage.php
	 * @return string error message from last formatter call.
	 */
	public function getErrorMessage () {}

}

class Normalizer  {
	const NONE = 1;
	const FORM_D = 2;
	const NFD = 2;
	const FORM_KD = 3;
	const NFKD = 3;
	const FORM_C = 4;
	const NFC = 4;
	const FORM_KC = 5;
	const NFKC = 5;


	/**
	 * Normalizes the input provided and returns the normalized string
	 * @link http://www.php.net/manual/en/normalizer.normalize.php
	 * @param input string <p>The input string to normalize</p>
	 * @param form string[optional] <p>One of the normalization forms.</p>
	 * @return string The normalized string or &null; if an error occurred.
	 */
	public static function normalize ($input, $form = null) {}

	/**
	 * Checks if the provided string is already in the specified normalization
   form.
	 * @link http://www.php.net/manual/en/normalizer.isnormalized.php
	 * @param input string <p>The input string to normalize</p>
	 * @param form string[optional] <p>
	 *       One of the normalization forms.
	 *      </p>
	 * @return bool true if normalized, false otherwise or if there an error
	 */
	public static function isNormalized ($input, $form = null) {}

}

class Locale  {
	const ACTUAL_LOCALE = 0;
	const VALID_LOCALE = 1;
	const DEFAULT_LOCALE = null;
	const LANG_TAG = "language";
	const EXTLANG_TAG = "extlang";
	const SCRIPT_TAG = "script";
	const REGION_TAG = "region";
	const VARIANT_TAG = "variant";
	const GRANDFATHERED_LANG_TAG = "grandfathered";
	const PRIVATE_TAG = "private";


	/**
	 * Gets the default locale value from the INTL global 'default_locale'
	 * @link http://www.php.net/manual/en/locale.getdefault.php
	 * @return string The current runtime locale
	 */
	public static function getDefault () {}

	/**
	 * sets the default runtime locale
	 * @link http://www.php.net/manual/en/locale.setdefault.php
	 * @param locale string <p>
	 *       Is a BCP 47 compliant language tag containing the
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public static function setDefault ($locale) {}

	/**
	 * Gets the primary language for the input locale
	 * @link http://www.php.net/manual/en/locale.getprimarylanguage.php
	 * @param locale string <p>
	 *       The locale to extract the primary language code from
	 *      </p>
	 * @return string The language code associated with the language or &null; in case of error.
	 */
	public static function getPrimaryLanguage ($locale) {}

	/**
	 * Gets the script for the input locale
	 * @link http://www.php.net/manual/en/locale.getscript.php
	 * @param locale string <p>
	 *       The locale to extract the script code from
	 *      </p>
	 * @return string The script subtag for the locale or &null; if not present
	 */
	public static function getScript ($locale) {}

	/**
	 * Gets the region for the input locale
	 * @link http://www.php.net/manual/en/locale.getregion.php
	 * @param locale string <p>
	 *       The locale to extract the region code from
	 *      </p>
	 * @return string The region subtag for the locale or &null; if not present
	 */
	public static function getRegion ($locale) {}

	/**
	 * Gets the keywords for the input locale
	 * @link http://www.php.net/manual/en/locale.getkeywords.php
	 * @param locale string <p>
	 *       The locale to extract the keywords from
	 *      </p>
	 * @return array Associative array containing the keyword-value pairs for this locale
	 */
	public static function getKeywords ($locale) {}

	/**
	 * Returns an appropriately localized display name for script of the input locale
	 * @link http://www.php.net/manual/en/locale.getdisplayscript.php
	 * @param locale string <p>
	 *       The locale to return a display script for
	 *      </p>
	 * @param in_locale string[optional] <p>
	 *       Optional format locale to use to display the script name
	 *      </p>
	 * @return string Display name of the script for the $locale in the format appropriate for
	 *   $in_locale.
	 */
	public static function getDisplayScript ($locale, $in_locale = null) {}

	/**
	 * Returns an appropriately localized display name for region of the input locale
	 * @link http://www.php.net/manual/en/locale.getdisplayregion.php
	 * @param locale string <p>
	 *       The locale to return a display region for.
	 *      </p>
	 * @param in_locale string[optional] <p>
	 *       Optional format locale to use to display the region name
	 *      </p>
	 * @return string display name of the region for the $locale in the format appropriate for
	 *   $in_locale.
	 */
	public static function getDisplayRegion ($locale, $in_locale = null) {}

	/**
	 * Returns an appropriately localized display name for the input locale
	 * @link http://www.php.net/manual/en/locale.getdisplayname.php
	 * @param locale string <p>
	 *       The locale to return a display name for.
	 *      </p>
	 * @param in_locale string[optional] <p>optional format locale</p>
	 * @return string Display name of the locale in the format appropriate for $in_locale.
	 */
	public static function getDisplayName ($locale, $in_locale = null) {}

	/**
	 * Returns an appropriately localized display name for language of the inputlocale
	 * @link http://www.php.net/manual/en/locale.getdisplaylanguage.php
	 * @param locale string <p>
	 *       The locale to return a display language for
	 *      </p>
	 * @param in_locale string[optional] <p>
	 *       Optional format locale to use to display the language name
	 *      </p>
	 * @return string display name of the language for the $locale in the format appropriate for
	 *   $in_locale.
	 */
	public static function getDisplayLanguage ($locale, $in_locale = null) {}

	/**
	 * Returns an appropriately localized display name for variants of the input locale
	 * @link http://www.php.net/manual/en/locale.getdisplayvariant.php
	 * @param locale string <p>
	 *       The locale to return a display variant for
	 *      </p>
	 * @param in_locale string[optional] <p>
	 *       Optional format locale to use to display the variant name
	 *      </p>
	 * @return string Display name of the variant for the $locale in the format appropriate for
	 *   $in_locale.
	 */
	public static function getDisplayVariant ($locale, $in_locale = null) {}

	/**
	 * Returns a correctly ordered and delimited locale ID
	 * @link http://www.php.net/manual/en/locale.composelocale.php
	 * @param subtags array <p>
	 *       an array containing a list of key-value pairs, where the keys identify
	 *       the particular locale ID subtags, and the values are the associated
	 *       subtag values. 
	 *       
	 *        <p>
	 *         The 'variant' and 'private' subtags can take maximum 15 values
	 *         whereas 'extlang' can take maximum 3 values.e.g. Variants are allowed
	 *         with the suffix ranging from 0-14. Hence the keys for the input array
	 *         can be variant0, variant1, ...,variant14. In the returned locale id,
	 *         the subtag is ordered by suffix resulting in variant0 followed by
	 *         variant1 followed by variant2 and so on.
	 *        </p>
	 *        <p>
	 *         The 'variant', 'private' and 'extlang' multiple values can be specified both
	 *         as array under specific key (e.g. 'variant') and as multiple numbered keys
	 *         (e.g. 'variant0', 'variant1', etc.).
	 *        </p>
	 *       
	 *      </p>
	 * @return string The corresponding locale identifier.
	 */
	public static function composeLocale (array $subtags) {}

	/**
	 * Returns a key-value array of locale ID subtag elements.
	 * @link http://www.php.net/manual/en/locale.parselocale.php
	 * @param locale string <p>
	 *       The locale to extract the subtag array from. Note: The 'variant' and
	 *       'private' subtags can take maximum 15 values whereas 'extlang' can take
	 *       maximum 3 values.
	 *      </p>
	 * @return array an array containing a list of key-value pairs, where the keys
	 *   identify the particular locale ID subtags, and the values are the
	 *   associated subtag values. The array will be ordered as the locale id
	 *   subtags e.g. in the locale id if variants are '-varX-varY-varZ' then the
	 *   returned array will have variant0=>varX , variant1=>varY ,
	 *   variant2=>varZ
	 */
	public static function parseLocale ($locale) {}

	/**
	 * Gets the variants for the input locale
	 * @link http://www.php.net/manual/en/locale.getallvariants.php
	 * @param locale string <p>
	 *       The locale to extract the variants from
	 *      </p>
	 * @return array The array containing the list of all variants subtag for the locale 
	 *   or &null; if not present
	 */
	public static function getAllVariants ($locale) {}

	/**
	 * Checks if a language tag filter matches with locale
	 * @link http://www.php.net/manual/en/locale.filtermatches.php
	 * @param langtag string <p>
	 *       The language tag to check
	 *      </p>
	 * @param locale string <p>
	 *       The language range to check against
	 *      </p>
	 * @param canonicalize bool[optional] <p>
	 *      If true, the arguments will be converted to canonical form before
	 *      matching. 
	 *     </p>
	 * @return bool true if $locale matches $langtag false otherwise.
	 */
	public static function filterMatches ($langtag, $locale, $canonicalize = null) {}

	/**
	 * Searches the language tag list for the best match to the language
	 * @link http://www.php.net/manual/en/locale.lookup.php
	 * @param langtag array <p>
	 *       An array containing a list of language tags to compare to 
	 *       locale. Maximum 100 items allowed.
	 *      </p>
	 * @param locale string <p>
	 *       The locale to use as the language range when matching.
	 *      </p>
	 * @param canonicalize bool[optional] <p>
	 *           If true, the arguments will be converted to canonical form before
	 *           matching.
	 *         </p>
	 * @param default string[optional] <p>
	 *       The locale to use if no match is found.
	 *      </p>
	 * @return string The closest matching language tag or default value.
	 */
	public static function lookup (array $langtag, $locale, $canonicalize = null, $default = null) {}

	/**
	 * @param arg1
	 */
	public static function canonicalize ($arg1) {}

	/**
	 * Tries to find out best available locale based on HTTP "Accept-Language" header
	 * @link http://www.php.net/manual/en/locale.acceptfromhttp.php
	 * @param header string <p>
	 *       The string containing the "Accept-Language" header according to format in RFC 2616.
	 *      </p>
	 * @return string The corresponding locale identifier.
	 */
	public static function acceptFromHttp ($header) {}

}

class MessageFormatter  {

	/**
	 * @param locale
	 * @param pattern
	 */
	public function __construct ($locale, $pattern) {}

	/**
	 * Constructs a new Message Formatter
	 * @link http://www.php.net/manual/en/messageformatter.create.php
	 * @param locale string <p>
	 *       The locale to use when formatting arguments
	 *      </p>
	 * @param pattern string <p>
	 *       The pattern string to stick arguments into. 
	 *       The pattern uses an 'apostrophe-friendly' syntax; it is run through
	 *       umsg_autoQuoteApostrophe 
	 *       before being interpreted.
	 *      </p>
	 * @return MessageFormatter The formatter object
	 */
	public static function create ($locale, $pattern) {}

	/**
	 * Format the message
	 * @link http://www.php.net/manual/en/messageformatter.format.php
	 * @param args array <p>
	 *       Arguments to insert into the format string
	 *      </p>
	 * @return string The formatted string, or false if an error occurred
	 */
	public function format (array $args) {}

	/**
	 * Quick format message
	 * @link http://www.php.net/manual/en/messageformatter.formatmessage.php
	 * @param locale string <p>
	 *       The locale to use for formatting locale-dependent parts
	 *      </p>
	 * @param pattern string <p>
	 *       The pattern string to insert things into.
	 *       The pattern uses an 'apostrophe-friendly' syntax; it is run through
	 *       umsg_autoQuoteApostrophe 
	 *       before being interpreted.
	 *      </p>
	 * @param args array <p>
	 *       The array of values to insert into the format string
	 *      </p>
	 * @return string The formatted pattern string or false if an error occurred
	 */
	public static function formatMessage ($locale, $pattern, array $args) {}

	/**
	 * Parse input string according to pattern
	 * @link http://www.php.net/manual/en/messageformatter.parse.php
	 * @param value string <p>
	 *       The string to parse
	 *      </p>
	 * @return array An array containing the items extracted, or false on error
	 */
	public function parse ($value) {}

	/**
	 * Quick parse input string
	 * @link http://www.php.net/manual/en/messageformatter.parsemessage.php
	 * @param locale string <p>
	 *       The locale to use for parsing locale-dependent parts
	 *      </p>
	 * @param pattern string <p>
	 *       The pattern with which to parse the value.
	 *      </p>
	 * @param source string <p>
	 *       The string to parse, conforming to the pattern.
	 *      </p>
	 * @return array An array containing items extracted, or false on error
	 */
	public static function parseMessage ($locale, $pattern, $source) {}

	/**
	 * Set the pattern used by the formatter
	 * @link http://www.php.net/manual/en/messageformatter.setpattern.php
	 * @param pattern string <p>
	 *       The pattern string to use in this message formatter.
	 *       The pattern uses an 'apostrophe-friendly' syntax; it is run through
	 *       umsg_autoQuoteApostrophe 
	 *       before being interpreted.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setPattern ($pattern) {}

	/**
	 * Get the pattern used by the formatter
	 * @link http://www.php.net/manual/en/messageformatter.getpattern.php
	 * @return string The pattern string for this message formatter
	 */
	public function getPattern () {}

	/**
	 * Get the locale for which the formatter was created.
	 * @link http://www.php.net/manual/en/messageformatter.getlocale.php
	 * @return string The locale name
	 */
	public function getLocale () {}

	/**
	 * Get the error code from last operation
	 * @link http://www.php.net/manual/en/messageformatter.geterrorcode.php
	 * @return int The error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	public function getErrorCode () {}

	/**
	 * Get the error text from the last operation
	 * @link http://www.php.net/manual/en/messageformatter.geterrormessage.php
	 * @return string Description of the last error.
	 */
	public function getErrorMessage () {}

}

class IntlDateFormatter  {
	const FULL = 0;
	const LONG = 1;
	const MEDIUM = 2;
	const SHORT = 3;
	const NONE = -1;
	const GREGORIAN = 1;
	const TRADITIONAL = 0;


	/**
	 * @param locale
	 * @param datetype
	 * @param timetype
	 * @param timezone[optional]
	 * @param calendar[optional]
	 * @param pattern[optional]
	 */
	public function __construct ($locale, $datetype, $timetype, $timezone, $calendar, $pattern) {}

	/**
	 * Create a date formatter
	 * @link http://www.php.net/manual/en/intldateformatter.create.php
	 * @param locale string <p>
	 *       Locale to use when formatting or parsing.
	 *      </p>
	 * @param datetype int <p>
	 *       Date type to use (none, 
	 *       short, medium, 
	 *       long, full). 
	 *       This is one of the 
	 *       IntlDateFormatter constants.
	 *      </p>
	 * @param timetype int <p>
	 *       Time type to use (none, 
	 *       short, medium, 
	 *       long, full).
	 *       This is one of the 
	 *       IntlDateFormatter constants.
	 *      </p>
	 * @param timezone string[optional] <p>
	 *       Time zone ID, default is system default.      
	 *      </p>
	 * @param calendar int[optional] <p>
	 *       Calendar to use for formatting or parsing; default is Gregorian.
	 *       This is one of the 
	 *       IntlDateFormatter calendar constants.
	 *      </p>
	 * @param pattern string[optional] <p>
	 *       Optional pattern to use when formatting or parsing.
	 *       Possible patterns are documented at &url.icu.datepattern;.
	 *      </p>
	 * @return IntlDateFormatter 
	 */
	public static function create ($locale, $datetype, $timetype, $timezone = null, $calendar = null, $pattern = null) {}

	/**
	 * Get the datetype used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.getdatetype.php
	 * @return int The current date type value of the formatter.
	 */
	public function getDateType () {}

	/**
	 * Get the timetype used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.gettimetype.php
	 * @return int The current date type value of the formatter.
	 */
	public function getTimeType () {}

	/**
	 * Get the calendar used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.getcalendar.php
	 * @return int The calendar being used by the formatter.
	 */
	public function getCalendar () {}

	/**
	 * sets the calendar used to the appropriate calendar, which must be
	 * @link http://www.php.net/manual/en/intldateformatter.setcalendar.php
	 * @param which int <p>
	 *       The calendar to use.
	 *       Default is IntlDateFormatter::GREGORIAN.
	 *     </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setCalendar ($which) {}

	/**
	 * Get the timezone-id used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.gettimezoneid.php
	 * @return string ID string for the time zone used by this formatter.
	 */
	public function getTimeZoneId () {}

	/**
	 * Sets the time zone to use
	 * @link http://www.php.net/manual/en/intldateformatter.settimezoneid.php
	 * @param zone string <p>
	 *       The time zone ID string of the time zone to use.
	 *       If &null; or the empty string, the default time zone for the runtime is used.
	 *     </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setTimeZoneId ($zone) {}

	/**
	 * Set the pattern used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.setpattern.php
	 * @param pattern string <p>
	 *       New pattern string to use.
	 *       Possible patterns are documented at &url.icu.datepattern;.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 *   Bad formatstrings are usually the cause of the failure.
	 */
	public function setPattern ($pattern) {}

	/**
	 * Get the pattern used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.getpattern.php
	 * @return string The pattern string being used to format/parse.
	 */
	public function getPattern () {}

	/**
	 * Get the locale used by formatter
	 * @link http://www.php.net/manual/en/intldateformatter.getlocale.php
	 * @param which int[optional] 
	 * @return string the locale of this formatter  or 'false' if error
	 */
	public function getLocale ($which = null) {}

	/**
	 * Set the leniency of the parser
	 * @link http://www.php.net/manual/en/intldateformatter.setlenient.php
	 * @param lenient bool <p>
	 *       Sets whether the parser is lenient or not, default is false (strict).
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setLenient ($lenient) {}

	/**
	 * Get the lenient used for the IntlDateFormatter
	 * @link http://www.php.net/manual/en/intldateformatter.islenient.php
	 * @return bool true if parser is lenient, false if parser is strict. By default the parser is strict.
	 */
	public function isLenient () {}

	/**
	 * Format the date/time value as a string
	 * @link http://www.php.net/manual/en/intldateformatter.format.php
	 * @param value mixed <p>
	 *       Value to format. This may be a DateTime object,
	 *       an integer representing a Unix timestamp value (seconds
	 *       since epoch, UTC) or an array in the format output by
	 *       localtime.
	 *      </p>
	 * @return string The formatted string or, if an error occurred, false.
	 */
	public function format ($value) {}

	/**
	 * Parse string to a timestamp value
	 * @link http://www.php.net/manual/en/intldateformatter.parse.php
	 * @param value string <p>
	 *       string to convert to a time 
	 *      </p>
	 * @param position int[optional] <p>
	 *       Position at which to start the parsing in $value (zero-based).
	 *       If no error occurs before $value is consumed, $parse_pos will contain -1
	 *       otherwise it will contain the position at which parsing ended (and the error occurred).
	 *       This variable will contain the end position if the parse fails.
	 *       If $parse_pos > strlen($value), the parse fails immediately.  
	 *     </p>
	 * @return int timestamp  parsed value
	 */
	public function parse ($value, &$position = null) {}

	/**
	 * Parse string to a field-based time value
	 * @link http://www.php.net/manual/en/intldateformatter.localtime.php
	 * @param value string <p>
	 *       string to convert to a time      
	 *      </p>
	 * @param position int[optional] <p>
	 *       Position at which to start the parsing in $value (zero-based).
	 *       If no error occurs before $value is consumed, $parse_pos will contain -1
	 *       otherwise it will contain the position at which parsing ended .
	 *       If $parse_pos > strlen($value), the parse fails immediately.  
	 *      </p>
	 * @return array Localtime compatible array of integers  : contains 24 hour clock value in tm_hour field
	 */
	public function localtime ($value, &$position = null) {}

	/**
	 * Get the error code from last operation
	 * @link http://www.php.net/manual/en/intldateformatter.geterrorcode.php
	 * @return int The error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	public function getErrorCode () {}

	/**
	 * Get the error text from the last operation.
	 * @link http://www.php.net/manual/en/intldateformatter.geterrormessage.php
	 * @return string Description of the last error.
	 */
	public function getErrorMessage () {}

}

class ResourceBundle  {

	/**
	 * @param locale
	 * @param bundlename
	 * @param fallback[optional]
	 */
	public function __construct ($locale, $bundlename, $fallback) {}

	/**
	 * Create a resource bundle
	 * @link http://www.php.net/manual/en/resourcebundle.create.php
	 * @param locale string <p>
	 *       Locale for which the resources should be loaded (locale name, e.g. en_CA).
	 *      </p>
	 * @param bundlename string <p>
	 *       The directory where the data is stored or the name of the .dat file.
	 *      </p>
	 * @param fallback bool[optional] <p>
	 *       Whether locale should match exactly or fallback to parent locale is allowed.
	 *      </p>
	 * @return ResourceBundle ResourceBundle object or false on error.
	 */
	public static function create ($locale, $bundlename, $fallback = null) {}

	/**
	 * Get data from the bundle
	 * @link http://www.php.net/manual/en/resourcebundle.get.php
	 * @param index string|int <p>
	 *       Data index, must be string or integer.
	 *      </p>
	 * @return mixed the data located at the index or &null; on error. Strings, integers and binary data strings
	 *   are returned as corresponding PHP types, integer array is returned as PHP array. Complex types are
	 *   returned as ResourceBundle object.
	 */
	public function get ($index) {}

	/**
	 * Get number of elements in the bundle
	 * @link http://www.php.net/manual/en/resourcebundle.count.php
	 * @return int number of elements in the bundle.
	 */
	public function count () {}

	/**
	 * Get supported locales
	 * @link http://www.php.net/manual/en/resourcebundle.locales.php
	 * @param bundlename
	 * @return array the list of locales supported by the bundle.
	 */
	public static function getLocales ($bundlename) {}

	/**
	 * Get bundle's last error code.
	 * @link http://www.php.net/manual/en/resourcebundle.geterrorcode.php
	 * @return int error code from last bundle object call.
	 */
	public function getErrorCode () {}

	/**
	 * Get bundle's last error message.
	 * @link http://www.php.net/manual/en/resourcebundle.geterrormessage.php
	 * @return string error message from last bundle object's call.
	 */
	public function getErrorMessage () {}

}

/**
 * @param arg1
 */
function collator_create ($arg1) {}

/**
 * @param object Collator
 * @param arg1
 * @param arg2
 */
function collator_compare (Collator $object, $arg1, $arg2) {}

/**
 * @param object Collator
 * @param arg1
 */
function collator_get_attribute (Collator $object, $arg1) {}

/**
 * @param object Collator
 * @param arg1
 * @param arg2
 */
function collator_set_attribute (Collator $object, $arg1, $arg2) {}

/**
 * @param object Collator
 */
function collator_get_strength (Collator $object) {}

/**
 * @param object Collator
 * @param arg1
 */
function collator_set_strength (Collator $object, $arg1) {}

/**
 * @param object Collator
 * @param arr
 * @param sort_flags[optional]
 */
function collator_sort (Collator $objectarray , &$arr, $sort_flags) {}

/**
 * @param object Collator
 * @param arr
 * @param sort_flags[optional]
 */
function collator_sort_with_sort_keys (Collator $objectarray , &$arr, $sort_flags) {}

/**
 * @param object Collator
 * @param arr
 * @param sort_flags[optional]
 */
function collator_asort (Collator $objectarray , &$arr, $sort_flags) {}

/**
 * @param object Collator
 * @param arg1
 */
function collator_get_locale (Collator $object, $arg1) {}

/**
 * @param object Collator
 */
function collator_get_error_code (Collator $object) {}

/**
 * @param object Collator
 */
function collator_get_error_message (Collator $object) {}

/**
 * @param object Collator
 * @param arg1
 * @param arg2
 */
function collator_get_sort_key (Collator $object, $arg1, $arg2) {}

/**
 * @param locale
 * @param style
 * @param pattern[optional]
 */
function numfmt_create ($locale, $style, $pattern) {}

/**
 * @param nf
 * @param num
 * @param type[optional]
 */
function numfmt_format ($nf, $num, $type) {}

/**
 * @param formatter
 * @param string
 * @param type[optional]
 * @param position[optional]
 */
function numfmt_parse ($formatter, $string, $type, &$position) {}

/**
 * @param nf
 * @param num
 * @param currency
 */
function numfmt_format_currency ($nf, $num, $currency) {}

/**
 * @param formatter
 * @param string
 * @param currency
 * @param position[optional]
 */
function numfmt_parse_currency ($formatter, $string, &$currency, &$position) {}

/**
 * @param nf
 * @param attr
 * @param value
 */
function numfmt_set_attribute ($nf, $attr, $value) {}

/**
 * @param nf
 * @param attr
 */
function numfmt_get_attribute ($nf, $attr) {}

/**
 * @param nf
 * @param attr
 * @param value
 */
function numfmt_set_text_attribute ($nf, $attr, $value) {}

/**
 * @param nf
 * @param attr
 */
function numfmt_get_text_attribute ($nf, $attr) {}

/**
 * @param nf
 * @param attr
 * @param symbol
 */
function numfmt_set_symbol ($nf, $attr, $symbol) {}

/**
 * @param nf
 * @param attr
 */
function numfmt_get_symbol ($nf, $attr) {}

/**
 * @param nf
 * @param pattern
 */
function numfmt_set_pattern ($nf, $pattern) {}

/**
 * @param nf
 */
function numfmt_get_pattern ($nf) {}

/**
 * @param nf
 * @param type[optional]
 */
function numfmt_get_locale ($nf, $type) {}

/**
 * @param nf
 */
function numfmt_get_error_code ($nf) {}

/**
 * @param nf
 */
function numfmt_get_error_message ($nf) {}

/**
 * @param input
 * @param form[optional]
 */
function normalizer_normalize ($input, $form) {}

/**
 * @param input
 * @param form[optional]
 */
function normalizer_is_normalized ($input, $form) {}

/**
 * Get the default Locale
 * @link http://www.php.net/manual/en/function.locale-get-default.php
 * @return string a string with the current Locale.
 */
function locale_get_default () {}

/**
 * Set the default Locale
 * @link http://www.php.net/manual/en/function.locale-set-default.php
 * @param name string <p>
 *       The new Locale name. A comprehensive list of the supported locales is
 *       available at .
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function locale_set_default ($name) {}

/**
 * @param arg1
 */
function locale_get_primary_language ($arg1) {}

/**
 * @param arg1
 */
function locale_get_script ($arg1) {}

/**
 * @param arg1
 */
function locale_get_region ($arg1) {}

/**
 * @param arg1
 */
function locale_get_keywords ($arg1) {}

/**
 * @param arg1
 * @param arg2
 */
function locale_get_display_script ($arg1, $arg2) {}

/**
 * @param arg1
 * @param arg2
 */
function locale_get_display_region ($arg1, $arg2) {}

/**
 * @param arg1
 * @param arg2
 */
function locale_get_display_name ($arg1, $arg2) {}

/**
 * @param arg1
 * @param arg2
 */
function locale_get_display_language ($arg1, $arg2) {}

/**
 * @param arg1
 * @param arg2
 */
function locale_get_display_variant ($arg1, $arg2) {}

/**
 * @param arg1
 */
function locale_compose ($arg1) {}

/**
 * @param arg1
 */
function locale_parse ($arg1) {}

/**
 * @param arg1
 */
function locale_get_all_variants ($arg1) {}

/**
 * @param arg1
 * @param arg2
 * @param arg3
 */
function locale_filter_matches ($arg1, $arg2, $arg3) {}

/**
 * @param arg1
 */
function locale_canonicalize ($arg1) {}

/**
 * @param arg1
 * @param arg2
 * @param arg3
 * @param arg4
 */
function locale_lookup ($arg1, $arg2, $arg3, $arg4) {}

/**
 * @param arg1
 */
function locale_accept_from_http ($arg1) {}

/**
 * @param locale
 * @param pattern
 */
function msgfmt_create ($locale, $pattern) {}

/**
 * @param nf
 * @param args
 */
function msgfmt_format ($nf, $args) {}

/**
 * @param locale
 * @param pattern
 * @param args
 */
function msgfmt_format_message ($locale, $pattern, $args) {}

/**
 * @param nf
 * @param source
 */
function msgfmt_parse ($nf, $source) {}

/**
 * @param locale
 * @param pattern
 * @param source
 */
function msgfmt_parse_message ($locale, $pattern, $source) {}

/**
 * @param mf
 * @param pattern
 */
function msgfmt_set_pattern ($mf, $pattern) {}

/**
 * @param mf
 */
function msgfmt_get_pattern ($mf) {}

/**
 * @param mf
 */
function msgfmt_get_locale ($mf) {}

/**
 * @param nf
 */
function msgfmt_get_error_code ($nf) {}

/**
 * @param coll
 */
function msgfmt_get_error_message ($coll) {}

/**
 * @param locale
 * @param date_type
 * @param time_type
 * @param timezone_str[optional]
 * @param calendar[optional]
 * @param pattern[optional]
 */
function datefmt_create ($locale, $date_type, $time_type, $timezone_str, $calendar, $pattern) {}

/**
 * @param mf
 */
function datefmt_get_datetype ($mf) {}

/**
 * @param mf
 */
function datefmt_get_timetype ($mf) {}

/**
 * @param mf
 */
function datefmt_get_calendar ($mf) {}

/**
 * @param mf
 * @param calendar
 */
function datefmt_set_calendar ($mf, $calendar) {}

/**
 * @param mf
 */
function datefmt_get_locale ($mf) {}

/**
 * @param mf
 */
function datefmt_get_timezone_id ($mf) {}

/**
 * @param mf
 */
function datefmt_set_timezone_id ($mf) {}

/**
 * @param mf
 */
function datefmt_get_pattern ($mf) {}

/**
 * @param mf
 * @param pattern
 */
function datefmt_set_pattern ($mf, $pattern) {}

/**
 * @param mf
 */
function datefmt_is_lenient ($mf) {}

/**
 * @param mf
 */
function datefmt_set_lenient ($mf) {}

/**
 * @param args[optional]
 * @param array[optional]
 */
function datefmt_format ($args, $array) {}

/**
 * @param formatter
 * @param string
 * @param position[optional]
 */
function datefmt_parse ($formatter, $string, &$position) {}

/**
 * @param formatter
 * @param string
 * @param position[optional]
 */
function datefmt_localtime ($formatter, $string, &$position) {}

/**
 * @param nf
 */
function datefmt_get_error_code ($nf) {}

/**
 * @param coll
 */
function datefmt_get_error_message ($coll) {}

/**
 * Get string length in grapheme units
 * @link http://www.php.net/manual/en/function.grapheme-strlen.php
 * @param input string <p>
 *       The string being measured for length. It must be a valid UTF-8 string.
 *      </p>
 * @return int The length of the string on success, and 0 if the string is empty.
 */
function grapheme_strlen ($input) {}

/**
 * Find position (in grapheme units) of first occurrence of a string
 * @link http://www.php.net/manual/en/function.grapheme-strpos.php
 * @param haystack string <p>
 *       The string to look in. Must be valid UTF-8.
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8.
 *      </p>
 * @param offset int[optional] <p>
 *       The optional $offset parameter allows you to specify where in $haystack to
 *       start searching as an offset in grapheme units (not bytes or characters).
 *       The position returned is still relative to the beginning of haystack
 *       regardless of the value of $offset.
 *      </p>
 * @return int the position as an integer. If needle is not found, strpos() will return boolean FALSE.
 */
function grapheme_strpos ($haystack, $needle, $offset = null) {}

/**
 * Find position (in grapheme units) of first occurrence of a case-insensitive string
 * @link http://www.php.net/manual/en/function.grapheme-stripos.php
 * @param haystack string <p>
 *       The string to look in. Must be valid UTF-8.
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8. 
 *      </p>
 * @param offset int[optional] <p>
 *       The optional $offset parameter allows you to specify where in haystack to
 *       start searching as an offset in grapheme units (not bytes or characters).
 *       The position returned is still relative to the beginning of haystack
 *       regardless of the value of $offset.
 *      </p>
 * @return int the position as an integer. If needle is not found, grapheme_stripos() will return boolean FALSE.
 */
function grapheme_stripos ($haystack, $needle, $offset = null) {}

/**
 * Find position (in grapheme units) of last occurrence of a string
 * @link http://www.php.net/manual/en/function.grapheme-strrpos.php
 * @param haystack string <p>
 *       The string to look in. Must be valid UTF-8.
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8.
 *      </p>
 * @param offset int[optional] <p>
 *       The optional $offset parameter allows you to specify where in $haystack to
 *       start searching as an offset in grapheme units (not bytes or characters).
 *       The position returned is still relative to the beginning of haystack
 *       regardless of the value of $offset.
 *      </p>
 * @return int the position as an integer. If needle is not found, grapheme_strrpos() will return boolean FALSE.
 */
function grapheme_strrpos ($haystack, $needle, $offset = null) {}

/**
 * Find position (in grapheme units) of last occurrence of a case-insensitive string
 * @link http://www.php.net/manual/en/function.grapheme-strripos.php
 * @param haystack string <p>
 *       The string to look in. Must be valid UTF-8.
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8.
 *      </p>
 * @param offset int[optional] <p>
 *       The optional $offset parameter allows you to specify where in $haystack to
 *       start searching as an offset in grapheme units (not bytes or characters).
 *       The position returned is still relative to the beginning of haystack
 *       regardless of the value of $offset.
 *      </p>
 * @return int the position as an integer. If needle is not found, grapheme_strripos() will return boolean FALSE.
 */
function grapheme_strripos ($haystack, $needle, $offset = null) {}

/**
 * Return part of a string
 * @link http://www.php.net/manual/en/function.grapheme-substr.php
 * @param string string <p>
 *       The input string. Must be valid UTF-8. 
 *      </p>
 * @param start int <p>
 *       Start position in default grapheme units.
 *       If $start is non-negative, the returned string will start at the
 *       $start'th position in $string, counting from zero. If $start is negative,
 *       the returned string will start at the $start'th grapheme unit from the 
 *       end of string.
 *      </p>
 * @param length int[optional] <p>
 *       Length in grapheme units.
 *       If $length  is given and is positive, the string returned will contain
 *       at most $length grapheme units beginning from $start (depending on the 
 *       length of string). If $length is given and is negative, then
 *       that many grapheme units will be omitted from the end of string (after the
 *       start position has been calculated when a start is negative). If $start
 *       denotes a position beyond this truncation, false will be returned. 
 *      </p>
 * @return int the extracted part of $string.
 */
function grapheme_substr ($string, $start, $length = null) {}

/**
 * Returns part of haystack string from the first occurrence of needle to the end of haystack.
 * @link http://www.php.net/manual/en/function.grapheme-strstr.php
 * @param haystack string <p>
 *       The input string. Must be valid UTF-8. 
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8. 
 *      </p>
 * @param before_needle bool[optional] <p>
 *       If true, grapheme_strstr() returns the part of the
 *       haystack before the first occurrence of the needle.
 *      </p>
 * @return string the portion of string, or FALSE if needle is not found.
 */
function grapheme_strstr ($haystack, $needle, $before_needle = null) {}

/**
 * Returns part of haystack string from the first occurrence of case-insensitive needle to the end of haystack.
 * @link http://www.php.net/manual/en/function.grapheme-stristr.php
 * @param haystack string <p>
 *       The input string. Must be valid UTF-8.
 *      </p>
 * @param needle string <p>
 *       The string to look for. Must be valid UTF-8.
 *      </p>
 * @param before_needle bool[optional] <p>
 *       If true, grapheme_strstr() returns the part of the
 *       haystack before the first occurrence of the needle.
 *      </p>
 * @return string the portion of $haystack, or FALSE if $needle is not found.
 */
function grapheme_stristr ($haystack, $needle, $before_needle = null) {}

/**
 * Function to extract a sequence of default grapheme clusters from a text buffer, which must be encoded in UTF-8.
 * @link http://www.php.net/manual/en/function.grapheme-extract.php
 * @param haystack string <p>
 *       String to search.
 *      </p>
 * @param size int <p>
 *       Maximum number items - based on the $extract_type - to return.
 *      </p>
 * @param extract_type int[optional] <p>
 *       Defines the type of units referred to by the $size parameter:
 *      </p>
 *       <p>
 *        
 *         GRAPHEME_EXTR_COUNT (default) - $size is the number of default
 *         grapheme clusters to extract.
 *         GRAPHEME_EXTR_MAXBYTES - $size is the maximum number of bytes
 *         returned.
 *         GRAPHEME_EXTR_MAXCHARS - $size is the maximum number of UTF-8
 *         characters returned.
 *        
 *       </p>
 * @param start int[optional] <p>
 *       Starting position in $haystack in bytes - if given, it must be zero or a
 *       positive value that is less than or equal to the length of $haystack in
 *       bytes. If $start does not point to the first byte of a UTF-8
 *       character, the start position is moved to the next character boundary.
 *      </p>
 * @param next int[optional] <p>
 *       Reference to a value that will be set to the next starting position.
 *       When the call returns, this may point to the first byte position past the end of the string.
 *      </p>
 * @return string A string starting at offset $start and ending on a default grapheme cluster
 *   boundary that conforms to the $size and $extract_type specified.
 */
function grapheme_extract ($haystack, $size, $extract_type = null, $start = null, &$next = null) {}

/**
 * Convert domain name to IDNA ASCII form.
 * @link http://www.php.net/manual/en/function.idn-to-ascii.php
 * @param utf8_domain string <p>
 *      The UTF-8 encoded domain name.
 *        
 *         <p>
 *         If e.g. an ISO-8859-1 (aka Western Europe latin1) encoded string is
 *         passed it will be converted into an ACE encoded "xn--" string. 
 *         It will not be the one you expected though!
 *         </p>
 *        
 *      </p>
 * @param errorcode int[optional] <p>
 *       Will be set to the IDNA error code. 
 *      </p>
 * @param domain string <p>
 *       Domain to convert. In PHP 5 must be UTF-8 encoded.
 *      </p>
 * @param options int[optional] <p>
 *       Conversion options - combination of IDNA_* constants.
 *      </p>
 * @return string Domain name encoded in ASCII-compatible form. &return.falseforfailure;
 */
function idn_to_ascii ($utf8_domain, &$errorcode = null, $domain, $options = null) {}

/**
 * Convert domain name from IDNA ASCII to Unicode.
 * @link http://www.php.net/manual/en/function.idn-to-utf8.php
 * @param ascii_domain string <p>
 *        The ASCII encoded domain name. Looks like "xn--..." if the it originally contained non-ASCII characters.
 *      </p>
 * @param errorcode int[optional] <p>
 *        Will be set to the IDNA error code.
 *      </p>
 * @param domain string <p>
 *       Domain to convert in IDNA ASCII-compatible format.
 *      </p>
 * @param options int[optional] <p>
 *       Conversion options - combination of IDNA_* constants.
 *      </p>
 * @return string Domain name in Unicode. In PHP 5, domain name will be in UTF-8. &return.falseforfailure;
 */
function idn_to_utf8 ($ascii_domain, &$errorcode = null, $domain, $options = null) {}

/**
 * @param locale
 * @param bundlename
 * @param fallback[optional]
 */
function resourcebundle_create ($locale, $bundlename, $fallback) {}

/**
 * @param bundle
 * @param index
 * @param fallback[optional]
 */
function resourcebundle_get ($bundle, $index, $fallback) {}

/**
 * @param bundle
 */
function resourcebundle_count ($bundle) {}

/**
 * @param bundlename
 */
function resourcebundle_locales ($bundlename) {}

/**
 * @param bundle
 */
function resourcebundle_get_error_code ($bundle) {}

/**
 * @param bundle
 */
function resourcebundle_get_error_message ($bundle) {}

/**
 * Get the last error code
 * @link http://www.php.net/manual/en/function.intl-get-error-code.php
 * @return int Error code returned by the last API function call.
 */
function intl_get_error_code () {}

/**
 * Get description of the last error
 * @link http://www.php.net/manual/en/function.intl-get-error-message.php
 * @return string Description of an error occurred in the last API function call.
 */
function intl_get_error_message () {}

/**
 * Check whether the given error code indicates failure
 * @link http://www.php.net/manual/en/function.intl-is-failure.php
 * @param error_code int <p>
 *       is a value that returned by functions:
 *       intl_get_error_code,
 *       collator_get_error_code .
 *      </p>
 * @return bool true if it the code indicates some failure, and false
 *   in case of success or a warning.
 */
function intl_is_failure ($error_code) {}

/**
 * Get symbolic name for a given error code
 * @link http://www.php.net/manual/en/function.intl-error-name.php
 * @param error_code int <p>
 *       ICU error code.
 *      </p>
 * @return string The returned string will be the same as the name of the error code
 *   constant.
 */
function intl_error_name ($error_code) {}


/**
 * Limit on locale length, set to 80 in PHP code. Locale names longer 
 *      than this limit will not be accepted.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('INTL_MAX_LOCALE_LEN', 80);
define ('INTL_ICU_VERSION', 3.6);
define ('ULOC_ACTUAL_LOCALE', 0);
define ('ULOC_VALID_LOCALE', 1);
define ('GRAPHEME_EXTR_COUNT', 0);
define ('GRAPHEME_EXTR_MAXBYTES', 1);
define ('GRAPHEME_EXTR_MAXCHARS', 2);
define ('U_USING_FALLBACK_WARNING', -128);
define ('U_ERROR_WARNING_START', -128);
define ('U_USING_DEFAULT_WARNING', -127);
define ('U_SAFECLONE_ALLOCATED_WARNING', -126);
define ('U_STATE_OLD_WARNING', -125);
define ('U_STRING_NOT_TERMINATED_WARNING', -124);
define ('U_SORT_KEY_TOO_SHORT_WARNING', -123);
define ('U_AMBIGUOUS_ALIAS_WARNING', -122);
define ('U_DIFFERENT_UCA_VERSION', -121);
define ('U_ERROR_WARNING_LIMIT', -120);
define ('U_ZERO_ERROR', 0);
define ('U_ILLEGAL_ARGUMENT_ERROR', 1);
define ('U_MISSING_RESOURCE_ERROR', 2);
define ('U_INVALID_FORMAT_ERROR', 3);
define ('U_FILE_ACCESS_ERROR', 4);
define ('U_INTERNAL_PROGRAM_ERROR', 5);
define ('U_MESSAGE_PARSE_ERROR', 6);
define ('U_MEMORY_ALLOCATION_ERROR', 7);
define ('U_INDEX_OUTOFBOUNDS_ERROR', 8);
define ('U_PARSE_ERROR', 9);
define ('U_INVALID_CHAR_FOUND', 10);
define ('U_TRUNCATED_CHAR_FOUND', 11);
define ('U_ILLEGAL_CHAR_FOUND', 12);
define ('U_INVALID_TABLE_FORMAT', 13);
define ('U_INVALID_TABLE_FILE', 14);
define ('U_BUFFER_OVERFLOW_ERROR', 15);
define ('U_UNSUPPORTED_ERROR', 16);
define ('U_RESOURCE_TYPE_MISMATCH', 17);
define ('U_ILLEGAL_ESCAPE_SEQUENCE', 18);
define ('U_UNSUPPORTED_ESCAPE_SEQUENCE', 19);
define ('U_NO_SPACE_AVAILABLE', 20);
define ('U_CE_NOT_FOUND_ERROR', 21);
define ('U_PRIMARY_TOO_LONG_ERROR', 22);
define ('U_STATE_TOO_OLD_ERROR', 23);
define ('U_TOO_MANY_ALIASES_ERROR', 24);
define ('U_ENUM_OUT_OF_SYNC_ERROR', 25);
define ('U_INVARIANT_CONVERSION_ERROR', 26);
define ('U_INVALID_STATE_ERROR', 27);
define ('U_COLLATOR_VERSION_MISMATCH', 28);
define ('U_USELESS_COLLATOR_ERROR', 29);
define ('U_NO_WRITE_PERMISSION', 30);
define ('U_STANDARD_ERROR_LIMIT', 31);
define ('U_BAD_VARIABLE_DEFINITION', 65536);
define ('U_PARSE_ERROR_START', 65536);
define ('U_MALFORMED_RULE', 65537);
define ('U_MALFORMED_SET', 65538);
define ('U_MALFORMED_SYMBOL_REFERENCE', 65539);
define ('U_MALFORMED_UNICODE_ESCAPE', 65540);
define ('U_MALFORMED_VARIABLE_DEFINITION', 65541);
define ('U_MALFORMED_VARIABLE_REFERENCE', 65542);
define ('U_MISMATCHED_SEGMENT_DELIMITERS', 65543);
define ('U_MISPLACED_ANCHOR_START', 65544);
define ('U_MISPLACED_CURSOR_OFFSET', 65545);
define ('U_MISPLACED_QUANTIFIER', 65546);
define ('U_MISSING_OPERATOR', 65547);
define ('U_MISSING_SEGMENT_CLOSE', 65548);
define ('U_MULTIPLE_ANTE_CONTEXTS', 65549);
define ('U_MULTIPLE_CURSORS', 65550);
define ('U_MULTIPLE_POST_CONTEXTS', 65551);
define ('U_TRAILING_BACKSLASH', 65552);
define ('U_UNDEFINED_SEGMENT_REFERENCE', 65553);
define ('U_UNDEFINED_VARIABLE', 65554);
define ('U_UNQUOTED_SPECIAL', 65555);
define ('U_UNTERMINATED_QUOTE', 65556);
define ('U_RULE_MASK_ERROR', 65557);
define ('U_MISPLACED_COMPOUND_FILTER', 65558);
define ('U_MULTIPLE_COMPOUND_FILTERS', 65559);
define ('U_INVALID_RBT_SYNTAX', 65560);
define ('U_INVALID_PROPERTY_PATTERN', 65561);
define ('U_MALFORMED_PRAGMA', 65562);
define ('U_UNCLOSED_SEGMENT', 65563);
define ('U_ILLEGAL_CHAR_IN_SEGMENT', 65564);
define ('U_VARIABLE_RANGE_EXHAUSTED', 65565);
define ('U_VARIABLE_RANGE_OVERLAP', 65566);
define ('U_ILLEGAL_CHARACTER', 65567);
define ('U_INTERNAL_TRANSLITERATOR_ERROR', 65568);
define ('U_INVALID_ID', 65569);
define ('U_INVALID_FUNCTION', 65570);
define ('U_PARSE_ERROR_LIMIT', 65571);
define ('U_UNEXPECTED_TOKEN', 65792);
define ('U_FMT_PARSE_ERROR_START', 65792);
define ('U_MULTIPLE_DECIMAL_SEPARATORS', 65793);
define ('U_MULTIPLE_DECIMAL_SEPERATORS', 65793);
define ('U_MULTIPLE_EXPONENTIAL_SYMBOLS', 65794);
define ('U_MALFORMED_EXPONENTIAL_PATTERN', 65795);
define ('U_MULTIPLE_PERCENT_SYMBOLS', 65796);
define ('U_MULTIPLE_PERMILL_SYMBOLS', 65797);
define ('U_MULTIPLE_PAD_SPECIFIERS', 65798);
define ('U_PATTERN_SYNTAX_ERROR', 65799);
define ('U_ILLEGAL_PAD_POSITION', 65800);
define ('U_UNMATCHED_BRACES', 65801);
define ('U_UNSUPPORTED_PROPERTY', 65802);
define ('U_UNSUPPORTED_ATTRIBUTE', 65803);
define ('U_FMT_PARSE_ERROR_LIMIT', 65804);
define ('U_BRK_INTERNAL_ERROR', 66048);
define ('U_BRK_ERROR_START', 66048);
define ('U_BRK_HEX_DIGITS_EXPECTED', 66049);
define ('U_BRK_SEMICOLON_EXPECTED', 66050);
define ('U_BRK_RULE_SYNTAX', 66051);
define ('U_BRK_UNCLOSED_SET', 66052);
define ('U_BRK_ASSIGN_ERROR', 66053);
define ('U_BRK_VARIABLE_REDFINITION', 66054);
define ('U_BRK_MISMATCHED_PAREN', 66055);
define ('U_BRK_NEW_LINE_IN_QUOTED_STRING', 66056);
define ('U_BRK_UNDEFINED_VARIABLE', 66057);
define ('U_BRK_INIT_ERROR', 66058);
define ('U_BRK_RULE_EMPTY_SET', 66059);
define ('U_BRK_UNRECOGNIZED_OPTION', 66060);
define ('U_BRK_MALFORMED_RULE_TAG', 66061);
define ('U_BRK_ERROR_LIMIT', 66062);
define ('U_REGEX_INTERNAL_ERROR', 66304);
define ('U_REGEX_ERROR_START', 66304);
define ('U_REGEX_RULE_SYNTAX', 66305);
define ('U_REGEX_INVALID_STATE', 66306);
define ('U_REGEX_BAD_ESCAPE_SEQUENCE', 66307);
define ('U_REGEX_PROPERTY_SYNTAX', 66308);
define ('U_REGEX_UNIMPLEMENTED', 66309);
define ('U_REGEX_MISMATCHED_PAREN', 66310);
define ('U_REGEX_NUMBER_TOO_BIG', 66311);
define ('U_REGEX_BAD_INTERVAL', 66312);
define ('U_REGEX_MAX_LT_MIN', 66313);
define ('U_REGEX_INVALID_BACK_REF', 66314);
define ('U_REGEX_INVALID_FLAG', 66315);
define ('U_REGEX_LOOK_BEHIND_LIMIT', 66316);
define ('U_REGEX_SET_CONTAINS_STRING', 66317);
define ('U_REGEX_ERROR_LIMIT', 66318);
define ('U_STRINGPREP_PROHIBITED_ERROR', 66560);
define ('U_STRINGPREP_UNASSIGNED_ERROR', 66561);
define ('U_STRINGPREP_CHECK_BIDI_ERROR', 66562);
define ('U_ERROR_LIMIT', 66568);

/**
 * Prohibit processing of unassigned codepoints in the input for IDN
 *      functions and do not check if the input conforms to domain name ASCII rules.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('IDNA_DEFAULT', 0);

/**
 * Allow processing of unassigned codepoints in the input for IDN functions.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('IDNA_ALLOW_UNASSIGNED', 1);

/**
 * Check if the input for IDN functions conforms to domain name ASCII rules.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('IDNA_USE_STD3_RULES', 2);

// End of intl v.1.1.0
?>
