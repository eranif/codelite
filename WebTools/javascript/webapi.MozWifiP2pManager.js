
/**
 * @brief The MozWifiP2pManager is an interface that allows to control Wi-Fi connection with other computers using Wi-Fi Direct.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager
 */
function MozWifiP2pManager() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/setScanEnabled
	 */
	this.setScanEnabled = function() {};

	/**
	 * @brief The MozWifiP2pManager.connect() method binds, with a given configuration, to a peer identified by its MAC address and returns a DOMRequest that will report if the connect request has been issued successfully or not (but not about its result).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/connect
	 * @param address - Is a DOMString containing the MAC Address of the peer to connect to.
	 * @param wps - Is a keyword containing the WPS method to use. The following values are valid:
 
  "pbc" (push-button connection), the connection is activated by a negotiation between both devices, without the user interacting anymore.
  "display", the device display a PIN on its display and expect it to be entered on the peer before activating the connection.
  "keypad", the connection is activated by entering a PIN code on the keypad. It is the symmetric of "display".
	 * @param intent - Is a positive number between 0 and 15, included, that indicates if the local host wants to be the group owner. A value of 0 means that the device don't want to be the group owner, a value of 15 that he wants to be it. The device with the higher value wins.
	 */
	this.connect = function(address, wps, intent) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/disconnect
	 */
	this.disconnect = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/getPeerList
	 */
	this.getPeerList = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/setPairingConfirmation
	 */
	this.setPairingConfirmation = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozWifiP2pManager/setDeviceName
	 */
	this.setDeviceName = function() {};

}

