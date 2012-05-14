#include "clang.h"
#include <clang-c/Index.h>
#include <wx/filename.h>
#include "clang_utils.h"
#include <wx/ffile.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "../CodeLite/cpp_scanner.h"
#include "../CodeLite/y.tab.h"

////////////////////////////////////////////////////////////////////////////
// Internal class used for traversing the macro found in a translation UNIT

struct MacroClientData {
	std::set<wxString> macros;
	std::set<wxString> interestingMacros;
	wxString           filename;

	wxString intersect() const {
		std::set<wxString> resultSet;
		std::set<wxString>::const_iterator iter = this->interestingMacros.begin();
		for(; iter != this->interestingMacros.end(); iter++) {
			if(this->macros.find(*iter) != this->macros.end()) {
				// this macro exists in both lists
				resultSet.insert(*iter);
			}
		}

		wxString macrosAsStr;
		std::set<wxString>::const_iterator it = resultSet.begin();
		for(; it != resultSet.end(); it++) {
			macrosAsStr << (*it) << wxT(" ");
		}
		return macrosAsStr;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// CC Helper method
static void DoParseCompletionString(CXCompletionString str, int depth, wxString &entryName, wxString &signature, wxString &completeString, wxString &returnValue)
{
 
    bool collectingSignature = false;
	int numOfChunks = clang_getNumCompletionChunks(str);
	for (int j=0 ; j<numOfChunks; j++) {

		CXString chunkText = clang_getCompletionChunkText(str, j);
		CXCompletionChunkKind chunkKind = clang_getCompletionChunkKind(str, j);

		switch(chunkKind) {
		case CXCompletionChunk_TypedText:
			entryName = wxString(clang_getCString(chunkText), wxConvUTF8);
			completeString += entryName;
			break;

		case CXCompletionChunk_ResultType:
			completeString += wxString(clang_getCString(chunkText), wxConvUTF8);
			completeString += wxT(" ");
			returnValue = wxString(clang_getCString(chunkText), wxConvUTF8);
			break;

		case CXCompletionChunk_Optional: {
			// Optional argument
			CXCompletionString optStr = clang_getCompletionChunkCompletionString(str, j);
			wxString optionalString;
			wxString dummy;
			// Once we hit the 'Optional Chunk' only the 'completeString' is matter
			DoParseCompletionString(optStr, depth + 1, dummy, dummy, optionalString, dummy);
			if(collectingSignature) {
				signature += optionalString;
			}
			completeString += optionalString;
		}
		break;
		case CXCompletionChunk_LeftParen:
			collectingSignature = true;
			signature += wxT("(");
			completeString += wxT("(");
			break;

		case CXCompletionChunk_RightParen:
			collectingSignature = true;
			signature += wxT(")");
			completeString += wxT(")");
			break;

		default:
			if(collectingSignature) {
				signature += wxString(clang_getCString(chunkText), wxConvUTF8);
			}
			completeString += wxString(clang_getCString(chunkText), wxConvUTF8);
			break;
		}
		clang_disposeString(chunkText);
	}

	// To make this tag compatible with ctags one, we need to place
	// a /^ and $/ in the pattern string (we add this only to the top level completionString)
	if(depth == 0) {
		completeString.Prepend(wxT("/^ "));
		completeString.Append(wxT(" $/"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//

enum CXChildVisitResult Clang::MacrosCallback(CXCursor cursor,
        CXCursor parent,
        CXClientData clientData)
{
	// Get all macros here...
	if(cursor.kind == CXCursor_MacroDefinition) {

		// Dont collect macro defined in this file
		CXSourceLocation loc = clang_getCursorLocation(cursor);
		CXFile file;
		unsigned line, col, off;
		clang_getSpellingLocation(loc, &file, &line, &col, &off);

		CXString strFileName = clang_getFileName(file);
		wxFileName fn(wxString(clang_getCString(strFileName), wxConvUTF8));
		clang_disposeString(strFileName);
		MacroClientData *cd = (MacroClientData*)clientData;

		if(cd->filename != fn.GetFullPath()) {
			CXString displayName = clang_getCursorDisplayName(cursor);
			cd->macros.insert(wxString(clang_getCString(displayName), wxConvUTF8));
			clang_disposeString(displayName);
		}

	}
	return CXChildVisit_Continue;
}

Clang::Clang(const char* file, const char* command, int argc, char** argv)
	: m_isOK(true)
{
	wxString cmd(command, wxConvUTF8);
	if(cmd == wxT("parse")) {
		m_command      = Parse;
		m_outputFolder = wxString(argv[0], wxConvUTF8);
		argv++;
		argc--;
	} else if(cmd == wxT("parse-macros")) {
		
		m_command      = ParseMacros;
		m_outputFolder = wxString(argv[0], wxConvUTF8);
		argv++;
		argc--;
		
	} else if(cmd == wxT("print-macros")) {
		m_command = PrintMacros;
		m_astFile = wxString(argv[0], wxConvUTF8);
		argv++;
		argc--;

	} else if(cmd == wxT("code-complete")) {
		m_command = CC;
		m_astFile = wxString(argv[0], wxConvUTF8);
		argv++;

		m_loc = wxString(argv[0], wxConvUTF8);
		argv++;
		argc--;

	} else {
		m_isOK = false;
	}

	m_file = wxString(file, wxConvUTF8);
	m_argv = argv;
	m_argc = argc;
}

Clang::~Clang()
{
}

int Clang::Run()
{
	switch(m_command) {
	case Parse:
		return DoParse();
	
	case ParseMacros:
		return DoParseMacros();
		
	case PrintMacros:
		return DoPrintMacros();

	case CC:
		return DoCC();

	}
	fprintf(stderr, "Unknown command\n");
	return -1;
}

int Clang::DoParse()
{
	// Prepare the TU
	// First time, need to create it
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit TU = clang_parseTranslationUnit(index,
	                       m_file.mb_str(wxConvUTF8).data(),
	                       m_argv,
	                       m_argc,
	                       NULL, 0,
	                       CXTranslationUnit_CXXPrecompiledPreamble
	                       | CXTranslationUnit_CacheCompletionResults
	                       | CXTranslationUnit_PrecompiledPreamble
	                       | CXTranslationUnit_Incomplete
	                       | CXTranslationUnit_DetailedPreprocessingRecord
	                       | CXTranslationUnit_CXXChainedPCH
	                       | CXTranslationUnit_SkipFunctionBodies);
	if(TU) {

		//ClangUtils::printDiagnosticsToLog(TU);

		// The output file
		wxString outputFile;
		wxFileName ASTFile(m_file);
		outputFile << m_outputFolder << wxFileName::GetPathSeparator() << ASTFile.GetFullName() << wxT(".TU");
		// Write the TU
		if(clang_saveTranslationUnit(TU, outputFile.mb_str(wxConvUTF8).data(), clang_defaultSaveOptions(TU)) != 0) {
			clang_disposeTranslationUnit(TU);
			clang_disposeIndex(index);
			return -1;
		}

		clang_disposeTranslationUnit(TU);
		clang_disposeIndex(index);
		wxPrintf(wxT("%s\n"), outputFile.c_str());
		return 0;
	}
	return -1;
}

int Clang::DoPrintMacros()
{
	MacroClientData clientData;
	clientData.filename = m_file;

	// Load the TU
	CXIndex index = clang_createIndex(1, 1);
	CXTranslationUnit TU = clang_createTranslationUnit(index, m_astFile.mb_str(wxConvUTF8).data());

	if(TU) {
		// Traverse the AST and get a list of macros collected
		CXCursorVisitor visitor = Clang::MacrosCallback;
		clang_visitChildren(clang_getTranslationUnitCursor(TU), visitor, (CXClientData)&clientData);
		clang_disposeTranslationUnit(TU);
		clang_disposeIndex(index);

		// get a list of "interestring" macros from the current source file
		// By interesting we refer to macros that are actually used in the 
		// current source file
		DoGetUsedMacros(m_file);
		clientData.interestingMacros = m_interestingMacros;
		
		// Intersect between the lists and print it 
		// as a space delimited string to the stdout
		wxPrintf(wxT("%s\n"), clientData.intersect().c_str());
		return 0;
	}
	return -1;
}

int Clang::DoCC()
{
	unsigned line, col;
	wxString sLine = m_loc.BeforeFirst(wxT(':'));
	wxString sCol  = m_loc.AfterFirst(wxT(':'));
	sLine.Trim().Trim(false);
	sCol.Trim().Trim(false);

	sCol.ToULong((unsigned long*)&col);
	sLine.ToULong((unsigned long*)&line);

	CXIndex idx = clang_createIndex(1, 1);
	CXTranslationUnit TU = clang_createTranslationUnit(idx, m_astFile.mb_str(wxConvUTF8).data());
	if(TU) {
		wxFFile fp(m_file, wxT("rb"));
		if(fp.IsOpened()) {
			wxString content;
			fp.ReadAll(&content, wxConvUTF8);
			fp.Close();

			std::string cbFileName = m_file.mb_str(wxConvUTF8).data();
			std::string cbBuffer   = content.mb_str(wxConvUTF8).data();
			CXUnsavedFile unsavedFile = { cbFileName.c_str(), cbBuffer.c_str(), cbBuffer.length() };

			CXCodeCompleteResults *ccResults = clang_codeCompleteAt(TU, cbFileName.data(), line, col, &unsavedFile, 1, clang_defaultCodeCompleteOptions());
			if(ccResults) {
				unsigned numResults = ccResults->NumResults;
				clang_sortCodeCompletionResults(ccResults->Results, ccResults->NumResults);
				for(unsigned i=0; i<numResults; i++) {
					CXCompletionResult result = ccResults->Results[i];
					CXCompletionString str    = result.CompletionString;
					CXCursorKind       kind   = result.CursorKind;

					if(kind == CXCursor_NotImplemented)
						continue;
					
					if (clang_getCompletionAvailability(str) != CXAvailability_Available)
						continue;
						
					wxString entryName, entrySignature, entryPattern, entryReturnValue;
					DoParseCompletionString(str, 0, entryName, entrySignature, entryPattern, entryReturnValue);

					wxPrintf(wxT("%s : %s %s\n"), entryReturnValue.c_str(), entryName.c_str(), entrySignature.c_str());
					
				}
				clang_disposeCodeCompleteResults(ccResults);
				return 0;
			}
		}
	}
	return -1;
}

int Clang::DoParseMacros()
{
	wxFileName sourceFile(m_file);
	wxFileName astFile   (m_outputFolder, sourceFile.GetFullName() + wxT(".TU"));
	
	bool bParse = false;
	if(!astFile.FileExists()) {
		bParse = true;
		
	} else if(astFile.GetModificationTime().GetTicks() < sourceFile.GetModificationTime().GetTicks()) {
		bParse = true;
	}
	
	if(bParse) DoParse();
	
	m_astFile = astFile.GetFullPath();
	return DoPrintMacros();
}

void Clang::DoGetUsedMacros(const wxString &filename)
{
	static wxRegEx reMacro(wxT("#[ \t]*((if)|(elif)|(ifdef)|(ifndef))[ \t]*"));

	m_interestingMacros.clear();
	wxString fileContent;
	
	wxFFile fp(filename, wxT("rb"));
	if(fp.IsOpened()) {
		fp.ReadAll(&fileContent, wxConvUTF8);
		fp.Close();
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
					m_interestingMacros.insert(intMacro);
				}
			}
		}
	}
}
