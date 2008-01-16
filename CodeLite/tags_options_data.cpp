#include "tags_options_data.h"

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData() 
: SerializedObject()
, m_ccFlags(0)
, m_fileSpec(wxT("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++"))
{
	m_languages.Add(wxT("C++"));
	m_languages.Add(wxT("Java"));
}

TagsOptionsData::~TagsOptionsData()
{
}

void TagsOptionsData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_ccFlags"), m_ccFlags);
	arch.Write(wxT("m_prep"), m_prep);
	arch.Write(wxT("m_fileSpec"), m_fileSpec);
	arch.Write(wxT("m_languages"), m_languages);
}

void TagsOptionsData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_ccFlags"), m_ccFlags);
	arch.Read(wxT("m_prep"), m_prep);
	arch.Read(wxT("m_fileSpec"), m_fileSpec);
	arch.Read(wxT("m_languages"), m_languages);
}

wxString TagsOptionsData::ToString() const
{
	wxString options(wxEmptyString);
	if(m_prep.GetCount() > 0){
		options = wxT(" -I");
		for(size_t i=0; i<m_prep.GetCount(); i++){
			wxString item = GetPreprocessor().Item(i);
			item = item.Trim();
			item = item.Trim(false);
			if(item.IsEmpty()){
				continue;
			}
			options += item;
			options += wxT(",");
		}
		options.RemoveLast();
		options += wxT(" ");
	}
	
	if(GetLanguages().IsEmpty() == false){
		options += wxT(" --language-force=");
		options += GetLanguages().Item(0);
		options += wxT(" ");
	}
	return options;
}

void TagsOptionsData::SetLanguageSelection(const wxString &lang)
{
	int where = m_languages.Index(lang);
	if(where != wxNOT_FOUND)
	{
		m_languages.RemoveAt(where);
	}
	m_languages.Insert(lang, 0);
}
