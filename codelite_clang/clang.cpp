#include "clang.h"
#include <clang-c/Index.h>
#include <wx/filename.h>
#include "clang_utils.h"

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

    } else if(cmd == wxT("reparse")) {
        m_command = ReParse;

    } else if(cmd == wxT("write-pch")) {
        m_command = WritePch;
        m_outputFolder = wxString(argv[0], wxConvUTF8);
        argv++;
        argc--;

    } else if(cmd == wxT("print-macros")) {
        m_command = PrintMacros;
        m_astFile = wxString(argv[0], wxConvUTF8);
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

    case ReParse:
        return DoReparse();

    case WritePch:
        return DoWritePch();

    case PrintMacros:
        return DoPrintMacros();

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

        ClangUtils::printDiagnosticsToLog(TU);

        // The output file
        wxString outputFile;
        wxFileName ASTFile(m_file);
        outputFile << m_outputFolder << wxFileName::GetPathSeparator() << ASTFile.GetFullName() << wxT(".AST");
        // Write the TU
        if(clang_saveTranslationUnit(TU, outputFile.mb_str(wxConvUTF8).data(), clang_defaultSaveOptions(TU)) != 0) {
            clang_disposeTranslationUnit(TU);
            return -1;
        }
        clang_disposeTranslationUnit(TU);
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
        CXCursorVisitor visitor = Clang::MacrosCallback;
        clang_visitChildren(clang_getTranslationUnitCursor(TU), visitor, (CXClientData)&clientData);
        clang_disposeTranslationUnit(TU);
        
        std::set<wxString>::iterator iter = clientData.macros.begin();
        for(; iter != clientData.macros.end(); iter++) {
            wxPrintf(wxT("%s\n"), iter->c_str());
        }
        
        return 0;
    }
    return -1;
}

int Clang::DoReparse()
{
    // Load the TU
    CXIndex index = clang_createIndex(1, 1);
    CXTranslationUnit TU = clang_createTranslationUnit(index, m_file.mb_str(wxConvUTF8).data());
    if(TU) {
        if(!clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU))) {
            wxPrintf(wxT("ERROR during reparsing of TU\n"));
            clang_disposeTranslationUnit(TU);
            return -1;
        }
        
        if(clang_saveTranslationUnit(TU, m_file.mb_str(wxConvUTF8).data(), clang_defaultSaveOptions(TU)) != 0) {
            wxPrintf(wxT("ERROR during saving of TU\n"));
            clang_disposeTranslationUnit(TU);
            return -1;
        }
        clang_disposeTranslationUnit(TU);
        return 0;
    }
    
    return -1;
}

int Clang::DoWritePch()
{
    return -1;
}
