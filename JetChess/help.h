#ifndef _help_h
#define _help_h

#include <wx/wx.h>

class MyHelpDialog : public wxDialog
{
public:
    MyHelpDialog(wxWindow *parent);

    void OnOk(wxCommandEvent& event);

private:
    
	wxBitmap *bm1;
	wxStaticBitmap *bm2;

    DECLARE_EVENT_TABLE()
};


#endif // _help_h
