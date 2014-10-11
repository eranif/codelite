#include "XDebugTester.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>
#include "clZipReader.h"
#include "phpexecutor.h"
#include <wx/msgdlg.h>
#include "json_node.h"
#include "php_configuration_data.h"

XDebugTester::XDebugTester() {}

XDebugTester::~XDebugTester() {}

bool XDebugTester::RunTest()
{
    // Get the path to the xdebug tester script
    wxFileName xdebugTesterScript(clStandardPaths::Get().GetUserDataDir(), "TestXDebugSettings.php");
    if(!xdebugTesterScript.Exists()) {
        clZipReader zipReader(wxFileName(clStandardPaths::Get().GetDataDir(), "PHP.zip"));
        zipReader.Extract(xdebugTesterScript.GetFullName(), xdebugTesterScript.GetPath());
    }

    if(xdebugTesterScript.Exists()) {
        PHPConfigurationData globalConf;
        globalConf.Load();

        PHPExecutor executor;
        wxString php_output;
        if(executor.RunScript(xdebugTesterScript.GetFullPath(), php_output)) {
            JSONRoot root(php_output);
            JSONElement rootElement = root.toElement();

            //////////////////////////////////////////////////
            // Directives
            //////////////////////////////////////////////////

            m_results.insert(
                std::make_pair(wxString("xdebug.remote_connect_back"),
                               std::make_pair(rootElement.namedObject("_remoteConnectBack").toString(),
                                              "If you don't know CodeLite's IP, set this directive to 1")));

            m_results.insert(std::make_pair(wxString("xdebug.ide_key"),
                                            std::make_pair(rootElement.namedObject("_ideKey").toString(),
                                                           "The session name between CodeLite and XDebug")));

            m_results.insert(std::make_pair(
                wxString("xdebug.remote_port"),
                std::make_pair(rootElement.namedObject("_remotePort").toString(),
                               "The port to which Xdebug tries to connect on the remote host. The default is 9000")));

            m_results.insert(std::make_pair(wxString("xdebug.remote_host"),
                                            std::make_pair(rootElement.namedObject("_remoteHost").toString(),
                                                           "Selects the host where the debug client is running\nYou "
                                                           "can either use a host name or an IP address.\nThis setting "
                                                           "is ignored if xdebug.remote_connect_back is enabled")));
            m_results.insert(
                std::make_pair(wxString("xdebug.remote_enable"),
                               std::make_pair(rootElement.namedObject("_remoteEnable").toString(),
                                              "This switch controls whether Xdebug should try to contact a debug "
                                              "client which is listening on the host and port as set with the settings "
                                              "xdebug.remote_host and xdebug.remote_port. If a connection can not be "
                                              "established the script will just continue as if this setting was 0")));

            // XDebug loaded
            m_results.insert(std::make_pair(wxString("XDebug Loaded"),
                                            std::make_pair(rootElement.namedObject("_xdebugLoaded").toString(),
                                                           "Is XDebug extension loaded? This value should be '1'")));

            // Zend Debugger loaded
            m_results.insert(std::make_pair(wxString("Zend Debugger Loaded"),
                                            std::make_pair(rootElement.namedObject("_zendDebuggerLoaded").toString(),
                                                           "Is Zend Debugger extension loaded? When using XDebug, it "
                                                           "is recommended to disable Zend Debugger")));
            return true;
        }
    }
    return false;
}
