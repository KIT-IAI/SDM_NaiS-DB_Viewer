#include "wxprec.hpp"
#include <iostream>
#include <numeric>
#include <stdlib.h>
#include "sqlite3.h"

#include "utils.hpp"
#include "sqlTest_id.hpp"
#include "dataSources.hpp"

#include "EpdPage.hpp"

EpdPage::EpdPage(wxWindow* parent, sqlite3* db)
	: m_db(db)
{
	Create(parent, ID_EPD_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	getEpdNum();

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* searchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* searchGrid = new wxFlexGridSizer(0, 2, 0, 0);
	searchGrid->SetFlexibleDirection(wxBOTH);
	searchGrid->AddGrowableCol(0);
	searchGrid->AddGrowableCol(1);
	searchGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto source_text = new wxStaticText(this, wxID_ANY, "Data source");
	searchGrid->Add(source_text, 0, wxALL | wxEXPAND, 5);

	auto source_text_content = new wxStaticText(this, wxID_ANY, "IBU Epd");
	searchGrid->Add(source_text_content, 0, wxALL | wxEXPAND, 5);

	auto search_text = new wxStaticText(this, wxID_ANY, "Product name");
	searchGrid->Add(search_text, 0, wxALL | wxEXPAND, 5);

	m_productName_textCtrl = new wxTextCtrl(this, ID_EPD_PRODUCT_NAME_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	searchGrid->Add(m_productName_textCtrl, 0, wxALL | wxEXPAND, 5);

	searchSizer->Add(searchGrid, 0, wxALL | wxEXPAND, 5);

	m_epd_search_result_text = new wxStaticText(this, wxID_ANY, "");
	m_epd_search_result_text->SetLabel(std::to_string(m_epd_num) + " entries of products in total.");
	searchSizer->Add(m_epd_search_result_text, 0, wxALL | wxEXPAND, 5);

	topSizer->Add(searchSizer, 1, wxALL | wxEXPAND, 5);

	auto vLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	topSizer->Add(vLine1, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);

	m_db_table = new wxDataViewListCtrl(this, ID_IWU_NWG_DB_DATAVIEWLIST_CTRL);
	m_db_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	tableSizer->Add(m_db_table, 1, wxALL | wxEXPAND, 5);

	topSizer->Add(tableSizer, 3, wxALL | wxEXPAND, 5);

	showDB();
	updateDB();

	BindEvent();

	this->SetSizer(topSizer);
	this->Layout();
	topSizer->Fit(this);
}

EpdPage::~EpdPage()
{

}

void EpdPage::showDB()
{
	m_db_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("declaration number", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("product name", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("owner", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("exhibition year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("valid until", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("density", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("raw density", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("thermal conductivity", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_db_table->AppendTextColumn("technical data", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

}

void EpdPage::updateDB()
{
	int num = 0;
	m_db_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + EPD + " WHERE id NOTNULL";

	if (m_productName_textCtrl->GetValue() != "")
	{
		auto p_name = m_productName_textCtrl->GetValue().utf8_string();
		sentence += " AND product_name LIKE " + searchText(p_name);
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

			for (int i = 0; i < 11; ++i)
			{
				if (i == 1)
					continue;
				cells.push_back(transform(stmt, i));
			}

			m_db_table->AppendItem(cells);
			m_db_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);

	if (num == m_epd_num)
	{
		m_epd_search_result_text->SetLabel(std::to_string(m_epd_num) + " entries of products in total.");
	}
	else
	{
		m_epd_search_result_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_epd_num) + " products.");
	}
}

void EpdPage::getEpdNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + EPD + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_epd_num = 0;
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
	m_epd_num = count;
}

void EpdPage::searchProductName(wxCommandEvent& evt)
{
	updateDB();
}

void EpdPage::BindEvent()
{
	Bind(wxEVT_TEXT, &EpdPage::searchProductName, this, ID_EPD_PRODUCT_NAME_TEXTCTRL);
}