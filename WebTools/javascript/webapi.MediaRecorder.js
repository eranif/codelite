
/**
 * @brief The MediaRecorder interface of the MediaRecorder API provides functionality to easily capture media. It is created by the invocation of the MediaRecorder() constructor.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder
 */
function MediaRecorder() {

	/**
	 * @brief The Media.pause() method (part of the MediaRecorder API) is used to pause recording of media streams.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder/pause
	 */
	this.pause = function() {};

	/**
	 * @brief The Media.requestData() method (part of the MediaRecorder API) is used to raise a dataavailable event containing a Blob object of the captured media as it was when the method was called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder/requestData
	 */
	this.requestData = function() {};

	/**
	 * @brief The Media.resume() method (part of the MediaRecorder API) is used to resume media recording when it has been previously paused.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder/resume
	 */
	this.resume = function() {};

	/**
	 * @brief The Media.start() method (part of the MediaRecorder API) is used to start capturing media into a Blob.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder/start
	 * @param timeslice Optional - This parameter takes a value of milliseconds, and represents the length of media capture to return in each Blob. If it is not specified, all media captured will be returned in a single Blob, unless one or more calls are made to MediaRecorder.requestData.
	 */
	this.start = function(timeslice Optional) {};

	/**
	 * @brief The Media.stop() method (part of the MediaRecorder API) is used to stop media capture.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder/stop
	 */
	this.stop = function() {};

}

