<?php

// Start of Zend Job Queue v.4.0

class ZendJobQueue  {
	const TYPE_HTTP_RELATIVE = 0;
	const TYPE_HTTP = 1;
	const TYPE_SHELL = 2;
	const JOB_TYPE_HTTP_RELATIVE = 1;
	const JOB_TYPE_HTTP = 2;
	const JOB_TYPE_SHELL = 4;
	const PRIORITY_LOW = 0;
	const PRIORITY_NORMAL = 1;
	const PRIORITY_HIGH = 2;
	const PRIORITY_URGENT = 3;
	const JOB_PRIORITY_LOW = 1;
	const JOB_PRIORITY_NORMAL = 2;
	const JOB_PRIORITY_HIGH = 4;
	const JOB_PRIORITY_URGENT = 8;
	const STATUS_PENDING = 0;
	const STATUS_WAITING_PREDECESSOR = 1;
	const STATUS_RUNNING = 2;
	const STATUS_COMPLETED = 3;
	const STATUS_OK = 4;
	const STATUS_FAILED = 5;
	const STATUS_LOGICALLY_FAILED = 6;
	const STATUS_TIMEOUT = 7;
	const STATUS_REMOVED = 8;
	const STATUS_SCHEDULED = 9;
	const STATUS_SUSPENDED = 10;
	const JOB_STATUS_PENDING = 1;
	const JOB_STATUS_WAITING_PREDECESSOR = 2;
	const JOB_STATUS_RUNNING = 4;
	const JOB_STATUS_COMPLETED = 8;
	const JOB_STATUS_OK = 16;
	const JOB_STATUS_FAILED = 32;
	const JOB_STATUS_LOGICALLY_FAILED = 64;
	const JOB_STATUS_TIMEOUT = 128;
	const JOB_STATUS_REMOVED = 256;
	const JOB_STATUS_SCHEDULED = 512;
	const JOB_STATUS_SUSPENDED = 1024;
	const SORT_NONE = 0;
	const SORT_BY_ID = 1;
	const SORT_BY_TYPE = 2;
	const SORT_BY_SCRIPT = 3;
	const SORT_BY_APPLICATION = 4;
	const SORT_BY_NAME = 5;
	const SORT_BY_PRIORITY = 6;
	const SORT_BY_STATUS = 7;
	const SORT_BY_PREDECESSOR = 8;
	const SORT_BY_PERSISTENCE = 9;
	const SORT_BY_CREATION_TIME = 10;
	const SORT_BY_SCHEDULE_TIME = 11;
	const SORT_BY_START_TIME = 12;
	const SORT_BY_END_TIME = 13;
	const SORT_ASC = 0;
	const SORT_DESC = 1;
	const OK = 0;
	const FAILED = 1;


	public function ZendJobQueue () {}

	public function createHttpJob () {}

	public function getJobStatus () {}

	public function removeJob () {}

	public function restartJob () {}

	public function isSuspended () {}

	public function suspendQueue () {}

	public function resumeQueue () {}

	public function getStatistics () {}

	public function getConfig () {}

	public function reloadConfig () {}

	public function getJobInfo () {}

	public function getDependentJobs () {}

	/**
	 * @param filter
	 * @param total[optional]
	 */
	public function getJobsList ($filter, &$total) {}

	public function getApplications () {}

	public function getSchedulingRules () {}

	public function getSchedulingRule () {}

	public function deleteSchedulingRule () {}

	public function suspendSchedulingRule () {}

	public function resumeSchedulingRule () {}

	public function updateSchedulingRule () {}

	public static function getCurrentJobParams () {}

	public static function setCurrentJobStatus () {}

	public static function getCurrentJobId () {}

	public static function isJobQueueDaemonRunning () {}

}

class ZendJobQueueException extends Exception  {
	const UNKNOWN = -1;
	const INVALID_PARAM = 0;
	const SERVER_ERR = 1;
	const SERVER_BUSY = 2;
	const NETWORK_ERR = 3;
	const TIMEOUT = 4;
	const LICENSE = 5;

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
// End of Zend Job Queue v.4.0
?>
