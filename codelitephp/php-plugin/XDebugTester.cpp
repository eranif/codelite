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

            m_results.insert(std::make_pair(
                wxString("xdebug.remote_connect_back"),
                std::make_pair(rootElement.namedObject("_remoteConnectBack").toString(), wxString("1"))));
            m_results.insert(std::make_pair(
                wxString("xdebug.ide_key"),
                std::make_pair(rootElement.namedObject("_ideKey").toString(), globalConf.GetXdebugIdeKey())));
            m_results.insert(std::make_pair(wxString("xdebug.remote_port"),
                                            std::make_pair(rootElement.namedObject("_remotePort").toString(),
                                                           wxString::Format("%d", globalConf.GetXdebugPort()))));
            m_results.insert(std::make_pair(
                wxString("xdebug.remote_host"),
                std::make_pair(rootElement.namedObject("_remoteHost").toString(), globalConf.GetXdebugHost())));
            m_results.insert(
                std::make_pair(wxString("xdebug.remote_enable"),
                               std::make_pair(rootElement.namedObject("_remoteEnable").toString(), wxString("1"))));

            // XDebug loaded
            m_results.insert(
                std::make_pair(wxString("XDebug Loaded"),
                               std::make_pair(rootElement.namedObject("_xdebugLoaded").toString(), wxString("1"))));
            // Zend Debugger loaded
            m_results.insert(std::make_pair(
                wxString("Zend Debugger Loaded"),
                std::make_pair(rootElement.namedObject("_zendDebuggerLoaded").toString(), wxString("0"))));
            return true;
        }
    }
    return false;
}
