#include "about.h"
#include "hyperlink.h"
#include "rtf_resources.h"

#define ID_OK   (30)

BEGIN_EVENT_TABLE(MyAboutDialog, wxDialog)
      EVT_BUTTON(ID_OK, MyAboutDialog::OnOk)
END_EVENT_TABLE()


MyAboutDialog::MyAboutDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString(_T("Information")), wxDefaultPosition, wxSize(325,360), wxDEFAULT_DIALOG_STYLE)
{
	wxButton *btnOk = new wxButton(this, wxID_OK, _T("&OK"),wxPoint(128,302));

	wxBitmap bm3 = wxBITMAP(pillar);
	bm2 = new wxStaticBitmap(this,-1,bm3,wxPoint(10,10),wxDefaultSize);

	this->SetBackgroundColour(wxColor(212,198,185));

  wxStaticText *t1 = new wxStaticText( this, -1, "JetChess  1.0.0.0   07.11.2006", wxPoint(100,20), wxSize(200,20), wxALIGN_LEFT|wxST_NO_AUTORESIZE );
  t1->SetFont( wxFont(8,wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	
	wxString copyright = wxString::FromUTF8("Copyright © 2006 Thomas Zipproth");
    wxStaticText *t3 = new wxStaticText( this, -1, copyright, wxPoint(100,50), wxSize(240,20), wxALIGN_LEFT|wxST_NO_AUTORESIZE );
    t3->SetFont( wxFont(8,wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	wxString remark1 = "No warranty for any possible program bugs";
    wxStaticText *t4 = new wxStaticText( this, -1, remark1, wxPoint(100,70), wxSize(240,24), wxALIGN_LEFT|wxST_NO_AUTORESIZE );
    t4->SetFont( wxFont(8,wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	wxHyperLink *wxh2 = new  wxHyperLink(this,-1,_T("http://www.zipproth.de/jetchess/index.html"),_T("JetChess homepage"),wxNullColour,wxNullColour,wxPoint(100,100),wxDefaultSize);       

	wxTextCtrl *moveBox2 = new wxTextCtrl(this, -1 ,wxEmptyString , wxPoint(10,140), wxSize(298,150), wxTE_MULTILINE | wxTE_RICH2, wxDefaultValidator, wxTextCtrlNameStr);

    void *wHandle = moveBox2->GetHandle();
	char *cpt = get_resource_string(40013);
	fill_richtext_window(wHandle,cpt);
    SetClientSize(350,335);
    CentreOnParent();
}

void MyAboutDialog::OnOk(wxCommandEvent& event)
{

}

