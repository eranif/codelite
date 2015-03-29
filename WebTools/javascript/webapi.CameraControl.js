
/**
 * @brief When you use the CameraManager.getCamera() method to get a reference to a camera, you specify a callback function to be invoked on success. That function receives as a parameter a CameraControl object. You can use its methods and properties to manage and make use of the camera.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl
 */
function CameraControl() {

	/**
	 * @brief The capabilities property return a CameraCapabilities object, which describes the capabilities of the camera.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/capabilities
	 */
	this.effect = '';

	/**
	 * @brief The exposureCompensation property value is a number used to compensate the camera exposure.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/exposureCompensation
	 */
	this.flashMode = '';

	/**
	 * @brief The focalLength property value is a read-only property that returns a number that expresses the camera's focal length in millimeters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/focalLength
	 */
	this.focusAreas = '';

	/**
	 * @brief The focusDistanceFar property value is a distance in meters used with CameraControl.focusDistanceNear to define the image's depth of field. The value for this property may be Infinity.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/focusDistanceFar
	 */
	this.focusMode = '';

	/**
	 * @brief The meteringAreas property is an Array of one or more Area objects that define where the camera will perform light metering.
 If the array contains more objects than the number defined inside CameraCapabilities.maxMeteringAreas, extra objects will be ignored.
 If set to null, the camera will determine the light metering areas by itself.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/meteringAreas
	 */
	this.meteringAreas = '';

	/**
	 * @brief The onShutter property is used to set an event handler for the camera's &quot;shutter&quot; event, to trigger a shutter sound and/or a visual shutter indicator.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/onShutter
	 */
	this.onShutter = '';

	/**
	 * @brief The onClosed property is used to set an event handler that will track when a new CameraControl object in the same app takes over the camera.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/onClosed
	 */
	this.onClosed = '';

	/**
	 * @brief The onRecorderStateChange property is used to set an event handler to listen to the recorder state change. This can happen either because the recording process encountered an error, or because one of the recording limits set with CameraControl.startRecording() was reached.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/onRecorderStateChange
	 */
	this.onRecorderStateChange = '';

	/**
	 * @brief The sceneMode property value is a string that defines the quality of exposure to use while taking pictures. When set, its new value must be chosen from the list of options specified by  CameraCapabilities.sceneModes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/sceneMode
	 */
	this.sceneMode = '';

	/**
	 * @brief The whiteBalanceMode property value is a string that defines the white balance mode to use. When set, its new value must be chosen from the list of options specified by  CameraCapabilities.whiteBalanceModes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/whiteBalanceMode
	 */
	this.whiteBalanceMode = '';

	/**
	 * @brief The zoom property value is a number that defines the zoom factor that is to be used when taking photographs. When set, its new value must be chosen from the list of options specified by  CameraCapabilities.zoomRatios. If the new value is not one of those possible values, the new value is rounded to the nearest supported value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/zoom
	 */
	this.zoom = '';

	/**
	 * @brief This method attempts to focus the camera. If the camera is able to attempt to focus, a success callback is issued, regardless of whether or not the focusing attempt succeeds. If unable to attempt to focus, an error callback is performed instead.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/autoFocus
	 * @param onsuccess - A callback function called when a focus attempt is made
	 * @param onerror Optional - An optional callback function that accepts an error string as an input parameter; this is called if it's not possible to attempt to focus the camera.
	 */
	this.autoFocus = function(onsuccess, onerror Optional) {};

	/**
	 * @brief This method is used to configure and access a MediaStream from the camera. The resulting stream is ready for you to use to capture still photos.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/getPreviewStream
	 * @param options - An object containing two properties: width and height. This object must be equal to one of the objects available through CameraCapabilities.previewSizes
	 * @param onsuccess - A callback function that accepts one parameter. This parameter is a MediaStream object, ready for use in capturing still images.
	 * @param onerror Optional - A callback function that accepts an error string as parameter; it's called if an error occurs while attempting to obtain the MediaStream.
	 */
	this.getPreviewStream = function(options, onsuccess, onerror Optional) {};

	/**
	 * @brief This method is used to configure and access a MediaStream from the camera. You use the resulting MediaStream object to record video.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/getPreviewStreamVideoMode
	 * @param options - An object defining the video mode to use. This object must be equal to one of the objects available through CameraCapabilities.videoSizes
	 * @param onsuccess - A callback function that will accept one parameter. This parameter is a MediaStream object.
	 * @param onerror Optional - A callback function that accepts an error string as parameter. This is called if an error occurs while attempting to get the MediaStream.
	 */
	this.getPreviewStreamVideoMode = function(options, onsuccess, onerror Optional) {};

	/**
	 * @brief Releases the camera so that other applications can use it. You should call this whenever the camera is not actively being used by your application.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/release
	 * @param onsuccess Optional - A callback function which is called once the hardware has actually been released.
	 * @param onerror Optional - A callback function that accepts an error string; this is called if an error occurs while releasing the camera.
	 */
	this.release = function(onsuccess Optional, onerror Optional) {};

	/**
	 * @brief This method is used to resume the preview stream.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/resumePreview
	 */
	this.resumePreview = function() {};

	/**
	 * @brief This method is used to change the exposure compensation value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/setExposureCompensation
	 * @param compensation Optional - The new value for the exposure compensation. This parameter is optional; if it's missing, the camera will use automatic exposure compensation.
	 */
	this.setExposureCompensation = function(compensation Optional) {};

	/**
	 * @brief This method is used to start recording a video file on the device; the video is stored as a 3gp file.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/startRecording
	 * @param options - An object specifying options for the video capture operation; see Options below.
	 * @param storage - An object of type DeviceStorage that defines where to store the output file.
	 * @param filename - The name of the output file.
	 * @param onsuccess - A callback function that is called once the recording has began.
	 * @param onerror Optional - A callback function that accepts an error string as parameter.
	 */
	this.startRecording = function(options, storage, filename, onsuccess, onerror Optional) {};

	/**
	 * @brief Stops recording video on the device; you should call this at some point after calling CameraControl.startRecording().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/stopRecording
	 */
	this.stopRecording = function() {};

	/**
	 * @brief Captures a still image from the camera and passes it as a Blob to a success callback handler, which can manipulate the image as you wish (for example, by manipulating its pixel data or storing it).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraControl/takePicture
	 * @param options - An object specifying options for the photo capture operation; see Options below.
	 * @param onsuccess - A callback function to be invoked once the image has been captured. The function receives one parameter: a Blob containing a JPEG formatted image.
	 * @param onerror Optional - An optional callback function that's invoked if an error occurs; it receives one parameter: an error string.
	 */
	this.takePicture = function(options, onsuccess, onerror Optional) {};

}

