
/**
 * @brief If you're developing privileged code, and would like to create a worker that can use js-ctypes to perform calls to native code, you can do so by using ChromeWorker instead of the standard Worker object. It works exactly like a standard Worker, except that it has access to js-ctypes via a global ctypes object available in the global scope of the worker.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ChromeWorker
 */
function ChromeWorker() {

}

