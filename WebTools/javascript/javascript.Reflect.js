
/**
 * @brief Reflect is a built-in object that provides methods for interceptable JavaScript operations. The methods are the same as those of the proxy handlers. Reflect It is not a function object.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Reflect
 */
function Reflect() {

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

