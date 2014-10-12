<?php

// Start of date v.5.6.0

interface DateTimeInterface  {

	/**
	 * @param format
	 */
	abstract public function format ($format) {}

	abstract public function getTimezone () {}

	abstract public function getOffset () {}

	abstract public function getTimestamp () {}

	/**
	 * @param object
	 * @param absolute[optional]
	 */
	abstract public function diff ($object, $absolute) {}

	abstract public function __wakeup () {}

}

class DateTime implements DateTimeInterface {
	const ATOM = "Y-m-d\TH:i:sP";
	const COOKIE = "l, d-M-Y H:i:s T";
	const ISO8601 = "Y-m-d\TH:i:sO";
	const RFC822 = "D, d M y H:i:s O";
	const RFC850 = "l, d-M-y H:i:s T";
	const RFC1036 = "D, d M y H:i:s O";
	const RFC1123 = "D, d M Y H:i:s O";
	const RFC2822 = "D, d M Y H:i:s O";
	const RFC3339 = "Y-m-d\TH:i:sP";
	const RSS = "D, d M Y H:i:s O";
	const W3C = "Y-m-d\TH:i:sP";


	/**
	 * @param time[optional]
	 * @param object[optional]
	 */
	public function __construct ($time, $object) {}

	public function __wakeup () {}

	public static function __set_state () {}

	/**
	 * @param format
	 * @param time
	 * @param object[optional]
	 */
	public static function createFromFormat ($format, $time, $object) {}

	public static function getLastErrors () {}

	/**
	 * @param format
	 */
	public function format ($format) {}

	/**
	 * @param modify
	 */
	public function modify ($modify) {}

	/**
	 * @param interval
	 */
	public function add ($interval) {}

	/**
	 * @param interval
	 */
	public function sub ($interval) {}

	public function getTimezone () {}

	/**
	 * @param timezone
	 */
	public function setTimezone ($timezone) {}

	public function getOffset () {}

	/**
	 * @param hour
	 * @param minute
	 * @param second[optional]
	 */
	public function setTime ($hour, $minute, $second) {}

	/**
	 * @param year
	 * @param month
	 * @param day
	 */
	public function setDate ($year, $month, $day) {}

	/**
	 * @param year
	 * @param week
	 * @param day[optional]
	 */
	public function setISODate ($year, $week, $day) {}

	/**
	 * @param unixtimestamp
	 */
	public function setTimestamp ($unixtimestamp) {}

	public function getTimestamp () {}

	/**
	 * @param object
	 * @param absolute[optional]
	 */
	public function diff ($object, $absolute) {}

}

class DateTimeImmutable implements DateTimeInterface {

	/**
	 * @param time[optional]
	 * @param object[optional]
	 */
	public function __construct ($time, $object) {}

	public function __wakeup () {}

	public static function __set_state () {}

	/**
	 * @param format
	 * @param time
	 * @param object[optional]
	 */
	public static function createFromFormat ($format, $time, $object) {}

	public static function getLastErrors () {}

	/**
	 * @param format
	 */
	public function format ($format) {}

	public function getTimezone () {}

	public function getOffset () {}

	public function getTimestamp () {}

	/**
	 * @param object
	 * @param absolute[optional]
	 */
	public function diff ($object, $absolute) {}

	/**
	 * @param modify
	 */
	public function modify ($modify) {}

	/**
	 * @param interval
	 */
	public function add ($interval) {}

	/**
	 * @param interval
	 */
	public function sub ($interval) {}

	/**
	 * @param timezone
	 */
	public function setTimezone ($timezone) {}

	/**
	 * @param hour
	 * @param minute
	 * @param second[optional]
	 */
	public function setTime ($hour, $minute, $second) {}

	/**
	 * @param year
	 * @param month
	 * @param day
	 */
	public function setDate ($year, $month, $day) {}

	/**
	 * @param year
	 * @param week
	 * @param day[optional]
	 */
	public function setISODate ($year, $week, $day) {}

	/**
	 * @param unixtimestamp
	 */
	public function setTimestamp ($unixtimestamp) {}

	/**
	 * @param DateTime
	 */
	public static function createFromMutable ($DateTime) {}

}

