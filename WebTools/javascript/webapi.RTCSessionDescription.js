
/**
 * @brief The RTCSessionDescription interface represents the parameters of a session. It consists of the kind of description, which part of a offer/answer negotiation it describes and of the SDP descriptor of the session.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCSessionDescription
 */
function RTCSessionDescription() {

	/**
	 * @brief The property RTCSessionDescription.type is an enum of type RTCSdpType describing the type of the description.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCSessionDescription/type
	 */
	this.type = '';

	/**
	 * @brief The RTCSessionDescription() constructor creates a new RTCSessionDescription.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCSessionDescription/RTCSessionDescription
	 */
	this.TCSessionDescription = function() {};

	/**
	 * @brief The RTCPeerConnection.toJSON() method generates a JSON description of the object. Both properties, type and sdp, are contained in the generated JSON.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/RTCSessionDescription/toJSON
	 */
	this.toJSON = function() {};

}

