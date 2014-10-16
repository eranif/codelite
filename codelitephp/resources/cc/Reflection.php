<?php

// Start of Reflection v.$Id: 8e5fcadef43ea0f77c7886557613923e30b979dd $

class ReflectionException extends Exception  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;


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

	public function __toString () {}

}

class Reflection  {

	/**
	 * @param modifiers
	 */
	public static function getModifierNames ($modifiers) {}

	/**
	 * @param reflector Reflector
	 * @param return[optional]
	 */
	public static function export (Reflector $reflector, $return) {}

}

interface Reflector  {

	abstract public static function export () ;

	abstract public function __toString () ;

}

class ReflectionFunctionAbstract implements Reflector {
	abstract public $name;


	final private function __clone () {}

	abstract public function __toString () {}

	public function inNamespace () {}

	public function isClosure () {}

	public function isDeprecated () {}

	public function isInternal () {}

	public function isUserDefined () {}

	public function isGenerator () {}

	public function isVariadic () {}

	public function getClosureThis () {}

	public function getClosureScopeClass () {}

	public function getDocComment () {}

	public function getEndLine () {}

	public function getExtension () {}

	public function getExtensionName () {}

	public function getFileName () {}

	public function getName () {}

	public function getNamespaceName () {}

	public function getNumberOfParameters () {}

	public function getNumberOfRequiredParameters () {}

	public function getParameters () {}

	public function getShortName () {}

	public function getStartLine () {}

	public function getStaticVariables () {}

	public function returnsReference () {}

}

class ReflectionFunction extends ReflectionFunctionAbstract implements Reflector {
	const IS_DEPRECATED = 262144;

	public $name;


	/**
	 * @param name
	 */
	public function __construct ($name) {}

	public function __toString () {}

	/**
	 * @param name
	 * @param return[optional]
	 */
	public static function export ($name, $return) {}

	public function isDisabled () {}

	/**
	 * @param args[optional]
	 */
	public function invoke ($args) {}

	/**
	 * @param args
	 */
	public function invokeArgs (array $args) {}

	public function getClosure () {}

	final private function __clone () {}

	public function inNamespace () {}

	public function isClosure () {}

	public function isDeprecated () {}

	public function isInternal () {}

	public function isUserDefined () {}

	public function isGenerator () {}

	public function isVariadic () {}

	public function getClosureThis () {}

	public function getClosureScopeClass () {}

	public function getDocComment () {}

	public function getEndLine () {}

	public function getExtension () {}

	public function getExtensionName () {}

	public function getFileName () {}

	public function getName () {}

	public function getNamespaceName () {}

	public function getNumberOfParameters () {}

	public function getNumberOfRequiredParameters () {}

	public function getParameters () {}

	public function getShortName () {}

	public function getStartLine () {}

	public function getStaticVariables () {}

	public function returnsReference () {}

}

class ReflectionParameter implements Reflector {
	public $name;


	final private function __clone () {}

	/**
	 * @param function
	 * @param parameter
	 * @param return[optional]
	 */
	public static function export ($function, $parameter, $return) {}

	/**
	 * @param function
	 * @param parameter
	 */
	public function __construct ($function, $parameter) {}

	public function __toString () {}

	public function getName () {}

	public function isPassedByReference () {}

	public function canBePassedByValue () {}

	public function getDeclaringFunction () {}

	public function getDeclaringClass () {}

	public function getClass () {}

	public function isArray () {}

	public function isCallable () {}

	public function allowsNull () {}

	public function getPosition () {}

	public function isOptional () {}

	public function isDefaultValueAvailable () {}

	public function getDefaultValue () {}

	public function isDefaultValueConstant () {}

	public function getDefaultValueConstantName () {}

	public function isVariadic () {}

}

class ReflectionMethod extends ReflectionFunctionAbstract implements Reflector {
	const IS_STATIC = 1;
	const IS_PUBLIC = 256;
	const IS_PROTECTED = 512;
	const IS_PRIVATE = 1024;
	const IS_ABSTRACT = 2;
	const IS_FINAL = 4;

	public $name;
	public $class;


	/**
	 * @param class
	 * @param name
	 * @param return[optional]
	 */
	public static function export ($class, $name, $return) {}

	/**
	 * @param class_or_method
	 * @param name[optional]
	 */
	public function __construct ($class_or_method, $name) {}

	public function __toString () {}

	public function isPublic () {}

	public function isPrivate () {}

	public function isProtected () {}

	public function isAbstract () {}

