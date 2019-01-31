#include "wxgui_globals.h"
#include <wx/busyinfo.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

namespace wxCrafter
{
void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText, int imgId)
{
    wxListItem list_item;
    list_item.SetId(indx);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list_item.SetText(rText);
    list_item.SetImage(imgId);
    list->SetItem(list_item);
}

wxString GetColumnText(wxListCtrl* list, long index, long column)
{
    wxListItem list_item;
    list_item.SetId(index);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list->GetItem(list_item);
    return list_item.GetText();
}

long AppendListCtrlRow(wxListCtrl* list)
{
    long item;
    list->GetItemCount() ? item = list->GetItemCount() : item = 0;

    wxListItem info;
    // Set the item display name
    info.SetColumn(0);
    info.SetId(item);
    item = list->InsertItem(info);
    return item;
}

bool ReadFileContent(const wxString& fileName, wxString& content)
{
    wxLogNull noLog;
    content.Clear();
    wxFFile file(fileName, wxT("rb"));

    if(file.IsOpened()) {

        file.ReadAll(&content);
        if(content.IsEmpty()) {
            // now try the Utf8
            file.ReadAll(&content, wxConvUTF8);
        }
    }
    return !content.IsEmpty();
}

bool ExtractFileFromZip(const wxString& zipPath, const wxString& filename, const wxString& targetDir,
                        wxString& targetFileName)
{
    wxZipEntry* entry(NULL);
    wxFFileInputStream in(zipPath);
    wxZipInputStream zip(in);

    // Make sure the target directory exists...
    wxLogNull noLog;
    wxMkdir(targetDir);

    wxString lowerCaseName(filename);
    lowerCaseName.MakeLower();

    entry = zip.GetNextEntry();
    while(entry) {
        wxString name = entry->GetName();
        name.MakeLower();
        name.Replace(wxT("\\"), wxT("/"));

        if(name == lowerCaseName) {
            targetFileName.Clear();
            name.Replace(wxT("/"), wxT("_"));
            targetFileName << targetDir << "/" << name;
            wxFFileOutputStream out(targetFileName);
            zip.Read(out);
            out.Close();
            delete entry;
            return true;
        }

        delete entry;
        entry = zip.GetNextEntry();
    }
    return false;
}

void ListZipFiles(const wxString& zipPath, wxArrayString& files)
{
    wxZipEntry* entry(NULL);
    wxFFileInputStream in(zipPath);
    wxZipInputStream zip(in);

    entry = zip.GetNextEntry();
    while(entry) {
        wxString name = entry->GetName();
        name.Replace(wxT("\\"), wxT("/"));
        files.Add(name);
        delete entry;

        entry = zip.GetNextEntry();
    }
}
} // namespace wxCrafter
