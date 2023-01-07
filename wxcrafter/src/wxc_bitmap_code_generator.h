#ifndef WXCBITMAPCODEGENERATOR_H
#define WXCBITMAPCODEGENERATOR_H

#include <asyncprocess.h>
#include <macros.h>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/filename.h>

extern const wxEventType wxEVT_BITMAP_CODE_GENERATION_DONE;

class TopLevelWinWrapper;
class wxcCodeGeneratorHelper : public wxEvtHandler
{
    typedef std::map<wxString, wxString> MapString_t;

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

private:
    wxcCodeGeneratorHelper();
    virtual ~wxcCodeGeneratorHelper();
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
    bool CreateXRC();
    wxString GenerateInitCode(TopLevelWinWrapper* tw) const;
    wxString GenerateExternCode() const;
    wxString GenerateWinIdEnum() const;
    wxString BitmapCode(const wxString& bmp, const wxString& bmpname = wxEmptyString) const;
    bool Contains(const wxString& bmp) const { return m_bitmapMap.count(bmp); }
};

#endif // WXCBITMAPCODEGENERATOR_H
