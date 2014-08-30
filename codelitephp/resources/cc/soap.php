<?php

// Start of soap v.

class SoapClient  {

	/**
	 * SoapClient constructor
	 * @link http://www.php.net/manual/en/soapclient.soapclient.php
	 * @param wsdl
	 * @param options[optional]
	 */
	public function SoapClient ($wsdl, $options) {}

	/**
	 * Calls a SOAP function (deprecated)
	 * @link http://www.php.net/manual/en/soapclient.call.php
	 * @param function_name string 
	 * @param arguments string 
	 * @return mixed 
	 */
	public function __call ($function_name, $arguments) {}

	/**
	 * Calls a SOAP function
	 * @link http://www.php.net/manual/en/soapclient.soapcall.php
	 * @param function_name string <p>
	 *       The name of the SOAP function to call.
	 *      </p>
	 * @param arguments array <p>
	 *       An array of the arguments to pass to the function. This can be either
	 *       an ordered or an associative array.
	 *      </p>
	 * @param options array[optional] <p>
	 *       An associative array of options to pass to the client.
	 *      </p>
	 *      <p>
	 *       The location option is the URL of the remote Web service.
	 *      </p>
	 *      <p>
	 *       The uri option is the target namespace of the SOAP service.
	 *      </p>
	 *      <p>
	 *        The soapaction option is the action to call.
	 *      </p>
	 * @param input_headers mixed[optional] <p>
	 *       An array of headers to be sent along with the SOAP request.
	 *      </p>
	 * @param output_headers array[optional] <p>
	 *       If supplied, this array will be filled with the headers from the SOAP response.
	 *      </p>
	 * @return mixed SOAP functions may return one, or multiple values. If only one value is returned
	 *   by the SOAP function, the return value of __soapCall will be
	 *   a simple value (e.g. an integer, a string, etc). If multiple values are
	 *   returned, __soapCall will return
	 *   an associative array of named output parameters.
	 *  </p>
	 *  <p>
	 *   On error, if the SoapClient object was constructed with the trace
	 *   option set to false, a SoapFault object will be returned.
	 */
	public function __soapCall ($function_name, array $arguments, array $options = null, $input_headers = null, array &$output_headers = null) {}

	/**
	 * Returns last SOAP request
	 * @link http://www.php.net/manual/en/soapclient.getlastrequest.php
	 * @return string The last SOAP request, as an XML string.
	 */
	public function __getLastRequest () {}

	/**
	 * Returns last SOAP response
	 * @link http://www.php.net/manual/en/soapclient.getlastresponse.php
	 * @return string The last SOAP response, as an XML string.
	 */
	public function __getLastResponse () {}

	/**
	 * Returns the SOAP headers from the last request
	 * @link http://www.php.net/manual/en/soapclient.getlastrequestheaders.php
	 * @return string The last SOAP request headers.
	 */
	public function __getLastRequestHeaders () {}

	/**
	 * Returns the SOAP headers from the last response
	 * @link http://www.php.net/manual/en/soapclient.getlastresponseheaders.php
	 * @return string The last SOAP response headers.
	 */
	public function __getLastResponseHeaders () {}

	/**
	 * Returns list of available SOAP functions
	 * @link http://www.php.net/manual/en/soapclient.getfunctions.php
	 * @return array The array of SOAP function prototypes, detailing the return type,
	 *   the function name and type-hinted paramaters.
	 */
	public function __getFunctions () {}

	/**
	 * Returns a list of SOAP types
	 * @link http://www.php.net/manual/en/soapclient.gettypes.php
	 * @return array The array of SOAP types, detailing all structures and types.
	 */
	public function __getTypes () {}

