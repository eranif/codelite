#include "open_resource_dialog.h"
#include "bitmap_loader.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "window_locker.h"
#include "editor_config.h"
#include "ieditor.h"
#include "ctags_manager.h"
#include <wx/filefn.h>
#include "project.h"
#include "workspace.h"
#include <wx/wupdlock.h>
#include "imanager.h"
#include "windowattrmanager.h"
#include <vector>

BEGIN_EVENT_TABLE(OpenResourceDialog, OpenResourceDialogBase)
    EVT_TIMER(XRCID("OR_TIMER"), OpenResourceDialog::OnTimer)
END_EVENT_TABLE()

OpenResourceDialog::OpenResourceDialog( wxWindow* parent, IManager *manager)
    : OpenResourceDialogBase( parent )
    , m_manager(manager)
    , m_needRefresh(false)
{
    Hide();
    // Create an image list
    wxImageList *li = new wxImageList(16, 16, true);
    BitmapLoader *bmpLoader = m_manager->GetStdIcons();

    m_tagImgMap[wxT("class")]              = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));
    m_tagImgMap[wxT("struct")]             = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct")));
    m_tagImgMap[wxT("namespace")]          = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/namespace")));
    m_tagImgMap[wxT("typedef")]            = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));
    m_tagImgMap[wxT("member_private")]     = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_private")));
    m_tagImgMap[wxT("member_public")]      = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));
    m_tagImgMap[wxT("member_protected")]   = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_protected")));
    m_tagImgMap[wxT("function_private")]   = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));
    m_tagImgMap[wxT("function_public")]    = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));
    m_tagImgMap[wxT("function_protected")] = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected")));
    m_tagImgMap[wxT("enum")]               = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/enum")));
    m_tagImgMap[wxT("enumerator")]         = li->Add(bmpLoader->LoadBitmap(wxT("cc/16/enumerator")));
    m_tagImgMap[wxT("cpp")]                = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));
    m_tagImgMap[wxT("h")]                  = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/h")));
    m_tagImgMap[wxT("text")]               = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/text")));
    m_tagImgMap[wxT("c")]                  = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/c")));
    m_tagImgMap[wxT("wxfb")]               = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/wxfb")));
    m_tagImgMap[wxT("wxcp")]               = li->Add(bmpLoader->LoadBitmap(wxT("mime/16/wxcp")));

    m_listOptions->AssignImageList(li, wxIMAGE_LIST_SMALL);

    m_timer = new wxTimer(this, XRCID("OR_TIMER"));
    MSWSetNativeTheme(m_listOptions);
    m_listOptions->InsertColumn(0, wxT("Name"));
    m_listOptions->InsertColumn(1, wxT("Full path"));

    m_textCtrlResourceName->SetFocus();
    SetLabel(_("Open resource..."));

    WindowAttrManager::Load(this, wxT("OpenResourceDialog"), m_manager->GetConfigTool());

    //load all files from the workspace
    if ( m_manager->IsWorkspaceOpen() ) {
        wxArrayString projects;
        m_manager->GetWorkspace()->GetProjectList( projects );

        for ( size_t i=0; i<projects.GetCount(); i++ ) {
            std::vector<wxFileName> fileNames;
            wxString errmsg;
            ProjectPtr p = m_manager->GetWorkspace()->FindProjectByName(projects.Item(i), errmsg);

            if ( p ) {
                p->GetFiles(fileNames, true);

                //convert std::vector to wxArrayString
                for ( std::vector<wxFileName>::iterator it = fileNames.begin(); it != fileNames.end(); it ++ ) {
                    wxString name = it->GetFullName().MakeLower();
                    m_files.insert(std::make_pair(name, it->GetFullPath()) );
                }
            }
        }
    }
    m_listOptions->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( OpenResourceDialog::OnItemActivated ), NULL, this );
    m_listOptions->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED,  wxListEventHandler( OpenResourceDialog::OnItemSelected ), NULL, this );
}

OpenResourceDialog::~OpenResourceDialog()
{
    m_timer->Stop();
    delete m_timer;
    WindowAttrManager::Save(this, wxT("OpenResourceDialog"), m_manager->GetConfigTool());
}

void OpenResourceDialog::OnText( wxCommandEvent& event )
{
    event.Skip();
    m_timer->Stop();
    m_timer->Start(200, true);

    wxString filter = m_textCtrlResourceName->GetValue();
    filter.Trim().Trim(false);

    if(filter.IsEmpty()) {
        // The filter content is cleared, delete all entries
        Clear();
        m_needRefresh = false;

    } else {
        m_needRefresh = true;

    }
}

void OpenResourceDialog::OnUsePartialMatching( wxCommandEvent& event )
{
    wxUnusedVar(event);
    DoPopulateList();
    m_textCtrlResourceName->SetFocus();
}

void OpenResourceDialog::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int sel = m_listOptions->GetFirstSelected();
    if (sel != wxNOT_FOUND) {
        OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(sel);
        if (data) {
            m_selection = *data;
            EndModal(wxID_OK);
        }
    }
}

