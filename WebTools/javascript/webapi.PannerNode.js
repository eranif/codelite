
/**
 * @brief A PannerNode always has exactly one input and one output: the input can be mono or stereo but the output is always stereo (2 channels) — you need stereo sound for panning effects!.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode
 */
function PannerNode() {

	/**
	 * @brief A enum — see PanningModelType.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/panningModel
	 */
	this.panningModel = '';

	/**
	 * @brief A enum — see DistanceModelType.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/distanceModel
	 */
	this.distanceModel = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/refDistance
	 */
	this.refDistance = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/maxDistance
	 */
	this.maxDistance = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/rolloffFactor
	 */
	this.rolloffFactor = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/coneInnerAngle
	 */
	this.coneInnerAngle = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/coneOuterAngle
	 */
	this.coneOuterAngle = '';

	/**
	 * @brief A double.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/coneOuterGain
	 */
	this.coneOuterGain = '';

	/**
	 * @brief Void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/PannerNode/setPosition
	 */
	this.setPosition = function() {};

}

