#include "help.h"
#include "rtf_resources.h"

#define ID_OK   (31)

BEGIN_EVENT_TABLE(MyHelpDialog, wxDialog)
      EVT_BUTTON(ID_OK, MyHelpDialog::OnOk)
END_EVENT_TABLE()


MyHelpDialog::MyHelpDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString(_T("JetChess Help")), wxDefaultPosition, wxSize(640,480), wxDEFAULT_DIALOG_STYLE)
{
	wxButton *btnOk = new wxButton(this, wxID_OK, _T("&OK"),wxPoint(280,422));

	this->SetBackgroundColour(wxColor(212,198,185));

	wxTextCtrl *moveBox2 = new wxTextCtrl(this, -1 ,wxEmptyString , wxPoint(10,10), wxSize(610,400), wxTE_MULTILINE | wxTE_RICH2, wxDefaultValidator, wxTextCtrlNameStr);

    void *wHandle = moveBox2->GetHandle();
	char *cpt = get_resource_string(40014);
	fill_richtext_window(wHandle,cpt);
    SetClientSize(630,460);
    CentreOnParent();
}

void MyHelpDialog::OnOk(wxCommandEvent& event)
{

}

