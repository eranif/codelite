
/**
 * @brief The CameraManager interface provides access to any cameras available on the device being used.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraManager
 */
function CameraManager() {

	/**
	 * @brief This method is used to access to one of the cameras available on the device based on its identifier. You can get a list of the available cameras by calling the CameraManager.getListOfCameras() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraManager/getCamera
	 */
	this.getCamera() = '';

	/**
	 * @brief This method is used to get an Array of identifiers of all the cameras available on the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraManager/getListOfCameras
	 */
	this.getListOfCameras() = '';

	/**
	 * @brief This method is used to access to one of the cameras available on the device based on its identifier. You can get a list of the available cameras by calling the CameraManager.getListOfCameras() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraManager/getCamera
	 * @param camera - Specify the camera you want to use (the device may have more than one camera).
	 * @param cameraConfiguration - An object allowing you to set camera options for this camera: mode, previewSize and recorderProfile.
	 * @param onsuccess - A callback function that take a CameraControl object as parameter.
	 * @param onerror - An optional callback function that accepts an error string as a parameter.
	 */
	this.getCamera = function(camera, cameraConfiguration, onsuccess, onerror) {};

	/**
	 * @brief This method is used to get an Array of identifiers of all the cameras available on the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CameraManager/getListOfCameras
	 */
	this.getListOfCameras = function() {};

}

