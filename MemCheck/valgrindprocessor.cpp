/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include "valgrindprocessor.h"

#include "file_logger.h"
#include "memcheckdefs.h"
#include "memchecksettings.h"
#include "workspace.h"

#include <wx/stdpaths.h>
#include <wx/textfile.h>

ValgrindMemcheckProcessor::ValgrindMemcheckProcessor(MemCheckSettings* const settings)
    : IMemCheckProcessor(settings)
{
}

wxArrayString ValgrindMemcheckProcessor::GetSuppressionFiles()
{
    wxArrayString suppFiles = m_settings->GetValgrindSettings().GetSuppFiles();
    if(clCxxWorkspaceST::Get()->IsOpen() && m_settings->GetValgrindSettings().GetSuppFileInPrivateFolder()) {
        wxTextFile defaultSupp(
            wxFileName(clCxxWorkspaceST::Get()->GetPrivateFolder(), "valgrind.memcheck.supp").GetFullPath());
        if(!defaultSupp.Exists())
            defaultSupp.Create();
        suppFiles.Insert(defaultSupp.GetName(), 0);
    }
    return suppFiles;
}

void ValgrindMemcheckProcessor::GetExecutionCommand(const wxString& originalCommand, wxString& command,
                                                    wxString& command_args)
{
    m_outputLogFileName = m_settings->GetValgrindSettings().GetOutputFile();
    if(m_settings->GetValgrindSettings().GetOutputInPrivateFolder() && m_outputLogFileName.IsEmpty())
        if(m_settings->GetValgrindSettings().GetOutputInPrivateFolder() || m_outputLogFileName.IsEmpty()) {
            if(clCxxWorkspaceST::Get()->IsOpen())
                m_outputLogFileName =
                    wxFileName(clCxxWorkspaceST::Get()->GetPrivateFolder(), "valgrind.memcheck.log.xml").GetFullPath();
            else
                m_outputLogFileName =
                    wxFileName(clStandardPaths::Get().GetTempDir(), "valgrind.memcheck.log.xml").GetFullPath();
        }

    wxArrayString suppFiles = GetSuppressionFiles();
    wxString suppresions;
    for(wxArrayString::iterator it = suppFiles.begin(); it != suppFiles.end(); ++it)
        suppresions.Append(
            wxString::Format(" %s=%s", m_settings->GetValgrindSettings().GetSuppressionFileOption(), *it));

    command = m_settings->GetValgrindSettings().GetBinary();
    command_args = wxString::Format(
        "%s %s %s %s %s", m_settings->GetValgrindSettings().GetMandatoryOptions(),
        wxString::Format("%s=%s", m_settings->GetValgrindSettings().GetOutputFileOption(), m_outputLogFileName),
        suppresions, m_settings->GetValgrindSettings().GetOptions(), originalCommand);
}

bool ValgrindMemcheckProcessor::Process(const wxString& outputLogFileName)
{
    // CL_DEBUG1(PLUGIN_PREFIX("ValgrindMemcheckProcessor::Process()"));

    if(!outputLogFileName.IsEmpty())
        m_outputLogFileName = outputLogFileName;

    wxXmlDocument doc;
    if(!doc.Load(m_outputLogFileName) || doc.GetRoot()->GetName() != wxT("valgrindoutput")) {
        return false;
    }
    m_errorList.clear();

    int i = 0;
    wxXmlNode* errorNode = doc.GetRoot()->GetChildren();
    while(errorNode) {
        if(errorNode->GetName() == wxT("error")) {
            m_errorList.push_back(ProcessError(doc, errorNode));
        }
        errorNode = errorNode->GetNext();

        if(i < 1000)
            i++;
        else {
            i = 0;
            // ATTN  m_mgr->GetTheApp()
            wxTheApp->Yield();
        }
    }
    return true;
}

