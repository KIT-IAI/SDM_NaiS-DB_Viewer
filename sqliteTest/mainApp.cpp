#include "wxprec.hpp"

#include "mainApp.hpp"
#include "mainFrame.hpp"

IMPLEMENT_APP(SqlApp)

bool SqlApp::OnInit()
{
	SetAppName("NaiS DB");

	MainFrame* frame = new MainFrame();
	frame->Show();
	frame->Center();

	SetTopWindow(frame);

	return true;
}