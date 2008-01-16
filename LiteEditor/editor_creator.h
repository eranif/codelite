#ifndef EDITOR_CREATOR_H
#define EDITOR_CREATOR_H

#include "singleton.h"
#include "wx/string.h"
#include "deque"

class LEditor;
class wxWindow;

/**
 * Under GTK, a creation of Scintilla object is slow, the creator runs a background thread
 * the always creates scintilla objects and keep them in a pool
 */
class EditorCreator {
	friend class Singleton<EditorCreator>;
	std::deque<LEditor*> m_queue;
	wxWindow *m_parent;

public:
	EditorCreator();
	virtual ~EditorCreator();

	LEditor *NewInstance();
	void SetParent(wxWindow *parent);
	void Add(LEditor *editor);
	size_t GetQueueCount();
};

typedef Singleton<EditorCreator> EditorCreatorST;

#endif // EDITOR_CREATOR_H
