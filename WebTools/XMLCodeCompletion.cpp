#include "XMLCodeCompletion.h"
#include "WebToolsConfig.h"
#include "XMLBuffer.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "webtools.h"
#include "wxCodeCompletionBoxManager.h"
#include <wx/app.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

XMLCodeCompletion::XMLCodeCompletion(WebTools* plugin)
    : ServiceProvider("WebTools: XML", eServiceType::kCodeCompletion)
    , m_completeReason(kNone)
    , m_plugin(plugin)
{
    PrepareHtmlCompletions();
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &XMLCodeCompletion::OnCodeCompleted, this);

    WebToolsConfig& conf = WebToolsConfig::Get();
    m_htmlCcEnabeld = conf.HasHtmlFlag(WebToolsConfig::kHtmlEnableCC);
    m_xmlCcEnabled = conf.HasXmlFlag(WebToolsConfig::kXmlEnableCC);
    Bind(wxEVT_CC_CODE_COMPLETE, &XMLCodeCompletion::OnCodeComplete, this);
}

XMLCodeCompletion::~XMLCodeCompletion()
{
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &XMLCodeCompletion::OnCodeCompleted, this);
    Unbind(wxEVT_CC_CODE_COMPLETE, &XMLCodeCompletion::OnCodeComplete, this);
}

void XMLCodeCompletion::SuggestClosingTag(IEditor* editor, bool html)
{
    // CC was triggered by "</"
    // Read backward until we find the matching open tag
    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    XMLBuffer buffer(ctrl->GetTextRange(0, ctrl->GetCurrentPos()), html);
    buffer.Parse();
    if(buffer.InCData() || buffer.InComment()) {
        // dont offer code completion when inside CDATA or COMMENT blocks
        return;
    }

    XMLBuffer::Scope currentScope = buffer.GetCurrentScope();
    if(!currentScope.IsOk())
        return;

    wxCodeCompletionBox::BmpVec_t bitmaps;
    bitmaps.push_back(wxXmlResource::Get()->LoadBitmap("code-tags"));

    wxCodeCompletionBoxEntry::Vec_t entries;
    wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New("</" + currentScope.tag + ">", 0);
    entries.push_back(entry);

    m_completeReason = kCloseSequence;
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(editor->GetCtrl(), entries, bitmaps, 0, GetWordStartPos(editor),
                                                        this);
}

void XMLCodeCompletion::XmlCodeComplete(IEditor* editor)
{
    if(!m_xmlCcEnabled)
        return;

    // Perform XML code completion
    wxStyledTextCtrl* ctrl = editor->GetCtrl();

    wxChar ch = ctrl->GetCharAt(ctrl->PositionBefore(ctrl->GetCurrentPos()));
    if(ch == '/') {
        SuggestClosingTag(editor, false);

    } else {
        // CC was triggered by "<"
        // In this case, we simply trigger the word completion
        wxCommandEvent event(wxEVT_MENU, XRCID("simple_word_completion"));
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(event);
    }
}

void XMLCodeCompletion::HtmlCodeComplete(IEditor* editor)
{
    if(!m_htmlCcEnabeld)
        return;

    // Perform HTML code completion
    wxStyledTextCtrl* ctrl = editor->GetCtrl();

    wxChar ch = ctrl->GetCharAt(ctrl->PositionBefore(ctrl->GetCurrentPos()));
    if(ch == '/') {
        SuggestClosingTag(editor, true);

    } else {
        wxCodeCompletionBox::BmpVec_t bitmaps;
        bitmaps.push_back(wxXmlResource::Get()->LoadBitmap("code-tags"));

        wxCodeCompletionBoxEntry::Vec_t entries;
        for(size_t i = 0; i < m_htmlCompletions.size(); ++i) {
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(m_htmlCompletions.at(i).m_tag, 0);
            entry->SetComment(m_htmlCompletions.at(i).m_comment);
            entries.push_back(entry);
        }
        m_completeReason = kHtmlOpenSequence;
        wxCodeCompletionBoxManager::Get().ShowCompletionBox(editor->GetCtrl(), entries, bitmaps, 0,
                                                            GetWordStartPos(editor), this);
    }
}

