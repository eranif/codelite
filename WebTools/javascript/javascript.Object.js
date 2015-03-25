
/**
 * @brief The Object constructor creates an object wrapper.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object
 */
function Object() {

	/**
	 * @brief The __count__ property used to store the count of enumerable properties on the object, but it has been removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/count
	 */
	this.__count__ = '';

	/**
	 * @brief The __noSuchMethod__ property references a function to be executed when a non-existent method is called on an object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/noSuchMethod
	 */
	this.__noSuchMethod__ = '';

	/**
	 * @brief The __parent__ property used to point to an object's context, but it has been removed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/Parent
	 */
	this.__parent__ = '';

	/**
	 * @brief The __proto__ property of Object.prototype is an accessor property (a getter function and a setter function) that exposes the internal [[Prototype]] (either an object or null) of the object through which it is accessed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/proto
	 */
	this.__proto__ = '';

	/**
	 * @brief Returns a reference to the Object function that created the instance's prototype. Note that the value of this property is a reference to the function itself, not a string containing the function's name. The value is only read-only for primitive values such as 1, true and &quot;test&quot;.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/constructor
	 */
	this.constructor = '';

	/**
	 * @brief The Object.assign() method is used to copy the values of all enumerable own properties from one or more source objects to a target object. It will return the target object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/assign
	 * @param target - The target object.
	 * @param sources - The source object(s).
	 */
	this.assign = function(target, sources) {};

	/**
	 * @brief The Object.create() method creates a new object with the specified prototype object and properties.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/create
	 * @param proto - The object which should be the prototype of the newly-created object.
	 * @param propertiesObject - Optional. If specified and not undefined, an object whose enumerable own properties (that is, those properties defined upon itself and not enumerable properties along its prototype chain) specify property descriptors to be added to the newly-created object, with the corresponding property names. These properties correspond to the second argument of Object.defineProperties().
	 */
	this.create = function(proto, propertiesObject) {};

	/**
	 * @brief The Object.defineProperties() method defines new or modifies existing properties directly on an object, returning the object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/defineProperties
	 * @param obj - The object on which to define or modify properties.
	 * @param props - An object whose own enumerable properties constitute descriptors for the properties to be defined or modified. Properties have the following optional keys:         configurable       true if and only if the type of this property descriptor may be changed and if the property may be deleted from the corresponding object.    Defaults to false.
	 * @param enumerable - true if and only if this property shows up during enumeration of the properties on the corresponding object.    Defaults to false.
	 * @param value - The value associated with the property. Can be any valid JavaScript value (number, object, function, etc).    Defaults to undefined.
	 * @param writable - true if and only if the value associated with the property may be changed with an assignment operator.    Defaults to false.
	 * @param get - A function which serves as a getter for the property, or undefined if there is no getter. The function return will be used as the value of property.    Defaults to undefined.
	 * @param set - A function which serves as a setter for the property, or undefined if there is no setter. The function will receive as only argument the new value being assigned to the property.    Defaults to undefined.
	 */
	this.defineProperties = function(obj, props, enumerable, value, writable, get, set) {};

	/**
	 * @brief The Object.defineProperty() method defines a new property directly on an object, or modifies an existing property on an object, and returns the object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/defineProperty
	 * @param obj - The object on which to define the property.
	 * @param prop - The name of the property to be defined or modified.
	 * @param descriptor - The descriptor for the property being defined or modified.
	 */
	this.defineProperty = function(obj, prop, descriptor) {};

	/**
	 * @brief The Object.freeze() method freezes an object: that is, prevents new properties from being added to it; prevents existing properties from being removed; and prevents existing properties, or their enumerability, configurability, or writability, from being changed. In essence the object is made effectively immutable. The method returns the object being frozen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/freeze
	 * @param obj - The object to freeze.
	 */
	this.freeze = function(obj) {};

	/**
	 * @brief The Object.getOwnPropertyDescriptor() method returns a property descriptor for an own property (that is, one directly present on an object, not present by dint of being along an object's prototype chain) of a given object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/getOwnPropertyDescriptor
	 * @param obj - The object in which to look for the property.
	 * @param prop - The name of the property whose description is to be retrieved.
	 */
	this.getOwnPropertyDescriptor = function(obj, prop) {};

	/**
	 * @brief The Object.getOwnPropertyNames() method returns an array of all properties (enumerable or not) found directly upon a given object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/getOwnPropertyNames
	 * @param obj - The object whose enumerable and non-enumerable own properties are to be returned.
	 */
	this.getOwnPropertyNames = function(obj) {};

	/**
	 * @brief The Object.getOwnPropertySymbols() method returns an array of all symbol properties found directly upon a given object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/getOwnPropertySymbols
	 * @param obj - The object whose symbol properties are to be returned.
	 */
	this.getOwnPropertySymbols = function(obj) {};

	/**
	 * @brief The Object.getPrototypeOf() method returns the prototype (i.e. the value of the internal [[Prototype]] property) of the specified object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/getPrototypeOf
	 * @param obj - The object whose prototype is to be returned.
	 */
	this.getPrototypeOf = function(obj) {};

	/**
	 * @brief The Object.is() method determines whether two values are the same value.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/is
	 * @param value1 - The first value to compare.
	 * @param value2 - The second value to compare.
	 */
	this.is = function(value1, value2) {};

	/**
	 * @brief The Object.isExtensible() method determines if an object is extensible (whether it can have new properties added to it).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/isExtensible
	 * @param obj - The object which should be checked.
	 */
	this.isExtensible = function(obj) {};

	/**
	 * @brief The Object.isFrozen() determines if an object is frozen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/isFrozen
	 * @param obj - The object which should be checked.
	 */
	this.isFrozen = function(obj) {};

	/**
	 * @brief The Object.isSealed() method determines if an object is sealed.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/isSealed
	 * @param obj - The object which should be checked.
	 */
	this.isSealed = function(obj) {};

	/**
	 * @brief The Object.keys() method returns an array of a given object's own enumerable properties, in the same order as that provided by a for...in loop (the difference being that a for-in loop enumerates properties in the prototype chain as well).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/keys
	 * @param obj - The object whose enumerable own properties are to be returned.
	 */
	this.keys = function(obj) {};

	/**
	 * @brief The Object.observe() method is used for asynchronously observing the changes to an object. It provides a stream of changes in the order in which they occur.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/observe
	 * @param obj - The object to be observed.
	 * @param callback - The function called each time changes are made, with the following argument:   changes  An array of objects each representing a change. The properties of these change objects are:     name: The name of the property which was changed.   object: The changed object after the change was made.   type: A string indicating the type of change taking place. One of "add", "update", or "delete".   oldValue: Only for "update" and "delete" types. The value before the change.
	 * @param acceptList - The list of types of changes to be observed on the given object for the given callback. If omitted, the array ["add", "update", "delete", "reconfigure", "setPrototype", "preventExtensions"] will be used.
	 */
	this.observe = function(obj, callback, acceptList) {};

	/**
	 * @brief The Object.preventExtensions() method prevents new properties from ever being added to an object (i.e. prevents future extensions to the object).
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/preventExtensions
	 * @param obj - The object which should be made non-extensible.
	 */
	this.preventExtensions = function(obj) {};

	/**
	 * @brief The Object.seal() method seals an object, preventing new properties from being added to it and marking all existing properties as non-configurable. Values of present properties can still be changed as long as they are writable.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/seal
	 * @param obj - The object which should be sealed.
	 */
	this.seal = function(obj) {};

	/**
	 * @brief The Object.setPrototype() method sets the prototype (i.e., the internal [[Prototype]] property) of a specified object to another object or null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/setPrototypeOf
	 * @param obj - The object which is to have its prototype set.
	 * @param prototype - The object's new prototype (an object or null).
	 */
	this.setPrototypeOf = function(obj, prototype) {};

}

