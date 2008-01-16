#include "editor_creator.h"
#include "editor.h"
#include "wx/window.h"
#include "frame.h"


EditorCreator::EditorCreator()
: m_parent(NULL)
{
}


EditorCreator::~EditorCreator()
{
}

void EditorCreator::SetParent(wxWindow *parent)
{
	m_parent = parent;
}

LEditor *EditorCreator::NewInstance()
{
	return new LEditor(Frame::Get()->GetNotebook(), wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, false);
}

void EditorCreator::Add(LEditor *editor)
{
	m_queue.push_back(editor);
}

size_t EditorCreator::GetQueueCount()
{
	return m_queue.size();
}
