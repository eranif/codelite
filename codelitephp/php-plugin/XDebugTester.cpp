#include "XDebugTester.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>
#include "clZipReader.h"
#include "phpexecutor.h"
#include <wx/msgdlg.h>
#include "JSON.h"
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
            JSON root(php_output);
            JSONItem rootElement = root.toElement();

            //////////////////////////////////////////////////
            // Directives
            //////////////////////////////////////////////////
            {
                wxString msg;
                if(rootElement.namedObject("_remoteConnectBack").toString() != "1") {
                    msg << "<font color=\"red\">Failed. This value should be set to 1</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                m_results.insert(
                    std::make_pair(wxString("xdebug.remote_connect_back"),
                                   std::make_pair(rootElement.namedObject("_remoteConnectBack").toString(), msg)));
            }
            {
                wxString msg;
                if(rootElement.namedObject("_ideKey").toString() != globalConf.GetXdebugIdeKey()) {
                    msg << "<font color=\"red\">Failed. This value should be set to \"" << globalConf.GetXdebugIdeKey()
                        << "\"</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                m_results.insert(std::make_pair(wxString("xdebug.idekey"),
                                                std::make_pair(rootElement.namedObject("_ideKey").toString(), msg)));
            }
            m_results.insert(std::make_pair(
                wxString("xdebug.remote_port"),
                std::make_pair(rootElement.namedObject("_remotePort").toString(),
                               "The port to which Xdebug tries to connect on the remote host. The default is 9000")));

            m_results.insert(std::make_pair(wxString("xdebug.remote_host"),
                                            std::make_pair(rootElement.namedObject("_remoteHost").toString(),
                                                           "Selects the host where the debug client is running\nYou "
                                                           "can either use a host name or an IP address.\nThis setting "
                                                           "is ignored if xdebug.remote_connect_back is enabled")));
            {
                wxString msg;
                if(rootElement.namedObject("_remoteEnable").toString() != "1") {
                    msg << "<font color=\"red\">Failed. This value should be set to 1</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                m_results.insert(
                    std::make_pair(wxString("xdebug.remote_enable"),
                                   std::make_pair(rootElement.namedObject("_remoteEnable").toString(), msg)));
            }
            {
                // XDebug loaded
                wxString msg;
                if(rootElement.namedObject("_xdebugLoaded").toString() != "1") {
                    msg << "<font color=\"red\">Failed. XDebug is NOT loaded</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                m_results.insert(
                    std::make_pair(wxString("XDebug Loaded"),
                                   std::make_pair(rootElement.namedObject("_xdebugLoaded").toString(), msg)));
            }

            // Zend Debugger loaded
            {
                // If Zend Debugger is loaded, mark it the message with RED
                wxString msg;
                if(rootElement.namedObject("_zendDebuggerLoaded").toString() == "1") {
                    msg << "<font color=\"red\">Failed. Unload Zend Debugger extension"
                           "</font>";
                    m_results.insert(
                        std::make_pair(wxString("Zend Debugger Loaded"),
                                       std::make_pair(rootElement.namedObject("_zendDebuggerLoaded").toString(), msg)));
                }
            }
            return true;
        }
    }
    return false;
}
