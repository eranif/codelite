#ifndef __quickfinder__
#define __quickfinder__

#include <wx/string.h>
#include <vector>
#include "entry.h"

class IManager;

class QuickFinder {
	static IManager *m_manager;
	private:
	static bool FilterAndDisplayTags(const std::vector<TagEntryPtr> &tags, const wxString &matchPattern);
public:

	static void Initialize(IManager *manager);

	/**
	 * @brief open a workspace file
	 * @param s
	 * @return
	 */
	static bool OpenWorkspaceFile(const wxString& s);

	/**
	 * @brief open and display user selection dialog for a given kind
	 * @param s
	 */
	static bool OpenType(const wxString &s, const wxArrayString &kind);

	/**
	 * @brief set the focus to the active editor
	 */
	static void FocusActiveEditor();
};
#endif // __quickfinder__
