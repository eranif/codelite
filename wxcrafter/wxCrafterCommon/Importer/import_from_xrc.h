#ifndef IMPORTFROMXRC_H
#define IMPORTFROMXRC_H

#include "wxc_widget.h"

namespace ImportFromXrc
{

void DoProcessButtonNode(const wxXmlNode* node, wxcWidget* wrapper);
void ProcessBitmapProperty(const wxXmlNode* node,
                           wxcWidget* wrapper,
                           const wxString& property = PROP_BITMAP_PATH,
                           const wxString& client_hint = "");
wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented);
void ProcessNamedNode(wxXmlNode* node,
                      wxcWidget* parentwrapper,
                      const wxString& name); // Used for auitoolbar dropdown nodes

} // namespace ImportFromXrc

#endif // IMPORTFROMXRC_H
