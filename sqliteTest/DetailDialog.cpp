#include "sqlTest_id.hpp"
#include "wxprec.hpp"
#include "DetailDialog.hpp"

DetailDialog::DetailDialog(wxWindow* parent, std::vector<std::pair<std::string, std::string>> info)
{
	Create(parent, ID_DIALOG_DETAIL_INFO, "Energy system component information", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	SetSize(wxSize(800, 600));

	auto topSizer = new wxBoxSizer(wxVERTICAL);

	auto firstSizer = new wxFlexGridSizer(2);

	for (auto content : info)
	{
		auto title = new wxStaticText(this, wxID_ANY, content.first);
		firstSizer->Add(title, 0, wxALL | wxEXPAND, 5);

		auto thing = new wxStaticText(this, wxID_ANY, content.second);
		firstSizer->Add(thing, 0, wxALL | wxEXPAND, 5);
	}

	topSizer->Add(firstSizer, 0, wxALL | wxEXPAND);

	this->SetSizer(topSizer);
	this->Layout();
	Centre();
}
