#pragma once

class DetailDialog : public wxDialog
{
public:
	DetailDialog(wxWindow* parent, std::vector<std::pair<std::string, std::string>> info);
	virtual ~DetailDialog() {}
};