void OpenResourceDialog::OnItemActivated(wxListEvent& event)
{
    int sel = event.m_itemIndex;
    if (sel != wxNOT_FOUND) {
        OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(sel);
        if (data) {
            m_selection = *data;
            EndModal(wxID_OK);
        }
    }
}

void OpenResourceDialog::DoPopulateList()
{
    wxString name = m_textCtrlResourceName->GetValue();
    name.Trim().Trim(false);
    if(name.IsEmpty())
        return;

    Clear();

    wxWindowUpdateLocker locker(m_listOptions);

    // First add the workspace files
    
    // Prepare the user filter
    m_userFilters.Clear();
    m_userFilters = ::wxStringTokenize(name, " \t", wxTOKEN_STRTOK);
    for(size_t i=0; i<m_userFilters.GetCount(); ++i) {
        m_userFilters.Item(i).MakeLower();
    }
    
    DoPopulateWorkspaceFile();
    DoPopulateTags();

    if(m_listOptions->GetItemCount()) {
        m_listOptions->SetColumnWidth(0, wxLIST_AUTOSIZE);
        m_listOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
    } else {
        m_listOptions->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
        m_listOptions->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    }
}

void OpenResourceDialog::DoPopulateTags()
{
    bool gotExactMatch(false);

    // Next, add the tags
    TagEntryPtrVector_t tags;
    if ( m_userFilters.IsEmpty() )
        return;
    
    m_manager->GetTagsManager()->GetTagsByPartialName(m_userFilters.Item(0), tags);

    for (size_t i=0; i<tags.size(); i++) {
        TagEntryPtr tag = tags.at(i);
        
        // Filter out non relevanting entries
        if ( !m_filters.IsEmpty() && m_filters.Index(tag->GetKind()) == wxNOT_FOUND )
            continue;
        
        if ( !MatchesFilter( tag->GetName()) )
            continue;
            
        wxString name(tag->GetName());

        // keep the fullpath
        int index(0);
        wxString fullname;
        if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype")) {
            fullname = wxString::Format(wxT("%s::%s%s"), tag->GetScope().c_str(), tag->GetName().c_str(), tag->GetSignature().c_str());
            index = DoAppendLine(tag->GetName(),
                                 fullname,
                                 (tag->GetKind() == wxT("function")),
                                 new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(), tag->GetName(), tag->GetScope()),
                                 DoGetTagImgId(tag));
        } else {

            fullname = wxString::Format(wxT("%s::%s"), tag->GetScope().c_str(), tag->GetName().c_str());
            index = DoAppendLine(tag->GetName(),
                                 fullname,
                                 false,
                                 new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(), tag->GetName(), tag->GetScope()),
                                 DoGetTagImgId(tag));

        }
        
        if ((m_userFilters.GetCount() == 1) && (m_userFilters.Item(0).CmpNoCase(name) == 0) && !gotExactMatch) {
            gotExactMatch = true;
            DoSelectItem(index);
        }
    }

    if (!gotExactMatch && m_listOptions->GetItemCount()) {
        DoSelectItem(0);
    }
}

void OpenResourceDialog::DoPopulateWorkspaceFile()
{
    // do we need to include files?
    if ( !m_filters.IsEmpty() && m_filters.Index(TagEntry::KIND_FILE) == wxNOT_FOUND)
        return;
    
    if ( !m_userFilters.IsEmpty() ) {

        std::multimap<wxString, wxString>::iterator iter  = m_files.begin();
        for(; iter != m_files.end(); iter++) {
            
            if ( !MatchesFilter( iter->first ) )
                continue;
            
            wxFileName fn(iter->second);
            FileExtManager::FileType type = FileExtManager::GetType(fn.GetFullName());
            int imgId = m_tagImgMap[wxT("text")];
            switch(type) {
            case FileExtManager::TypeSourceC:
                imgId = m_tagImgMap[wxT("c")];
                break;

            case FileExtManager::TypeSourceCpp:
                imgId = m_tagImgMap[wxT("cpp")];
                break;
            case FileExtManager::TypeHeader:
                imgId = m_tagImgMap[wxT("h")];
                break;
            case FileExtManager::TypeFormbuilder:
                imgId = m_tagImgMap[wxT("wxfb")];
                break;
            case FileExtManager::TypeWxCrafter:
                imgId = m_tagImgMap[wxT("wxcp")];
                break;
            default:
                break;
            }
            DoAppendLine(fn.GetFullName(),
                         fn.GetFullPath(),
                         false,
                         new OpenResourceDialogItemData(fn.GetFullPath(), -1, wxT(""), fn.GetFullName(), wxT("")),
                         imgId);
        }
    }
}

void OpenResourceDialog::Clear()
{
    // list control does not own the client data, we need to free it ourselves
    for(int i=0; i<m_listOptions->GetItemCount(); i++) {
        OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(i);
        if (data) {
            delete data;
        }
    }
    m_listOptions->DeleteAllItems();
    m_userFilters.Clear();
}

void OpenResourceDialog::OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager)
{
    if ( manager && manager->OpenFile(selection.m_file, wxEmptyString, selection.m_line) ) {
        IEditor *editor = manager->GetActiveEditor();
        if ( editor && !selection.m_name.IsEmpty() && !selection.m_pattern.IsEmpty()) {
            editor->FindAndSelectV(selection.m_pattern, selection.m_name);
        }
    }
}

