#include "wxprec.hpp"
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "sqlite3.h"

#include "utils.hpp"
#include "AltbaualtasPage.hpp"
#include "sqlTest_id.hpp"
#include "dataSources.hpp"

AltbaualtasPage::AltbaualtasPage(wxWindow* parent, sqlite3* db)
	:m_db(db)
{
	Create(parent, ID_ALTBAUATLAS_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	getConstructionNum();
	getMaterialNum();

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	m_notebook = new wxAuiNotebook(this, ID_ALTBAUATLAS_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0);
	
	// Construction page

	m_construction_page = new wxPanel(m_notebook, ID_ALTBAUATLAS_CONSTRUCTION_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	
	wxBoxSizer* consTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* consSearchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* consGrid = new wxFlexGridSizer(0, 2, 0, 0);
	consGrid->SetFlexibleDirection(wxBOTH);
	consGrid->AddGrowableCol(0);
	consGrid->AddGrowableCol(1);
	consGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto construction_source_text = new wxStaticText(m_construction_page, wxID_ANY, "Data source");
	consGrid->Add(construction_source_text, 0, wxALL | wxEXPAND, 5);

	auto construction_source_text_content = new wxStaticText(m_construction_page, wxID_ANY, "ALTBAU ATLAS");
	consGrid->Add(construction_source_text_content, 0, wxALL | wxEXPAND, 5);

	auto construction_element_text = new wxStaticText(m_construction_page, wxID_ANY, "Element type");
	consGrid->Add(construction_element_text, 0, wxALL | wxEXPAND, 5);

	m_construction_element_combobox = new wxComboBox(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_ELEMENT_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getElementStrings(), 0);
	consGrid->Add(m_construction_element_combobox, 0, wxALL | wxEXPAND, 5);

	auto construction_search_text = new wxStaticText(m_construction_page, wxID_ANY, "Search (name, materials)");
	consGrid->Add(construction_search_text, 0, wxALL | wxEXPAND, 5);

	m_construction_search_textctrl = new wxTextCtrl(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_SEARCH_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	consGrid->Add(m_construction_search_textctrl, 0, wxALL | wxEXPAND, 5);

	auto construction_year_text = new wxStaticText(m_construction_page, wxID_ANY, "Construction year");
	consGrid->Add(construction_year_text, 0, wxALL | wxEXPAND, 5);

	m_construction_year_textctrl = new wxTextCtrl(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_construction_year_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	consGrid->Add(m_construction_year_textctrl, 0, wxALL | wxEXPAND, 5);

	auto construction_postal_text = new wxStaticText(m_construction_page, wxID_ANY, "Postal code");
	consGrid->Add(construction_postal_text, 0, wxALL | wxEXPAND, 5);

	m_construction_postal_textctrl = new wxTextCtrl(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_POSTAL_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_construction_postal_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	consGrid->Add(m_construction_postal_textctrl, 0, wxALL | wxEXPAND, 5);

	consSearchSizer->Add(consGrid, 0, wxALL | wxEXPAND, 5);

	m_construction_search_result_text = new wxStaticText(m_construction_page, wxID_ANY, "");
	m_construction_search_result_text->SetLabel(std::to_string(m_construction_num) + " entries of construction in total.");
	consSearchSizer->Add(m_construction_search_result_text, 0, wxALL | wxEXPAND, 5);

	auto consSearchHorizontalLine = new wxStaticLine(m_construction_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	consSearchSizer->Add(consSearchHorizontalLine, 0, wxEXPAND | wxALL, 5);

	m_construction_material_mini_table = new wxDataViewListCtrl(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_MINI_MATERIAL_TABLE);
	m_construction_material_mini_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	consSearchSizer->Add(m_construction_material_mini_table, 1, wxALL | wxEXPAND, 5);

	consTopSizer->Add(consSearchSizer, 1, wxEXPAND | wxALL, 5);

	auto constructionVerticalLine = new wxStaticLine(m_construction_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	consTopSizer->Add(constructionVerticalLine, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* constructionTableSizer = new wxBoxSizer(wxVERTICAL);

	m_construction_table = new wxDataViewCtrl(m_construction_page, ID_ALTBAUATLAS_CONSTRUCTION_TABLE, wxDefaultPosition, wxDefaultSize,	wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);
	m_construction_model = new ConstructionDataModel();
	m_construction_table->AssociateModel(m_construction_model);
	m_construction_model->DecRef();

	constructionTableSizer->Add(m_construction_table, 1, wxALL | wxEXPAND, 5);

	consTopSizer->Add(constructionTableSizer, 4, wxEXPAND | wxALL, 5);

	m_construction_page->SetSizer(consTopSizer);
	m_construction_page->Layout();
	consTopSizer->Fit(m_construction_page);

	m_notebook->AddPage(m_construction_page, wxT("Construction"), true, wxNullBitmap);

	// Material page

	m_material_page = new wxPanel(m_notebook, ID_ALTBAUATLAS_MATERIAL_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* materialTopSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* materialSearchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* materialGrid = new wxFlexGridSizer(0, 2, 0, 0);
	materialGrid->SetFlexibleDirection(wxBOTH);
	materialGrid->AddGrowableCol(0);
	materialGrid->AddGrowableCol(1);
	materialGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto material_source_text = new wxStaticText(m_material_page, wxID_ANY, "Data source");
	materialGrid->Add(material_source_text, 0, wxALL | wxEXPAND, 5);

	auto material_source_text_content = new wxStaticText(m_material_page, wxID_ANY, "ALTBAU ATLAS");
	materialGrid->Add(material_source_text_content, 0, wxALL | wxEXPAND, 5);

	auto material_name_text = new wxStaticText(m_material_page, wxID_ANY, "Name");
	materialGrid->Add(material_name_text, 0, wxALL | wxEXPAND, 5);

	m_material_name_textctrl = new wxTextCtrl(m_material_page, ID_ALTBAUATLAS_MATERIAL_NAME_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	materialGrid->Add(m_material_name_textctrl, 0, wxALL | wxEXPAND, 5);

	auto material_year_text = new wxStaticText(m_material_page, wxID_ANY, "Construction year");
	materialGrid->Add(material_year_text, 0, wxALL | wxEXPAND, 5);

	m_material_year_textctrl = new wxTextCtrl(m_material_page, ID_ALTBAUATLAS_MATERIAL_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_material_year_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	materialGrid->Add(m_material_year_textctrl, 0, wxALL | wxEXPAND, 5);

	auto material_postal_text = new wxStaticText(m_material_page, wxID_ANY, "Postal code");
	materialGrid->Add(material_postal_text, 0, wxALL | wxEXPAND, 5);

	m_material_postal_textctrl = new wxTextCtrl(m_material_page, ID_ALTBAUATLAS_MATERIAL_POSTAL_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_material_postal_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	materialGrid->Add(m_material_postal_textctrl, 0, wxALL | wxEXPAND, 5);

	materialSearchSizer->Add(materialGrid, 0, wxALL | wxEXPAND, 5);

	m_material_search_result_text = new wxStaticText(m_material_page, wxID_ANY, "");
	m_material_search_result_text->SetLabel(std::to_string(m_material_num) + " entries of material in total.");
	materialSearchSizer->Add(m_material_search_result_text, 0, wxALL | wxEXPAND, 5);

	materialTopSizer->Add(materialSearchSizer, 1, wxEXPAND | wxALL, 5);

	auto materialVerticalLine = new wxStaticLine(m_material_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	materialTopSizer->Add(materialVerticalLine, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* materialTableSizer = new wxBoxSizer(wxVERTICAL);

	m_material_table = new wxDataViewCtrl(m_material_page, ID_ALTBAUATLAS_MATERIAL_TABLE, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);
	m_material_model = new MaterialDataModel();
	m_material_table->AssociateModel(m_material_model);
	m_material_model->DecRef();

	materialTableSizer->Add(m_material_table, 1, wxALL | wxEXPAND, 5);

	materialTopSizer->Add(materialTableSizer, 4, wxEXPAND | wxALL, 5);

	m_material_page->SetSizer(materialTopSizer);
	m_material_page->Layout();
	materialTopSizer->Fit(m_material_page);

	m_notebook->AddPage(m_material_page, wxT("Material"), false, wxNullBitmap);

	topSizer->Add(m_notebook, 1, wxALL | wxEXPAND, 5);

	showDB_material();
	updateDB_material();

	showDB_construction();
	updateDB_construction();

	showDB_miniMaterial();
	updateDB_miniMaterial("");

	BindEvent();

	this->SetSizer(topSizer);
	this->Layout();
	this->Centre(wxBOTH);
}

AltbaualtasPage::~AltbaualtasPage()
{
}

void AltbaualtasPage::BindEvent()
{
	Bind(wxEVT_TEXT, &AltbaualtasPage::materialEnterName, this, ID_ALTBAUATLAS_MATERIAL_NAME_TEXTCTRL);
	Bind(wxEVT_TEXT, &AltbaualtasPage::materialEnterYear, this, ID_ALTBAUATLAS_MATERIAL_YEAR_TEXTCTRL);
	Bind(wxEVT_TEXT, &AltbaualtasPage::materialEnterPostal, this, ID_ALTBAUATLAS_MATERIAL_POSTAL_TEXTCTRL);

	Bind(wxEVT_COMBOBOX, &AltbaualtasPage::constructionSelectElement, this, ID_ALTBAUATLAS_CONSTRUCTION_ELEMENT_COMBOBOX);
	Bind(wxEVT_TEXT, &AltbaualtasPage::constructionEnterYear, this, ID_ALTBAUATLAS_CONSTRUCTION_YEAR_TEXTCTRL);
	Bind(wxEVT_TEXT, &AltbaualtasPage::constructionEnterPostal, this, ID_ALTBAUATLAS_CONSTRUCTION_POSTAL_TEXTCTRL);
	Bind(wxEVT_TEXT, &AltbaualtasPage::constructionSearch, this, ID_ALTBAUATLAS_CONSTRUCTION_SEARCH_TEXTCTRL);

	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &AltbaualtasPage::constructionTableSelectConstruction, this, ID_ALTBAUATLAS_CONSTRUCTION_TABLE);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &AltbaualtasPage::constructionMiniTableSelectMaterial, this, ID_ALTBAUATLAS_CONSTRUCTION_MINI_MATERIAL_TABLE);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &AltbaualtasPage::constructionTableSelectPdf, this, ID_ALTBAUATLAS_CONSTRUCTION_TABLE);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &AltbaualtasPage::materialTableSelectPdf, this, ID_ALTBAUATLAS_MATERIAL_TABLE);
}

void AltbaualtasPage::getConstructionNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + ALTBAUATLAS_CONSTRUCTION + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_construction_num = 0;
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
	m_construction_num = count;
}

void AltbaualtasPage::getMaterialNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + ALTBAUATLAS_MATERIAL + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_material_num = 0;
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
	m_material_num = count;
}

void AltbaualtasPage::showDB_material()
{
	m_material_table->AppendTextColumn("id", 0, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT,wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);	
	m_material_table->AppendTextColumn("name", 1, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("postal_code", 2, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("first_year", 3, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("last_year", 4, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("strength", 5, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("raw_density", 6, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_material_table->AppendTextColumn("lambda", 7, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	
	wxDataViewTextRenderer* linkRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
	linkRenderer->EnableMarkup(true);

	wxDataViewColumn* linkColumn = new wxDataViewColumn(
		"source_id",
		linkRenderer,
		8,
		TEXT_CELL_WIDTH,
		wxALIGN_LEFT,
		wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE
	);

	m_material_table->AppendColumn(linkColumn);
}

void AltbaualtasPage::updateDB_material()
{
	int num = 0;
	std::vector<MaterialRow> rows;

	std::string sentence = "SELECT * FROM " + ALTBAUATLAS_MATERIAL + " WHERE id NOTNULL";

	if (m_material_name_textctrl->GetValue() != "")
	{
		auto name = m_material_name_textctrl->GetValue().utf8_string();
		sentence += " AND name LIKE " + searchText(name);
	}

	if (m_material_year_textctrl->GetValue() != "")
	{
		auto year = std::stoi(m_material_year_textctrl->GetValue().utf8_string());
		sentence += " AND first_year <= " + std::to_string(year) + " AND last_year >= " + std::to_string(year);
	}

	if (m_material_postal_textctrl->GetValue() != "")
	{
		auto postal = m_material_postal_textctrl->GetValue().utf8_string().substr(0, 2);
		sentence += " AND postal_code LIKE " + searchText(postal);
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
			MaterialRow row;
			row.id = transform(stmt, 0);
			row.name = transform(stmt, 1);	
			row.postal_code = transform(stmt, 2);
			row.first_year = transform(stmt, 3);
			row.last_year = transform(stmt, 4);
			row.strength = transform(stmt, 5);
			row.raw_density = transform(stmt, 6);
			row.lambda = transform(stmt, 7);
			row.source_id = transform(stmt, 8);
			rows.push_back(row);
		}
	}

	sqlite3_finalize(stmt);

	if (m_material_model)
		m_material_model->SetRows(rows);

	if (num == m_material_num)
	{
		m_material_search_result_text->SetLabel(std::to_string(m_material_num) + " entries of material in total.");
	}
	else
	{
		m_material_search_result_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_material_num) + " materials.");
	}
}

void AltbaualtasPage::showDB_construction()
{
	m_construction_table->AppendTextColumn("id", 0, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("name", 1, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("element_type", 2, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("materials", 3, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("postal_code", 4, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("first_year", 5, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("last_year", 6, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("materials_code", 7, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_table->AppendTextColumn("u_value", 8, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	
	wxDataViewTextRenderer* linkRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
	linkRenderer->EnableMarkup(true);

	wxDataViewColumn* linkColumn = new wxDataViewColumn(
		"source_id",
		linkRenderer,
		9,
		TEXT_CELL_WIDTH,
		wxALIGN_LEFT,
		wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE
	);

	m_construction_table->AppendColumn(linkColumn);
}

void AltbaualtasPage::updateDB_construction()
{
	int num = 0;
	std::vector<ConstructionRow> rows;

	std::string sentence = "SELECT * FROM " + ALTBAUATLAS_CONSTRUCTION + " WHERE id NOTNULL";

	if (!(m_construction_element_combobox->GetSelection() == wxNOT_FOUND || m_construction_element_combobox->GetStringSelection() == ""))
	{
		auto element = m_construction_element_combobox->GetStringSelection().utf8_string();
		sentence += " AND element_type IS " + quoteWord(element);
	}

	if (m_construction_year_textctrl->GetValue() != "")
	{
		auto year = std::stoi(m_construction_year_textctrl->GetValue().utf8_string());
		sentence += " AND first_year <= " + std::to_string(year) + " AND last_year >= " + std::to_string(year);
	}

	if (m_construction_postal_textctrl->GetValue() != "")
	{
		auto postal = m_construction_postal_textctrl->GetValue().utf8_string().substr(0, 2);
		sentence += " AND postal_code LIKE " + searchText(postal);
	}

	if (m_construction_search_textctrl->GetValue() != "")
	{
		auto search = m_construction_search_textctrl->GetValue().utf8_string();
		sentence += " AND (name LIKE " + searchText(search) + " OR materials LIKE " + searchText(search) + ")";
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
			ConstructionRow row;
			row.id = transform(stmt, 0);
			row.name = transform(stmt, 1);
			row.element_type = transform(stmt, 2);			
			row.materials = transform(stmt, 3);
			row.postal_code = transform(stmt, 4);
			row.first_year = transform(stmt, 5);
			row.last_year = transform(stmt, 6);
			row.materials_code = transform(stmt, 7);
			row.u_value = transform(stmt, 8);
			row.source_id = transform(stmt, 9);
			rows.push_back(row);
		}
	}

	sqlite3_finalize(stmt);
	if (m_construction_model)
		m_construction_model->SetRows(rows);

	if (num == m_construction_num)
	{
		m_construction_search_result_text->SetLabel(std::to_string(m_construction_num) + " entries of construction in total.");
	}
	else
	{
		m_construction_search_result_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_construction_num) + " constructions.");
	}
}

void AltbaualtasPage::showDB_miniMaterial()
{
	m_construction_material_mini_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_construction_material_mini_table->AppendTextColumn("name", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void AltbaualtasPage::updateDB_miniMaterial(const std::string& pdf)
{
	if (pdf == "")
	{
		return;
	}

	m_construction_material_mini_table->DeleteAllItems();	
	
	std::string sentence = "SELECT id, name FROM " + ALTBAUATLAS_MATERIAL + " WHERE id NOTNULL AND source_id IS " + quoteWord(pdf);	

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 2; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_construction_material_mini_table->AppendItem(cells);
			m_construction_material_mini_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void AltbaualtasPage::setMaterialTableSelection(const std::string& id)
{
	if (!m_material_table || !m_material_model)
		return;
	wxString target = wxString::FromUTF8(id.c_str());
	m_material_table->UnselectAll();
	for (unsigned int row = 0; row < m_material_model->GetCount(); ++row)
	{
		const MaterialRow* rowData = m_material_model->GetRowData(row);
		if (!rowData)
			continue;

		if (rowData->id == target)
		{
			wxDataViewItem item = m_material_model->GetItem(row);
			if (item.IsOk())
			{
				m_material_table->Select(item);
				m_material_table->EnsureVisible(item);
			}
			break;
		}
	}
}

void AltbaualtasPage::materialEnterName(wxCommandEvent& evt)
{
	updateDB_material();
}

void AltbaualtasPage::materialEnterYear(wxCommandEvent& evt)
{
	updateDB_material();
}

void AltbaualtasPage::materialEnterPostal(wxCommandEvent& evt)
{
	updateDB_material();
}

void AltbaualtasPage::materialTableSelectPdf(wxDataViewEvent& evt)
{
	if (evt.GetColumn() != 8 || !m_material_model)
		return;

	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk())
		return;

	unsigned int row = m_material_model->GetRow(item);
	const MaterialRow* rowData = m_material_model->GetRowData(row);
	if (!rowData)
		return;

	std::string source_id = rowData->source_id.ToStdString();
	wxString url = "https://www.altbauatlas.de/create_datenblatt.php?id=" + source_id;

	if (!wxLaunchDefaultBrowser(url))
	{
		wxMessageBox("Unable to open link: \n" + url, "ERROR", wxOK | wxICON_ERROR, this);
	}
}

void AltbaualtasPage::constructionSelectElement(wxCommandEvent& evt)
{
	updateDB_construction();
}

void AltbaualtasPage::constructionEnterYear(wxCommandEvent& evt)
{
	updateDB_construction();
}

void AltbaualtasPage::constructionEnterPostal(wxCommandEvent& evt)
{
	updateDB_construction();
}

void AltbaualtasPage::constructionSearch(wxCommandEvent& evt)
{
	updateDB_construction();
}

void AltbaualtasPage::constructionTableSelectConstruction(wxDataViewEvent& evt)
{
	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk() || !m_construction_model)
		return;

	unsigned int row = m_construction_model->GetRow(item);
	const ConstructionRow* rowData = m_construction_model->GetRowData(row);
	if (!rowData)
		return;

	updateDB_miniMaterial(rowData->source_id.utf8_string());
}

void AltbaualtasPage::constructionMiniTableSelectMaterial(wxCommandEvent& evt)
{
	auto selection = m_construction_material_mini_table->GetSelection();

	if (!selection) return;

	auto row = m_construction_material_mini_table->ItemToRow(selection);
	auto id = m_construction_material_mini_table->GetTextValue(row, 0).utf8_string();
	setMaterialTableSelection(id);

	m_notebook->SetSelection(1);
}

void AltbaualtasPage::constructionTableSelectPdf(wxDataViewEvent& evt)
{
	if (evt.GetColumn() != 9 || !m_construction_model)
		return;

	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk())
		return;

	unsigned int row = m_construction_model->GetRow(item);
	const ConstructionRow* rowData = m_construction_model->GetRowData(row);
	if (!rowData)
		return;

	std::string source_id = rowData->source_id.ToStdString();
	wxString url = "https://www.altbauatlas.de/create_datenblatt.php?id=" + source_id;

	if (!wxLaunchDefaultBrowser(url))
	{
		wxMessageBox("Unable to open link: \n" + url, "ERROR", wxOK | wxICON_ERROR, this);
	}	
}

wxArrayString AltbaualtasPage::getElementStrings()
{
	wxArrayString types(1);

	types.Add("");
	types.Add(wxString("Auﬂenwand"));
	types.Add(wxString("Decke"));
	types.Add(wxString("Flachdach"));
	types.Add(wxString("Fuﬂboden"));
	types.Add(wxString("Kellerdecke"));
	types.Add(wxString("Steildach"));

	return types;
}