class DateTimeZone  {
	const AFRICA = 1;
	const AMERICA = 2;
	const ANTARCTICA = 4;
	const ARCTIC = 8;
	const ASIA = 16;
	const ATLANTIC = 32;
	const AUSTRALIA = 64;
	const EUROPE = 128;
	const INDIAN = 256;
	const PACIFIC = 512;
	const UTC = 1024;
	const ALL = 2047;
	const ALL_WITH_BC = 4095;
	const PER_COUNTRY = 4096;


	/**
	 * @param timezone
	 */
	public function __construct ($timezone) {}

	public function __wakeup () {}

	public static function __set_state () {}

	public function getName () {}

	/**
	 * @param datetime
	 */
	public function getOffset ($datetime) {}

	/**
	 * @param timestamp_begin
	 * @param timestamp_end
	 */
	public function getTransitions ($timestamp_begin, $timestamp_end) {}

	public function getLocation () {}

	public static function listAbbreviations () {}

	/**
	 * @param what[optional]
	 * @param country[optional]
	 */
	public static function listIdentifiers ($what, $country) {}

}

class DateInterval  {

	/**
	 * @param interval_spec[optional]
	 */
	public function __construct ($interval_spec) {}

	public function __wakeup () {}

	public static function __set_state () {}

	/**
	 * @param format
	 */
	public function format ($format) {}

	/**
	 * @param time
	 */
	public static function createFromDateString ($time) {}

}

class DatePeriod implements Traversable {
	const EXCLUDE_START_DATE = 1;


	/**
	 * @param start
	 * @param interval
	 * @param end
	 */
	public function __construct ($start, $interval, $end) {}

	public function __wakeup () {}

	public static function __set_state () {}

}

/**
 * @param time
 * @param now[optional]
 */
function strtotime ($time, $now) {}

/**
 * @param format
 * @param timestamp[optional]
 */
function date ($format, $timestamp) {}

/**
 * @param format
 * @param timestamp[optional]
 */
function idate ($format, $timestamp) {}

/**
 * @param format
 * @param timestamp[optional]
 */
function gmdate ($format, $timestamp) {}

/**
 * @param hour[optional]
 * @param min[optional]
 * @param sec[optional]
 * @param mon[optional]
 * @param day[optional]
 * @param year[optional]
 */
function mktime ($hour, $min, $sec, $mon, $day, $year) {}

/**
 * @param hour[optional]
 * @param min[optional]
 * @param sec[optional]
 * @param mon[optional]
 * @param day[optional]
 * @param year[optional]
 */
function gmmktime ($hour, $min, $sec, $mon, $day, $year) {}

/**
 * @param month
 * @param day
 * @param year
 */
function checkdate ($month, $day, $year) {}

/**
 * @param format
 * @param timestamp[optional]
 */
function strftime ($format, $timestamp) {}

/**
 * @param format
 * @param timestamp[optional]
 */
function gmstrftime ($format, $timestamp) {}

function time () {}

/**
 * @param timestamp[optional]
 * @param associative_array[optional]
 */
function localtime ($timestamp, $associative_array) {}

/**
 * @param timestamp[optional]
 */
function getdate ($timestamp) {}

/**
 * @param time[optional]
 * @param object[optional]
 */
function date_create ($time, $object) {}

/**
 * @param time[optional]
 * @param object[optional]
 */
function date_create_immutable ($time, $object) {}

/**
 * @param format
 * @param time
 * @param object[optional]
 */
function date_create_from_format ($format, $time, $object) {}

/**
 * @param format
 * @param time
 * @param object[optional]
 */
function date_create_immutable_from_format ($format, $time, $object) {}

/**
 * @param date
 */
function date_parse ($date) {}

/**
 * @param format
 * @param date
 */
function date_parse_from_format ($format, $date) {}

function date_get_last_errors () {}

/**
 * @param object
 * @param format
 */
function date_format ($object, $format) {}

/**
 * @param object
 * @param modify
 */
function date_modify ($object, $modify) {}

/**
 * @param object
 * @param interval
 */
function date_add ($object, $interval) {}

/**
 * @param object
 * @param interval
 */
function date_sub ($object, $interval) {}

/**
 * @param object
 */
