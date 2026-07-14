#ifndef VISUALIZERMGR_H
#define VISUALIZERMGR_H

#include "events_database.h"
#include "wxgui_bitmaploader.h"

#include <map>
#include <wx/imaglist.h>

class wxcWidget;

class wxMenu;
/**
 * @class Allocator
 * @author eran
 * @date 9/25/2012
 * @file allocator_mgr.h
 * @brief
 */
class Allocator
{
public:
    enum {
        INSERT_MAIN_SIZER,
        INSERT_SIBLING,
        INSERT_CHILD,
        INSERT_TOP_LEVEL,
        INSERT_PROMPT_CHILD_OR_SIBLING,
        INSERT_NONE
    };

protected:
    static Allocator* ms_instance;
    using ImageMap_t = std::map<int, int>;
    using InsertMap_t = std::map<int, std::map<int, int>>;

protected:
    wxImageList* m_imageList;
    ImageMap_t m_imageIds;
    InsertMap_t m_relations;
    wxCrafter::ResourceLoader m_bmpLoader;

    void DoLink(int selected, int aboutToBeInsert, int relation);
    void DoLinkAll();

public:
    static Allocator* Instance();
    static void Release();

private:
    Allocator();
    virtual ~Allocator() = default;

public:
    // int GetInsertionType(int controlId, int targetControlId) const
    int GetInsertionType(int controlId, int targetControlId, bool allowPrompt, wxcWidget* selectedWidget = NULL) const;
    bool CanPaste(wxcWidget* source, wxcWidget* target) const;

    void Register(wxcWidget* obj, const wxString& bmpname, int id = -1);
    wxImageList* GetImageList() { return m_imageList; }

    int GetImageId(int controlId) const;
};

#endif // VISUALIZERMGR_H