/**
 * @brief The __defineGetter__ method binds an object's property to a function to be called when that property is looked up.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/__defineGetter__
 * @param prop - A string containing the name of the property to bind to the given function.
 * @param func - A function to be bound to a lookup of the specified property.
 */
Object.prototype.__defineGetter__ = function(prop, func) {};

/**
 * @brief The __defineSetter__ method binds an object's property to a function to be called when an attempt is made to set that property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/__defineSetter__
 * @param prop - A string containing the name of the property to be bound to the given function.
 * @param fun - A function to be called when there is an attempt to set the specified property. This function takes the form  function(val) { . . . }         val       An alias for the variable that holds the value attempted to be assigned to prop.
 */
Object.prototype.__defineSetter__ = function(prop, fun) {};

/**
 * @brief The __lookupGetter__ method returns the function bound as a getter to the specified property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/__lookupGetter__
 * @param sprop - A string containing the name of the property whose getter should be returned.
 */
Object.prototype.__lookupGetter__ = function(sprop) {};

/**
 * @brief The __lookupSetter__ method returns the function bound as a setter to the specified property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/__lookupSetter__
 * @param sprop - A string containing the name of the property whose setter should be returned.
 */
Object.prototype.__lookupSetter__ = function(sprop) {};

/**
 * @brief The Object.eval() method used to evaluate a string of JavaScript code in the context of an object, however, this method has been removed.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/eval
 * @param string - Any string representing a JavaScript expression, statement, or sequence of statements. The expression can include variables and properties of existing objects.
 */
Object.prototype.eval = function(string) {};

/**
 * @brief The hasOwnProperty() method returns a boolean indicating whether the object has the specified property.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/hasOwnProperty
 * @param prop - The name of the property to test.
 */
Object.prototype.hasOwnProperty = function(prop) {};

/**
 * @brief The isPrototypeOf() method tests for an object in another object's prototype chain.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/isPrototypeOf
 * @param prototypeObj - An object to be tested against each link in the prototype chain of the object argument.
 * @param object - The object whose prototype chain will be searched.
 */
Object.prototype.isPrototypeOf = function(prototypeObj, object) {};

/**
 * @brief The propertyIsEnumerable() method returns a Boolean indicating whether the specified property is enumerable.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/propertyIsEnumerable
 * @param prop - The name of the property to test.
 */
Object.prototype.propertyIsEnumerable = function(prop) {};

/**
 * @brief The toLocaleString() method returns a string representing the object. This method is meant to be overriden by derived objects for locale-specific purposes.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/toLocaleString
 */
Object.prototype.toLocaleString = function() {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/toSource
 */
Object.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/toString
 */
Object.prototype.toString = function() {};

/**
 * @brief The unwatch() method removes a watchpoint set with the watch() method.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/unwatch
 * @param prop - The name of a property of the object to stop watching.
 */
Object.prototype.unwatch = function(prop) {};

/**
 * @brief The valueOf() method returns the primitive value of the specified object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/valueOf
 */
Object.prototype.valueOf = function() {};

/**
 * @brief The watch() method watches for a property to be assigned a value and runs a function when that occurs.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/watch
 * @param prop - The name of a property of the object on which you wish to monitor changes.
 * @param handler - A function to call when the specified property's value changes.
 */
Object.prototype.watch = function(prop, handler) {};

