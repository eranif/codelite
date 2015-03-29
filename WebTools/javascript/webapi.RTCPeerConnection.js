
/**
 * @brief The RTCPeerConnection interface represents a WebRTC connection and handles efficient streaming of data between two peers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection
 */
function RTCPeerConnection() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/RTCPeerConnection
	 */
	this.TCPeerConnection = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/createOffer
	 */
	this.createOffer = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/createAnswer
	 */
	this.createAnswer = function() {};

	/**
	 * @brief The RTCPeerConnection.setLocalDescription() method changes the local description associated with the connection. The description defines the properties of the connection like its codec. The connection is affected by this change and must be able to support both old and new descriptions. The method takes three parameters, a RTCSessionDescription object to set, and two callbacks, one called if the change of description succeeds, another called if it failed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/setLocalDescription
	 * @param sessionDescription - Is a RTCSessionDescription is the description of the parameters to be applied to the local session.
	 * @param successCallback - Is a Function without parameter which will be called when the description has been successfully set. At this point, one can send the offer to a remote server that can forward it to a remote client
	 * @param errorCallback - Is a RTCPeerConnectionErrorCallback which will be called if the description can't be set. It takes the following parameter:
 
  errorInformation which is a DOMString describing the reason why the description has not been set.
	 */
	this.setLocalDescription = function(sessionDescription, successCallback, errorCallback) {};

	/**
	 * @brief The RTCPeerConnection.setRemoteDescription() method changes the remote description associated with the connection. The description defines the properties of the connection like its codec. The connection is affected by this change and must be able to support both old and new descriptions. The method takes three parameters, a RTCSessionDescription object to set, and two callbacks, one called if the change of description succeeds, another called if it failed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/setRemoteDescription
	 * @param sessionDescription - Is a DOMString is the description of the parameters to be applied to the remote session.
	 * @param successCallback - Is a Function without parameter which will be called when the description has been successfully set. At this point, one can send the offer to a remote server that can forward it to a remote client
	 * @param errorCallback - Is a RTCPeerConnectionErrorCallback which will be called if the description can't be set. It takes the following parameter:
 
  errorInformation which is a DOMString describing the reason why the description has not been set.
	 */
	this.setRemoteDescription = function(sessionDescription, successCallback, errorCallback) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/updateIce
	 */
	this.updateIce = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/addIceCandidate
	 */
	this.addIceCandidate = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/getConfiguration
	 */
	this.getConfiguration = function() {};

	/**
	 * @brief The RTCPeerConnection.getLocalStreams() method returns an array of MediaStream associated with the local end of the connection. The array may be empty.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/getLocalStreams
	 */
	this.getLocalStreams = function() {};

	/**
	 * @brief The RTCPeerConnection.getRemoteStreams() method returns an array of MediaStream associated with the remote end of the connection. The array may be empty.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/getRemoteStreams
	 */
	this.getRemoteStreams = function() {};

	/**
	 * @brief The RTCPeerConnection.getStreamById() method returns the MediaStream with the given id that is associated with local or remote end of the connection. If no stream matches, it returns null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/getStreamById
	 * @param id - Is a DOMString corresponding to the stream to return.
	 */
	this.getStreamById = function(id) {};

	/**
	 * @brief The RTCPeerConnection.addStream() method adds a MediaStream as a local source of audio or video. If the negotiation already happened, a new one will be needed for the remote peer to be able to use it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/addStream
	 * @param mediaStream - Is a MediaStream is the stream to add.
	 */
	this.addStream = function(mediaStream) {};

	/**
	 * @brief The RTCPeerConnection.removeStream() method removes a MediaStream as a local source of audio or video. If the negotiation already happened, a new one will be needed for the remote peer to be able to use it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/removeStream
	 * @param mediaStream - Is a MediaStream is the stream to remove.
	 */
	this.removeStream = function(mediaStream) {};

	/**
	 * @brief The RTCPeerConnection.close() method closes the current peer connection. This should be called when you're ending your session.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/close
	 */
	this.close = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/createDataChannel
	 */
	this.createDataChannel = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/createDTMFSender
	 */
	this.createDTMFSender = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/getStats
	 */
	this.getStats = function() {};

	/**
	 * @brief The RTCPeerConnection.setIdentityProvider() method sets the Identity Provider (IdP) to the triplet given in parameter: its name, the protocol used to communicate with it (optional) and an optional username. The IdP will be used only when an assertion is needed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/setIdentityProvider
	 * @param domainname - Is a DOMString is the domain name where the IdP is.
	 * @param protocol Optional - Is a DOMString representing the protocol used to communicate with the IdP. It defaults to "default" and is used to determine the URL where the IdP is listening.
	 * @param username Optional - Is a DOMString representing the username associated with the IdP.
	 */
	this.setIdentityProvider = function(domainname, protocol Optional, username Optional) {};

}

