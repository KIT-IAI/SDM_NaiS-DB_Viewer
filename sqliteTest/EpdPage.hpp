#pragma once
class EpdPage : public wxPanel
{
public:
	EpdPage(wxWindow* parent, sqlite3* db);
	~EpdPage();


private:
	void BindEvent();

	void showDB();
	void updateDB();
	void getEpdNum();

	void searchProductName(wxCommandEvent& evt);

private:

	sqlite3* m_db;
	int m_epd_num;
	wxStaticText* m_epd_search_result_text;
	wxDataViewListCtrl* m_db_table;

	wxTextCtrl* m_productName_textCtrl;
	wxTextCtrl* m_search_textctrl;

};
