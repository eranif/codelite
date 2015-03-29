
/**
 * @brief This API is used to get information about the current mobile voice and data connection states of the device. It is accessible through navigator.mozMobileConnections, which returns an array of MozMobileConnection objects.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection
 */
function MozMobileConnection() {

	/**
	 * @brief The voice is a read-only property that gives access to information about the voice connection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/voice
	 */
	this.oncfstatechange = '';

	/**
	 * @brief The ondatachange property specifies an event listener to receive datachange events. These events occur whenever the MozMobileConnection.data connection object changes values.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/ondatachange
	 */
	this.ondatachange = '';

	/**
	 * @brief The ondataerror property specifies an event listener to receive dataerror events. These events occur whenever the MozMobileConnection.data connection object receives an error from the RIL.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/ondataerror
	 */
	this.ondataerror = '';

	/**
	 * @brief The onussdreceived property specifies an event listener to receive ussdreceived events. These events occur whenever a new USSD message is received.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/onussdreceived
	 */
	this.onussdreceived = '';

	/**
	 * @brief The onvoicechange property specifies an event listener to receive voicechange events. These events occur whenever the MozMobileConnection.voice connection object changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/onvoicechange
	 */
	this.onvoicechange = '';

	/**
	 * @brief The cancelMMI method cancels the current MMI request if one exists.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/cancelMMI
	 */
	this.cancelMMI = function() {};

	/**
	 * @brief The getCallForwardingOption method is used to query current options for call forwarding.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/getCallForwardingOption
	 * @param reason - This parameter is a number that indicates the reason the call is being forwarded. It is one of:
 
  0: unconditional
  1: mobile busy
  2: no reply
  3: not reachable
  4: forwarding all calls
  5: conditionally forwarding all calls
 
 All these values are available as constants in the MozMobileCFInfo interface.
	 */
	this.getCallForwardingOption = function(reason) {};

	/**
	 * @brief The getNetworks method is used to search for available networks.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/getNetworks
	 */
	this.getNetworks = function() {};

	/**
	 * @brief The selectNetwork method is used to manually select a network, overriding the radio's current selection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/selectNetwork
	 */
	this.selectNetwork = function() {};

	/**
	 * @brief The selectNetworkAutomatically method is used to tell the radio to automatically select a network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/selectNetworkAutomatically
	 */
	this.selectNetworkAutomatically = function() {};

	/**
	 * @brief The sendMMI method is used to send an MMI code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/sendMMI
	 * @param message - A string representing an MMI code that can be associated with a USSD request or other RIL functionality.
	 */
	this.sendMMI = function(message) {};

	/**
	 * @brief The setCallForwardingOption method is used to configure call forward options.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileConnection/setCallForwardingOption
	 * @param options - An object containing the call forward rule to set. This object expects the same properties as those defined in the MozMobileCFInfo interface except for the active property which has no meaning when setting such a call forward rule.
	 */
	this.setCallForwardingOption = function(options) {};

}