void XMLCodeCompletion::PrepareHtmlCompletions()
{
    // Special complete patterns, the PIPE specifies the caret location
    m_completePattern.insert(std::make_pair("<a", "<a href=\"|\"></a>"));
    m_completePattern.insert(std::make_pair("<img", "<img src=\"|\" />"));
    m_completePattern.insert(std::make_pair("<?php", "<?php | ?>"));
    m_completePattern.insert(std::make_pair("<!--", "<!-- | -->"));

    // Create list of HTML tags
    m_htmlCompletions.push_back(HtmlCompletion("<?php", "Opens a PHP block within the HTML document"));
    m_htmlCompletions.push_back(HtmlCompletion("<!--", "Insert comment block"));
    m_htmlCompletions.push_back(HtmlCompletion("<!doctype", "Defines the document type"));
    m_htmlCompletions.push_back(HtmlCompletion("<a", "Defines a hyperlink"));
    m_htmlCompletions.push_back(HtmlCompletion("<abbr", "Defines an abbreviation or an acronym"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<acronym", "Not supported in HTML5. Use <abbr> instead. Defines an acronym"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<address", "Defines contact information for the author/owner of a document"));
    m_htmlCompletions.push_back(HtmlCompletion(
        "applet", "Not supported in HTML5. Use <embed> or <object> instead. Defines an embedded applet"));
    m_htmlCompletions.push_back(HtmlCompletion("<area", "Defines an area inside an image-map"));
    m_htmlCompletions.push_back(HtmlCompletion("<article", "Defines an article"));
    m_htmlCompletions.push_back(HtmlCompletion("<aside", "Defines content aside from the page content"));
    m_htmlCompletions.push_back(HtmlCompletion("<audio", "Defines sound content"));
    m_htmlCompletions.push_back(HtmlCompletion("<b", "Defines bold text"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<base", "Specifies the base URL/target for all relative URLs in a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<basefont",
                                               "Not supported in HTML5. Use CSS instead. Specifies a "
                                               "default color, size, and font for all text in a document"));
    m_htmlCompletions.push_back(HtmlCompletion(
        "bdi", "Isolates a part of text that might be formatted in a different direction from other text outside it"));
    m_htmlCompletions.push_back(HtmlCompletion("<bdo", "Overrides the current text direction"));
    m_htmlCompletions.push_back(HtmlCompletion("<big", "Not supported in HTML5. Use CSS instead. Defines big text"));
    m_htmlCompletions.push_back(HtmlCompletion("<blockquote", "Defines a section that is quoted from another source"));
    m_htmlCompletions.push_back(HtmlCompletion("<body", "Defines the document's body"));
    m_htmlCompletions.push_back(HtmlCompletion("<br", "Defines a single line break"));
    m_htmlCompletions.push_back(HtmlCompletion("<button", "Defines a clickable button"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<canvas", "Used to draw graphics, on the fly, via scripting (usually JavaScript)"));
    m_htmlCompletions.push_back(HtmlCompletion("<caption", "Defines a table caption"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<center", "Not supported in HTML5. Use CSS instead. Defines centered text"));
    m_htmlCompletions.push_back(HtmlCompletion("<cite", "Defines the title of a work"));
    m_htmlCompletions.push_back(HtmlCompletion("<code", "Defines a piece of computer code"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<col", "Specifies column properties for each column within a <colgroup> element "));
    m_htmlCompletions.push_back(
        HtmlCompletion("<colgroup", "Specifies a group of one or more columns in a table for formatting"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<datalist", "Specifies a list of pre-defined options for input controls"));
    m_htmlCompletions.push_back(HtmlCompletion("<dd", "Defines a description/value of a term in a description list"));
    m_htmlCompletions.push_back(HtmlCompletion("<del", "Defines text that has been deleted from a document"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<details", "Defines additional details that the user can view or hide"));
    m_htmlCompletions.push_back(HtmlCompletion("<dfn", "Represents the defining instance of a term"));
    m_htmlCompletions.push_back(HtmlCompletion("<dialog", "Defines a dialog box or window"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<dir", "Not supported in HTML5. Use <ul> instead. Defines a directory list"));
    m_htmlCompletions.push_back(HtmlCompletion("<div", "Defines a section in a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<dl", "Defines a description list"));
    m_htmlCompletions.push_back(HtmlCompletion("<dt", "Defines a term/name in a description list"));
    m_htmlCompletions.push_back(HtmlCompletion("<em", "Defines emphasized text "));
    m_htmlCompletions.push_back(HtmlCompletion("<embed", "Defines a container for an external (non-HTML) application"));
    m_htmlCompletions.push_back(HtmlCompletion("<fieldset", "Groups related elements in a form"));
    m_htmlCompletions.push_back(HtmlCompletion("<figcaption", "Defines a caption for a <figure> element"));
    m_htmlCompletions.push_back(HtmlCompletion("<figure", "Specifies self-contained content"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<font", "Not supported in HTML5. Use CSS instead. Defines font, color, and size for text"));
    m_htmlCompletions.push_back(HtmlCompletion("<footer", "Defines a footer for a document or section"));
    m_htmlCompletions.push_back(HtmlCompletion("<form", "Defines an HTML form for user input"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<frame", "Not supported in HTML5. Defines a window (a frame) in a frameset"));
    m_htmlCompletions.push_back(HtmlCompletion("<frameset", "Not supported in HTML5. Defines a set of frames"));
    m_htmlCompletions.push_back(HtmlCompletion("<h1", "Defines HTML headings 1"));
    m_htmlCompletions.push_back(HtmlCompletion("<h2", "Defines HTML headings 2"));
    m_htmlCompletions.push_back(HtmlCompletion("<h3", "Defines HTML headings 3"));
    m_htmlCompletions.push_back(HtmlCompletion("<h4", "Defines HTML headings 4"));
    m_htmlCompletions.push_back(HtmlCompletion("<h5", "Defines HTML headings 5"));
    m_htmlCompletions.push_back(HtmlCompletion("<h6", "Defines HTML headings 6"));
    m_htmlCompletions.push_back(HtmlCompletion("<head", "Defines information about the document"));
    m_htmlCompletions.push_back(HtmlCompletion("<header", "Defines a header for a document or section"));
    m_htmlCompletions.push_back(HtmlCompletion("<hr", "Defines a thematic change in the content"));
    m_htmlCompletions.push_back(HtmlCompletion("<html", "Defines the root of an HTML document"));
    m_htmlCompletions.push_back(HtmlCompletion("<i", "Defines a part of text in an alternate voice or mood"));
    m_htmlCompletions.push_back(HtmlCompletion("<iframe", "Defines an inline frame"));
    m_htmlCompletions.push_back(HtmlCompletion("<img", "Defines an image"));
    m_htmlCompletions.push_back(HtmlCompletion("<input", "Defines an input control"));
    m_htmlCompletions.push_back(HtmlCompletion("<ins", "Defines a text that has been inserted into a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<kbd", "Defines keyboard input"));
    m_htmlCompletions.push_back(HtmlCompletion("<keygen", "Defines a key-pair generator field (for forms)"));
    m_htmlCompletions.push_back(HtmlCompletion("<label", "Defines a label for an <input> element"));
    m_htmlCompletions.push_back(HtmlCompletion("<legend", "Defines a caption for a <fieldset> element"));
    m_htmlCompletions.push_back(HtmlCompletion("<li", "Defines a list item"));
    m_htmlCompletions.push_back(HtmlCompletion(
        "link",
        "Defines the relationship between a document and an external resource (most used to link to style sheets)"));
    m_htmlCompletions.push_back(HtmlCompletion("<main", "Specifies the main content of a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<map", "Defines a client-side image-map"));
    m_htmlCompletions.push_back(HtmlCompletion("<mark", "Defines marked/highlighted text"));
    m_htmlCompletions.push_back(HtmlCompletion("<menu", "Defines a list/menu of commands"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<menuitem", "Defines a command/menu item that the user can invoke from a popup menu"));
    m_htmlCompletions.push_back(HtmlCompletion("<meta", "Defines metadata about an HTML document"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<meter", "Defines a scalar measurement within a known range (a gauge)"));
    m_htmlCompletions.push_back(HtmlCompletion("<nav", "Defines navigation links"));
    m_htmlCompletions.push_back(HtmlCompletion(
        "noframes", "Not supported in HTML5. Defines an alternate content for users that do not support frames"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<noscript", "Defines an alternate content for users that do not support client-side scripts"));
    m_htmlCompletions.push_back(HtmlCompletion("<object", "Defines an embedded object"));
    m_htmlCompletions.push_back(HtmlCompletion("<ol", "Defines an ordered list"));
    m_htmlCompletions.push_back(HtmlCompletion("<optgroup", "Defines a group of related options in a drop-down list"));
    m_htmlCompletions.push_back(HtmlCompletion("<option", "Defines an option in a drop-down list"));
    m_htmlCompletions.push_back(HtmlCompletion("<output", "Defines the result of a calculation"));
    m_htmlCompletions.push_back(HtmlCompletion("<p", "Defines a paragraph"));
    m_htmlCompletions.push_back(HtmlCompletion("<param", "Defines a parameter for an object"));
    m_htmlCompletions.push_back(HtmlCompletion("<pre", "Defines preformatted text"));
    m_htmlCompletions.push_back(HtmlCompletion("<progress", "Represents the progress of a task"));
    m_htmlCompletions.push_back(HtmlCompletion("<q", "Defines a short quotation"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<rp", "Defines what to show in browsers that do not support ruby annotations"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<rt", "Defines an explanation/pronunciation of characters (for East Asian typography)"));
    m_htmlCompletions.push_back(HtmlCompletion("<ruby", "Defines a ruby annotation (for East Asian typography)"));
    m_htmlCompletions.push_back(HtmlCompletion("<s", "Defines text that is no longer correct"));
    m_htmlCompletions.push_back(HtmlCompletion("<samp", "Defines sample output from a computer program"));
    m_htmlCompletions.push_back(HtmlCompletion("<script", "Defines a client-side script"));
    m_htmlCompletions.push_back(HtmlCompletion("<section", "Defines a section in a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<select", "Defines a drop-down list"));
    m_htmlCompletions.push_back(HtmlCompletion("<small", "Defines smaller text"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<source", "Defines multiple media resources for media elements (<video> and <audio>)"));
    m_htmlCompletions.push_back(HtmlCompletion("<span", "Defines a section in a document"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<strike", "Not supported in HTML5. Use <del> or <s> instead. Defines strikethrough text"));
    m_htmlCompletions.push_back(HtmlCompletion("<strong", "Defines important text"));
    m_htmlCompletions.push_back(HtmlCompletion("<style", "Defines style information for a document"));
    m_htmlCompletions.push_back(HtmlCompletion("<sub", "Defines subscripted text"));
    m_htmlCompletions.push_back(HtmlCompletion("<summary", "Defines a visible heading for a <details> element"));
    m_htmlCompletions.push_back(HtmlCompletion("<sup", "Defines superscripted text"));
    m_htmlCompletions.push_back(HtmlCompletion("<table", "Defines a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<tbody", "Groups the body content in a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<td", "Defines a cell in a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<textarea", "Defines a multiline input control (text area)"));
    m_htmlCompletions.push_back(HtmlCompletion("<tfoot", "Groups the footer content in a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<th", "Defines a header cell in a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<thead", "Groups the header content in a table"));
    m_htmlCompletions.push_back(HtmlCompletion("<time", "Defines a date/time"));
    m_htmlCompletions.push_back(HtmlCompletion("<title", "Defines a title for the document"));
    m_htmlCompletions.push_back(HtmlCompletion("<tr", "Defines a row in a table"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<track", "Defines text tracks for media elements (<video> and <audio>)"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<tt", "Not supported in HTML5. Use CSS instead. Defines teletype text"));
    m_htmlCompletions.push_back(
        HtmlCompletion("<u", "Defines text that should be stylistically different from normal text"));
    m_htmlCompletions.push_back(HtmlCompletion("<ul", "Defines an unordered list"));
    m_htmlCompletions.push_back(HtmlCompletion("<var", "Defines a variable"));
    m_htmlCompletions.push_back(HtmlCompletion("<video", "Defines a video or movie"));
    m_htmlCompletions.push_back(HtmlCompletion("<wbr", "Defines a possible line-break"));
}

void XMLCodeCompletion::OnCodeCompleted(clCodeCompletionEvent& event)
{
    event.Skip();
    if(event.GetEventObject() != this) {
        return;
    }

    // sanity
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor)
        return;

    // HTML triggered the code complete?
    if(m_completeReason == kHtmlOpenSequence) {
        event.Skip(false);
        const wxString& selection = event.GetWord();
        if(XMLBuffer::IsEmptyHtmlTag(selection) && !HasSpecialInsertPattern(selection)) {

            // an empty html tag, just complete it
            wxString textToInsert = selection;
            textToInsert << ">";

            int selStart = GetWordStartPos(editor);
            int selEnd = editor->GetCurrentPosition();
            if((selEnd - selStart) >= 0) {
                editor->SelectText(selStart, selEnd - selStart);
                editor->ReplaceSelection(textToInsert);
                editor->SetCaretAt(selStart + textToInsert.length());
            }
        } else {
            wxString completePattern = GetCompletePattern(selection);
            int caretPos = completePattern.Find("|");
            completePattern.Replace("|", "");
            int selStart = GetWordStartPos(editor);

            int selEnd = editor->GetCurrentPosition();
            if((selEnd - selStart) >= 0) {
                editor->SelectText(selStart, selEnd - selStart);
                editor->ReplaceSelection(completePattern);
                editor->SetCaretAt(selStart + caretPos);
            }
        }
    } else if(m_completeReason == kCloseSequence) {
        // User typed "</"
        event.Skip(false);
        const wxString& selection = event.GetWord();
        int selStart = GetWordStartPos(editor);
        int selEnd = editor->GetCurrentPosition();
        if((selEnd - selStart) >= 0) {
            editor->SelectText(selStart, selEnd - selStart);
            editor->ReplaceSelection(selection);
            editor->SetCaretAt(selStart + selection.length());
        }
    } else {
        event.Skip();
    }
}

wxString XMLCodeCompletion::GetCompletePattern(const wxString& tag) const
{
    if(m_completePattern.find(tag.Lower()) == m_completePattern.end()) {
        // The default:
        // <tag>|</tag>
        wxString t = tag;
        if(t.StartsWith("<")) {
            t.Remove(0, 1);
        }
        return wxString() << "<" << t << ">|</" << t << ">";
    } else {
        return m_completePattern.find(tag.Lower())->second;
    }
}

bool XMLCodeCompletion::HasSpecialInsertPattern(const wxString& tag) const
{
    return m_completePattern.count(tag.Lower());
}

void XMLCodeCompletion::Reload()
{
    WebToolsConfig& conf = WebToolsConfig::Get();
    m_htmlCcEnabeld = conf.HasHtmlFlag(WebToolsConfig::kHtmlEnableCC);
    m_xmlCcEnabled = conf.HasXmlFlag(WebToolsConfig::kXmlEnableCC);
}

int XMLCodeCompletion::GetWordStartPos(IEditor* editor)
{
    // Dont use WordStartPos / WordEndPos, instead, search for the first "<"
    // from this line backward and use that as the first insert position
    int minPos = editor->PosFromLine(editor->GetCurrentLine());
    int curpos = editor->GetCurrentPosition() - 1;
    for(int i = curpos; i >= minPos; --i) {
        if(editor->GetCharAtPos(i) == '<') {
            return i;
        }
    }
    return editor->WordStartPos(editor->GetCurrentPosition(), true);
}

void XMLCodeCompletion::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = GetEditor(event.GetFileName());
    if(editor && editor->GetCtrl()->GetLexer() == wxSTC_LEX_XML) {
        // an XML file
        event.Skip(false);
        XmlCodeComplete(editor);
    } else if(editor && m_plugin->IsHTMLFile(editor)) {
        // Html code completion
        event.Skip(false);
        HtmlCodeComplete(editor);
    }
}

IEditor* XMLCodeCompletion::GetEditor(const wxString& filename) const
{
    auto editor = clGetManager()->FindEditor(filename);
    if(editor && editor == clGetManager()->GetActiveEditor()) {
        return editor;
    }
    return nullptr;
}
