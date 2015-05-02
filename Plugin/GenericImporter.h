#ifndef GENERICIMPORTER_H
#define GENERICIMPORTER_H

#include <wx/string.h>
#include <vector>
#include <memory>

#ifdef __WXMSW__
#define STATIC_LIBRARY_EXT wxT(".lib")
#define DYNAMIC_LIBRARY_EXT wxT(".dll")
#define EXECUTABLE_EXT wxT(".exe")
#else
#define STATIC_LIBRARY_EXT wxT(".a")
#define DYNAMIC_LIBRARY_EXT wxT(".so")
#define EXECUTABLE_EXT wxT("")
#endif

enum class GenericCfgType { DYNAMIC_LIBRARY, STATIC_LIBRARY, EXECUTABLE };

struct GenericProjectCfg {
    wxString name;
    wxString outputFilename;
    wxString intermediateDirectory;
    wxString includePath;
    wxString preprocessor;
    wxString libraries;
    wxString libPath;
    wxString cCompilerOptions;
    wxString cppCompilerOptions;
    wxString linkerOptions;
    wxString preCompiledHeader;
	wxString command;
    GenericCfgType type;
};

typedef std::shared_ptr<GenericProjectCfg> GenericProjectCfgPtr;

struct GenericProjectFile {
    wxString name;
    wxString vpath;
};

typedef std::shared_ptr<GenericProjectFile> GenericProjectFilePtr;

struct GenericProject {
    wxString name;
    wxString path;
    GenericCfgType cfgType;
    std::vector<GenericProjectCfgPtr> cfgs;
    std::vector<GenericProjectFilePtr> files;
};

typedef std::shared_ptr<GenericProject> GenericProjectPtr;

struct GenericWorkspace {
    wxString name;
    wxString path;
    std::vector<GenericProjectPtr> projects;
};

typedef std::shared_ptr<GenericWorkspace> GenericWorkspacePtr;

class GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler) = 0;
    virtual bool isSupportedWorkspace() = 0;
    virtual GenericWorkspacePtr PerformImport() = 0;
};

#endif // GENERICIMPORTER_H