	/**
	 * Performs a SOAP request
	 * @link http://www.php.net/manual/en/soapclient.dorequest.php
	 * @param request string <p>
	 *       The XML SOAP request.
	 *      </p>
	 * @param location string <p>
	 *       The URL to request.
	 *      </p>
	 * @param action string <p>
	 *       The SOAP action.
	 *      </p>
	 * @param version int <p>
	 *       The SOAP version.
	 *      </p>
	 * @param one_way int[optional] <p>
	 *       If one_way is set to 1, this method returns nothing. 
	 *       Use this where a response is not expected.
	 *      </p>
	 * @return string The XML SOAP response.
	 */
	public function __doRequest ($request, $location, $action, $version, $one_way = null) {}

	/**
	 * The __setCookie purpose
	 * @link http://www.php.net/manual/en/soapclient.setcookie.php
	 * @param name string <p>
	 *       The name of the cookie.
	 *      </p>
	 * @param value string[optional] <p>
	 *       The value of the cookie. If not specified, the cookie will be deleted.
	 *      </p>
	 * @return void 
	 */
	public function __setCookie ($name, $value = null) {}

	/**
	 * Sets the location of the Web service to use
	 * @link http://www.php.net/manual/en/soapclient.setlocation.php
	 * @param new_location string[optional] <p>
	 *       The new endpoint URL.
	 *      </p>
	 * @return string The old endpoint URL.
	 */
	public function __setLocation ($new_location = null) {}

	/**
	 * Sets SOAP headers for subsequent calls
	 * @link http://www.php.net/manual/en/soapclient.setsoapheaders.php
	 * @param soapheaders mixed[optional] <p>
	 *       The headers to be set. It could be SoapHeader
	 *       object or array of SoapHeader objects.
	 *       If not specified or set to &null;, the headers will be deleted.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function __setSoapHeaders ($soapheaders = null) {}

}

class SoapVar  {

	/**
	 * SoapVar constructor
	 * @link http://www.php.net/manual/en/soapvar.soapvar.php
	 * @param data
	 * @param encoding
	 * @param type_name[optional]
	 * @param type_namespace[optional]
	 * @param node_name[optional]
	 * @param node_namespace[optional]
	 */
	public function SoapVar ($data, $encoding, $type_name, $type_namespace, $node_name, $node_namespace) {}

}

class SoapServer  {

	/**
	 * SoapServer constructor
	 * @link http://www.php.net/manual/en/soapserver.soapserver.php
	 * @param wsdl
	 * @param options[optional]
	 */
	public function SoapServer ($wsdl, $options) {}

	/**
	 * Sets SoapServer persistence mode
	 * @link http://www.php.net/manual/en/soapserver.setpersistence.php
	 * @param mode int <p>
	 *       One of the SOAP_PERSISTENCE_XXX constants.
	 *      </p>
	 *      <p>
	 *       SOAP_PERSISTENCE_REQUEST - persist the object for the duration of a request.
	 *      </p>
	 *      <p>
	 *       SOAP_PERSISTENCE_SESSION - persist the object for the duration of a session.
	 *      </p>
	 * @return void 
	 */
	public function setPersistence ($mode) {}

	/**
	 * Sets the class which handles SOAP requests
	 * @link http://www.php.net/manual/en/soapserver.setclass.php
	 * @param class_name string <p>
	 *       The name of the exported class.
	 *      </p>
	 * @param args mixed[optional] <p>
	 *       These optional parameters will be passed to the default class constructor
	 *       during object creation. 
	 *      </p>
	 * @param _ mixed[optional] 
	 * @return void 
	 */
	public function setClass ($class_name, $args = null, $_ = null) {}

	/**
	 * Sets the object which will be used to handle SOAP requests
	 * @link http://www.php.net/manual/en/soapserver.setobject.php
	 * @param object object <p>
	 *       The object to handle the requests.
	 *      </p>
	 * @return void 
	 */
	public function setObject ($object) {}

