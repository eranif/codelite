
/**
 * @brief The BluetoothAdapter API is used to handle all the operations requested by Bluetooth networks. A Bluetooth adapter is the physical interface which is used to interact with local Bluetooth device. Currently only one BluetoothAdapter object is supported on a device, which can be created from BluetoothManager.getDefaultAdapter(). In the whole Bluetooth API, it's the most important interface because it is used to manage all the interactions between local Bluetooth device and remote Bluetooth devices.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter
 */
function BluetoothAdapter() {

	/**
	 * @brief The answerWaitingCall method is used to allow HFP devices to answer a pending incoming call on a CDMA network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/answerWaitingCall
	 */
	this.answerWaitingCall = function() {};

	/**
	 * @brief The confirmReceivingFile method is used to accept or reject the incoming file transfer request received from the remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/confirmReceivingFile
	 * @param deviceAddress - The Bluetooth address of the device to receive a file from.
	 * @param flag - A boolean indicating if the incoming file transfer request is accepted(true) or rejected(false).
	 */
	this.confirmReceivingFile = function(deviceAddress, flag) {};

	/**
	 * @brief The connect method is used to connect the device's adapter to a specific service of a remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/connect
	 * @param device - A BluetoothDevice object representing the remote device to connect with.
	 * @param service Optional - A number representing the UUIDs of the service to connect with, as defined in the Bluetooth specification. If that parameter is undefined, the connection will try to connect to all the possible and compatible services available on the remote device.
	 */
	this.connect = function(device, service Optional) {};

	/**
	 * @brief The connectSco method is used to allow the device to enable the SCO connection to broadcast and listen to audio to/from the remote devices it is connected to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/connectSco
	 */
	this.connectSco = function() {};

	/**
	 * @brief The disconnect method is used to disconnect the device's adapter from a specific service of a remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/disconnect
	 * @param device - A BluetoothDevice object representing the remote device to disconnect from.
	 * @param service Optional - A number representing the UUIDs of the service to disconnect from, as defined in the Bluetooth specification. If that parameter is undefined, the connection will be closed for all the services currently connected to the remote device.
	 */
	this.disconnect = function(device, service Optional) {};

	/**
	 * @brief The disconnectSco method is used to allow the device to disable the SCO connection with the remote devices it is connected to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/disconnectSco
	 */
	this.disconnectSco = function() {};

	/**
	 * @brief The getConnectedDevices method is used to retrieve the full list of all devices connected with a specific service of local adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/getConnectedDevices
	 * @param serviceUuid - A number representing a UUID of Bluetooth service.
	 */
	this.getConnectedDevices = function(serviceUuid) {};

	/**
	 * @brief The getPairedDevices method is used to retrieve the full list of all devices paired with the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/getPairedDevices
	 */
	this.getPairedDevices = function() {};

	/**
	 * @brief The ignoreWaitingCall method is used to allow HFP devices to dismiss a pending incoming call on a CDMA network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/ignoreWaitingCall
	 */
	this.ignoreWaitingCall = function() {};

	/**
	 * @brief The isConnected method is used to check if a device with a given Bluetooth profile is connected.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/isConnected
	 * @param serviceUuid - A number representing a UUID of Bluetooth service.
	 */
	this.isConnected = function(serviceUuid) {};

	/**
	 * @brief The isScoConnected method is used to allow to know if there is a SCO connection between local and the remote Bluetooth device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/isScoConnected
	 */
	this.isScoConnected = function() {};

	/**
	 * @brief The pair method is used to start pairing a remote device with the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/pair
	 * @param deviceAddress - The Bluetooth micro-network address of the device to pair with.
	 */
	this.pair = function(deviceAddress) {};

	/**
	 * @brief Technical review completed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/sendFile
	 * @param deviceAddress - The Bluetooth micro-network address of the device to send a file to.
	 * @param file - A Blob object representing the file to send.
	 */
	this.sendFile = function(deviceAddress, file) {};

	/**
	 * @brief The sendMediaMetaData method is used to send metadata about media to remote devices connected with the AVRCP 1.3 profile.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/sendMediaMetaData
	 * @param metadata - A configuration object with the following properties:
 
  title           : A string giving the title of the media
  artist          : A string giving the name of the artist
  album           : A string giving the name of the album the media is coming from
  mediaNumber     : A number representing the track number of the media
  totalMediaCount : A number representing the total number of tracks in the album
  duration        : A number representing the playing time of the media in milliseconds
	 */
	this.sendMediaMetaData = function(metadata) {};

	/**
	 * @brief The sendMediaPlayStatus method is used to send the play status of the device to remote devices connected with the AVRCP 1.3 profile.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/sendMediaPlayStatus
	 * @param metadata - A configuration object with the following properties:
 
  duration   : A number representing the current track length in milliseconds
  position   : A number representing the current playing time in milliseconds
  playStatus : A string representing the play status, one of: STOPPED, PLAYING, PAUSED, FWD_SEEK, REV_SEEK, or ERROR
	 */
	this.sendMediaPlayStatus = function(metadata) {};

	/**
	 * @brief The setDiscoverable method is used to change the value of the discoverable property for the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setDiscoverable
	 * @param flag - A boolean indicating if the device is discoverable (true) or not (false).
	 */
	this.setDiscoverable = function(flag) {};

	/**
	 * @brief The setDiscoverableTimeout method is used to change the value of the discoverableTimeout property for the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setDiscoverableTimeout
	 * @param duration - A number indicating how many seconds the device can remain discoverable.
	 */
	this.setDiscoverableTimeout = function(duration) {};

	/**
	 * @brief The setName method is used to change the value of the name property for the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setName
	 * @param name - A string representing the new name to set.
	 */
	this.setName = function(name) {};

	/**
	 * @brief The setPairingConfirmation method is used to send back the pairing confirmation when the device's adapter tries to pair itself with a remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setPairingConfirmation
	 * @param deviceAddress - The Bluetooth micro-network address of the device for which the confirmation is required.
	 * @param flag - A boolean indicating if the pairing is confirmed (true) or not (false).
	 */
	this.setPairingConfirmation = function(deviceAddress, flag) {};

	/**
	 * @brief The setPasskey method is used to send back the requested Passkey code when the device's adapter tries to pair itself with a remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setPasskey
	 * @param deviceAddress - The Bluetooth micro-network address of the device for which the confirmation is required.
	 * @param key - A number representing the Passkey code set by the user.
	 */
	this.setPasskey = function(deviceAddress, key) {};

	/**
	 * @brief The setPinCode method is used to send back the requested PIN code when the device's adapter tries to pair itself with a remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/setPinCode
	 * @param deviceAddress - The Bluetooth micro-network address of the device for which the confirmation is required.
	 * @param code - A string representing the PIN code set by the user.
	 */
	this.setPinCode = function(deviceAddress, code) {};

	/**
	 * @brief The startDiscovery method is used to have the device's adapter start seeking for remote devices.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/startDiscovery
	 */
	this.startDiscovery = function() {};

	/**
	 * @brief The stopDiscovery method is used to have the device's adapter stop seeking for remote devices.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/stopDiscovery
	 */
	this.stopDiscovery = function() {};

	/**
	 * @brief The stopSendingFile method is used to abort sending a file to a given remote device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/stopSendingFile
	 * @param deviceAddress - The Bluetooth micro-network address of the device a file is sent to.
	 */
	this.stopSendingFile = function(deviceAddress) {};

	/**
	 * @brief The toggleCalls method is used to allow HFP devices to switch from one call to another on a CDMA network.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/toggleCalls
	 */
	this.toggleCalls = function() {};

	/**
	 * @brief The unpair method is used to remove the paired device from the paired device list of the device's adapter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BluetoothAdapter/unpair
	 * @param deviceAddress - The Bluetooth micro-network address of the device to unpair with.
	 */
	this.unpair = function(deviceAddress) {};

}

