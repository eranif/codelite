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
	 * @param arg1
	 */
	public function __construct ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function create ($arg1) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public function compare ($arg1, $arg2) {}

	/**
	 * @param arr
	 * @param flags[optional]
	 */
	public function sort (array &$arr, $flags) {}

	/**
	 * @param arr
	 * @param flags[optional]
	 */
	public function sortWithSortKeys (array &$arr, $flags) {}

	/**
	 * @param arr
	 * @param flags[optional]
	 */
	public function asort (array &$arr, $flags) {}

	/**
	 * @param arg1
	 */
	public function getAttribute ($arg1) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public function setAttribute ($arg1, $arg2) {}

	public function getStrength () {}

	/**
	 * @param arg1
	 */
	public function setStrength ($arg1) {}

	/**
	 * @param arg1
	 */
	public function getLocale ($arg1) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public function getSortKey ($arg1, $arg2) {}

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
	const PATTERN_RULEBASED = 9;
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
	 * @param locale
	 * @param style
	 * @param pattern[optional]
	 */
	public static function create ($locale, $style, $pattern) {}

	/**
	 * @param num
	 * @param type[optional]
	 */
	public function format ($num, $type) {}

	/**
	 * @param string
	 * @param type[optional]
	 * @param position[optional]
	 */
	public function parse ($string, $type, &$position) {}

	/**
	 * @param num
	 * @param currency
	 */
	public function formatCurrency ($num, $currency) {}

	/**
	 * @param string
	 * @param currency
	 * @param position[optional]
	 */
	public function parseCurrency ($string, &$currency, &$position) {}

	/**
	 * @param attr
	 * @param value
	 */
	public function setAttribute ($attr, $value) {}

	/**
	 * @param attr
	 */
	public function getAttribute ($attr) {}

	/**
	 * @param attr
	 * @param value
	 */
	public function setTextAttribute ($attr, $value) {}

	/**
	 * @param attr
	 */
	public function getTextAttribute ($attr) {}

	/**
	 * @param attr
	 * @param symbol
	 */
	public function setSymbol ($attr, $symbol) {}

	/**
	 * @param attr
	 */
	public function getSymbol ($attr) {}

	/**
	 * @param pattern
	 */
	public function setPattern ($pattern) {}

	public function getPattern () {}

	/**
	 * @param type[optional]
	 */
	public function getLocale ($type) {}

	public function getErrorCode () {}

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
	 * @param arg1
	 * @param arg2
	 * @param arg3
	 */
	public static function normalize ($arg1, $arg2, $arg3) {}

	/**
	 * @param arg1
	 * @param arg2
	 * @param arg3
	 */
	public static function isNormalized ($arg1, $arg2, $arg3) {}

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


	public static function getDefault () {}

	/**
	 * @param arg1
	 */
	public static function setDefault ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function getPrimaryLanguage ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function getScript ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function getRegion ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function getKeywords ($arg1) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public static function getDisplayScript ($arg1, $arg2) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public static function getDisplayRegion ($arg1, $arg2) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public static function getDisplayName ($arg1, $arg2) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public static function getDisplayLanguage ($arg1, $arg2) {}

	/**
	 * @param arg1
	 * @param arg2
	 */
	public static function getDisplayVariant ($arg1, $arg2) {}

	/**
	 * @param arg1
	 */
	public static function composeLocale ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function parseLocale ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function getAllVariants ($arg1) {}

	/**
	 * @param arg1
	 * @param arg2
	 * @param arg3
	 */
	public static function filterMatches ($arg1, $arg2, $arg3) {}

	/**
	 * @param arg1
	 * @param arg2
	 * @param arg3
	 * @param arg4
	 */
	public static function lookup ($arg1, $arg2, $arg3, $arg4) {}

	/**
	 * @param arg1
	 */
	public static function canonicalize ($arg1) {}

	/**
	 * @param arg1
	 */
	public static function acceptFromHttp ($arg1) {}

}

class MessageFormatter  {

	/**
	 * @param locale
	 * @param pattern
	 */
	public function __construct ($locale, $pattern) {}

	/**
	 * @param locale
	 * @param pattern
	 */
	public static function create ($locale, $pattern) {}

	/**
	 * @param args
	 */
	public function format ($args) {}

	/**
	 * @param locale
	 * @param pattern
	 * @param args
	 */
	public static function formatMessage ($locale, $pattern, $args) {}

	/**
	 * @param source
	 */
	public function parse ($source) {}

	/**
	 * @param locale
	 * @param pattern
	 * @param args
	 */
	public static function parseMessage ($locale, $pattern, $args) {}

	/**
	 * @param pattern
	 */
	public function setPattern ($pattern) {}

	public function getPattern () {}

	public function getLocale () {}

	public function getErrorCode () {}

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
	 * @param locale
	 * @param datetype
	 * @param timetype
	 * @param timezone[optional]
	 * @param calendar[optional]
	 * @param pattern[optional]
	 */
	public static function create ($locale, $datetype, $timetype, $timezone, $calendar, $pattern) {}