MemCheckError ValgrindMemcheckProcessor::ProcessError(wxXmlDocument& doc, wxXmlNode* errorNode)
{
    // CL_DEBUG1(PLUGIN_PREFIX("ValgrindMemcheckProcessor::ProcessError()"));

    bool auxiliary = false;
    MemCheckError result;
    result.type = MemCheckError::TYPE_ERROR;
    MemCheckError auxiliaryResult;

    wxXmlNode* suberrorNode = errorNode->GetChildren();
    while(suberrorNode) {

        // retrieving error label
        if(suberrorNode->GetName() == wxT("what")) {
            result.label = suberrorNode->GetNodeContent();
        } else if(suberrorNode->GetName() == wxT("xwhat")) {
            wxXmlNode* child = suberrorNode->GetChildren();
            while(child) {
                if(child->GetName() == wxT("text")) {
                    result.label = child->GetNodeContent();
                    break;
                }
                child = child->GetNext();
            }
        } else if(suberrorNode->GetName() == wxT("auxwhat")) {
            auxiliaryResult.label = suberrorNode->GetNodeContent();
            auxiliaryResult.type = MemCheckError::TYPE_AUXILIARY;
            auxiliary = true;
        } else if(suberrorNode->GetName() == wxT("stack")) {
            wxXmlNode* locationNode = suberrorNode->GetChildren();
            while(locationNode) {
                if(locationNode->GetName() == wxT("frame")) {
                    if(auxiliary) {
                        auxiliaryResult.locations.push_back(ProcessLocation(doc, locationNode));
                    } else {
                        result.locations.push_back(ProcessLocation(doc, locationNode));
                    }
                }
                locationNode = locationNode->GetNext();
            }
        } else if(suberrorNode->GetName() == wxT("suppression")) {
            wxXmlNode* child = suberrorNode->GetChildren();
            while(child) {
                if(child->GetName() == wxT("rawtext")) {
                    result.suppression = child->GetNodeContent();
                    break;
                }
                child = child->GetNext();
            }
        }
        suberrorNode = suberrorNode->GetNext();
    }

    if(!result.suppression)
        result.suppression = wxT("#Suppresion pattern not present in output log.\n#This plugin requires Valgrind to be "
                                 "run with '--gen-suppressions=all' option");

    if(auxiliary)
        result.nestedErrors.push_back(auxiliaryResult);

    // TODO ? add checout ?
    // add check for empty locationArrays
    //  CL_DEBUG1(PLUGIN_PREFIX("\t equal #0 and #1 = %s", (errorArray.Item(0) == errorArray.Item(1)?"true":"false") ));
    //  CL_DEBUG1(PLUGIN_PREFIX("\t equal #1 and #2 = %s", (*(errorArray.Item(2)) ==
    //  *(errorArray.Item(1))?"true":"false") )); CL_DEBUG1(PLUGIN_PREFIX("\t equal #2 and #3 = %s", (errorArray.Item(2)
    //  == errorArray.Item(3)?"true":"false") ));

    // check if required fields was found
    //  CL_ERROR1(PLUGIN_PREFIX("Broken input, tag <what> or <xwhat> not found, can't continue!"));
    //  CL_ERROR1(PLUGIN_PREFIX("Broken input, tag <stack> not found, can't continue!"));
    return result;
}

MemCheckErrorLocation ValgrindMemcheckProcessor::ProcessLocation(wxXmlDocument& doc, wxXmlNode* locationNode)
{
    // CL_DEBUG1(PLUGIN_PREFIX("ValgrindMemcheckProcessor::ProcessLocation()"));

    MemCheckErrorLocation result;
    result.line = -1;
    wxString file;
    wxString dir;

    wxXmlNode* subframeNode = locationNode->GetChildren();
    while(subframeNode) {

        if(subframeNode->GetName() == wxT("ip")) {
            // ignoring
        } else if(subframeNode->GetName() == wxT("obj")) {
            result.obj = subframeNode->GetNodeContent();
        } else if(subframeNode->GetName() == wxT("fn")) {
            result.func = subframeNode->GetNodeContent();
        } else if(subframeNode->GetName() == wxT("dir")) {
            dir = subframeNode->GetNodeContent();
        } else if(subframeNode->GetName() == wxT("file")) {
            file = subframeNode->GetNodeContent();
        } else if(subframeNode->GetName() == wxT("line")) {
            result.line = wxAtoi(subframeNode->GetNodeContent());
        }
        subframeNode = subframeNode->GetNext();
    }

    if(!dir.IsEmpty() && !dir.EndsWith(wxT("/")))
        dir.Append(wxT("/"));
    result.file = dir + file;

    // TODO ? add checkout ?
    return result;
}
