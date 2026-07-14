#pragma once

struct ConstructionRow 
{
	wxString id;
	wxString name;
	wxString element_type;	
	wxString materials;
	wxString postal_code;
	wxString first_year;
	wxString last_year;
	wxString materials_code;
	wxString u_value;
	wxString source_id;
};

struct MaterialRow
{
	wxString id;
	wxString name;
	wxString postal_code;
	wxString first_year;
	wxString last_year;
	wxString strength;
	wxString raw_density;
	wxString lambda;
	wxString source_id;
};

class ConstructionDataModel : public wxDataViewVirtualListModel
{
public:
	ConstructionDataModel() : wxDataViewVirtualListModel(0) {}
	unsigned int GetColumnCount() const override
	{
		return 10;
	}

	wxString GetColumnType(unsigned int) const override
	{
		return "string";
	}

	void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const override
	{
		if (row >= m_rows.size())
		{
			variant = "";
			return;
		}

		const auto& r = m_rows[row];
		switch (col)
		{
		case 0: variant = r.id; break;
		case 1: variant = r.element_type; break;
		case 2: variant = r.name; break;
		case 3: variant = r.materials; break;
		case 4: variant = r.postal_code; break;
		case 5: variant = r.first_year; break;
		case 6: variant = r.last_year; break;
		case 7: variant = r.materials_code; break;
		case 8: variant = r.u_value; break;
		case 9:
			if (!r.source_id.IsEmpty())
				variant = "<span foreground=\"blue\"><u>" + r.source_id + "</u></span>";
			else
				variant = "";
			break;
		default:
			variant = "";
			break;
		}
	}

	bool SetValueByRow(const wxVariant&, unsigned int, unsigned int) override
	{
		return false;
	}

	void SetRows(const std::vector<ConstructionRow>& rows)
	{
		m_rows = rows;
		Reset(static_cast<unsigned int>(m_rows.size()));
	}

	const ConstructionRow* GetRowData(unsigned int row) const
	{
		if (row >= m_rows.size())
			return nullptr;
		return &m_rows[row];
	}

private:
	std::vector<ConstructionRow> m_rows;
};

class MaterialDataModel : public wxDataViewVirtualListModel
{
public:
	MaterialDataModel() : wxDataViewVirtualListModel(0) {}
	unsigned int GetColumnCount() const override
	{
		return 9;
	}

	wxString GetColumnType(unsigned int) const override
	{
		return "string";
	}

	void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const override
	{
		if (row >= m_rows.size())
		{
			variant = "";
			return;
		}

		const auto& r = m_rows[row];
		switch (col)
		{
		case 0: variant = r.id; break;
		case 1: variant = r.name; break;
		case 2: variant = r.postal_code; break;
		case 3: variant = r.first_year; break;
		case 4: variant = r.last_year; break;
		case 5: variant = r.strength; break;
		case 6: variant = r.raw_density; break;
		case 7: variant = r.lambda; break;
		case 8:
			if (!r.source_id.IsEmpty())
				variant = "<span foreground=\"blue\"><u>" + r.source_id + "</u></span>";
			else
				variant = "";
			break;
		default:
			variant = "";
			break;
		}
	}

	bool SetValueByRow(const wxVariant&, unsigned int, unsigned int) override
	{
		return false;
	}

	void SetRows(const std::vector<MaterialRow>& rows)
	{
		m_rows = rows;
		Reset(static_cast<unsigned int>(m_rows.size()));
	}

	const MaterialRow* GetRowData(unsigned int row) const
	{
		if (row >= m_rows.size())
			return nullptr;
		return &m_rows[row];
	}

private:
	std::vector<MaterialRow> m_rows;
};

class AltbaualtasPage : public wxPanel
{
public:
	AltbaualtasPage(wxWindow* parent, sqlite3* db);
	~AltbaualtasPage();

private:

	void BindEvent();

	void getConstructionNum();
	void getMaterialNum();

	void showDB_material();
	void updateDB_material();

	void showDB_construction();
	void updateDB_construction();

	void showDB_miniMaterial();
	void updateDB_miniMaterial(const std::string& pdf);

	void setMaterialTableSelection(const std::string& id);

	void materialEnterName(wxCommandEvent& evt);
	void materialEnterYear(wxCommandEvent& evt);
	void materialEnterPostal(wxCommandEvent& evt);
	void materialTableSelectPdf(wxDataViewEvent& evt);

	void constructionSelectElement(wxCommandEvent& evt);
	void constructionEnterYear(wxCommandEvent& evt);
	void constructionEnterPostal(wxCommandEvent& evt);
	void constructionSearch(wxCommandEvent& evt);

	void constructionTableSelectConstruction(wxDataViewEvent& evt);
	void constructionMiniTableSelectMaterial(wxCommandEvent& evt);
	void constructionTableSelectPdf(wxDataViewEvent& evt);

	wxArrayString getElementStrings();

private:
	sqlite3* m_db;
	
	int m_construction_num;
	int m_material_num;
	wxStaticText* m_construction_search_result_text;
	wxStaticText* m_material_search_result_text;

	wxAuiNotebook* m_notebook;

	wxPanel* m_material_page;
	wxPanel* m_construction_page;

	wxDataViewCtrl* m_construction_table = nullptr;
	ConstructionDataModel* m_construction_model = nullptr;
	wxDataViewListCtrl* m_construction_material_mini_table;
	wxDataViewCtrl* m_material_table = nullptr;
	MaterialDataModel* m_material_model = nullptr;
	wxTextCtrl* m_construction_search_textctrl;
	wxTextCtrl* m_construction_year_textctrl;
	wxTextCtrl* m_construction_postal_textctrl;
	wxComboBox* m_construction_element_combobox;

	wxTextCtrl* m_material_name_textctrl;
	wxTextCtrl* m_material_postal_textctrl;
	wxTextCtrl* m_material_year_textctrl;

};