function date_timezone_get ($object) {}

/**
 * @param object
 * @param timezone
 */
function date_timezone_set ($object, $timezone) {}

/**
 * @param object
 */
function date_offset_get ($object) {}

/**
 * @param object
 * @param object2
 * @param absolute[optional]
 */
function date_diff ($object, $object2, $absolute) {}

/**
 * @param object
 * @param hour
 * @param minute
 * @param second[optional]
 */
function date_time_set ($object, $hour, $minute, $second) {}

/**
 * @param object
 * @param year
 * @param month
 * @param day
 */
function date_date_set ($object, $year, $month, $day) {}

/**
 * @param object
 * @param year
 * @param week
 * @param day[optional]
 */
function date_isodate_set ($object, $year, $week, $day) {}

/**
 * @param object
 * @param unixtimestamp
 */
function date_timestamp_set ($object, $unixtimestamp) {}

/**
 * @param object
 */
function date_timestamp_get ($object) {}

/**
 * @param timezone
 */
function timezone_open ($timezone) {}

/**
 * @param object
 */
function timezone_name_get ($object) {}

/**
 * @param abbr
 * @param gmtoffset[optional]
 * @param isdst[optional]
 */
function timezone_name_from_abbr ($abbr, $gmtoffset, $isdst) {}

/**
 * @param object
 * @param datetime
 */
function timezone_offset_get ($object, $datetime) {}

/**
 * @param object
 * @param timestamp_begin[optional]
 * @param timestamp_end[optional]
 */
function timezone_transitions_get ($object, $timestamp_begin, $timestamp_end) {}

/**
 * @param object
 */
function timezone_location_get ($object) {}

/**
 * @param what[optional]
 * @param country[optional]
 */
function timezone_identifiers_list ($what, $country) {}

function timezone_abbreviations_list () {}

function timezone_version_get () {}

/**
 * @param time
 */
function date_interval_create_from_date_string ($time) {}

/**
 * @param object
 * @param format
 */
function date_interval_format ($object, $format) {}

/**
 * @param timezone_identifier
 */
function date_default_timezone_set ($timezone_identifier) {}

function date_default_timezone_get () {}

/**
 * @param time
 * @param format[optional]
 * @param latitude[optional]
 * @param longitude[optional]
 * @param zenith[optional]
 * @param gmt_offset[optional]
 */
function date_sunrise ($time, $format, $latitude, $longitude, $zenith, $gmt_offset) {}

/**
 * @param time
 * @param format[optional]
 * @param latitude[optional]
 * @param longitude[optional]
 * @param zenith[optional]
 * @param gmt_offset[optional]
 */
function date_sunset ($time, $format, $latitude, $longitude, $zenith, $gmt_offset) {}

/**
 * @param time
 * @param latitude
 * @param longitude
 */
function date_sun_info ($time, $latitude, $longitude) {}

define ('DATE_ATOM', "Y-m-d\TH:i:sP");
define ('DATE_COOKIE', "l, d-M-Y H:i:s T");
define ('DATE_ISO8601', "Y-m-d\TH:i:sO");
define ('DATE_RFC822', "D, d M y H:i:s O");
define ('DATE_RFC850', "l, d-M-y H:i:s T");
define ('DATE_RFC1036', "D, d M y H:i:s O");
define ('DATE_RFC1123', "D, d M Y H:i:s O");
define ('DATE_RFC2822', "D, d M Y H:i:s O");
define ('DATE_RFC3339', "Y-m-d\TH:i:sP");
define ('DATE_RSS', "D, d M Y H:i:s O");
define ('DATE_W3C', "Y-m-d\TH:i:sP");

/**
 * Timestamp
 * @link http://www.php.net/manual/en/datetime.constants.php
 */
define ('SUNFUNCS_RET_TIMESTAMP', 0);

/**
 * Hours:minutes (example: 08:02)
 * @link http://www.php.net/manual/en/datetime.constants.php
 */
define ('SUNFUNCS_RET_STRING', 1);

/**
 * Hours as floating point number (example 8.75)
 * @link http://www.php.net/manual/en/datetime.constants.php
 */
define ('SUNFUNCS_RET_DOUBLE', 2);

// End of date v.5.6.0
?>
