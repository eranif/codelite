
/**
 * @brief An AudioContext can be a target of events, therefore it implements the EventTarget interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext
 */
function AudioContext() {

	/**
	 * @brief An AudioBuffer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createBuffer
	 * @param numOfChannels - An integer representing the number of channels this buffer should have. Implementations must support a minimum 32 channels.
	 * @param length - An integer representing the size of the buffer in sample-frames.
	 * @param sampleRate - The sample-rate of the linear audio data in sample-frames per second. An implementation must support sample-rates in at least the range 22050 to 96000.
	 */
	this.createBuffer = function(numOfChannels, length, sampleRate) {};

	/**
	 * @brief An AudioBufferSourceNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createBufferSource
	 */
	this.createBufferSource = function() {};

	/**
	 * @brief For more details about media element audio source nodes, check out the MediaElementAudioSourceNode reference page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createMediaElementSource
	 * @param myMediaElement - An HTMLMediaElement object that you want to feed into an audio processing graph to manipulate.
	 */
	this.createMediaElementSource = function(myMediaElement) {};

	/**
	 * @brief For more details about media stream audio source nodes, check out the MediaStreamAudioSourceNode reference page.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createMediaStreamSource
	 * @param stream - A MediaStream object that you want to feed into an audio processing graph to manipulate.
	 */
	this.createMediaStreamSource = function(stream) {};

	/**
	 * @brief The MediaStream is created when the node is created and is accessible via the MediaStreamAudioDestinationNode's stream attribute. This stream can be used in a similar way as a MediaStream obtained via navigator.getUserMedia — it can, for example, be sent to a remote peer using the RTCPeerConnection addStream() method.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createMediaStreamDestination
	 */
	this.createMediaStreamDestination = function() {};

	/**
	 * @brief A ScriptProcessorNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createScriptProcessor
	 * @param bufferSize - The buffer size in units of sample-frames. If specified, the bufferSize must be one of the following values: 256, 512, 1024, 2048, 4096, 8192, 16384. If it's not passed in, or if the value is 0, then the implementation will choose the best buffer size for the given environment, which will be a constant power of 2 throughout the lifetime of the node.
	 * @param numberOfInputChannels - Integer specifying the number of channels for this node's input, defaults to 2. Values of up to 32 are supported.
	 * @param numberOfOutputChannels - Integer specifying the number of channels for this node's output, defaults to 2. Values of up to 32 are supported.
	 */
	this.createScriptProcessor = function(bufferSize, numberOfInputChannels, numberOfOutputChannels) {};

	/**
	 * @brief A StereoPannerNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createStereoPanner
	 */
	this.createStereoPanner = function() {};

	/**
	 * @brief An AnalyserNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createAnalyser
	 */
	this.createAnalyser = function() {};

	/**
	 * @brief A BiquadFilterNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createBiquadFilter
	 */
	this.createBiquadFilter = function() {};

	/**
	 * @brief A ChannelMergerNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createChannelMerger
	 * @param numberOfInputs - The number of channels in the input audio streams, which the output stream will contain; the default is 6 is this parameter is not specified.
	 */
	this.createChannelMerger = function(numberOfInputs) {};

	/**
	 * @brief A ChannelSplitterNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createChannelSplitter
	 * @param numberOfOutputs - The number of channels in the input audio stream that you want to output separately; the default is 6 is this parameter is not specified.
	 */
	this.createChannelSplitter = function(numberOfOutputs) {};

	/**
	 * @brief A ConvolverNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createConvolver
	 */
	this.createConvolver = function() {};

	/**
	 * @brief A DelayNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createDelay
	 * @param maxDelayTime - The maximum amount of time, in seconds, that the audio signal can be delayed by. The default value is 0.
	 */
	this.createDelay = function(maxDelayTime) {};

	/**
	 * @brief Compression lowers the volume of the loudest parts of the signal and raises the volume of the softest parts. Overall, a louder, richer, and fuller sound can be achieved. It is especially important in games and musical applications where large numbers of individual sounds are played simultaneously, where you want to control the overall signal level and help avoid clipping (distorting) of the audio output.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createDynamicsCompressor
	 */
	this.createDynamicsCompressor = function() {};

	/**
	 * @brief This is the preferred method of creating an audio source for Web Audio API from an audio track.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/decodeAudioData
	 */
	this.decodeAudioData = function() {};

	/**
	 * @brief A GainNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createGain
	 */
	this.createGain = function() {};

	/**
	 * @brief An OscillatorNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createOscillator
	 */
	this.createOscillator = function() {};

	/**
	 * @brief The panner node is spatialized in relation to the AudioContext's AudioListener (defined by the AudioContext.listener attribute), which represents the position and orientation of the person listening to the audio.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createPanner
	 */
	this.createPanner = function() {};

	/**
	 * @brief A PeriodicWave.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createPeriodicWave
	 */
	this.createPeriodicWave = function() {};

	/**
	 * @brief A WaveShaperNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createWaveShaper
	 */
	this.createWaveShaper = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createAudioWorker
	 */
	this.createAudioWorker = function() {};

}

