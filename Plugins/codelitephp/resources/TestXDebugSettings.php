<?php

class TestXDebugSettings {
    public $_ideKey;
    public $_zendDebuggerLoaded;
    public $_xdebugLoaded;
    public $_remoteConnectBack;
    public $_remoteEnable;
    public $_remoteHost;
    public $_remotePort;

    public function __construct(){
        $this->_zendDebuggerLoaded = "0";
        $this->_xdebugLoaded = "0";
        $this->_ideKey = "";
        $this->_remoteConnectBack = "0";
        $this->_remoteEnable = "0";
        $this->_remoteHost = "";
        $this->_remotePort = "0";
    }
    
    /**
     * @brief a helper method for reading directive using ini_get
     * If the directive could not be found, return $defaultValue
     * @param string $directive
     * @param string $defaultValue
     * @return mixed
     */
    private function readIni($directive, $defaultValue){
        $val = get_cfg_var($directive);
        if($val === false){
            return $defaultValue;
        }
        return (string)$val;
    }
    
    /**
     * @brief test XDebug INI settings
     */
    private function TestDirectives(){
        $this->_remoteConnectBack = $this->readIni("xdebug.remote_connect_back", "0");
        $this->_ideKey = $this->readIni("xdebug.ide_key", "");
        $this->_remoteEnable = $this->readIni("xdebug.remote_enable", "0");
        $this->_remoteHost = $this->readIni("xdebug.remote_host", "");
        $this->_remotePort = $this->readIni("xdebug.remote_port", "");
    }
    
    /**
     * @brief ensure that Zend Debugger is not loaded
     */
    private function TestZendDebuggerLoaded(){
        $loadedModules = get_loaded_extensions(true);

        foreach($loadedModules as $module){
            if(strtolower($module)== "zend debugger"){
                $this->_zendDebuggerLoaded = "1";
                return;
            }
        }
    }
    
    /**
     * @brief ensure that xdebug is loaded into PHP
     */
    private function TestXDebugLoaded(){
        $loadedModules = get_loaded_extensions(true);

        foreach($loadedModules as $module){
            if(strtolower($module)== "xdebug"){
                $this->_xdebugLoaded = "1";
                return;
            }
        }
    }
    
    /**
     * @brief run a small test to test the PHP settings to help the user
     * ensures that everything is setup properly for debugging with XDebug
     */
    public function Test(){
        // XDebug can not co-exist with Zend Debugger
        $this->TestZendDebuggerLoaded();
        // Ensure that XDebug is loaded
        $this->TestXDebugLoaded();
        // Test xdebug directives
        $this->TestDirectives();
        // Print the result as JSON output
        echo json_encode($this);
    }
}
$tester = new TestXDebugSettings();
$tester->Test();
