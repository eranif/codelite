
/**
 * @brief The HTMLCanvasElement interface provides properties and methods for manipulating the layout and presentation of canvas elements. The HTMLCanvasElement interface also inherits the properties and methods of the HTMLElement interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement
 */
function HTMLCanvasElement() {

	/**
	 * @brief The HTMLCanvasElement.height property is a positive integer reflecting the height HTML attribute of the &lt;canvas> element interpreted in CSS pixels. When the attribute is not specified, or if it is set to an invalid value, like a negative, the default value of 150 is used.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/height
	 */
	this.height = '';

	/**
	 * @brief The HTMLCanvasElement.mozOpaque property is a Boolean reflecting the moz-opaque HTML attribute of the &lt;canvas> element. It lets the canvas know whether or not translucency will be a factor. If the canvas knows there's no translucency, painting performance can be optimized.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/mozOpaque
	 */
	this.width = '';

	/**
	 * @brief The HTMLCanvasElement.getContext() method returns a drawing context on the canvas, or null if the context identifier is not supported.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/getContext
	 * @param contextType - Is a DOMString containing the context identifier defining the drawing context associated to the canvas. Possible values are:
 
  "2d", leading to the creation of a CanvasRenderingContext2D object representing a two-dimensional rendering context.
  "webgl" (or "experimental-webgl") which will create a WebGLRenderingContext object representing a three-dimensional rendering context. This context is only available on browsers that implement WebGL version 1 (OpenGL ES 2.0).
  "webgl2" (or "experimental-webgl2") which will create a WebGL2RenderingContext object representing a three-dimensional rendering context. This context is only available on browsers that implement WebGL version 2 (OpenGL ES 3.0).  
 

 Note: The identifiers "experimental-webgl" or "experimental-webgl2" are used in new implementations of WebGL. These implementation have not reached test suite conformance or the graphic drivers situation on the platform is not yet stable. The Khronos Group certifies WebGL implementations under certain conformance rules.
	 * @param contextAttributes - You can use several context attributes when creating your rendering context, for example:

 canvas.getContext("webgl", 
                 { antialias: false,
                   depth: false });
 2d context attributes:

 
  alpha: Boolean that indicates if the canvas contains an alpha channel. If set to false, the browser now knows that the backdrop is always opaque, which can speed up drawing of transparent content and images then.
    (Gecko only) willReadFrequently: Boolean that indicates whether or not a lot of read-back operations are planned. This will force the use of a software (instead of hardware accelerated) 2D canvas and can save memory when calling getImageData() frequently. This option is only available, if the flag gfx.canvas.willReadFrequently.enable is set to true (which, by default, is only the case for B2G/Firefox OS).
    (Blink only) storage: String that indicates which storage is used ("persistent" by default).
 
 WebGL context attributes:

 
  alpha: Boolean that indicates if the canvas contains an alpha buffer.
  depth: Boolean that indicates that the drawing buffer has a depth buffer of at least 16 bits.
  stencil: Boolean that indicates that the drawing buffer has a stencil buffer of at least 8 bits.
  antialias: Boolean that indicates whether or not to perform anti-aliasing.
  premultipliedAlpha: Boolean that indicates that the page compositor will assume the drawing buffer contains colors with pre-multiplied alpha.
  preserveDrawingBuffer: If the value is true the buffers will not be cleared and will preserve their values until cleared or overwritten by the author.
	 */
	this.getContext = function(contextType, contextAttributes) {};

	/**
	 * @brief The HTMLCanvasElement.toDataURL() method returns a data URIs containing a representation of the image in the format specified by the type parameter (defaults to PNG). The returned image is in a resolution of 96 dpi.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/toDataURL
	 * @param type Optional - A DOMString indicating the image format. The default type is image/png.
	 * @param encoderOptions Optional - A Number between 0 and 1 indicating image quality if the requested type is image/jpeg or image/webp.
 If this argument is anything else, the default value for image quality is used. Other arguments are ignored.
	 */
	this.toDataURL = function(type Optional, encoderOptions Optional) {};

	/**
	 * @brief The HTMLCanvasElement.toBlob() method creates a Blob object representing the image contained in the canvas; this file may be cached on the disk or stored in memory at the discretion of the user agent. If type is not specified, the image type is image/png. The created image is in a resolution of 96dpi.
 The third argument is used with image/jpeg images to specify the quality of the output.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/toBlob
	 * @param callback - A callback function with the resulting Blob object as a single argument.
	 * @param type Optional - A DOMString indicating the image format. The default type is image/png.
	 * @param encoderOptions Optional - A Number between 0 and 1 indicating image quality if the requested type is image/jpeg or image/webp. If this argument is anything else, the default value for image quality is used. Other arguments are ignored.
	 */
	this.toBlob = function(callback, type Optional, encoderOptions Optional) {};

	/**
	 * @brief The HTMLCanvasElement.mozGetAsFille() method returns a File object representing the image contained in the canvas; this file is a memory-based file, with the specified name. If type is not specified, the image type is image/png.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/mozGetAsFile
	 * @param name - A DOMString indicating the file name.
	 * @param type Optional - A DOMString indicating the image format. The default type is image/png.
	 */
	this.mozGetAsFile = function(name, type Optional) {};

	/**
	 * @brief The HTMLCanvasElement.mozFetchAsStream() method creates a new input stream that, when ready, will provide the contents of the canvas as image data. When the new stream is ready, the specified callback's nsIInputStreamCallback.onInputStreamReady() method is called. If type is not specified, the image type is image/png.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/mozFetchAsStream
	 * @param callback - An nsIInputStreamCallback.
	 * @param type Optional - A DOMString indicating the image format. The default type is image/png.
	 */
	this.mozFetchAsStream = function(callback, type Optional) {};

}

