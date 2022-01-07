#ifndef WXCPROJECTMETADATA_H
#define WXCPROJECTMETADATA_H

#include "json_node.h"
#include "wxc_widget.h"

#include <wx/event.h>
#include <wx/filename.h>
#include <wx/string.h>

extern const wxEventType wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED;
extern const wxEventType wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED;

class wxcProjectMetadata
{
    enum GenerateCodeType { wxcGenerateCPPCode = 1, wxcGenerateXRC };

protected:
    wxString m_projectFile;
    wxString m_bitmapsFile;
    size_t m_objCounter;
    wxString m_generatedFilesDir;
    wxArrayString m_includeFiles;
    wxFileName m_generatedHeader;  // Derived class
    wxFileName m_generatedSource;  // Derived class
    wxString m_generatedClassName; // Derived class
    wxString m_virtualFolder;
    wxString m_bitmapFunction;
    int m_GenerateCodeTypes;
    CustomControlTemplateMap_t m_projctCustomControls;
    wxStringMap_t m_additionalFiles;
    wxString m_outputFileName;
    bool m_useHpp;
    int m_firstWindowId;
    bool m_useEnum;
    bool m_useUnderscoreMacro;
    bool m_addHandlers;

private:
    wxcProjectMetadata();
    virtual ~wxcProjectMetadata();
    void DoGenerateBitmapFunctionName();
    wxString DoGenerateBitmapsFile() const;

public:
    /**
     * @brief fix various paths to be relative
     */
    void UpdatePaths();

    void SetAddHandlers(bool addHandlers) { this->m_addHandlers = addHandlers; }
    bool IsAddHandlers() const { return m_addHandlers; }

    void SetProjctCustomControls(const CustomControlTemplateMap_t& projctCustomControls)
    {
        this->m_projctCustomControls = projctCustomControls;
    }
    const CustomControlTemplateMap_t& GetProjctCustomControls() const { return m_projctCustomControls; }

    static wxcProjectMetadata& Get();
    void Reset();
    void ClearAggregatedData();

    void SetFirstWindowId(int firstWindowId) { this->m_firstWindowId = firstWindowId; }
    void SetUseEnum(bool useEnum) { this->m_useEnum = useEnum; }
    int GetFirstWindowId() const { return m_firstWindowId; }
    bool IsUseEnum() const { return m_useEnum; }
    void SetOutputFileName(const wxString& outputFileName) { this->m_outputFileName = outputFileName; }
    wxString GetOutputFileName() const;
    wxString GetHeaderFileExt() const;

    bool IsLoaded() const { return m_projectFile.IsEmpty() == false; }

    wxString GetCppFileName() const;
    wxString GetHeaderFileName() const;
    wxString GetXrcFileName() const;

    void SetBitmapsFile(const wxString& bitmapsFile) { this->m_bitmapsFile = bitmapsFile; }

    wxString GetBitmapsFile() const;

    void SetAdditionalFiles(const wxStringMap_t& additionalFiles) { this->m_additionalFiles = additionalFiles; }
    const wxStringMap_t& GetAdditionalFiles() const { return m_additionalFiles; }
    void SetGeneratedClassName(const wxString& generatedClassName) { this->m_generatedClassName = generatedClassName; }
    const wxString& GetGeneratedClassName() const { return m_generatedClassName; }
    void SetBitmapFunction(const wxString& bitmapFunction) { this->m_bitmapFunction = bitmapFunction; }
    const wxString& GetBitmapFunction() const { return m_bitmapFunction; }
    void SetVirtualFolder(const wxString& virtualFolder) { this->m_virtualFolder = virtualFolder; }
    const wxString& GetVirtualFolder() const { return m_virtualFolder; }
    void SetGeneratedHeader(const wxFileName& generatedHeader) { this->m_generatedHeader = generatedHeader; }
    void SetGeneratedSource(const wxFileName& generatedSource) { this->m_generatedSource = generatedSource; }
    const wxFileName& GetGeneratedHeader() const { return m_generatedHeader; }
    const wxFileName& GetGeneratedSource() const { return m_generatedSource; }
    void SetIncludeFiles(const wxArrayString& includeFiles) { this->m_includeFiles = includeFiles; }
    const wxArrayString& GetIncludeFiles() const { return m_includeFiles; }
    wxString GetProjectPath() const;
    void SetGeneratedFilesDir(const wxString& generatedFilesDir) { this->m_generatedFilesDir = generatedFilesDir; }
    wxFileName BaseCppFile() const;
    wxFileName BaseHeaderFile() const;
    wxString GetGeneratedFilesDir() const;
    void SetProjectFile(const wxString& filename);
    void SetObjCounter(size_t objCounter) { this->m_objCounter = objCounter; }
    const wxString& GetProjectFile() const { return m_projectFile; }
    wxFileName GetProjectFileName() const { return wxFileName(m_projectFile); }
    size_t GetObjCounter() const { return m_objCounter; }

    void SetUseUnderscoreMacro(bool useUnderscoreMacro) { this->m_useUnderscoreMacro = useUnderscoreMacro; }
    bool IsUseUnderscoreMacro() const { return m_useUnderscoreMacro; }

    void GenerateBitmapFunctionName() { DoGenerateBitmapFunctionName(); }

    void AppendCustomControlsJSON(const wxArrayString& controls, JSONElement& element) const;
    JSONElement ToJSON();
    void FromJSON(const JSONElement& json);

    void SetGenerateCPPCode(bool GenerateCPPCode)
    {
        GenerateCPPCode ? m_GenerateCodeTypes |= wxcGenerateCPPCode : m_GenerateCodeTypes &= ~wxcGenerateCPPCode;
    }
    void SetGenerateXRC(bool GenerateXRC)
    {
        GenerateXRC ? m_GenerateCodeTypes |= wxcGenerateXRC : m_GenerateCodeTypes &= ~wxcGenerateXRC;
    }
    bool GetGenerateCPPCode() const { return m_GenerateCodeTypes & wxcGenerateCPPCode; }
    bool GetGenerateXRC() const { return m_GenerateCodeTypes & wxcGenerateXRC; }

    /**
     * @brief save list of top level wrappers into filename. In addition this method also saves the
     * an empty project content
     */
    void Serialize(const wxcWidget::List_t& topLevelsList, const wxFileName& filename);
};

#endif // WXCPROJECTMETADATA_H
