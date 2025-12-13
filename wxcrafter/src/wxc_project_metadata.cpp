#include "wxc_project_metadata.h"

#include "fileutils.h"
#include "json_utils.h"

#include <wx/filename.h>

const wxEventType wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED = wxNewEventType();
const wxEventType wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED = wxNewEventType();

wxcProjectMetadata::wxcProjectMetadata()
    : m_objCounter(0)
    , m_useHpp(true)
    , m_firstWindowId(10000)
    , m_useEnum(true)
    , m_useUnderscoreMacro(true)
    , m_addHandlers(true)
{
    SetGenerateCPPCode(true);
    SetGenerateXRC(false);
}

void wxcProjectMetadata::FromJSON(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    m_objCounter = json.value("m_objCounter", 0);
    m_generatedFilesDir = JsonUtils::ToString(json["m_generatedFilesDir"]);
    m_includeFiles = JsonUtils::ToArrayString(json["m_includeFiles"]);
    m_bitmapFunction = JsonUtils::ToString(json["m_bitmapFunction"]);
    m_bitmapsFile = JsonUtils::ToString(json["m_bitmapsFile"]);
    m_GenerateCodeTypes = json.value("m_GenerateCodeTypes", static_cast<int>(wxcGenerateCPPCode));
    m_outputFileName = JsonUtils::ToString(json["m_outputFileName"]);
    m_firstWindowId = json.value("m_firstWindowId", m_firstWindowId);
    m_useEnum = json.value("m_useEnum", true);
    m_useUnderscoreMacro = json.value("m_useUnderscoreMacro", true);
    m_addHandlers = json.value("m_addHandlers", m_addHandlers);

    wxcSettings::Get().MergeCustomControl(json["m_templateClasses"]);
    if(m_bitmapFunction.IsEmpty()) {
        DoGenerateBitmapFunctionName();
    }

    // for backward-compatibility, we continue to use .h file extension if it's already there
    wxFileName headerFile = BaseCppFile();
    headerFile.SetExt("h");
    if(headerFile.IsRelative()) {
        headerFile.MakeAbsolute(GetProjectPath());
    }
    wxString header_file = headerFile.GetFullPath();
    m_useHpp = !wxFileName::FileExists(header_file);
}

nlohmann::json wxcProjectMetadata::ToJSON()
{
    UpdatePaths();
    return {{"m_generatedFilesDir", m_generatedFilesDir},
            {"m_objCounter", (int)m_objCounter},
            {"m_includeFiles", m_includeFiles},
            {"m_bitmapFunction", m_bitmapFunction},
            {"m_bitmapsFile", m_bitmapsFile},
            {"m_GenerateCodeTypes", m_GenerateCodeTypes},
            {"m_outputFileName", m_outputFileName},
            {"m_firstWindowId", m_firstWindowId},
            {"m_useEnum", m_useEnum},
            {"m_useUnderscoreMacro", m_useUnderscoreMacro},
            {"m_addHandlers", m_addHandlers}
    };
}

void wxcProjectMetadata::AppendCustomControlsJSON(const wxArrayString& controls, nlohmann::json& element) const
{
    element.push_back(wxcSettings::Get().GetCustomControlsAsJSON(controls));
}

wxString wxcProjectMetadata::GetCppFileName() const
{
    wxFileName cpp(m_generatedFilesDir, m_projectFile);
    cpp.SetExt("cpp");
    return cpp.GetFullPath();
}

wxString wxcProjectMetadata::GetXrcFileName() const
{
    wxFileName xrc(m_projectFile);
    if(!xrc.IsAbsolute()) {
        xrc = wxFileName(m_generatedFilesDir, m_projectFile);
    }
    xrc.SetExt("xrc");
    return xrc.GetFullPath();
}

wxString wxcProjectMetadata::GetHeaderFileName() const
{
    wxFileName header(m_generatedFilesDir, m_projectFile);
    header.SetExt(GetHeaderFileExt());
    return header.GetFullPath();
}

wxcProjectMetadata& wxcProjectMetadata::Get()
{
    static wxcProjectMetadata p;
    return p;
}

wxString wxcProjectMetadata::GetProjectPath() const
{
    wxFileName fn(m_projectFile);
    return fn.GetPath();
}

