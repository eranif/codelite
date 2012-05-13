#ifndef CLANG_H
#define CLANG_H

#include <wx/string.h>
#include <set>
#include <clang-c/Index.h>

class Clang
{
public:
    enum {
        Parse,
        //WritePch,
        PrintMacros,
		ParseMacros,
		CC
    };

protected:
    wxString m_file;
    wxString m_astFile;
    int      m_command;
    bool     m_isOK;
    wxString m_outputFolder;
    char**   m_argv;
    int      m_argc;
    wxString m_loc;
	
public:
    static enum CXChildVisitResult MacrosCallback(CXCursor cursor,
                                                  CXCursor parent,
                                                  CXClientData clientData);
    
protected:
    int DoParse();
    //int DoWritePch();
    int DoPrintMacros();
    int DoCC();
	int DoParseMacros();
	
public:
    Clang(const char* file, const char* command, int argc, char **argv);
    virtual ~Clang();
    
    int Run();
    
    void SetCommand(int command) {
        this->m_command = command;
    }
    void SetFile(const wxString& file) {
        this->m_file = file;
    }
    void SetIsOK(bool isOK) {
        this->m_isOK = isOK;
    }
    int GetCommand() const {
        return m_command;
    }
    const wxString& GetFile() const {
        return m_file;
    }
    bool IsOK() const {
        return m_isOK;
    }
};

#endif // CLANG_H