void OpenResourceDialog::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DOWN && m_listOptions->GetItemCount()> 0) {
        //up key
        int cursel = m_listOptions->GetFirstSelected();
        if (cursel != wxNOT_FOUND) {
            //there is a selection in the listbox
            cursel++;
            if (cursel >= (int)m_listOptions->GetItemCount()) {
                //already at last item, cant scroll anymore
                return;
            }
            DoSelectItem(cursel);

        } else {
            //no selection is made
            DoSelectItem(0);
        }
        return;

    } else if (event.GetKeyCode() == WXK_UP && m_listOptions->GetItemCount() > 0) {
        //up key
        int cursel = m_listOptions->GetFirstSelected();
        if (cursel != wxNOT_FOUND) {
            //there is a selection in the listbox
            cursel--;
            if (cursel < 0) {
                //already at first item, cant scroll anymore
                return;
            }
            DoSelectItem(cursel);

        } else {
            //no selection is made
            DoSelectItem(0);
        }
        return;

    } else
        event.Skip();
}

void OpenResourceDialog::OnOK(wxCommandEvent& event)
{
    event.Skip();
}

void OpenResourceDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(m_selection.IsOk());
}


bool OpenResourceDialogItemData::IsOk() const
{
    return m_file.IsEmpty() == false;
}

void OpenResourceDialog::DoSelectItem(int selection, bool makeFirst)
{
    // Unselect current item first
    int currentSelection = m_listOptions->GetFirstSelected();
    if(currentSelection != wxNOT_FOUND) {
        m_listOptions->Select(currentSelection, false);
    }

    m_listOptions->Select(selection);
    if(makeFirst)
        m_listOptions->EnsureVisible(selection);

    // display the full name at the bottom static text control
    OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(selection);
    m_selection = *data;
}

void OpenResourceDialog::OnItemSelected(wxListEvent& event)
{
    event.Skip();
    if(event.m_itemIndex != wxNOT_FOUND) {
        // display the full name at the bottom static text control
        OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(event.m_itemIndex );
        m_selection = *data;
    }
}

int OpenResourceDialog::DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont, OpenResourceDialogItemData* clientData, int imgId)
{
    int index = AppendListCtrlRow(m_listOptions);
    SetColumnText(m_listOptions, index, 0, name, imgId);
    SetColumnText(m_listOptions, index, 1, fullname);

    m_listOptions->SetItemPtrData(index, (wxUIntPtr)(clientData));

    // Mark implementations with bold font
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if(boldFont)
        font.SetWeight(wxFONTWEIGHT_BOLD);
    m_listOptions->SetItemFont(index, font);
    return index;
}

void OpenResourceDialog::OnTimer(wxTimerEvent& event)
{
    if(m_needRefresh)
        DoPopulateList();

    m_needRefresh = false;
}

int OpenResourceDialog::DoGetTagImgId(TagEntryPtr tag)
{
    wxString kind   = tag->GetKind();
    wxString access = tag->GetAccess();
    int imgId = m_tagImgMap[wxT("text")];
    if (kind == wxT("class"))
        imgId = m_tagImgMap[wxT("class")];

    if (kind == wxT("struct"))
        imgId = m_tagImgMap[wxT("struct")];

    if (kind == wxT("namespace"))
        imgId = m_tagImgMap[wxT("namespace")];

    if (kind == wxT("variable"))
        imgId = m_tagImgMap[wxT("member_public")];

    if (kind == wxT("typedef"))
        imgId = m_tagImgMap[wxT("typedef")];

    if (kind == wxT("member") && access.Contains(wxT("private")))
        imgId = m_tagImgMap[wxT("member_private")];

    if (kind == wxT("member") && access.Contains(wxT("public")))
        imgId = m_tagImgMap[wxT("member_public")];

    if (kind == wxT("member") && access.Contains(wxT("protected")))
        imgId = m_tagImgMap[wxT("member_protected")];

    if (kind == wxT("member"))
        imgId = m_tagImgMap[wxT("member_public")];

    if ((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private")))
        imgId = m_tagImgMap[wxT("function_private")];

    if ((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
        imgId = m_tagImgMap[wxT("function_public")];

    if ((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected")))
        imgId = m_tagImgMap[wxT("function_protected")];

    if (kind == wxT("macro"))
        imgId = m_tagImgMap[wxT("typedef")];

    if (kind == wxT("enum"))
        imgId = m_tagImgMap[wxT("enum")];

    if (kind == wxT("enumerator"))
        imgId = m_tagImgMap[wxT("enumerator")];

    return imgId;
}

bool OpenResourceDialog::MatchesFilter(const wxString& name)
{
    wxString tmpname = name;
    tmpname.MakeLower();
    
    if ( m_userFilters.IsEmpty() )
        return false;
    
    
    for(size_t i=0; i<m_userFilters.GetCount(); ++i) {
        if ( !tmpname.Contains(m_userFilters.Item(i)) ) 
            return false;
    }
    return true;
}