	public function isFinal () {}

	public function isStatic () {}

	public function isConstructor () {}

	public function isDestructor () {}

	/**
	 * @param object
	 */
	public function getClosure ($object) {}

	public function getModifiers () {}

	/**
	 * @param object
	 * @param args
	 */
	public function invoke ($object, $args) {}

	/**
	 * @param object
	 * @param args
	 */
	public function invokeArgs ($objectarray , $args) {}

	public function getDeclaringClass () {}

	public function getPrototype () {}

	/**
	 * @param value
	 */
	public function setAccessible ($value) {}

	final private function __clone () {}

	public function inNamespace () {}

	public function isClosure () {}

	public function isDeprecated () {}

	public function isInternal () {}

	public function isUserDefined () {}

	public function isGenerator () {}

	public function isVariadic () {}

	public function getClosureThis () {}

	public function getClosureScopeClass () {}

	public function getDocComment () {}

	public function getEndLine () {}

	public function getExtension () {}

	public function getExtensionName () {}

	public function getFileName () {}

	public function getName () {}

	public function getNamespaceName () {}

	public function getNumberOfParameters () {}

	public function getNumberOfRequiredParameters () {}

	public function getParameters () {}

	public function getShortName () {}

	public function getStartLine () {}

	public function getStaticVariables () {}

	public function returnsReference () {}

}

class ReflectionClass implements Reflector {
	const IS_IMPLICIT_ABSTRACT = 16;
	const IS_EXPLICIT_ABSTRACT = 32;
	const IS_FINAL = 64;

	public $name;


	final private function __clone () {}

	/**
	 * @param argument
	 * @param return[optional]
	 */
	public static function export ($argument, $return) {}

	/**
	 * @param argument
	 */
	public function __construct ($argument) {}

	public function __toString () {}

	public function getName () {}

	public function isInternal () {}

	public function isUserDefined () {}

	public function isInstantiable () {}

	public function isCloneable () {}

	public function getFileName () {}

	public function getStartLine () {}

	public function getEndLine () {}

	public function getDocComment () {}

	public function getConstructor () {}

	/**
	 * @param name
	 */
	public function hasMethod ($name) {}

	/**
	 * @param name
	 */
	public function getMethod ($name) {}

	/**
	 * @param filter[optional]
	 */
	public function getMethods ($filter) {}

	/**
	 * @param name
	 */
	public function hasProperty ($name) {}

	/**
	 * @param name
	 */
	public function getProperty ($name) {}

	/**
	 * @param filter[optional]
	 */
	public function getProperties ($filter) {}

	/**
	 * @param name
	 */
	public function hasConstant ($name) {}

	public function getConstants () {}

	/**
	 * @param name
	 */
	public function getConstant ($name) {}

	public function getInterfaces () {}

	public function getInterfaceNames () {}

	public function isInterface () {}

	public function getTraits () {}

	public function getTraitNames () {}

	public function getTraitAliases () {}

	public function isTrait () {}

	public function isAbstract () {}

	public function isFinal () {}

	public function getModifiers () {}

	/**
	 * @param object
	 */
	public function isInstance ($object) {}

	/**
	 * @param args
	 */
	public function newInstance ($args) {}

	public function newInstanceWithoutConstructor () {}

	/**
	 * @param args[optional]
	 */
	public function newInstanceArgs (array $args) {}

	public function getParentClass () {}

	/**
	 * @param class
	 */
	public function isSubclassOf ($class) {}

	public function getStaticProperties () {}

	/**
	 * @param name
	 * @param default[optional]
	 */
	public function getStaticPropertyValue ($name, $default) {}

	/**
	 * @param name
	 * @param value
	 */
	public function setStaticPropertyValue ($name, $value) {}

	public function getDefaultProperties () {}

	public function isIterateable () {}

	/**
	 * @param interface
	 */
	public function implementsInterface ($interface) {}

	public function getExtension () {}

	public function getExtensionName () {}

	public function inNamespace () {}

	public function getNamespaceName () {}

	public function getShortName () {}

}

class ReflectionObject extends ReflectionClass implements Reflector {
	const IS_IMPLICIT_ABSTRACT = 16;
	const IS_EXPLICIT_ABSTRACT = 32;
	const IS_FINAL = 64;

	public $name;


	/**
	 * @param argument
	 * @param return[optional]
	 */
	public static function export ($argument, $return) {}

	/**
	 * @param argument
	 */
	public function __construct ($argument) {}

