#pragma once
class CrremPage : public wxPanel
{
public:
	CrremPage(wxWindow* parent, sqlite3* db);
	~CrremPage();

private:

	void BindEvent();
	void getCO2num();
	void getKwhNum();
	wxArrayString getUnitStrings();
	wxArrayString getCountryStrings();
	wxArrayString getAssetStrings();
	wxArrayString co2GetCountries();
	wxArrayString co2GetAssetLabels();
	wxArrayString euiGetAssetLabels();
	wxArrayString euiGetCountries();

	void showDB();
	void updateDB();

	void selectUnit(wxCommandEvent& evt);
	void selectCountry(wxCommandEvent& evt);
	void selectAsset(wxCommandEvent& evt);	
	void enterYear(wxCommandEvent& evt);

private:
	sqlite3* m_db;

	int m_co2_num;
	int m_kwh_num;

	wxStaticText* m_crrem_search_results_text;	

	wxDataViewListCtrl* m_table;
	wxTextCtrl* m_year_textCtrl;
	wxComboBox* m_unit_combobox;
	wxComboBox* m_country_combobox;
	wxComboBox* m_asset_combobox;
};

