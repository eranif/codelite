#include "clBoostrapWizard.h"
#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "CompilersDetectorManager.h"
#include "build_settings_config.h"
#include <wx/wupdlock.h>
#include <wx/dcmemory.h>

const wxString sampleText = "class Demo\n"
                            "{\n"
                            "private:\n"
                            "    std::string m_str;\n"
                            "    int m_integer;\n"
                            "    \n"
                            "public:\n"
                            "    /**\n"
                            "     * Creates a new demo.\n"
                            "     * @param o The object to demonstrate.\n"
                            "     */\n"
                            "    Demo(const Demo &other) {\n"
                            "        m_str = other.m_str;\n"
                            "        m_integer = other.m_integer;\n"
                            "    }\n"
                            "}";

clBoostrapWizard::clBoostrapWizard(wxWindow* parent)
    : clBoostrapWizardBase(parent)
{
    wxArrayString themes = ColoursAndFontsManager::Get().GetAvailableThemesForLexer("c++");
    m_choiceTheme->Append(themes);
    m_choiceTheme->SetSelection(m_choiceTheme->FindString("Monokai_2"));
    m_stc24->SetText(sampleText);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", "Monokai_2");
    if(lexer) {
        lexer->Apply(m_stc24, true);
    }
    m_stc24->SetKeyWords(1, "Demo std string");
    m_stc24->SetKeyWords(3, "other");

    ::clRecalculateSTCHScrollBar(m_stc24);
    m_stc24->SetEditable(false);
}

clBoostrapWizard::~clBoostrapWizard() {}

void clBoostrapWizard::OnThemeSelected(wxCommandEvent& event)
{
    m_stc24->SetEditable(true);
    wxString selection = m_choiceTheme->GetStringSelection();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", selection);
    if(lexer) {
        lexer->Apply(m_stc24, true);
    }
    m_stc24->SetKeyWords(1, "Demo std string");
    m_stc24->SetKeyWords(3, "other");
    ::clRecalculateSTCHScrollBar(m_stc24);
    m_stc24->SetEditable(false);
}
void clBoostrapWizard::OnScanForCompilers(wxCommandEvent& event)
{
    wxBusyCursor bc;
    m_compilers.clear();
    wxWindowUpdateLocker locker(m_wizardPageCompilers);
    m_cmdLnkBtnScanForCompilers->Hide();
    m_dvListCtrlCompilers->Show();

    CompilersDetectorManager detector;
    if(detector.Locate()) {
        m_compilers = detector.GetCompilersFound();
        for(size_t i = 0; i < m_compilers.size(); ++i) {
            wxVector<wxVariant> cols;
            cols.push_back(m_compilers.at(i)->GetName());
            cols.push_back(m_compilers.at(i)->GetInstallationPath());
            m_dvListCtrlCompilers->AppendItem(cols);
        }

        if(!detector.FoundMinGWCompiler()) {
            CompilersDetectorManager::MSWSuggestToDownloadMinGW();
        }

    } else {
        // nothing found on this machine, offer to download
        CompilersDetectorManager::MSWSuggestToDownloadMinGW();
    }
    m_wizardPageCompilers->GetSizer()->Layout();
}

wxBitmap clBoostrapWizard::GenerateBitmap(size_t labelIndex)
{
    wxArrayString labels;
    labels.Add("Compilers");
    labels.Add("Colours");
    labels.Add("Whitespace");

    wxBitmap bmp(100, 400);
    wxMemoryDC memDC(bmp);
    memDC.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    memDC.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    memDC.DrawRectangle(wxRect(bmp.GetSize()));
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont boldFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    boldFont.SetWeight(wxFONTWEIGHT_BOLD);

    wxBitmap arrowRight = wxXmlResource::Get()->LoadBitmap("next-16");
    for(size_t i = 0; i < labels.size(); ++i) {
        wxSize textSize = memDC.GetTextExtent("Tp");
        int rectHeight = textSize.y + 20; // 10 pixels margin x 2
        wxRect rect(0, i * rectHeight, bmp.GetWidth(), rectHeight);

        // Draw the text (align to the right)
        wxCoord textX, textY, bmpX, bmpY;
        memDC.SetFont(font);
        memDC.SetTextForeground(i == labelIndex ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) :
                                                  wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        memDC.SetFont(i == labelIndex ? boldFont : font);
        textSize = memDC.GetTextExtent(labels.Item(i));
        textX = /*bmp.GetWidth() - textSize.GetWidth() - 16*/ 16;
        textY = ((rect.GetHeight() - textSize.GetHeight()) / 2) + rect.GetY();
        memDC.DrawText(labels.Item(i), textX, textY);

        if(i == labelIndex) {
            bmpX = 0;
            bmpY = ((rect.GetHeight() - arrowRight.GetHeight()) / 2) + rect.GetY();
            memDC.DrawBitmap(arrowRight, bmpX, bmpY);
        }
    }
    memDC.SelectObject(wxNullBitmap);
    return bmp;
}

clBootstrapData clBoostrapWizard::GetData()
{
    clBootstrapData data;
    data.compilers = m_compilers;
    data.selectedTheme = m_choiceTheme->GetStringSelection();
    data.useTabs = (m_radioBoxSpacesVsTabs->GetSelection() == 1);
    data.whitespaceVisibility = m_radioBoxWhitespaceVisibility->GetSelection();
    return data;
}
