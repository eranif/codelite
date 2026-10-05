#ifndef WXCBITMAPCODEGENERATOR_H
#define WXCBITMAPCODEGENERATOR_H

#include "clResult.hpp"
#include "macros.h"

#include <functional>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/filename.h>

extern const wxEventType wxEVT_BITMAP_CODE_GENERATION_DONE;

class TopLevelWinWrapper;
class wxXmlDocument;
class wxcCodeGeneratorHelper : public wxEvtHandler
{
    using MapString_t = std::map<wxString, wxString>;

protected:
    MapString_t m_bitmapMap;
    wxArrayString m_icons;
    wxFileName m_xrcFile;
    wxFileName m_cppFile;
    wxFileName m_destCPP;
    wxString m_wxrcOutput;
    wxStringSet_t m_winIds;

protected:
    bool IsGenerateNeeded() const;

    /**
     * @brief build the XRC that describes the bitmaps of the project. Also set the XRC / CPP file names
     * @param requestDesignerRefresh called if a bitmap file was modified outside wxCrafter
     * @param doc [output] the XRC document
     * @param xrcText [output] the XRC document as text
     * @param isBitmapModifiedOutside [output] true if a bitmap file is newer than the XRC file
     */
    clStatus PrepareBitmapsXrc(std::function<void()> requestDesignerRefresh,
                               wxXmlDocument& doc,
                               wxString& xrcText,
                               bool& isBitmapModifiedOutside);

private:
    wxcCodeGeneratorHelper();
    ~wxcCodeGeneratorHelper() override = default;
    wxString GenerateTopLevelWindowIconCode() const;

public:
    static wxcCodeGeneratorHelper& Get();
    /**
     * @brief stop the worker thread
     */
    void UnInitialize();

    void Clear();
    void ClearWindowIds();
    void AddIcon(const wxString& bitmapFile);
    void ClearIcons();
    wxString AddBitmap(const wxString& bitmapFile, const wxString& name = wxEmptyString);
    void AddWindowId(const wxString& winid);
    clStatus CreateXRC(std::function<void()> requestDesignerRefresh,
                       std::function<void()> bitmapGenerationStart,
                       std::function<void()> bitmapGenerationEnd,
                       std::function<void(const wxFileName&)> onFileSaved);
    /**
     * @brief generate the bitmaps code and return it as a string, instead of writing it to a "_bitmaps.cpp"
     * file. The code should be placed inside the base class source file (see wxCrafter::WriteGeneratedOutput)
     */
    clStatusOr<wxString> GenerateBitmapsCode(std::function<void()> requestDesignerRefresh,
                                             std::function<void()> bitmapGenerationStart,
                                             std::function<void()> bitmapGenerationEnd);

    /**
     * @brief return true if the bitmaps code should be generated inside the base class source file (an option, see
     * wxcSettings::BITMAPS_IN_BASE_CLASS_FILE) and false if it should be written to its own file (the default,
     * see CreateXRC)
     */
    bool EmbedBitmapsInBaseClassFile() const;

    wxString GenerateInitCode(TopLevelWinWrapper* tw) const;
    wxString GenerateExternCode() const;
    wxString GenerateWinIdEnum() const;
    wxString BitmapCode(const wxString& bmp, const wxString& bmpname = wxEmptyString) const;
    bool Contains(const wxString& bmp) const { return m_bitmapMap.count(bmp); }

    /// The generated bitmaps source file. Valid after CreateXRC() was called.
    const wxFileName& GetBitmapsCppFile() const { return m_destCPP; }
};

#endif // WXCBITMAPCODEGENERATOR_H
