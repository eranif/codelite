#ifndef __WXGUI_GLOBALS__H__
#define __WXGUI_GLOBALS__H__

#include <map>
#include <wx/string.h>
#include <wx/buffer.h>
#include <wx/listctrl.h>

namespace wxCrafter
{
void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText, int imgId = wxNOT_FOUND);

/**
 * \brief return column's text
 * \param list the list control
 * \param index the row ID
 * \param column the column ID
 * \return the column's text
 */
wxString GetColumnText(wxListCtrl* list, long index, long column);

/**
 * \brief append row to list control
 * \param list the list
 * \return new row index
 */
long AppendListCtrlRow(wxListCtrl* list);

bool ReadFileContent(const wxString& fileName, wxString& content);
bool ExtractFileFromZip(const wxString& zipPath, const wxString& filename, const wxString& targetDir,
                        wxString& targetFileName);
void ListZipFiles(const wxString& zipPath, wxArrayString& files);
}; // namespace wxCrafter

#endif // __GLOBALS__H__
