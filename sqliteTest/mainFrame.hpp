#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include "wxprec.hpp"
#include "sqlite3.h"

#include "TabulaPage.hpp"
#include "IwunwgPage.hpp"
#include "AltbaualtasPage.hpp"
#include "EpdPage.hpp"
#include "CrremPage.hpp"

struct DataSourceRow
{
	wxString id;
	wxString name;
	wxString description;
	wxString url;
	wxString data_entry;
	wxString version;
};

class DataSourceModel : public wxDataViewVirtualListModel
{
public:
	DataSourceModel(): wxDataViewVirtualListModel(0) {}
	unsigned int GetColumnCount() const override
	{
		return 6;
	}

	wxString GetColumnType(unsigned int) const override
	{
		return "string";
	}

	void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const override
	{
		if (row >= m_rows.size())
			return;

		const auto& r = m_rows[row];
		switch (col)
		{
		case 0: variant = r.id; break;
		case 1: variant = r.name; break;
		case 2: variant = r.description; break;
		case 3: 
			if (!r.url.IsEmpty())
				variant = "<span foreground=\"blue\"><u>" + r.url + "</u></span>";
			else
				variant = "";
			break;
		case 4: variant = r.data_entry; break;
		case 5: variant = r.version; break;
		default: break;
		}
	}

	bool SetValueByRow(const wxVariant&, unsigned int, unsigned int) override
	{
		return false;
	}

	void SetRows(const std::vector<DataSourceRow>& rows)
	{
		m_rows = rows;
		Reset(static_cast<unsigned int>(m_rows.size()));
	}

	const DataSourceRow* GetRowData(unsigned int row) const
	{
		if (row >= m_rows.size())
			return nullptr;
		return &m_rows[row];
	}

private:
	std::vector<DataSourceRow> m_rows;
};

class MainFrame : public wxFrame
{
public:
	MainFrame();

	virtual ~MainFrame();

private:

	void readDefaultDB();	

	void showDB_dataSource();
	void updateDB_dataSource();	

	void BindEvent();	

	void onItemActivated(wxDataViewEvent& evt);

private:

	wxAuiNotebook* m_auinotebook;
	wxPanel* m_dataSource_page;
	
	TabulaPage* m_tabula_page;
	IwunwgPage* m_iwuNwg_page;
	AltbaualtasPage* m_altbauatlas_page;
	EpdPage* m_epd_page;
	CrremPage* m_crrem_page;

	wxDataViewCtrl* m_dataSource_table = nullptr;
	DataSourceModel* m_datasource_model = nullptr;

	sqlite3* m_db;
};