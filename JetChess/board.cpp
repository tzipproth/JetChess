#include "board.h"

BEGIN_EVENT_TABLE(Board, wxWindow)
	EVT_PAINT(Board::OnPaint)
END_EVENT_TABLE()


Board::Board(wxWindow* parent)
	: wxWindow(parent,-1,wxPoint(20,40), wxSize(388,388), wxSUNKEN_BORDER )
{
	src = new wxMemoryDC();
	src->SelectObject(wxBITMAP(pieces));

	board_activ=0;
}

Board::~Board()
{
	delete src;
}

void Board::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	
	if (board_activ==0)
		return;

	int k=63;

	for (int i=0; i<8; i++)
	{
		for (int j=7; j>=0; j--)
		{
		  int dstX = j*48;	
		  int dstY = i*48;	
		  int srcX = y_offset[k]*48;	
		  int srcY = x_offset[k]*48;	
		  int width = 48;	
		  int height = 48;	
	
		  dc.Blit( dstX, dstY, width, height, src, srcX, srcY );

		  k--;
		}
	}

	event.Skip();
}

void Board::set_board_offset(int n, int x_off, int y_off)
{
	x_offset[n] = x_off;	
	y_offset[n] = y_off;	
}
