#include "wxprec.hpp"
#include <iostream>
#include <numeric>
#include <stdlib.h>
#include "sqlite3.h"

#include "utils.hpp"
#include "IwunwgPage.hpp"
#include "sqlTest_id.hpp"
#include "dataSources.hpp"

IwunwgPage::IwunwgPage(wxWindow* parent, sqlite3* db)
	: m_db(db)
{
	Create(parent, ID_IWU_NWG_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	getIwuEntryNum();
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* searchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* searchGrid = new wxFlexGridSizer(0, 2, 0, 0);
	searchGrid->SetFlexibleDirection(wxBOTH);
	searchGrid->AddGrowableCol(0);
	searchGrid->AddGrowableCol(1);
	searchGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto source_text = new wxStaticText(this, wxID_ANY, "Data source");
	searchGrid->Add(source_text, 0, wxALL | wxEXPAND, 5);

	auto source_text_content = new wxStaticText(this, wxID_ANY, "IWU NWG (2023-11-24)");
	searchGrid->Add(source_text_content, 0, wxALL | wxEXPAND, 5);

	auto buildingUse_text = new wxStaticText(this, wxID_ANY, "Building Use");
	searchGrid->Add(buildingUse_text, 0, wxALL | wxEXPAND, 5);

	auto buildingUse_content = new wxStaticText(this, wxID_ANY, "Non-residential building");
	searchGrid->Add(buildingUse_content, 0, wxALL | wxEXPAND, 5);	

	auto year_text = new wxStaticText(this, wxID_ANY, "Construction year");
	searchGrid->Add(year_text, 0, wxALL | wxEXPAND, 5);

	m_year_textctrl = new wxTextCtrl(this, ID_IWU_NWG_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_year_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	searchGrid->Add(m_year_textctrl, 0, wxALL | wxEXPAND, 5);

	auto buildingFunc_text = new wxStaticText(this, wxID_ANY, "Building function");
	searchGrid->Add(buildingFunc_text, 0, wxALL | wxEXPAND, 5);

	m_buildingFunc_combobox = new wxComboBox(this, ID_IWU_NWG_BUILDING_FUNCTION_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getBuildingFunctionStrings(), 0);
	searchGrid->Add(m_buildingFunc_combobox, 0, wxALL | wxEXPAND, 5);

	auto element_text = new wxStaticText(this, wxID_ANY, "Building element");
	searchGrid->Add(element_text, 0, wxALL | wxEXPAND, 5);	

	m_element_combobox = new wxComboBox(this, ID_IWU_NWG_BUILDING_ELEMENT_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getElementStrings(), 0);
	searchGrid->Add(m_element_combobox, 0, wxALL | wxEXPAND, 5);	

	searchSizer->Add(searchGrid, 0, wxALL | wxEXPAND, 5);

	m_iwu_search_result_text = new wxStaticText(this, wxID_ANY, "");
	m_iwu_search_result_text->SetLabel(std::to_string(m_iwu_entry_num) + " entries of building type in total.");
	searchSizer->Add(m_iwu_search_result_text, 0, wxALL | wxEXPAND, 5);

	topSizer->Add(searchSizer, 1, wxALL | wxEXPAND, 5);

	auto vLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	topSizer->Add(vLine1, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);

	m_db_table = new wxDataViewListCtrl(this, ID_IWU_NWG_DB_DATAVIEWLIST_CTRL);
	m_db_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	tableSizer->Add(m_db_table, 2, wxALL | wxEXPAND, 5);

	topSizer->Add(tableSizer, 3, wxALL | wxEXPAND, 5);

	showDB();
	updateDB();

	BindEvent();
	this->SetSizer(topSizer);
	this->Layout();
	topSizer->Fit(this);
}

IwunwgPage::~IwunwgPage()
{
}

wxArrayString IwunwgPage::getBuildingFunctionStrings()
{
	wxArrayString types(1);

	types.Add("");
	types.Add(wxString("Büro-, Verwaltungs- oder Amtsgebäude"));
	types.Add(wxString("Gebäude für Forschung und Hochschullehre"));
	types.Add(wxString("Gebäude für Gesundheit und Pflege"));
	types.Add(wxString("Schule, Kindertagesstätte und sonstiges Betreuungsgebäude"));
	types.Add(wxString("Gebäude für Kultur und Freizeit"));
	types.Add(wxString("Sportgebäude"));
	types.Add(wxString("Beherbergungs- oder Unterbringungsgebäude, Gastronomie- oder Verpflegungsgebäude"));
	types.Add(wxString("Produktions-, Werkstatt-, Lager- oder Betriebsgebäude"));
	types.Add(wxString("Handelsgebäude"));
	types.Add(wxString("Technikgebäude (Ver- und Entsorgung)"));
	types.Add(wxString("Verkehrsgebäude"));

	return types;
}

wxArrayString IwunwgPage::getElementStrings()
{
	wxArrayString types(1);

	types.Add("");
	types.Add(wxString("Außenwand"));
	types.Add(wxString("Boden"));
	types.Add(wxString("Fenster"));
	types.Add(wxString("Opakes Dach"));

	return types;
}

void IwunwgPage::BindEvent()
{
	Bind(wxEVT_COMBOBOX, &IwunwgPage::selectBuildingFunc, this, ID_IWU_NWG_BUILDING_FUNCTION_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &IwunwgPage::selectElement, this, ID_IWU_NWG_BUILDING_ELEMENT_COMBOBOX);
	Bind(wxEVT_TEXT, &IwunwgPage::enterYear, this, ID_IWU_NWG_YEAR_TEXTCTRL);
}

void IwunwgPage::showDB()
{
	m_db_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("building function", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("building element", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("first year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("last year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("u value", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void IwunwgPage::updateDB()
{
	int num = 0;

	m_db_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + IWU_NWG + " WHERE id NOTNULL";

	if (!(m_buildingFunc_combobox->GetSelection() == wxNOT_FOUND || m_buildingFunc_combobox->GetStringSelection() == ""))
	{
		auto bFunc = m_buildingFunc_combobox->GetStringSelection().utf8_string();
		sentence += " AND building_function IS " + quoteWord(bFunc);
	}

	if (!(m_element_combobox->GetSelection() == wxNOT_FOUND || m_element_combobox->GetStringSelection() == ""))
	{
		auto element = m_element_combobox->GetStringSelection().utf8_string();
		sentence += " AND building_element IS " + quoteWord(element);
	}

	if (m_year_textctrl->GetValue() != "")
	{
		int year = std::stoi(m_year_textctrl->GetValue().utf8_string());
		sentence += " AND first_year <= " + std::to_string(year) + " AND last_year >= " + std::to_string(year);
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

			for (int i = 0; i < 6; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_db_table->AppendItem(cells);
			m_db_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);

	if (num == m_iwu_entry_num)
	{
		m_iwu_search_result_text->SetLabel(std::to_string(m_iwu_entry_num) + " entries of building type in total.");
	}
	else
	{
		m_iwu_search_result_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_iwu_entry_num) + " building types.");
	}
}

void IwunwgPage::getIwuEntryNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + IWU_NWG + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_iwu_entry_num = 0;
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
	m_iwu_entry_num = count;
}

void IwunwgPage::enterYear(wxCommandEvent& evt)
{
	updateDB();
}

void IwunwgPage::selectBuildingFunc(wxCommandEvent& evt)
{
	updateDB();
}

void IwunwgPage::selectElement(wxCommandEvent& evt)
{
	updateDB();
}