	/**
	 * Adds one or more functions to handle SOAP requests
	 * @link http://www.php.net/manual/en/soapserver.addfunction.php
	 * @param functions mixed <p>
	 *       To export one function, pass the function name into this parameter as
	 *       a string.
	 *      </p>
	 *      <p>
	 *       To export several functions, pass an array of function names.
	 *      </p>
	 *      <p>
	 *       To export all the functions, pass a special constant SOAP_FUNCTIONS_ALL.
	 *      </p>
	 *      
	 *       <p>
	 *        functions must receive all input arguments in the same
	 *        order as defined in the WSDL file (They should not receive any output parameters
	 *        as arguments) and return one or more values. To return several values they must
	 *        return an array with named output parameters.
	 *       </p>
	 * @return void 
	 */
	public function addFunction ($functions) {}

	/**
	 * Returns list of defined functions
	 * @link http://www.php.net/manual/en/soapserver.getfunctions.php
	 * @return array An array of the defined functions.
	 */
	public function getFunctions () {}

	/**
	 * Handles a SOAP request
	 * @link http://www.php.net/manual/en/soapserver.handle.php
	 * @param soap_request string[optional] <p>
	 *       The SOAP request. If this argument is omitted, the request is assumed to be
	 *       in the raw POST data of the HTTP request.
	 *      </p>
	 * @return void 
	 */
	public function handle ($soap_request = null) {}

	/**
	 * Issue SoapServer fault indicating an error
	 * @link http://www.php.net/manual/en/soapserver.fault.php
	 * @param code string <p>
	 *       The error code to return
	 *      </p>
	 * @param string string <p>
	 *       A brief description of the error
	 *      </p>
	 * @param actor string[optional] <p>
	 *       A string identifying the actor that caused the fault.
	 *      </p>
	 * @param details string[optional] <p>
	 *       More details of the fault
	 *      </p>
	 * @param name string[optional] <p>
	 *       The name of the fault. This can be used to select a name from a WSDL file.
	 *      </p>
	 * @return void 
	 */
	public function fault ($code, $string, $actor = null, $details = null, $name = null) {}

	/**
	 * Add a SOAP header to the response
	 * @link http://www.php.net/manual/en/soapserver.addsoapheader.php
	 * @param object SoapHeader <p>
	 *       The header to be returned.
	 *      </p>
	 * @return void 
	 */
	public function addSoapHeader (SoapHeader $object) {}

}

class SoapFault extends Exception  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;


	/**
	 * SoapFault constructor
	 * @link http://www.php.net/manual/en/soapfault.soapfault.php
	 * @param faultcode
	 * @param faultstring
	 * @param faultactor[optional]
	 * @param detail[optional]
	 * @param faultname[optional]
	 * @param headerfault[optional]
	 */
	public function SoapFault ($faultcode, $faultstring, $faultactor, $detail, $faultname, $headerfault) {}

	/**
	 * Obtain a string representation of a SoapFault
	 * @link http://www.php.net/manual/en/soapfault.tostring.php
	 * @return string A string describing the SoapFault.
	 */
	public function __toString () {}

	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

}

class SoapParam  {

	/**
	 * SoapParam constructor
	 * @link http://www.php.net/manual/en/soapparam.soapparam.php
	 * @param data
	 * @param name
	 */
	public function SoapParam ($data, $name) {}

}

class SoapHeader  {

	/**
	 * SoapHeader constructor
	 * @link http://www.php.net/manual/en/soapheader.soapheader.php
	 * @param namespace
	 * @param name
	 * @param data[optional]
	 * @param mustunderstand[optional]
	 * @param actor[optional]
	 */
	public function SoapHeader ($namespace, $name, $data, $mustunderstand, $actor) {}

}

/**
 * Set whether to use the SOAP error handler
 * @link http://www.php.net/manual/en/function.use-soap-error-handler.php
 * @param handler bool[optional] <p>
 *       Set to true to send error details to clients.
 *      </p>
 * @return bool the original value.
 */
function use_soap_error_handler ($handler = null) {}

/**
 * Checks if a SOAP call has failed
 * @link http://www.php.net/manual/en/function.is-soap-fault.php
 * @param object mixed <p>
 *       The object to test.
 *      </p>
 * @return bool This will return true on error, and false otherwise.
 */
function is_soap_fault ($object) {}

