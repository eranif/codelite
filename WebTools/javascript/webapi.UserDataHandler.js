
/**
 * @brief When associating user data with a key on a node, Node.setUserData() can also accept, in its third argument, a handler which will be called when the object is cloned, imported, deleted, renamed, or adopted. Per the specification, exceptions should not be thrown in a UserDataHandler. In both document.importNode() and Node.cloneNode(), although user data is not copied over, the handler will be called.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/UserDataHandler
 */
function UserDataHandler() {

}

