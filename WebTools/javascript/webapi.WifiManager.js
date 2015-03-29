
/**
 * @brief The WifiManager API provides access to the wifi device capability.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager
 */
function WifiManager() {

	/**
	 * @brief The associate method is used to associate (and connect) a device with a given WiFi network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/associate
	 * @param network - A network object as the ones provided by the getNetworks or getKnownNetworks methods.
	 */
	this.associate = function(network) {};

	/**
	 * @brief The forget method is used to make a device forget about a given WiFi network. This will remove the network from the list of known networks and will remove all the configuration settings related to that network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/forget
	 * @param network - A network object as the ones provided by the getNetworks or getKnownNetworks methods.
	 */
	this.forget = function(network) {};

	/**
	 * @brief The getKnownNetworks method is used to retrieve the whole list of WiFi networks the device knows, regardless if they are available or not in the area surrounding the device. A known network is any network the device previously associated with by using the WifiManager.associate() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/getKnownNetworks
	 */
	this.getKnownNetworks = function() {};

	/**
	 * @brief The getNetworks method is used to retrieve the whole list of available WiFi networks surrounding the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/getNetworks
	 */
	this.getNetworks = function() {};

	/**
	 * @brief The setPowerSavingMode method is used to have the WiFi adapter enter or exit the power saving mode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/setPowerSavingMode
	 * @param enabled - A boolean indicating if the device must enter (true) or exit (false) the power saving mode.
	 */
	this.setPowerSavingMode = function(enabled) {};

	/**
	 * @brief The setStaticIpMod method is used to define a static or dynamic (if the network has a DHCP server) IP address for the device on a given network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/setStaticIpMode
	 * @param param - A configuration object with the following properties:
 
  enabled : A boolean requesting if the static IP mode must be turned on (true) or off (false). If it's turned off and the current WiFi network is DHCP enabled, the device will get a dynamic IP.
  ipaddr : A string representing the IP address of the device in the dotted quad format.
  proxy :  A string representing the proxy server address (if any, otherwise an empty string).
  maskLength : A number representing the length of the network mask.
  gateway : A string representing a gateway address (if any, otherwise an empty string).
  dns1 : A string representing the first DNS server address.
  dns2 : A string representing the second DNS server address.
	 */
	this.setStaticIpMode = function(param) {};

	/**
	 * @brief The wps method is used to handle a WPS connection with networks supporting that feature.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/WifiManager/wps
	 * @param param - A configuration object with the following properties:
 
  method: A string, one of the following:
   
    cancel to abort a WPS connection attempt.
    pbs to try a connection by pushing the physical button on the WiFi router.
    pin to try a connection with a pin number.
   
  
  bssid: A string representing the bssid of the network to connect to. It is mandatory if the method property is set to pin.
  pin: A string representing the pin number typed by the user. It is mandatory if the method property is set to pin and the user had to type the pin on his device.
	 */
	this.wps = function(param) {};

}

