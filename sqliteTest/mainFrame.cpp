#include <iostream>
#include <numeric>
#include <stdlib.h>
#include "wxprec.hpp"
#include "sqlite3.h"

#include "sqlTest_id.hpp"
#include "country.hpp"
#include "utils.hpp"
#include "mainFrame.hpp"
#include "DetailDialog.hpp"
#include "dataSources.hpp"

const std::string APP_NAME = "Nais DB 1.0";

MainFrame::MainFrame()
	: wxFrame(nullptr, ID_MAIN_FRAME, APP_NAME)
{
	readDefaultDB();

	wxInitAllImageHandlers();
	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE);
	SetIcon(wxIcon(wxT("kiticon.ico"), wxBITMAP_TYPE_ICO, 16, 16));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetSize(1600, 750);

	wxBoxSizer* MainSizer = new wxBoxSizer(wxHORIZONTAL);

	m_auinotebook = new wxAuiNotebook(this, ID_MAIN_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0);
	m_auinotebook->SetWindowStyle(wxAUI_NB_TAB_FIXED_WIDTH);

	// Data source page

	m_dataSource_page = new wxPanel(m_auinotebook, ID_DATASOURCE_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* dataSourceTopSizer = new wxBoxSizer(wxHORIZONTAL);

	m_dataSource_table = new wxDataViewCtrl(m_dataSource_page, ID_DATAVIEWCTRL_DATASOURCE, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);
	m_datasource_model = new DataSourceModel();
	m_dataSource_table->AssociateModel(m_datasource_model);
	m_datasource_model->DecRef();
	dataSourceTopSizer->Add(m_dataSource_table, 1, wxALL | wxEXPAND, 5);

	m_dataSource_page->SetSizer(dataSourceTopSizer);
	m_dataSource_page->Layout();
	dataSourceTopSizer->Fit(m_dataSource_page);	

	m_auinotebook->AddPage(m_dataSource_page, wxT("Data Sources"), true, wxNullBitmap);

	// TABULA page

	m_tabula_page = new TabulaPage(m_auinotebook, m_db);

	m_tabula_page->Layout();

	m_auinotebook->AddPage(m_tabula_page, wxT("TABULA"), false, wxNullBitmap);

	// IWU NWG PAGE

	m_iwuNwg_page = new IwunwgPage(m_auinotebook, m_db);

	m_iwuNwg_page->Layout();

	m_auinotebook->AddPage(m_iwuNwg_page, wxT("IWU NWG"), false, wxNullBitmap);

	// ALTBAU ATLAS PAGE

	m_altbauatlas_page = new AltbaualtasPage(m_auinotebook, m_db);

	m_altbauatlas_page->Layout();

	m_auinotebook->AddPage(m_altbauatlas_page, wxT("ALTBAU ATLAS"), false, wxNullBitmap);

	// EPD MATERIAL PAGE

	m_epd_page = new EpdPage(m_auinotebook, m_db);

	m_epd_page->Layout();

	m_auinotebook->AddPage(m_epd_page, wxT("EPD MATERIAL"), false, wxNullBitmap);

	// CRREM Page

	m_crrem_page = new CrremPage(m_auinotebook, m_db);
	m_crrem_page->Layout();
	m_auinotebook->AddPage(m_crrem_page, wxT("CRREM"), false, wxNullBitmap);


	MainSizer->Add(m_auinotebook, 1, wxEXPAND, 5);	

	readDefaultDB();

	showDB_dataSource();
	updateDB_dataSource();

	this->SetSizer(MainSizer);
	this->Layout();

	this->Centre(wxBOTH);

	BindEvent();
}

MainFrame::~MainFrame()
{
	sqlite3_close(m_db);
}

void MainFrame::readDefaultDB()
{
	auto filename = "NaiS.db";

	sqlite3_open(filename, &m_db);
}

void MainFrame::showDB_dataSource()
{
	m_dataSource_table->AppendTextColumn("id", 0, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_dataSource_table->AppendTextColumn("name", 1, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_dataSource_table->AppendTextColumn("description", 2, wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	
	wxDataViewTextRenderer* linkRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
	linkRenderer->EnableMarkup(true);
	wxDataViewColumn* linkColumn = new wxDataViewColumn(
		"url",
		linkRenderer,
		3,
		2 * TEXT_CELL_WIDTH,
		wxALIGN_LEFT,
		wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE
	); 
	m_dataSource_table->AppendColumn(linkColumn);
	
	m_dataSource_table->AppendTextColumn("data_entry", 4, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_dataSource_table->AppendTextColumn("version", 5, wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void MainFrame::updateDB_dataSource()
{
	std::vector<DataSourceRow> rows;

	std::string sentence = "SELECT * FROM " + DATA_SOURCE + " WHERE id NOTNULL;";

	sqlite3_stmt* stmt = NULL;

	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			DataSourceRow row;
			row.id = transform(stmt, 0);
			row.name = transform(stmt, 1);
			row.description = transform(stmt, 2);
			row.url = transform(stmt, 3);
			row.data_entry = transform(stmt, 4);
			row.version = transform(stmt, 5);

			rows.push_back(row);
		}
	}
	sqlite3_finalize(stmt);

	if (m_datasource_model)
	{
		m_datasource_model->SetRows(rows);
	}
}

void MainFrame::BindEvent()
{		
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MainFrame::onItemActivated, this, ID_DATAVIEWCTRL_DATASOURCE);
}

void MainFrame::onItemActivated(wxDataViewEvent& evt)
{
	unsigned int col = evt.GetColumn();
	if (col != 3)
		return;
	
	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk() || !m_datasource_model)
		return;

	unsigned int row = m_datasource_model->GetRow(item);
	if (row == 0)
		return;

	const DataSourceRow* rowData = m_datasource_model->GetRowData(row);
	if (!rowData)
		return;
	wxString url = rowData->url;
	if (!url.empty())
	{
		if (!wxLaunchDefaultBrowser(url))
		{
			wxMessageBox("Unable to open link: \n" + url, "ERROR", wxOK | wxICON_ERROR, this);
		}
	}
}
