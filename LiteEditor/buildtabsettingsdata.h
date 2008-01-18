#ifndef __buildtabsettingsdata__
#define __buildtabsettingsdata__

#include "serialized_object.h"

class BuildTabSettingsData : public SerializedObject {
	bool m_skipWarnings;
	wxString m_warnColour;
	wxString m_errorColour;
	
private:
	BuildTabSettingsData(const BuildTabSettingsData& rhs);
	BuildTabSettingsData& operator=(const BuildTabSettingsData& rhs);

public:
	BuildTabSettingsData();
	~BuildTabSettingsData();

	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);
	
	
	//Setters
	void SetErrorColour(const wxString& errorColour) {this->m_errorColour = errorColour;}
	void SetSkipWarnings(const bool& skipWarnings) {this->m_skipWarnings = skipWarnings;}
	void SetWarnColour(const wxString& warnColour) {this->m_warnColour = warnColour;}
	//Getters
	const wxString& GetErrorColour() const {return m_errorColour;}
	const bool& GetSkipWarnings() const {return m_skipWarnings;}
	const wxString& GetWarnColour() const {return m_warnColour;}
	
};
#endif // __buildtabsettingsdata__
