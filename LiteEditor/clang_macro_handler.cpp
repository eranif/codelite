#include "clang_macro_handler.h"
#include "event_notifier.h"
#include "y.tab.h"
#include "cpp_scanner.h"
#include "plugin.h"
#include <wx/xrc/xmlres.h>
#include "processreaderthread.h"
#include <wx/regex.h>
#include "file_logger.h"
#include "globals.h"

const wxEventType wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE = XRCID("wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE");

BEGIN_EVENT_TABLE(ClangMacroHandler, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ClangMacroHandler::OnClangProcessTerminated)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ClangMacroHandler::OnClangProcessOutput)
END_EVENT_TABLE()

ClangMacroHandler::ClangMacroHandler()
	: m_process(NULL)
	, m_editor(NULL)
{
	EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING,      wxCommandEventHandler(ClangMacroHandler::OnEditorClosing), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSING, wxCommandEventHandler(ClangMacroHandler::OnAllEditorsClosing), NULL, this);
}

ClangMacroHandler::~ClangMacroHandler()
{
	EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING,      wxCommandEventHandler(ClangMacroHandler::OnEditorClosing), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSING, wxCommandEventHandler(ClangMacroHandler::OnAllEditorsClosing), NULL, this);
}

void ClangMacroHandler::OnClangProcessOutput(wxCommandEvent& e)
{
	ProcessEventData *ped = (ProcessEventData *)e.GetClientData();
	m_output << ped->GetData();
	delete ped;
}

void ClangMacroHandler::OnClangProcessTerminated(wxCommandEvent& e)
{
	ProcessEventData *ped = (ProcessEventData *)e.GetClientData();
	delete ped;
	if( m_process ) {
		delete m_process;
		m_process = NULL;
	}

	// Process the output here...
	wxArrayString macros = ::wxStringTokenize(m_output, wxT("\n\r"), wxTOKEN_STRTOK);
	CL_DEBUG(wxT("ClangMacroHandler: Found %d macros"), (int) macros.GetCount());
	
	wxCommandEvent evt(wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE);
	evt.SetClientData(this);
	EventNotifier::Get()->AddPendingEvent(evt);
}

void ClangMacroHandler::Cancel()
{
	m_editor = NULL;
}

void ClangMacroHandler::OnAllEditorsClosing(wxCommandEvent& e)
{
	e.Skip();
	Cancel();
}

void ClangMacroHandler::OnEditorClosing(wxCommandEvent& e)
{
	e.Skip();
	IEditor *editor = reinterpret_cast<IEditor*>(e.GetClientData());
	if(editor && editor == m_editor) {
		Cancel();
	}
}

void ClangMacroHandler::DoGetUsedMacros(const wxString &filename)
{
	static wxRegEx reMacro(wxT("#[ \t]*((if)|(elif)|(ifdef)|(ifndef))[ \t]*"));

	m_interestingMacros.clear();
	wxString fileContent;
	if(!::ReadFileWithConversion(filename, fileContent)) {
		return;
	}

	CppScannerPtr scanner(new CppScanner());
	wxArrayString lines = wxStringTokenize(fileContent, wxT("\r\n"));
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString line = lines.Item(i).Trim(false);
		if(line.StartsWith(wxT("#")) && reMacro.IsValid() && reMacro.Matches(line)) {
			// Macro line
			wxString match = reMacro.GetMatch(line, 0);
			wxString ppLine = line.Mid(match.Len());

			scanner->Reset();
			std::string cstr = ppLine.mb_str(wxConvUTF8).data();
			scanner->SetText(cstr.c_str());
			int type(0);
			while( (type = scanner->yylex()) != 0 ) {
				if(type == IDENTIFIER) {
					wxString intMacro = wxString(scanner->YYText(), wxConvUTF8);
					CL_DEBUG1(wxT("Found interesting macro: %s"), intMacro.c_str());
					m_interestingMacros.insert(intMacro);
				}
			}
		}
	}
}

void ClangMacroHandler::SetProcessAndEditor(IProcess* process, IEditor* editor)
{
	this->m_process = process;
	this->m_editor  = editor;
	if(m_editor) {
		DoGetUsedMacros(m_editor->GetFileName().GetFullPath());
	}
}