define ('SOAP_1_1', 1);
define ('SOAP_1_2', 2);
define ('SOAP_PERSISTENCE_SESSION', 1);
define ('SOAP_PERSISTENCE_REQUEST', 2);
define ('SOAP_FUNCTIONS_ALL', 999);
define ('SOAP_ENCODED', 1);
define ('SOAP_LITERAL', 2);
define ('SOAP_RPC', 1);
define ('SOAP_DOCUMENT', 2);
define ('SOAP_ACTOR_NEXT', 1);
define ('SOAP_ACTOR_NONE', 2);
define ('SOAP_ACTOR_UNLIMATERECEIVER', 3);
define ('SOAP_COMPRESSION_ACCEPT', 32);
define ('SOAP_COMPRESSION_GZIP', 0);
define ('SOAP_COMPRESSION_DEFLATE', 16);
define ('SOAP_AUTHENTICATION_BASIC', 0);
define ('SOAP_AUTHENTICATION_DIGEST', 1);
define ('UNKNOWN_TYPE', 999998);
define ('XSD_STRING', 101);
define ('XSD_BOOLEAN', 102);
define ('XSD_DECIMAL', 103);
define ('XSD_FLOAT', 104);
define ('XSD_DOUBLE', 105);
define ('XSD_DURATION', 106);
define ('XSD_DATETIME', 107);
define ('XSD_TIME', 108);
define ('XSD_DATE', 109);
define ('XSD_GYEARMONTH', 110);
define ('XSD_GYEAR', 111);
define ('XSD_GMONTHDAY', 112);
define ('XSD_GDAY', 113);
define ('XSD_GMONTH', 114);
define ('XSD_HEXBINARY', 115);
define ('XSD_BASE64BINARY', 116);
define ('XSD_ANYURI', 117);
define ('XSD_QNAME', 118);
define ('XSD_NOTATION', 119);
define ('XSD_NORMALIZEDSTRING', 120);
define ('XSD_TOKEN', 121);
define ('XSD_LANGUAGE', 122);
define ('XSD_NMTOKEN', 123);
define ('XSD_NAME', 124);
define ('XSD_NCNAME', 125);
define ('XSD_ID', 126);
define ('XSD_IDREF', 127);
define ('XSD_IDREFS', 128);
define ('XSD_ENTITY', 129);
define ('XSD_ENTITIES', 130);
define ('XSD_INTEGER', 131);
define ('XSD_NONPOSITIVEINTEGER', 132);
define ('XSD_NEGATIVEINTEGER', 133);
define ('XSD_LONG', 134);
define ('XSD_INT', 135);
define ('XSD_SHORT', 136);
define ('XSD_BYTE', 137);
define ('XSD_NONNEGATIVEINTEGER', 138);
define ('XSD_UNSIGNEDLONG', 139);
define ('XSD_UNSIGNEDINT', 140);
define ('XSD_UNSIGNEDSHORT', 141);
define ('XSD_UNSIGNEDBYTE', 142);
define ('XSD_POSITIVEINTEGER', 143);
define ('XSD_NMTOKENS', 144);
define ('XSD_ANYTYPE', 145);
define ('XSD_ANYXML', 147);
define ('APACHE_MAP', 200);
define ('SOAP_ENC_OBJECT', 301);
define ('SOAP_ENC_ARRAY', 300);
define ('XSD_1999_TIMEINSTANT', 401);
define ('XSD_NAMESPACE', "http://www.w3.org/2001/XMLSchema");
define ('XSD_1999_NAMESPACE', "http://www.w3.org/1999/XMLSchema");
define ('SOAP_SINGLE_ELEMENT_ARRAYS', 1);
define ('SOAP_WAIT_ONE_WAY_CALLS', 2);
define ('SOAP_USE_XSI_ARRAY_TYPE', 4);
define ('WSDL_CACHE_NONE', 0);
define ('WSDL_CACHE_DISK', 1);
define ('WSDL_CACHE_MEMORY', 2);
define ('WSDL_CACHE_BOTH', 3);

// End of soap v.
?>
