#include "wxprec.hpp"
#include <iostream>
#include <set>
#include <numeric>
#include <stdlib.h>
#include "sqlite3.h"

#include "utils.hpp"
#include "CrremPage.hpp"
#include "sqlTest_id.hpp"
#include "dataSources.hpp"

CrremPage::CrremPage(wxWindow* parent, sqlite3* db)
	: m_db(db)
{
	Create(parent, ID_CRREM_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	getCO2num();
	getKwhNum();
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* searchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* searchGrid = new wxFlexGridSizer(0, 2, 0, 0);
	searchGrid->SetFlexibleDirection(wxBOTH);
	searchGrid->AddGrowableCol(0);
	searchGrid->AddGrowableCol(1);
	searchGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto source_text = new wxStaticText(this, wxID_ANY, "Data source");
	searchGrid->Add(source_text, 0, wxALL | wxEXPAND, 5);

	auto source_text_content = new wxStaticText(this, wxID_ANY, "CRREM");
	searchGrid->Add(source_text_content, 0, wxALL | wxEXPAND, 5);

	auto scenario_text = new wxStaticText(this, wxID_ANY, "Scenario");
	searchGrid->Add(scenario_text, 0, wxALL | wxEXPAND, 5);

	auto scenario_text_content = new wxStaticText(this, wxID_ANY, "1.5 C");
	searchGrid->Add(scenario_text_content, 0, wxALL | wxEXPAND, 5);

	auto unit_text = new wxStaticText(this, wxID_ANY, "Unit");
	searchGrid->Add(unit_text, 0, wxALL | wxEXPAND, 5);

	m_unit_combobox = new wxComboBox(this, ID_CRREM_UNIT_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getUnitStrings(), 0);
	m_unit_combobox->SetSelection(0);
	searchGrid->Add(m_unit_combobox, 0, wxALL | wxEXPAND, 5);

	auto country_text = new wxStaticText(this, wxID_ANY, "Country");
	searchGrid->Add(country_text, 0, wxALL | wxEXPAND, 5);

	m_country_combobox = new wxComboBox(this, ID_CRREM_COUNTRY_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getCountryStrings(), 0);
	searchGrid->Add(m_country_combobox, 0, wxALL | wxEXPAND, 5);

	auto asset_text = new wxStaticText(this, wxID_ANY, "Asset label");
	searchGrid->Add(asset_text, 0, wxALL | wxEXPAND, 5);

	m_asset_combobox = new wxComboBox(this, ID_CRREM_ASSET_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getAssetStrings(), 0);
	searchGrid->Add(m_asset_combobox, 0, wxALL | wxEXPAND, 5);

	auto year_text = new wxStaticText(this, wxID_ANY, "Year");
	searchGrid->Add(year_text, 0, wxALL | wxEXPAND, 5);

	m_year_textCtrl = new wxTextCtrl(this, ID_CRREM_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_year_textCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	searchGrid->Add(m_year_textCtrl, 0, wxALL | wxEXPAND, 5);	

	searchSizer->Add(searchGrid, 0, wxALL | wxEXPAND, 5);

	m_crrem_search_results_text = new wxStaticText(this, wxID_ANY, "");
	if (m_unit_combobox->GetSelection() == 0)
	{
		m_crrem_search_results_text->SetLabel(std::to_string(m_co2_num) + " entries in total.");
	}
	else
	{
		m_crrem_search_results_text->SetLabel(std::to_string(m_kwh_num) + " entries in total.");
	}	
	searchSizer->Add(m_crrem_search_results_text, 0, wxALL | wxEXPAND, 5);

	topSizer->Add(searchSizer, 1, wxALL | wxEXPAND, 5);

	auto verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	topSizer->Add(verticalLine, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);

	m_table = new wxDataViewListCtrl(this, ID_CRREM_TABLE);
	m_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	tableSizer->Add(m_table, 1, wxALL | wxEXPAND, 5);
	topSizer->Add(tableSizer, 4, wxALL | wxEXPAND, 5);

	this->SetSizer(topSizer);
	this->Layout();
	this->Centre(wxBOTH);

	showDB();
	updateDB();
	
	BindEvent();
}

CrremPage::~CrremPage()
{
}

void CrremPage::BindEvent()
{
	Bind(wxEVT_COMBOBOX, &CrremPage::selectUnit, this, ID_CRREM_UNIT_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &CrremPage::selectAsset, this, ID_CRREM_ASSET_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &CrremPage::selectCountry, this, ID_CRREM_COUNTRY_COMBOBOX);
	Bind(wxEVT_TEXT, &CrremPage::enterYear, this, ID_CRREM_YEAR_TEXTCTRL);
}

void CrremPage::getCO2num()
{
	std::string sql = "SELECT COUNT(*) FROM " + CRREM_1_5C_CO2 + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_co2_num = 0;
	}

	int count = -1;

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
	}
	else
	{
		std::cerr << "sqlite3_step failed: " << sqlite3_errmsg(m_db) << std::endl;
	}

	sqlite3_finalize(stmt);
	m_co2_num = count;
}