	final private function __clone () {}

	public function __toString () {}

	public function getName () {}

	public function isInternal () {}

	public function isUserDefined () {}

	public function isInstantiable () {}

	public function isCloneable () {}

	public function getFileName () {}

	public function getStartLine () {}

	public function getEndLine () {}

	public function getDocComment () {}

	public function getConstructor () {}

	/**
	 * @param name
	 */
	public function hasMethod ($name) {}

	/**
	 * @param name
	 */
	public function getMethod ($name) {}

	/**
	 * @param filter[optional]
	 */
	public function getMethods ($filter) {}

	/**
	 * @param name
	 */
	public function hasProperty ($name) {}

	/**
	 * @param name
	 */
	public function getProperty ($name) {}

	/**
	 * @param filter[optional]
	 */
	public function getProperties ($filter) {}

	/**
	 * @param name
	 */
	public function hasConstant ($name) {}

	public function getConstants () {}

	/**
	 * @param name
	 */
	public function getConstant ($name) {}

	public function getInterfaces () {}

	public function getInterfaceNames () {}

	public function isInterface () {}

	public function getTraits () {}

	public function getTraitNames () {}

	public function getTraitAliases () {}

	public function isTrait () {}

	public function isAbstract () {}

	public function isFinal () {}

	public function getModifiers () {}

	/**
	 * @param object
	 */
	public function isInstance ($object) {}

	/**
	 * @param args
	 */
	public function newInstance ($args) {}

	public function newInstanceWithoutConstructor () {}

	/**
	 * @param args[optional]
	 */
	public function newInstanceArgs (array $args) {}

	public function getParentClass () {}

	/**
	 * @param class
	 */
	public function isSubclassOf ($class) {}

	public function getStaticProperties () {}

	/**
	 * @param name
	 * @param default[optional]
	 */
	public function getStaticPropertyValue ($name, $default) {}

	/**
	 * @param name
	 * @param value
	 */
	public function setStaticPropertyValue ($name, $value) {}

	public function getDefaultProperties () {}

	public function isIterateable () {}

	/**
	 * @param interface
	 */
	public function implementsInterface ($interface) {}

	public function getExtension () {}

	public function getExtensionName () {}

	public function inNamespace () {}

	public function getNamespaceName () {}

	public function getShortName () {}

}

class ReflectionProperty implements Reflector {
	const IS_STATIC = 1;
	const IS_PUBLIC = 256;
	const IS_PROTECTED = 512;
	const IS_PRIVATE = 1024;

	public $name;
	public $class;


	final private function __clone () {}

	/**
	 * @param class
	 * @param name
	 * @param return[optional]
	 */
	public static function export ($class, $name, $return) {}

	/**
	 * @param class
	 * @param name
	 */
	public function __construct ($class, $name) {}

	public function __toString () {}

	public function getName () {}

	/**
	 * @param object[optional]
	 */
	public function getValue ($object) {}

	/**
	 * @param object
	 * @param value[optional]
	 */
	public function setValue ($object, $value) {}

	public function isPublic () {}

	public function isPrivate () {}

	public function isProtected () {}

	public function isStatic () {}

	public function isDefault () {}

	public function getModifiers () {}

	public function getDeclaringClass () {}

	public function getDocComment () {}

	/**
	 * @param visible
	 */
	public function setAccessible ($visible) {}

}

class ReflectionExtension implements Reflector {
	public $name;


	final private function __clone () {}

	/**
	 * @param name
	 * @param return[optional]
	 */
	public static function export ($name, $return) {}

	/**
	 * @param name
	 */
	public function __construct ($name) {}

	public function __toString () {}

	public function getName () {}

	public function getVersion () {}

	public function getFunctions () {}

	public function getConstants () {}

	public function getINIEntries () {}

	public function getClasses () {}

	public function getClassNames () {}

	public function getDependencies () {}

	public function info () {}

	public function isPersistent () {}

	public function isTemporary () {}

}

class ReflectionZendExtension implements Reflector {
	public $name;


	final private function __clone () {}

	/**
	 * @param name
	 * @param return[optional]
	 */
	public static function export ($name, $return) {}

	/**
	 * @param name
	 */
	public function __construct ($name) {}

	public function __toString () {}

	public function getName () {}

	public function getVersion () {}

	public function getAuthor () {}

	public function getURL () {}

	public function getCopyright () {}

}
// End of Reflection v.$Id: 8e5fcadef43ea0f77c7886557613923e30b979dd $
?>
