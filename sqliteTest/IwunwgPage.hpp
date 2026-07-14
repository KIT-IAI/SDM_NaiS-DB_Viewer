#pragma once

class IwunwgPage : public wxPanel
{
public:
	IwunwgPage(wxWindow* parent, sqlite3* db);
	~IwunwgPage();

private:
	wxArrayString getBuildingFunctionStrings();
	wxArrayString getElementStrings();
	void BindEvent();

	void showDB();
	void updateDB();
	void getIwuEntryNum();

	void enterYear(wxCommandEvent& evt);
	void selectBuildingFunc(wxCommandEvent& evt);
	void selectElement(wxCommandEvent& evt);

private:
	sqlite3* m_db;
	int m_iwu_entry_num;
	wxDataViewListCtrl* m_db_table;
	wxStaticText* m_iwu_search_result_text;
	wxTextCtrl* m_year_textctrl;
	wxComboBox* m_buildingFunc_combobox;
	wxComboBox* m_element_combobox;
};