void CrremPage::getKwhNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + CRREM_1_5C_KWH + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_kwh_num = 0;
	}

	int count = -1;

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
	}
	else
	{
		std::cerr << "sqlite3_step failed: " << sqlite3_errmsg(m_db) << std::endl;
	}

	sqlite3_finalize(stmt);
	m_kwh_num = count;
}

wxArrayString CrremPage::getUnitStrings()
{
	wxArrayString units(1);

	units.Add("kgCO2/sqm");
	units.Add("kWh/sqm");

	return units;
}

wxArrayString CrremPage::getCountryStrings()
{
	if (!(m_unit_combobox->GetSelection() == wxNOT_FOUND || m_unit_combobox->GetStringSelection() == ""))
	{
		auto unit = m_unit_combobox->GetStringSelection().utf8_string();

		if (unit == "kgCO2/sqm")
		{
			return co2GetCountries();
		}
		else if (unit == "kWh/sqm")
		{
			return euiGetCountries();
		}
	}
}

wxArrayString CrremPage::getAssetStrings()
{
	if (!(m_unit_combobox->GetSelection() == wxNOT_FOUND || m_unit_combobox->GetStringSelection() == ""))
	{
		auto unit = m_unit_combobox->GetStringSelection().utf8_string();

		if (unit == "kgCO2/sqm")
		{
			return co2GetAssetLabels();
		}
		else if (unit == "kWh/sqm")
		{
			return euiGetAssetLabels();
		}
	}
}

void CrremPage::showDB()
{
	m_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_table->AppendTextColumn("year", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_table->AppendTextColumn("asset_type", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_table->AppendTextColumn("value", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void CrremPage::updateDB()
{
	int num = 0;
	m_table->DeleteAllItems();

	std::string sentence;

	if (!(m_unit_combobox->GetSelection() == wxNOT_FOUND || m_unit_combobox->GetStringSelection() == ""))
	{
		auto unit = m_unit_combobox->GetStringSelection().utf8_string();

		if (unit == "kgCO2/sqm")
		{
			sentence = "SELECT * FROM " + CRREM_1_5C_CO2 + " WHERE id NOTNULL";
		}
		else if (unit == "kWh/sqm")
		{
			sentence = "SELECT * FROM " + CRREM_1_5C_KWH + " WHERE id NOTNULL";
		}
	}	

	if (!(m_country_combobox->GetSelection() == wxNOT_FOUND || m_country_combobox->GetStringSelection() == ""))
	{
		auto country = m_country_combobox->GetStringSelection().utf8_string();
		sentence += " AND country IS " + quoteWord(country);
	}

	if (!(m_asset_combobox->GetSelection() == wxNOT_FOUND || m_asset_combobox->GetStringSelection() == ""))
	{
		auto asset = m_asset_combobox->GetStringSelection().utf8_string();
		sentence += " AND asset_label IS " + quoteWord(asset);
	}

	if (m_year_textCtrl->GetValue() != "")
	{
		auto year = m_year_textCtrl->GetValue().utf8_string();
		sentence += " AND year IS " + year;
	}

	sentence += ";";
	
	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			++num;
			wxVector<wxVariant> cells;

			for (int i = 0; i < 7; ++i)
			{
				if (i != 4 && i != 6)
				{
					auto str = transform(stmt, i);
					cells.push_back(str);
				}				
			}

			m_table->AppendItem(cells);
			m_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);

	if (m_unit_combobox->GetSelection() == 0)
	{
		if (num == m_co2_num)
		{
			m_crrem_search_results_text->SetLabel(std::to_string(m_co2_num) + " entries in total.");
		}
		else
		{
			m_crrem_search_results_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_co2_num) + " entries.");
		}
	}
	else
	{
		if (num == m_kwh_num)
		{
			m_crrem_search_results_text->SetLabel(std::to_string(m_kwh_num) + " entries in total.");
		}
		else
		{
			m_crrem_search_results_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_kwh_num) + " entries.");
		}
	}
}

