#ifndef _about_h
#define _about_h

#include <wx/wx.h>

class MyAboutDialog : public wxDialog
{
public:
    MyAboutDialog(wxWindow *parent);

    void OnOk(wxCommandEvent& event);

private:
    
	wxBitmap *bm1;
	wxStaticBitmap *bm2;

    DECLARE_EVENT_TABLE()
};


#endif // _about_h
