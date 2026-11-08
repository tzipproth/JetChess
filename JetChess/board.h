#ifndef _board_h
#define _board_h

#include <wx/wx.h>

class Board: public wxWindow
{
public:
	Board(wxWindow*);
	~Board();

	void set_board_offset(int n, int x_off, int y_off);

	int x_offset[64];
	int y_offset[64];
	int board_activ;

private:
	void OnPaint(wxPaintEvent&);

	wxMemoryDC* src;

    DECLARE_EVENT_TABLE()
};

#endif // _board_h
