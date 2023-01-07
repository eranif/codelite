#include "wxc_project_metadata.h"

#include "event_notifier.h"

#include <wx/ffile.h>
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

wxcProjectMetadata::~wxcProjectMetadata() {}

void wxcProjectMetadata::FromJSON(const JSONElement& json)
{
    m_objCounter = json.namedObject("m_objCounter").toInt();
    m_generatedFilesDir = json.namedObject("m_generatedFilesDir").toString();
    m_includeFiles = json.namedObject("m_includeFiles").toArrayString();
    m_bitmapFunction = json.namedObject("m_bitmapFunction").toString();
    m_bitmapsFile = json.namedObject("m_bitmapsFile").toString();
    m_GenerateCodeTypes = json.namedObject("m_GenerateCodeTypes").toInt(wxcGenerateCPPCode);
    m_outputFileName = json.namedObject("m_outputFileName").toString();
    m_firstWindowId = json.namedObject("m_firstWindowId").toInt(m_firstWindowId);
    m_useEnum = json.namedObject("m_useEnum").toBool(true);
    m_useUnderscoreMacro = json.namedObject("m_useUnderscoreMacro").toBool(true);
    m_addHandlers = json.namedObject("m_addHandlers").toBool(m_addHandlers);

    wxcSettings::Get().MergeCustomControl(json.namedObject("m_templateClasses"));
    if(m_bitmapFunction.IsEmpty()) {
        DoGenerateBitmapFunctionName();
    }

    // for backward-compatibility, we continue to use .h file extension if it's already there
    wxFileName headerFile = BaseCppFile();
    headerFile.SetExt("h");
    if(headerFile.IsRelative()) {
        headerFile.MakeAbsolute(GetProjectPath());
    }
    m_useHpp = !headerFile.FileExists();
}

JSONElement wxcProjectMetadata::ToJSON()
{
    JSONElement metadata = JSONElement::createObject("metadata");
    UpdatePaths();

    metadata.addProperty("m_generatedFilesDir", m_generatedFilesDir);
    metadata.addProperty("m_objCounter", (int)m_objCounter);
    metadata.addProperty("m_includeFiles", m_includeFiles);
    metadata.addProperty("m_bitmapFunction", m_bitmapFunction);
    metadata.addProperty("m_bitmapsFile", m_bitmapsFile);
    metadata.addProperty("m_GenerateCodeTypes", m_GenerateCodeTypes);
    metadata.addProperty("m_outputFileName", m_outputFileName);
    metadata.addProperty("m_firstWindowId", m_firstWindowId);
    metadata.addProperty("m_useEnum", m_useEnum);
    metadata.addProperty("m_useUnderscoreMacro", m_useUnderscoreMacro);
    metadata.addProperty("m_addHandlers", m_addHandlers);
    return metadata;
}

void wxcProjectMetadata::AppendCustomControlsJSON(const wxArrayString& controls, JSONElement& element) const
{
    JSONElement customControls = wxcSettings::Get().GetCustomControlsAsJSON(controls);
    element.append(customControls);
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

    JSONRoot root(cJSON_Object);
    root.toElement().append(p.ToJSON());

    // The windows
    JSONElement windows = JSONElement::createArray("windows");
    root.toElement().append(windows);

    wxFFile fp(filename.GetFullPath(), "w+b");
    if(fp.IsOpened()) {

        wxcWidget::List_t::const_iterator iter = topLevelsList.begin();
        for(; iter != topLevelsList.end(); ++iter) {
            JSONElement obj = JSONElement::createObject();
            (*iter)->FixPaths(filename.GetPath()); // Fix abs paths to fit the new project file
            (*iter)->Serialize(obj);
            windows.arrayAppend(obj);
        }

        fp.Write(root.toElement().format(), wxConvUTF8);
        fp.Close();
    }
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