wxString wxcProjectMetadata::GetGeneratedFilesDir() const
{
    wxString value;
    m_generatedFilesDir.IsEmpty() ? value = "." : value = m_generatedFilesDir;
    return value;
}

wxFileName wxcProjectMetadata::BaseCppFile() const
{
    wxFileName baseFile(GetGeneratedFilesDir(), GetOutputFileName());
    baseFile.SetExt("cpp");
    return baseFile;
}

wxFileName wxcProjectMetadata::BaseHeaderFile() const
{
    wxFileName f = BaseCppFile();
    f.SetExt(GetHeaderFileExt());
    return f;
}

void wxcProjectMetadata::ClearAggregatedData()
{
    m_generatedHeader.Clear();
    m_generatedSource.Clear();
    m_generatedClassName.Clear();
    m_additionalFiles.clear();
}

void wxcProjectMetadata::Reset()
{
    m_generatedHeader.Clear();
    m_generatedSource.Clear();
    m_generatedClassName.Clear();
    m_virtualFolder.Clear();
    m_includeFiles.Clear();
    m_projectFile.Clear();
    m_generatedFilesDir.Clear();
    m_bitmapsFile.Clear();
    m_additionalFiles.clear();
    m_outputFileName.Clear();
    m_useHpp = true;
    m_useEnum = true;
    m_useUnderscoreMacro = true;
    m_firstWindowId = 10000;
    m_addHandlers = true;
}

void wxcProjectMetadata::DoGenerateBitmapFunctionName()
{
    m_bitmapFunction.Clear();
    wxString name = wxFileName::CreateTempFileName("wxCrafter");
    wxFileName fn(name);
    name.Clear();
    name << fn.GetName() << "InitBitmapResources";
    m_bitmapFunction = name;
}

wxString wxcProjectMetadata::DoGenerateBitmapsFile() const
{
    if(GetProjectFile().IsEmpty()) {
        return "";
    }

    wxFileName projectFileName(GetProjectFile());
    wxFileName fn = wxFileName(GetProjectPath(), projectFileName.GetFullName());
    wxString projectName = projectFileName.GetName();

    const wxArrayString& dirs = projectFileName.GetDirs();
    wxString lastDirName = dirs.IsEmpty() ? "" : dirs.Last();
    lastDirName.MakeLower();

    fn.SetName(projectName + "_" + lastDirName + "_bitmaps");
    fn.SetExt("cpp");
    return fn.GetFullName();
}

wxString wxcProjectMetadata::GetBitmapsFile() const
{
    if(m_bitmapsFile.IsEmpty()) {
        return DoGenerateBitmapsFile();
    }

    return m_bitmapsFile;
}
void wxcProjectMetadata::Serialize(const wxcWidget::List_t& topLevelsList, const wxFileName& filename)
{
    wxcProjectMetadata p;
    p.GenerateBitmapFunctionName();

    nlohmann::json root;
    root["metadata"] = p.ToJSON();

    // The windows
    auto& windows = root["windows"];
    windows = nlohmann::json::array();
    for (auto widget : topLevelsList) {
        widget->FixPaths(filename.GetPath()); // Fix abs paths to fit the new project file
        nlohmann::json json;
        widget->Serialize(json);
        windows.push_back(json);
    }
    FileUtils::WriteFileContent(filename, root.dump());
}

wxString wxcProjectMetadata::GetOutputFileName() const
{
    if(m_outputFileName.IsEmpty()) {
        wxFileName fn(GetProjectFile());
        return fn.GetName();
    }
    return m_outputFileName;
}

wxString wxcProjectMetadata::GetHeaderFileExt() const { return m_useHpp ? "hpp" : "h"; }

void wxcProjectMetadata::SetProjectFile(const wxString& filename)
{
    this->m_projectFile = filename;
    UpdatePaths();
}

void wxcProjectMetadata::UpdatePaths()
{
    if(m_generatedFilesDir.IsEmpty()) {
        m_generatedFilesDir = ".";
    }
    if(m_bitmapsFile.IsEmpty()) {
        m_bitmapsFile = DoGenerateBitmapsFile();
    }
}