void CrremPage::selectUnit(wxCommandEvent& evt)
{
	updateDB();
}

void CrremPage::selectCountry(wxCommandEvent& evt)
{
	updateDB();
}

void CrremPage::selectAsset(wxCommandEvent& evt)
{
	updateDB();
}

void CrremPage::enterYear(wxCommandEvent& evt)
{
	updateDB();
}

wxArrayString CrremPage::co2GetCountries()
{
	wxArrayString countryStrings(1);
	countryStrings.Add("");

	std::set<std::string> countries;	

	std::string sentence = "SELECT * FROM " + CRREM_1_5C_CO2;
	const char* sqlSentence = sentence.c_str();
	sqlite3_stmt* stmt = NULL;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto country = transform(stmt, 1);
			countries.insert(country);
		}
	}
	sqlite3_finalize(stmt);

	for (auto c : countries)
	{
		countryStrings.Add(c);
	}

	return countryStrings;
}

wxArrayString CrremPage::co2GetAssetLabels()
{
	wxArrayString assetStrings(1);
	assetStrings.Add("");

	std::set<std::string> assets;

	std::string sentence = "SELECT * FROM " + CRREM_1_5C_CO2;
	const char* sqlSentence = sentence.c_str();
	sqlite3_stmt* stmt = NULL;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto asset = transform(stmt, 3);
			assets.insert(asset);
		}
	}
	sqlite3_finalize(stmt);

	for (auto c : assets)
	{
		assetStrings.Add(c);
	}

	return assetStrings;
}

wxArrayString CrremPage::euiGetCountries()
{
	wxArrayString countryStrings(1);
	countryStrings.Add("");

	std::set<std::string> countries;

	std::string sentence = "SELECT * FROM " + CRREM_1_5C_KWH;
	const char* sqlSentence = sentence.c_str();
	sqlite3_stmt* stmt = NULL;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto country = transform(stmt, 1);
			countries.insert(country);
		}
	}
	sqlite3_finalize(stmt);

	for (auto c : countries)
	{
		countryStrings.Add(c);
	}

	return countryStrings;
}

wxArrayString CrremPage::euiGetAssetLabels()
{
	wxArrayString assetStrings(1);
	assetStrings.Add("");

	std::set<std::string> assets;

	std::string sentence = "SELECT * FROM " + CRREM_1_5C_KWH;
	const char* sqlSentence = sentence.c_str();
	sqlite3_stmt* stmt = NULL;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto asset = transform(stmt, 3);
			assets.insert(asset);
		}
	}
	sqlite3_finalize(stmt);

	for (auto c : assets)
	{
		assetStrings.Add(c);
	}

	return assetStrings;
}
