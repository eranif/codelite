
/**
 * @brief The MozNetworkStatsManager interface provides methods and properties to monitor data usage.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager
 */
function MozNetworkStatsManager() {

	/**
	 * @brief A DOMRequest object represents an ongoing operation. It provides callbacks that are called when the operation completes, as well as a reference to the operation's result. A DOM method that initiates an ongoing operation may return a DOMRequest object that you can use to monitor the progress of that operation.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/addAlarm" title='The addAlarm method installs an alarm on a network. When total data usage reaches threshold bytes, a "networkstats-alarm" system message is sent to the application, where the optional parameter data must be a cloneable object.'><code>MozNetworkStatsManager.addAlarm()</code></a></dt>
 <dd>Allows to install an alarm on a network. Returns a <a href=
	 */
	this.OMRequest

	/**
	 * @brief The clearStats method removes all stats related to the provided network from DB.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/clearStats
	 */
	this.clearStats = function() {};

	/**
	 * @brief The getAllAlarms method obtains all alarms for those networks returned by MozNetworkStatsManager.getAvailableNetworks. If a network is provided, it only retrieves the alarms for that network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/getAllAlarms
	 */
	this.getAllAlarms = function() {};

	/**
	 * @brief The getAvailableNetworks method returns an Array of available networks that used to be saved in the database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/getAvailableNetworks
	 */
	this.getAvailableNetworks = function() {};

	/**
	 * @brief The getAvailableServiceTypes method returns an Array of available service types that used to be saved in the database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/getAvailableServiceTypes
	 */
	this.getAvailableServiceTypes = function() {};

	/**
	 * @brief The getSamples method asynchronously queries network interface statistics. The query may be filtered by connection type and date.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/getSamples
	 */
	this.getSamples = function() {};

	/**
	 * @brief The removeAlarms method removes all network alarms. If an alarmId is provided, then only that alarm is removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozNetworkStatsManager/removeAlarms
	 */
	this.removeAlarms = function() {};

}

