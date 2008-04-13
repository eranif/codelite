#ifndef CPP_COMMENT_CREATOR_H
#define CPP_COMMENT_CREATOR_H

#include "wx/string.h"
#include "entry.h"
#include "comment_creator.h"

class CppCommentCreator : public CommentCreator
{
	TagEntryPtr m_tag;
public:
	CppCommentCreator(TagEntryPtr tag, wxChar keyPrefix);
	virtual ~CppCommentCreator();
	virtual wxString CreateComment();

private:
	wxString FunctionComment();
};

#endif //CPP_COMMENT_CREATOR_H
