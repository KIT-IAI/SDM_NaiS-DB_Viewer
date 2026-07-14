#pragma once

#include <wx/wx.h>

class SqlApp : public wxApp
{
public:
	virtual ~SqlApp() {}

	virtual bool OnInit();
};

wxDECLARE_APP(SqlApp);