	public function getDateType () {}

	public function getTimeType () {}

	public function getCalendar () {}

	public function getCalendarObject () {}

	/**
	 * @param which
	 */
	public function setCalendar ($which) {}

	public function getTimeZoneId () {}

	/**
	 * @param zone
	 */
	public function setTimeZoneId ($zone) {}

	public function getTimeZone () {}

	/**
	 * @param zone
	 */
	public function setTimeZone ($zone) {}

	/**
	 * @param pattern
	 */
	public function setPattern ($pattern) {}

	public function getPattern () {}

	public function getLocale () {}

	/**
	 * @param lenient
	 */
	public function setLenient ($lenient) {}

	public function isLenient () {}

	/**
	 * @param args[optional]
	 * @param array[optional]
	 */
	public function format ($args, $array) {}

	/**
	 * @param object
	 * @param format[optional]
	 * @param locale[optional]
	 */
	public static function formatObject ($object, $format, $locale) {}

	/**
	 * @param string
	 * @param position[optional]
	 */
	public function parse ($string, &$position) {}

	/**
	 * @param string
	 * @param position[optional]
	 */
	public function localtime ($string, &$position) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class ResourceBundle implements Traversable {

	/**
	 * @param locale
	 * @param bundlename
	 * @param fallback[optional]
	 */
	public function __construct ($locale, $bundlename, $fallback) {}

	/**
	 * @param locale
	 * @param bundlename
	 * @param fallback[optional]
	 */
	public static function create ($locale, $bundlename, $fallback) {}

	/**
	 * @param index
	 * @param fallback[optional]
	 */
	public function get ($index, $fallback) {}

	public function count () {}

	/**
	 * @param bundlename
	 */
	public static function getLocales ($bundlename) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class Transliterator  {
	const FORWARD = 0;
	const REVERSE = 1;

	public $id;


	final private function __construct () {}

	/**
	 * @param id
	 * @param direction[optional]
	 */
	public static function create ($id, $direction) {}

	/**
	 * @param rules
	 * @param direction[optional]
	 */
	public static function createFromRules ($rules, $direction) {}

	public function createInverse () {}

	public static function listIDs () {}

	/**
	 * @param subject
	 * @param start[optional]
	 * @param end[optional]
	 */
	public function transliterate ($subject, $start, $end) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlTimeZone  {
	const DISPLAY_SHORT = 1;
	const DISPLAY_LONG = 2;
	const DISPLAY_SHORT_GENERIC = 3;
	const DISPLAY_LONG_GENERIC = 4;
	const DISPLAY_SHORT_GMT = 5;
	const DISPLAY_LONG_GMT = 6;
	const DISPLAY_SHORT_COMMONLY_USED = 7;
	const DISPLAY_GENERIC_LOCATION = 8;


	private function __construct () {}

	/**
	 * @param zoneId
	 */
	public static function createTimeZone ($zoneId) {}

	/**
	 * @param zoneId
	 */
	public static function fromDateTimeZone ($zoneId) {}

	public static function createDefault () {}

	public static function getGMT () {}

	/**
	 * @param countryOrRawOffset[optional]
	 */
	public static function createEnumeration ($countryOrRawOffset) {}

	/**
	 * @param zoneId
	 */
	public static function countEquivalentIDs ($zoneId) {}

	/**
	 * @param zoneId
	 * @param isSystemID[optional]
	 */
	public static function getCanonicalID ($zoneId, &$isSystemID) {}

	public static function getTZDataVersion () {}

	/**
	 * @param zoneId
	 * @param index
	 */
	public static function getEquivalentID ($zoneId, $index) {}

	public function getID () {}

	public function useDaylightTime () {}

	/**
	 * @param date
	 * @param local
	 * @param rawOffset
	 * @param dstOffset
	 */
	public function getOffset ($date, $local, &$rawOffset, &$dstOffset) {}

	public function getRawOffset () {}

	/**
	 * @param otherTimeZone IntlTimeZone
	 */
	public function hasSameRules (IntlTimeZone $otherTimeZone) {}

	/**
	 * @param isDaylight[optional]
	 * @param style[optional]
	 * @param locale[optional]
	 */
	public function getDisplayName ($isDaylight, $style, $locale) {}

	public function getDSTSavings () {}

	public function toDateTimeZone () {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlCalendar  {
	const FIELD_ERA = 0;
	const FIELD_YEAR = 1;
	const FIELD_MONTH = 2;
	const FIELD_WEEK_OF_YEAR = 3;
	const FIELD_WEEK_OF_MONTH = 4;
	const FIELD_DATE = 5;
	const FIELD_DAY_OF_YEAR = 6;
	const FIELD_DAY_OF_WEEK = 7;
	const FIELD_DAY_OF_WEEK_IN_MONTH = 8;
	const FIELD_AM_PM = 9;
	const FIELD_HOUR = 10;
	const FIELD_HOUR_OF_DAY = 11;
	const FIELD_MINUTE = 12;
	const FIELD_SECOND = 13;
	const FIELD_MILLISECOND = 14;
	const FIELD_ZONE_OFFSET = 15;
	const FIELD_DST_OFFSET = 16;
	const FIELD_YEAR_WOY = 17;
	const FIELD_DOW_LOCAL = 18;
	const FIELD_EXTENDED_YEAR = 19;
	const FIELD_JULIAN_DAY = 20;
	const FIELD_MILLISECONDS_IN_DAY = 21;
	const FIELD_IS_LEAP_MONTH = 22;
	const FIELD_FIELD_COUNT = 23;
	const FIELD_DAY_OF_MONTH = 5;
	const DOW_SUNDAY = 1;
	const DOW_MONDAY = 2;
	const DOW_TUESDAY = 3;
	const DOW_WEDNESDAY = 4;
	const DOW_THURSDAY = 5;
	const DOW_FRIDAY = 6;
	const DOW_SATURDAY = 7;
	const DOW_TYPE_WEEKDAY = 0;
	const DOW_TYPE_WEEKEND = 1;
	const DOW_TYPE_WEEKEND_OFFSET = 2;
	const DOW_TYPE_WEEKEND_CEASE = 3;


	private function __construct () {}

	/**
	 * @param timeZone[optional]
	 * @param locale[optional]
	 */
	public static function createInstance ($timeZone, $locale) {}

	/**
	 * @param key
	 * @param locale
	 * @param commonlyUsed
	 */
	public static function getKeywordValuesForLocale ($key, $locale, $commonlyUsed) {}

	public static function getNow () {}

	public static function getAvailableLocales () {}

	/**
	 * @param field
	 */
	public function get ($field) {}

	public function getTime () {}

	/**
	 * @param date
	 */
	public function setTime ($date) {}

	/**
	 * @param field
	 * @param amount
	 */
	public function add ($field, $amount) {}

	/**
	 * @param timeZone
	 */
	public function setTimeZone ($timeZone) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function after (IntlCalendar $calendar) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function before (IntlCalendar $calendar) {}

	/**
	 * @param fieldOrYear
	 * @param valueOrMonth
	 * @param dayOfMonth[optional]
	 * @param hour[optional]
	 * @param minute[optional]
	 * @param second[optional]
	 */
	public function set ($fieldOrYear, $valueOrMonth, $dayOfMonth, $hour, $minute, $second) {}

	/**
	 * @param field
	 * @param amountOrUpOrDown
	 */
	public function roll ($field, $amountOrUpOrDown) {}

	/**
	 * @param field[optional]
	 */
	public function clear ($field) {}

	/**
	 * @param when
	 * @param field
	 */
	public function fieldDifference ($when, $field) {}

	/**
	 * @param field
	 */
	public function getActualMaximum ($field) {}

	/**
	 * @param field
	 */
	public function getActualMinimum ($field) {}

	/**
	 * @param dayOfWeek
	 */
	public function getDayOfWeekType ($dayOfWeek) {}

	public function getFirstDayOfWeek () {}

	/**
	 * @param field
	 */
	public function getGreatestMinimum ($field) {}

	/**
	 * @param field
	 */
	public function getLeastMaximum ($field) {}

	/**
	 * @param localeType
	 */
	public function getLocale ($localeType) {}

	/**
	 * @param field
	 */
	public function getMaximum ($field) {}

	public function getMinimalDaysInFirstWeek () {}

	/**
	 * @param field
	 */
	public function getMinimum ($field) {}

	public function getTimeZone () {}

	public function getType () {}

	/**
	 * @param dayOfWeek
	 */
	public function getWeekendTransition ($dayOfWeek) {}

	public function inDaylightTime () {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function isEquivalentTo (IntlCalendar $calendar) {}

	public function isLenient () {}

	/**
	 * @param field
	 */
	public function isSet ($field) {}

	/**
	 * @param date[optional]
	 */
	public function isWeekend ($date) {}

	/**
	 * @param dayOfWeek
	 */
	public function setFirstDayOfWeek ($dayOfWeek) {}

	/**
	 * @param isLenient
	 */
	public function setLenient ($isLenient) {}

	/**
	 * @param numberOfDays
	 */
	public function setMinimalDaysInFirstWeek ($numberOfDays) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function equals (IntlCalendar $calendar) {}

	/**
	 * @param dateTime
	 */
	public static function fromDateTime ($dateTime) {}

	public function toDateTime () {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlGregorianCalendar extends IntlCalendar  {

	/**
	 * @param timeZoneOrYear[optional]
	 * @param localeOrMonth[optional]
	 * @param dayOfMonth[optional]
	 * @param hour[optional]
	 * @param minute[optional]
	 * @param second[optional]
	 */
	public function __construct ($timeZoneOrYear, $localeOrMonth, $dayOfMonth, $hour, $minute, $second) {}

	/**
	 * @param date
	 */
	public function setGregorianChange ($date) {}

	public function getGregorianChange () {}

	/**
	 * @param year
	 */
	public function isLeapYear ($year) {}

	/**
	 * @param timeZone[optional]
	 * @param locale[optional]
	 */
	public static function createInstance ($timeZone, $locale) {}

	/**
	 * @param key
	 * @param locale
	 * @param commonlyUsed
	 */
	public static function getKeywordValuesForLocale ($key, $locale, $commonlyUsed) {}

	public static function getNow () {}

	public static function getAvailableLocales () {}

	/**
	 * @param field
	 */
	public function get ($field) {}

	public function getTime () {}

	/**
	 * @param date
	 */
	public function setTime ($date) {}

	/**
	 * @param field
	 * @param amount
	 */
	public function add ($field, $amount) {}

	/**
	 * @param timeZone
	 */
	public function setTimeZone ($timeZone) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function after (IntlCalendar $calendar) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function before (IntlCalendar $calendar) {}

	/**
	 * @param fieldOrYear
	 * @param valueOrMonth
	 * @param dayOfMonth[optional]
	 * @param hour[optional]
	 * @param minute[optional]
	 * @param second[optional]
	 */
	public function set ($fieldOrYear, $valueOrMonth, $dayOfMonth, $hour, $minute, $second) {}

	/**
	 * @param field
	 * @param amountOrUpOrDown
	 */
	public function roll ($field, $amountOrUpOrDown) {}

	/**
	 * @param field[optional]
	 */
	public function clear ($field) {}

	/**
	 * @param when
	 * @param field
	 */
	public function fieldDifference ($when, $field) {}

	/**
	 * @param field
	 */
	public function getActualMaximum ($field) {}

	/**
	 * @param field
	 */
	public function getActualMinimum ($field) {}

	/**
	 * @param dayOfWeek
	 */
	public function getDayOfWeekType ($dayOfWeek) {}

	public function getFirstDayOfWeek () {}

	/**
	 * @param field
	 */
	public function getGreatestMinimum ($field) {}

	/**
	 * @param field
	 */
	public function getLeastMaximum ($field) {}

	/**
	 * @param localeType
	 */
	public function getLocale ($localeType) {}

	/**
	 * @param field
	 */
	public function getMaximum ($field) {}

	public function getMinimalDaysInFirstWeek () {}

	/**
	 * @param field
	 */
	public function getMinimum ($field) {}

	public function getTimeZone () {}

	public function getType () {}

	/**
	 * @param dayOfWeek
	 */
	public function getWeekendTransition ($dayOfWeek) {}

	public function inDaylightTime () {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function isEquivalentTo (IntlCalendar $calendar) {}

	public function isLenient () {}

	/**
	 * @param field
	 */
	public function isSet ($field) {}

	/**
	 * @param date[optional]
	 */
	public function isWeekend ($date) {}

	/**
	 * @param dayOfWeek
	 */
	public function setFirstDayOfWeek ($dayOfWeek) {}

	/**
	 * @param isLenient
	 */
	public function setLenient ($isLenient) {}

	/**
	 * @param numberOfDays
	 */
	public function setMinimalDaysInFirstWeek ($numberOfDays) {}

	/**
	 * @param calendar IntlCalendar
	 */
	public function equals (IntlCalendar $calendar) {}

	/**
	 * @param dateTime
	 */
	public static function fromDateTime ($dateTime) {}

	public function toDateTime () {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class Spoofchecker  {
	const SINGLE_SCRIPT_CONFUSABLE = 1;
	const MIXED_SCRIPT_CONFUSABLE = 2;
	const WHOLE_SCRIPT_CONFUSABLE = 4;
	const ANY_CASE = 8;
	const SINGLE_SCRIPT = 16;
	const INVISIBLE = 32;
	const CHAR_LIMIT = 64;


	public function __construct () {}

	/**
	 * @param text
	 * @param error[optional]
	 */
	public function isSuspicious ($text, &$error) {}

	/**
	 * @param s1
	 * @param s2
	 * @param error[optional]
	 */
	public function areConfusable ($s1, $s2, &$error) {}

	/**
	 * @param locale_list
	 */
	public function setAllowedLocales ($locale_list) {}

	/**
	 * @param checks
	 */
	public function setChecks ($checks) {}

}

class IntlException extends Exception  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;


	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

class IntlIterator implements Iterator, Traversable {

	public function current () {}

	public function key () {}

	public function next () {}

	public function rewind () {}

	public function valid () {}

}

class IntlBreakIterator implements Traversable {
	const DONE = -1;
	const WORD_NONE = 0;
	const WORD_NONE_LIMIT = 100;
	const WORD_NUMBER = 100;
	const WORD_NUMBER_LIMIT = 200;
	const WORD_LETTER = 200;
	const WORD_LETTER_LIMIT = 300;
	const WORD_KANA = 300;
	const WORD_KANA_LIMIT = 400;
	const WORD_IDEO = 400;
	const WORD_IDEO_LIMIT = 500;
	const LINE_SOFT = 0;
	const LINE_SOFT_LIMIT = 100;
	const LINE_HARD = 100;
	const LINE_HARD_LIMIT = 200;
	const SENTENCE_TERM = 0;
	const SENTENCE_TERM_LIMIT = 100;
	const SENTENCE_SEP = 100;
	const SENTENCE_SEP_LIMIT = 200;


	private function __construct () {}

	/**
	 * @param locale[optional]
	 */
	public static function createWordInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createLineInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createCharacterInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createSentenceInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createTitleInstance ($locale) {}

	public static function createCodePointInstance () {}

	public function getText () {}

	/**
	 * @param text
	 */
	public function setText ($text) {}

	public function first () {}

	public function last () {}

	public function previous () {}

	/**
	 * @param offset[optional]
	 */
	public function next ($offset) {}

	public function current () {}

	/**
	 * @param offset
	 */
	public function following ($offset) {}

	/**
	 * @param offset
	 */
	public function preceding ($offset) {}

	/**
	 * @param offset
	 */
	public function isBoundary ($offset) {}

	/**
	 * @param locale_type
	 */
	public function getLocale ($locale_type) {}

	/**
	 * @param key_type[optional]
	 */
	public function getPartsIterator ($key_type) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlRuleBasedBreakIterator extends IntlBreakIterator implements Traversable {
	const DONE = -1;
	const WORD_NONE = 0;
	const WORD_NONE_LIMIT = 100;
	const WORD_NUMBER = 100;
	const WORD_NUMBER_LIMIT = 200;
	const WORD_LETTER = 200;
	const WORD_LETTER_LIMIT = 300;
	const WORD_KANA = 300;
	const WORD_KANA_LIMIT = 400;
	const WORD_IDEO = 400;
	const WORD_IDEO_LIMIT = 500;
	const LINE_SOFT = 0;
	const LINE_SOFT_LIMIT = 100;
	const LINE_HARD = 100;
	const LINE_HARD_LIMIT = 200;
	const SENTENCE_TERM = 0;
	const SENTENCE_TERM_LIMIT = 100;
	const SENTENCE_SEP = 100;
	const SENTENCE_SEP_LIMIT = 200;


	/**
	 * @param rules
	 * @param areCompiled[optional]
	 */
	public function __construct ($rules, $areCompiled) {}

	public function getRules () {}

	public function getRuleStatus () {}

	public function getRuleStatusVec () {}

	/**
	 * @param locale[optional]
	 */
	public static function createWordInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createLineInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createCharacterInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createSentenceInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createTitleInstance ($locale) {}

	public static function createCodePointInstance () {}

	public function getText () {}

	/**
	 * @param text
	 */
	public function setText ($text) {}

	public function first () {}

	public function last () {}

	public function previous () {}

	/**
	 * @param offset[optional]
	 */
	public function next ($offset) {}

	public function current () {}

	/**
	 * @param offset
	 */
	public function following ($offset) {}

	/**
	 * @param offset
	 */
	public function preceding ($offset) {}

	/**
	 * @param offset
	 */
	public function isBoundary ($offset) {}

	/**
	 * @param locale_type
	 */
	public function getLocale ($locale_type) {}

	/**
	 * @param key_type[optional]
	 */
	public function getPartsIterator ($key_type) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlCodePointBreakIterator extends IntlBreakIterator implements Traversable {
	const DONE = -1;
	const WORD_NONE = 0;
	const WORD_NONE_LIMIT = 100;
	const WORD_NUMBER = 100;
	const WORD_NUMBER_LIMIT = 200;
	const WORD_LETTER = 200;
	const WORD_LETTER_LIMIT = 300;
	const WORD_KANA = 300;
	const WORD_KANA_LIMIT = 400;
	const WORD_IDEO = 400;
	const WORD_IDEO_LIMIT = 500;
	const LINE_SOFT = 0;
	const LINE_SOFT_LIMIT = 100;
	const LINE_HARD = 100;
	const LINE_HARD_LIMIT = 200;
	const SENTENCE_TERM = 0;
	const SENTENCE_TERM_LIMIT = 100;
	const SENTENCE_SEP = 100;
	const SENTENCE_SEP_LIMIT = 200;


	public function getLastCodePoint () {}

	private function __construct () {}

	/**
	 * @param locale[optional]
	 */
	public static function createWordInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createLineInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createCharacterInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createSentenceInstance ($locale) {}

	/**
	 * @param locale[optional]
	 */
	public static function createTitleInstance ($locale) {}

	public static function createCodePointInstance () {}

	public function getText () {}

	/**
	 * @param text
	 */
	public function setText ($text) {}

	public function first () {}

	public function last () {}

	public function previous () {}

	/**
	 * @param offset[optional]
	 */
	public function next ($offset) {}

	public function current () {}

	/**
	 * @param offset
	 */
	public function following ($offset) {}

	/**
	 * @param offset
	 */
	public function preceding ($offset) {}

	/**
	 * @param offset
	 */
	public function isBoundary ($offset) {}

	/**
	 * @param locale_type
	 */
	public function getLocale ($locale_type) {}

	/**
	 * @param key_type[optional]
	 */
	public function getPartsIterator ($key_type) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

}

class IntlPartsIterator extends IntlIterator implements Traversable, Iterator {
	const KEY_SEQUENTIAL = 0;
	const KEY_LEFT = 1;
	const KEY_RIGHT = 2;


	public function getBreakIterator () {}

	public function current () {}

	public function key () {}

	public function next () {}

	public function rewind () {}

	public function valid () {}

}

class UConverter  {
	const REASON_UNASSIGNED = 0;
	const REASON_ILLEGAL = 1;
	const REASON_IRREGULAR = 2;
	const REASON_RESET = 3;
	const REASON_CLOSE = 4;
	const REASON_CLONE = 5;
	const UNSUPPORTED_CONVERTER = -1;
	const SBCS = 0;
	const DBCS = 1;
	const MBCS = 2;
	const LATIN_1 = 3;
	const UTF8 = 4;
	const UTF16_BigEndian = 5;
	const UTF16_LittleEndian = 6;
	const UTF32_BigEndian = 7;
	const UTF32_LittleEndian = 8;
	const EBCDIC_STATEFUL = 9;
	const ISO_2022 = 10;
	const LMBCS_1 = 11;
	const LMBCS_2 = 12;
	const LMBCS_3 = 13;
	const LMBCS_4 = 14;
	const LMBCS_5 = 15;
	const LMBCS_6 = 16;
	const LMBCS_8 = 17;
	const LMBCS_11 = 18;
	const LMBCS_16 = 19;
	const LMBCS_17 = 20;
	const LMBCS_18 = 21;
	const LMBCS_19 = 22;
	const LMBCS_LAST = 22;
	const HZ = 23;
	const SCSU = 24;
	const ISCII = 25;
	const US_ASCII = 26;
	const UTF7 = 27;
	const BOCU1 = 28;
	const UTF16 = 29;
	const UTF32 = 30;
	const CESU8 = 31;
	const IMAP_MAILBOX = 32;


	/**
	 * @param destination_encoding[optional]
	 * @param source_encoding[optional]
	 */
	public function __construct ($destination_encoding, $source_encoding) {}

	/**
	 * @param encoding
	 */
	public function setSourceEncoding ($encoding) {}

	/**
	 * @param encoding
	 */
	public function setDestinationEncoding ($encoding) {}

	public function getSourceEncoding () {}

	public function getDestinationEncoding () {}

	public function getSourceType () {}

	public function getDestinationType () {}

	public function getSubstChars () {}

	/**
	 * @param chars
	 */
	public function setSubstChars ($chars) {}

	/**
	 * @param reason
	 * @param source
	 * @param codeUnits
	 * @param error
	 */
	public function toUCallback ($reason, $source, $codeUnits, &$error) {}

	/**
	 * @param reason
	 * @param source
	 * @param codePoint
	 * @param error
	 */
	public function fromUCallback ($reason, $source, $codePoint, &$error) {}

	/**
	 * @param str
	 * @param reverse[optional]
	 */
	public function convert ($str, $reverse) {}

	/**
	 * @param str
	 * @param toEncoding
	 * @param fromEncoding
	 * @param options[optional]
	 */
	public static function transcode ($str, $toEncoding, $fromEncodingarray , $options = null) {}

	public function getErrorCode () {}

	public function getErrorMessage () {}

	/**
	 * @param reason[optional]
	 */
	public static function reasonText ($reason) {}

	public static function getAvailable () {}

	/**
	 * @param name[optional]
	 */
	public static function getAliases ($name) {}

	public static function getStandards () {}

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

function locale_get_default () {}

/**
 * @param arg1
 */
function locale_set_default ($arg1) {}

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
 */
function datefmt_get_calendar_object ($mf) {}

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
 * @param timezone
 */
function datefmt_set_timezone_id ($mf, $timezone) {}

/**
 * @param mf
 */
function datefmt_get_timezone ($mf) {}

/**
 * @param mf
 * @param timezone
 */
function datefmt_set_timezone ($mf, $timezone) {}

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
 * @param object
 * @param format[optional]
 * @param locale[optional]
 */
function datefmt_format_object ($object, $format, $locale) {}

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
 * @param string
 */
function grapheme_strlen ($string) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function grapheme_strpos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function grapheme_stripos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function grapheme_strrpos ($haystack, $needle, $offset) {}

/**
 * @param haystack
 * @param needle
 * @param offset[optional]
 */
function grapheme_strripos ($haystack, $needle, $offset) {}

/**
 * @param string
 * @param start
 * @param length[optional]
 */
function grapheme_substr ($string, $start, $length) {}

/**
 * @param haystack
 * @param needle
 * @param before_needle[optional]
 */
function grapheme_strstr ($haystack, $needle, $before_needle) {}

/**
 * @param haystack
 * @param needle
 * @param before_needle[optional]
 */
function grapheme_stristr ($haystack, $needle, $before_needle) {}

/**
 * @param arg1
 * @param arg2
 * @param arg3[optional]
 * @param arg4[optional]
 * @param arg5[optional]
 */
function grapheme_extract ($arg1, $arg2, $arg3, $arg4, &$arg5) {}

/**
 * @param domain
 * @param option[optional]
 * @param variant[optional]
 * @param idn_info[optional]
 */
function idn_to_ascii ($domain, $option, $variant, &$idn_info) {}

/**
 * @param domain
 * @param option[optional]
 * @param variant[optional]
 * @param idn_info[optional]
 */
function idn_to_utf8 ($domain, $option, $variant, &$idn_info) {}

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
 * @param id
 * @param direction[optional]
 */
function transliterator_create ($id, $direction) {}

/**
 * @param rules
 * @param direction[optional]
 */
function transliterator_create_from_rules ($rules, $direction) {}

function transliterator_list_ids () {}

/**
 * @param orig_trans Transliterator
 */
function transliterator_create_inverse (Transliterator $orig_trans) {}

/**
 * @param trans
 * @param subject
 * @param start[optional]
 * @param end[optional]
 */
function transliterator_transliterate ($trans, $subject, $start, $end) {}

/**
 * @param trans Transliterator
 */
function transliterator_get_error_code (Transliterator $trans) {}

/**
 * @param trans Transliterator
 */
function transliterator_get_error_message (Transliterator $trans) {}

/**
 * @param zoneId
 */
function intltz_create_time_zone ($zoneId) {}

/**
 * @param dateTimeZone DateTimeZone
 */
function intltz_from_date_time_zone (DateTimeZone $dateTimeZone) {}

function intltz_create_default () {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_get_id (IntlTimeZone $timeZone) {}

function intltz_get_gmt () {}

/**
 * @param countryOrRawOffset[optional]
 */
function intltz_create_enumeration ($countryOrRawOffset) {}

/**
 * @param zoneId
 */
function intltz_count_equivalent_ids ($zoneId) {}

/**
 * @param zoneId
 * @param isSystemID[optional]
 */
function intltz_get_canonical_id ($zoneId, &$isSystemID) {}

function intltz_get_tz_data_version () {}

/**
 * @param zoneId
 * @param index
 */
function intltz_get_equivalent_id ($zoneId, $index) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_use_daylight_time (IntlTimeZone $timeZone) {}

/**
 * @param timeZone IntlTimeZone
 * @param date
 * @param local
 * @param rawOffset
 * @param dstOffset
 */
function intltz_get_offset (IntlTimeZone $timeZone, $date, $local, &$rawOffset, &$dstOffset) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_get_raw_offset (IntlTimeZone $timeZone) {}

/**
 * @param timeZone IntlTimeZone
 * @param otherTimeZone IntlTimeZone[optional]
 */
function intltz_has_same_rules (IntlTimeZone $timeZoneIntlTimeZone , $otherTimeZone) {}

/**
 * @param timeZone IntlTimeZone
 * @param isDaylight[optional]
 * @param style[optional]
 * @param locale[optional]
 */
function intltz_get_display_name (IntlTimeZone $timeZone, $isDaylight, $style, $locale) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_get_dst_savings (IntlTimeZone $timeZone) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_to_date_time_zone (IntlTimeZone $timeZone) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_get_error_code (IntlTimeZone $timeZone) {}

/**
 * @param timeZone IntlTimeZone
 */
function intltz_get_error_message (IntlTimeZone $timeZone) {}

/**
 * @param timeZone[optional]
 * @param locale[optional]
 */
function intlcal_create_instance ($timeZone, $locale) {}

/**
 * @param key
 * @param locale
 * @param commonlyUsed
 */
function intlcal_get_keyword_values_for_locale ($key, $locale, $commonlyUsed) {}

function intlcal_get_now () {}

function intlcal_get_available_locales () {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_time (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param date
 */
function intlcal_set_time (IntlCalendar $calendar, $date) {}

/**
 * @param calendar IntlCalendar
 * @param field
 * @param amount
 */
function intlcal_add (IntlCalendar $calendar, $field, $amount) {}

/**
 * @param calendar IntlCalendar
 * @param timeZone
 */
function intlcal_set_time_zone (IntlCalendar $calendar, $timeZone) {}

/**
 * @param calendar IntlCalendar
 * @param otherCalendar IntlCalendar
 */
function intlcal_after (IntlCalendar $calendarIntlCalendar , $otherCalendar) {}

/**
 * @param calendar IntlCalendar
 * @param otherCalendar IntlCalendar
 */
function intlcal_before (IntlCalendar $calendarIntlCalendar , $otherCalendar) {}

/**
 * @param calendar IntlCalendar
 * @param fieldOrYear
 * @param valueOrMonth
 * @param dayOfMonth[optional]
 * @param hour[optional]
 * @param minute[optional]
 * @param second[optional]
 */
function intlcal_set (IntlCalendar $calendar, $fieldOrYear, $valueOrMonth, $dayOfMonth, $hour, $minute, $second) {}

/**
 * @param calendar IntlCalendar
 * @param field
 * @param amountOrUpOrDown[optional]
 */
function intlcal_roll (IntlCalendar $calendar, $field, $amountOrUpOrDown) {}

/**
 * @param calendar IntlCalendar
 * @param field[optional]
 */
function intlcal_clear (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param when
 * @param field
 */
function intlcal_field_difference (IntlCalendar $calendar, $when, $field) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_actual_maximum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_actual_minimum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param dayOfWeek
 */
function intlcal_get_day_of_week_type (IntlCalendar $calendar, $dayOfWeek) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_first_day_of_week (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_greatest_minimum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_least_maximum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param localeType
 */
function intlcal_get_locale (IntlCalendar $calendar, $localeType) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_maximum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_minimal_days_in_first_week (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_get_minimum (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_time_zone (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_type (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param dayOfWeek
 */
function intlcal_get_weekend_transition (IntlCalendar $calendar, $dayOfWeek) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_in_daylight_time (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param otherCalendar IntlCalendar
 */
function intlcal_is_equivalent_to (IntlCalendar $calendarIntlCalendar , $otherCalendar) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_is_lenient (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 * @param field
 */
function intlcal_is_set (IntlCalendar $calendar, $field) {}

/**
 * @param calendar IntlCalendar
 * @param date[optional]
 */
function intlcal_is_weekend (IntlCalendar $calendar, $date) {}

/**
 * @param calendar IntlCalendar
 * @param dayOfWeek
 */
function intlcal_set_first_day_of_week (IntlCalendar $calendar, $dayOfWeek) {}

/**
 * @param calendar IntlCalendar
 * @param isLenient
 */
function intlcal_set_lenient (IntlCalendar $calendar, $isLenient) {}

/**
 * @param calendar IntlCalendar
 * @param numberOfDays
 */
function intlcal_set_minimal_days_in_first_week (IntlCalendar $calendar, $numberOfDays) {}

/**
 * @param calendar IntlCalendar
 * @param otherCalendar IntlCalendar
 */
function intlcal_equals (IntlCalendar $calendarIntlCalendar , $otherCalendar) {}

/**
 * @param dateTime
 */
function intlcal_from_date_time ($dateTime) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_to_date_time (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_error_code (IntlCalendar $calendar) {}

/**
 * @param calendar IntlCalendar
 */
function intlcal_get_error_message (IntlCalendar $calendar) {}

/**
 * @param timeZoneOrYear[optional]
 * @param localeOrMonth[optional]
 * @param dayOfMonth[optional]
 * @param hour[optional]
 * @param minute[optional]
 * @param second[optional]
 */
function intlgregcal_create_instance ($timeZoneOrYear, $localeOrMonth, $dayOfMonth, $hour, $minute, $second) {}

/**
 * @param calendar IntlGregorianCalendar
 * @param date
 */
function intlgregcal_set_gregorian_change (IntlGregorianCalendar $calendar, $date) {}

/**
 * @param calendar IntlGregorianCalendar
 */
function intlgregcal_get_gregorian_change (IntlGregorianCalendar $calendar) {}

/**
 * @param calendar IntlGregorianCalendar
 * @param year
 */
function intlgregcal_is_leap_year (IntlGregorianCalendar $calendar, $year) {}

function intl_get_error_code () {}

function intl_get_error_message () {}

/**
 * @param arg1
 */
function intl_is_failure ($arg1) {}

/**
 * @param arg1
 */
function intl_error_name ($arg1) {}


/**
 * Limit on locale length, set to 80 in PHP code. Locale names longer 
 *      than this limit will not be accepted.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('INTL_MAX_LOCALE_LEN', 80);
define ('INTL_ICU_VERSION', "4.4.1");
define ('INTL_ICU_DATA_VERSION', "4.4.0.1");
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
define ('U_ERROR_WARNING_LIMIT', -119);
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
define ('U_FMT_PARSE_ERROR_LIMIT', 65809);
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
define ('U_REGEX_ERROR_LIMIT', 66324);
define ('U_IDNA_PROHIBITED_ERROR', 66560);
define ('U_IDNA_ERROR_START', 66560);
define ('U_IDNA_UNASSIGNED_ERROR', 66561);
define ('U_IDNA_CHECK_BIDI_ERROR', 66562);
define ('U_IDNA_STD3_ASCII_RULES_ERROR', 66563);
define ('U_IDNA_ACE_PREFIX_ERROR', 66564);
define ('U_IDNA_VERIFICATION_ERROR', 66565);
define ('U_IDNA_LABEL_TOO_LONG_ERROR', 66566);
define ('U_IDNA_ZERO_LENGTH_LABEL_ERROR', 66567);
define ('U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR', 66568);
define ('U_IDNA_ERROR_LIMIT', 66569);
define ('U_STRINGPREP_PROHIBITED_ERROR', 66560);
define ('U_STRINGPREP_UNASSIGNED_ERROR', 66561);
define ('U_STRINGPREP_CHECK_BIDI_ERROR', 66562);
define ('U_ERROR_LIMIT', 66818);

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

/**
 * Use IDNA 2003 algorithm in idn_to_utf8 and
 *      idn_to_ascii. This is the default.
 * @link http://www.php.net/manual/en/intl.constants.php
 */
define ('INTL_IDNA_VARIANT_2003', 0);

// End of intl v.1.1.0
?>
