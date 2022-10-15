#ifndef SOURCEFORMATTERBASE_HPP
#define SOURCEFORMATTERBASE_HPP

#include "JSON.h"
#include "cl_command_event.h"
#include "fileextmanager.h"
#include "macros.h"
#include "wxStringHash.h"

#include <vector>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/string.h>

/// Defines the formatter interface
enum class FormatterFlags {
    // Is this formatter enabled?
    ENABLED = (1 << 0),

    // The formatter support file config (e.g. `.clang-format`)
    HAS_FILE_CONFIG = (1 << 1),

    // Indicate that this formatter fixes the file inline
    // and reload should take place after a successful formatting
    INPLACE_EDIT = (1 << 2),

    // trigger a formatting after saving the file
    FORMAT_ON_SAVE = (1 << 3),
};

#define __HAS_FLAG(flags, bit) (flags & (size_t)bit)
#define __SET_FLAG(flags, bit) (flags |= (size_t)bit)
#define __UNSET_FLAG(flags, bit) (flags &= ~(size_t)bit)

wxDECLARE_EVENT(wxEVT_FORMAT_COMPELTED, clSourceFormatEvent);
wxDECLARE_EVENT(wxEVT_FORMAT_INPLACE_COMPELTED, clSourceFormatEvent);

class SourceFormatterBase : public wxEvtHandler
{
protected:
    wxArrayString m_languages;
    size_t m_flags = (size_t)FormatterFlags::ENABLED;
    wxString m_configFile;
    wxString m_name;
    wxString m_description;
    wxString m_shortDescription;

protected:
    void SetFlag(FormatterFlags flag, bool set)
    {
        if(set) {
            __SET_FLAG(m_flags, flag);
        } else {
            __UNSET_FLAG(m_flags, flag);
        }
    }

    bool HasFlag(FormatterFlags flag) const { return __HAS_FLAG(m_flags, flag); }

public:
    SourceFormatterBase();
    virtual ~SourceFormatterBase();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    /**
     * @brief can this formatter handle `file_type`?
     */
    bool CanHandle(FileExtManager::FileType file_type) const;
    bool IsConfigFileSuppoerted() const { return HasFlag(FormatterFlags::HAS_FILE_CONFIG); }

    bool IsEnabled() const { return HasFlag(FormatterFlags::ENABLED); }
    void SetEnabled(bool b) { SetFlag(FormatterFlags::ENABLED, b); }

    bool IsFormatOnSave() const { return HasFlag(FormatterFlags::FORMAT_ON_SAVE); }
    void SetFormatOnSave(bool b) { SetFlag(FormatterFlags::FORMAT_ON_SAVE, b); }

    void SetInplaceFormatter(bool b) { SetFlag(FormatterFlags::INPLACE_EDIT, b); }
    bool IsInplaceFormatter() const { return HasFlag(FormatterFlags::INPLACE_EDIT); }

    virtual bool FormatFile(const wxFileName& file_path, wxEvtHandler* sink) = 0;
    virtual bool FormatFile(const wxString& file_path, wxEvtHandler* sink) = 0;
    virtual bool FormatRemoteFile(const wxString& file_path, wxEvtHandler* sink) = 0;
    virtual bool FormatString(const wxString& content, const wxString& fullpath, wxString* output) = 0;

    void SetFileTypes(const std::vector<FileExtManager::FileType>& types);
    void SetLanguages(const wxArrayString& langs) { m_languages = langs; }
    const wxArrayString& GetLanguages() const { return m_languages; }

    void SetConfigFilepath(const wxString& filepath);
    const wxString& GetConfigFilepath() const { return m_configFile; }

    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }

    void SetDescription(const wxString& description) { this->m_description = description; }
    const wxString& GetDescription() const { return m_description; }

    void SetShortDescription(const wxString& shortDescription) { this->m_shortDescription = shortDescription; }
    const wxString& GetShortDescription() const { return m_shortDescription; }
};

#endif // SOURCEFORMATTERBASE_HPP
