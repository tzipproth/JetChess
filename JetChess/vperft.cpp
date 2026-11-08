#include <wx/wx.h>
#include <wx/clipbrd.h>
#include "board.h"
#include "about.h"
#include "help.h"
#include "mgen6.h"

const wxSize frameSize(800,600);

#define ID_STOP    (0)
#define ID_EXIT    (1)
#define ID_ABOUT   (2)
#define ID_PERFT   (3)
#define ID_COPY    (4)
#define ID_LEFT    (5)
#define ID_RIGHT   (6)
#define ID_LEFT2   (7)
#define ID_RIGHT2  (8)
#define ID_FLAGS   (9)
#define ID_1GB (10)
#define ID_2GB (11)
#define ID_4GB (12)
#define ID_8GB (13)
#define ID_16GB (14)
#define ID_32GB (15)
#define ID_INFO   (16)
#define ID_HELP   (17)
#define ID_MOVE   (20)
#define ID_POS    (21)
#define ID_MATE   (22)
#define ID_BENCH  (23)
#define ID_TIMER1 (91)


class MyApp : public wxApp
{
public:
    bool OnInit();
};


class MyThread : public wxThread
{
public:
    MyThread();

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

	void set_fen(char *fen) {strcpy (my_fen,fen);};
	void set_level(int lv) {level=lv;};
	void set_steps(int st) {steps=st;};

private:
    char my_fen[256];
	int level;
	int steps;
};


MyThread::MyThread() : wxThread(wxTHREAD_JOINABLE)
{
	my_fen[0]='\0';
	level=5;
	steps=1;
}

void *MyThread::Entry()
{
    Anzahl_DoAll(level,my_fen,steps);	
	return NULL;
}

void MyThread::OnExit()
{
}


class MyFrame: public wxFrame
{
public:
    MyFrame();
    ~MyFrame() override;

private:
    void OnClose(wxCloseEvent&);
    void Enable_Items(bool b);
	
	void OnAbout(wxCommandEvent&);
    void OnStop(wxCommandEvent&);
	void OnQuit(wxCommandEvent&);
	void OnMove(wxCommandEvent&);
	void OnPos(wxCommandEvent&);
	void OnMate(wxCommandEvent&);
	void OnBench(wxCommandEvent&);		
	void On1GB(wxCommandEvent&);
	void On2GB(wxCommandEvent&);
	void On4GB(wxCommandEvent&);
	void On8GB(wxCommandEvent&);
	void On16GB(wxCommandEvent&);
	void On32GB(wxCommandEvent&);
	void OnInfo(wxCommandEvent&);
	void OnHelp(wxCommandEvent&);
	void OnPaint(wxPaintEvent&);
	void OnPerft(wxCommandEvent&);
	void OnCopy(wxCommandEvent&);
	void OnLeft(wxCommandEvent&);
	void OnRight(wxCommandEvent&);
	void OnLeft2(wxCommandEvent&);
	void OnRight2(wxCommandEvent&);
	void OnTimer1(wxTimerEvent&);

	int  Set_Board(char *fen);
	void Set_Hash(int hsize);
    void Check_Hash_Menu();
	void Set_OpMode(int omode, int itemid);
	void Display_Messages();

	wxMenu *fileMenu,*opMenu,*hashMenu,*aboutMenu;
	wxTextCtrl *fenBox,*moveBox,*resBox,*infoBox,*levelBox;
	wxStaticText *resText,*infoText,*levelText,*stepText;
	Board *board;
	wxButton *doPerft;
	wxBitmapButton *copyFen,*doLeft,*doRight,*doLeft2,*doRight2;

    wxComboBox * stepCombo;

	wxTimer *msgTimer;
	MyThread *doThread;

	int hash_size,op_mode,fen_pos;
	int akt_anztext,akt_anzinfo,akt_anzresult;
	int akt_level,akt_steps;

	wxMenuBar* menuBar;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnClose)
    EVT_MENU(ID_STOP, MyFrame::OnStop)
    EVT_MENU(ID_EXIT, MyFrame::OnQuit)
	EVT_MENU(ID_1GB, MyFrame::On1GB)
	EVT_MENU(ID_2GB, MyFrame::On2GB)
	EVT_MENU(ID_4GB, MyFrame::On4GB)
	EVT_MENU(ID_8GB, MyFrame::On8GB)
	EVT_MENU(ID_16GB, MyFrame::On16GB)
	EVT_MENU(ID_32GB, MyFrame::On32GB)
    EVT_MENU(ID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(ID_INFO, MyFrame::OnInfo)
	EVT_MENU(ID_HELP, MyFrame::OnHelp)
	EVT_MENU(ID_MOVE, MyFrame::OnMove)
	EVT_MENU(ID_POS, MyFrame::OnPos)
	EVT_MENU(ID_MATE, MyFrame::OnMate)
	EVT_MENU(ID_BENCH, MyFrame::OnBench)
	EVT_BUTTON(ID_PERFT, MyFrame::OnPerft)
	EVT_BUTTON(ID_COPY, MyFrame::OnCopy)
	EVT_BUTTON(ID_LEFT, MyFrame::OnLeft)
	EVT_BUTTON(ID_RIGHT, MyFrame::OnRight)
	EVT_BUTTON(ID_LEFT2, MyFrame::OnLeft2)
	EVT_BUTTON(ID_RIGHT2, MyFrame::OnRight2)
	EVT_TIMER(ID_TIMER1, MyFrame::OnTimer1)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    set_winflag(1);
	init_tables();
    init_mytime();	
    if (set_opmode(1) <= 0) {
        wxMessageBox("Could not allocate hash memory.");
        return false;
    }
	
	MyFrame* frame = new MyFrame();
    frame->Show(true);

    return true;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "JetChess", wxDefaultPosition, frameSize,
							 wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)
{
    doThread = nullptr;
    SetIcon(wxICON(vperft));
	SetBackgroundColour(wxColour(208,208,208));
    CreateStatusBar();
	SetStatusText("Welcome to JetChess 1.0");

// Toolbar erzeugen und bestücken
	CreateToolBar();
	GetToolBar()->AddTool(ID_ABOUT, "About", wxBITMAP(about), "About JetChess");
	GetToolBar()->Realize();

// Menü erzeugen und bestücken
	menuBar = new wxMenuBar();
    fileMenu = new wxMenu;
	opMenu = new wxMenu;
	hashMenu = new wxMenu;
	aboutMenu = new wxMenu;

    fileMenu->Append(ID_STOP, "S&top\tAlt-X", "Stop calculation" );
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_EXIT, "E&xit\tAlt-X", "Quit JetChess" );
    menuBar->Append(fileMenu, "&File");
	menuBar->Append(opMenu, "&Mode");
	menuBar->Append(hashMenu, "&Hash");
	menuBar->Append(aboutMenu, "&About");
    
	hashMenu->AppendCheckItem(ID_1GB, "1 GB", "Set hash size 1 GB" );
	hashMenu->AppendCheckItem(ID_2GB, "2 GB", "Set hash size 2 GB" );
	hashMenu->AppendCheckItem(ID_4GB, "4 GB", "Set hash size 4 GB" );
	hashMenu->AppendCheckItem(ID_8GB, "8 GB", "Set hash size 8 GB" );
	hashMenu->AppendCheckItem(ID_16GB, "16 GB", "Set hash size 16 GB" );
	hashMenu->AppendCheckItem(ID_32GB, "32 GB", "Set hash size 32 GB" );

	opMenu->AppendCheckItem(ID_MOVE, "Move Path", "Set mode to move path calculation" );
	opMenu->AppendCheckItem(ID_POS, "Position", "Set mode to position calculation" );
	opMenu->AppendCheckItem(ID_MATE, "Mate", "Set mode to mate calculation" );
	opMenu->AppendCheckItem(ID_BENCH, "Benchmark", "Set mode to benchmark" );

	aboutMenu->Append(ID_INFO, "Info", "Program Information");
	aboutMenu->Append(ID_HELP, "Help", "Program Help");
	
	SetMenuBar(menuBar);

	wxSize wsize = this->GetClientSize();
	if (wsize.GetHeight() != 512)	
		this->SetClientSize(wsize.GetWidth(),512);	

	fenBox = new wxTextCtrl(this,-1,"(hier FEN-Notation)",
		wxPoint(20,10), wxSize(frameSize.GetWidth()-150,23));

	moveBox = new wxTextCtrl(this,-1,"",
		wxPoint(422,40), wxSize(frameSize.GetWidth()-451,388), wxTE_MULTILINE);

	doPerft = new wxButton(this, ID_PERFT, "Perft!", wxPoint(422,442), wxSize(50,20) );
	copyFen = new wxBitmapButton(this, ID_COPY, wxBITMAP(copy), wxPoint(frameSize.GetWidth()-55,10), wxSize(24,23));
	doLeft = new wxBitmapButton(this, ID_LEFT, wxBITMAP(left), wxPoint(frameSize.GetWidth()-122,10), wxSize(24,23));
	doRight = new wxBitmapButton(this, ID_RIGHT, wxBITMAP(right), wxPoint(frameSize.GetWidth()-94,10), wxSize(24,23));

    resBox = new wxTextCtrl(this,-1,"", wxPoint(70,442), wxSize(338,23));
	infoBox = new wxTextCtrl(this,-1,"", wxPoint(70,478), wxSize(338,23));

	wxFont textFont1(10,wxFONTFAMILY_MODERN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	moveBox->SetFont(textFont1);

	wxFont textFont2(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	resBox->SetFont(textFont2);
	infoBox->SetFont(textFont2);
	
	resText = new wxStaticText(this, -1, _("Result:"), wxPoint(20,446), wxSize(40,23), 0);
	infoText = new wxStaticText(this, -1, _("Info:"), wxPoint(20,482), wxSize(40,23), 0);

	doPerft->SetToolTip("Start calculation");
	copyFen->SetToolTip("Copy from Clipboard");
	fenBox->SetToolTip("Position in FEN notation");
	doLeft->SetToolTip("Previous position");
	doRight->SetToolTip("Next position");


    levelBox = new wxTextCtrl(this,-1,"", wxPoint(683,442), wxSize(26,23));
    levelBox->SetFont(textFont2);	
	doLeft2 = new wxBitmapButton(this, ID_LEFT2, wxBITMAP(left), wxPoint(720,442), wxSize(24,23));
	doRight2= new wxBitmapButton(this, ID_RIGHT2, wxBITMAP(right), wxPoint(748,442), wxSize(24,23));

	levelText = new wxStaticText(this, -1, _("Level:"), wxPoint(640,446), wxSize(40,23), 0);
	stepText = new wxStaticText(this, -1, _("Steps:"), wxPoint(640,482), wxSize(40,23), 0);

	wxArrayString steps;
	steps.Add("1");
	steps.Add("2");
	steps.Add("4");
	steps.Add("8");
	steps.Add("16");
	steps.Add("32");
    stepCombo = new wxComboBox(this, -1, "1", wxPoint(683,478), wxSize(40,23), steps, wxCB_READONLY);
	
	board = new Board(this);

	menuBar->Check(ID_1GB,true);
	hash_size=1024;
	menuBar->Check(ID_MOVE,true);	
	op_mode=1;
	Set_Board(get_fen(0));
	fen_pos=0;
	akt_anztext=0;
	akt_anzinfo=0;
	akt_anzresult=0;
	akt_level=5;
	akt_steps=1;

	levelBox->SetValue("5");
	stepCombo->Disable();
	stepCombo->SetToolTip("Number of hash partitioning steps");
	levelBox->SetToolTip("Number of half moves");

	set_stopflag(1);

	msgTimer = new wxTimer(this,ID_TIMER1);
	bool tok = msgTimer->Start(200,wxTIMER_CONTINUOUS);
	if (!tok) wxMessageBox("Time not started!");
}

void MyFrame::Enable_Items(bool b)
{
	hashMenu->Enable(ID_1GB,b);
	hashMenu->Enable(ID_2GB,b);
	hashMenu->Enable(ID_4GB,b);
	hashMenu->Enable(ID_8GB,b);
	hashMenu->Enable(ID_16GB,b);
	hashMenu->Enable(ID_32GB,b);
	opMenu->Enable(ID_MOVE,b);
	opMenu->Enable(ID_POS,b);
	opMenu->Enable(ID_MATE,b);
	opMenu->Enable(ID_BENCH,b);

	doPerft->Enable(b);
	doLeft->Enable(b);
	doRight->Enable(b);
	copyFen->Enable(b);
    levelBox->Enable(b && op_mode != 4);
    doLeft2->Enable(b && op_mode != 4);
    doRight2->Enable(b && op_mode != 4);
    stepCombo->Enable(b && op_mode == 2);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	MyAboutDialog *md = new MyAboutDialog(this);
	md->Show();    
}

void MyFrame::OnStop(wxCommandEvent&)
{    
    if (doThread) set_stopflag(3);
}

void MyFrame::OnQuit(wxCommandEvent&)
{
    Close(true);
}

void MyFrame::On1GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(1024);
    Enable_Items(true);
}

void MyFrame::On2GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(2048);
    Enable_Items(true);
}

void MyFrame::On4GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(4096);
    Enable_Items(true);
}

void MyFrame::On8GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(8192);
    Enable_Items(true);
}

void MyFrame::On16GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(16384);
    Enable_Items(true);
}

void MyFrame::On32GB(wxCommandEvent&)
{
    Enable_Items(false);
    Set_Hash(32768);
    Enable_Items(true);
}

void MyFrame::OnInfo(wxCommandEvent&)
{
	MyAboutDialog *md = new MyAboutDialog(this);
	md->Show();
}

void MyFrame::OnHelp(wxCommandEvent&)
{
	MyHelpDialog *mh = new MyHelpDialog(this);
	mh->Show();
}

void MyFrame::OnMove(wxCommandEvent&)
{
	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();	
	Enable_Items(false);
	Set_OpMode(1,ID_MOVE);
	Enable_Items(true);
	doPerft->SetLabel("Perft!");
	levelBox->Enable();
	stepCombo->Disable();
	levelBox->SetToolTip("Number of half moves");
}

void MyFrame::OnPos(wxCommandEvent&)
{
	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();	
	Enable_Items(false);
	Set_OpMode(2,ID_POS);
	Enable_Items(true);
	doPerft->SetLabel("Position!");
	levelBox->Enable();
	stepCombo->Enable();
	levelBox->SetToolTip("Number of half moves");
}

void MyFrame::OnMate(wxCommandEvent&)
{
	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();	
	Enable_Items(false);
	Set_OpMode(3,ID_MATE);
	Enable_Items(true);
	doPerft->SetLabel("Mate!");
	levelBox->Enable();
	stepCombo->Disable();
	levelBox->SetToolTip("Number of moves");
}

void MyFrame::OnBench(wxCommandEvent&)
{
	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();	
	Enable_Items(false);
	Set_OpMode(4,ID_BENCH);
	Enable_Items(true);
	doPerft->SetLabel("Bench!");
	stepCombo->Disable();
	levelBox->Disable();
}

void MyFrame::OnPerft(wxCommandEvent&)
{
    if (doThread) return;
    if (hash_size <= 0) { wxMessageBox("Please allocate hash memory first."); return; }
    long requestedLevel;
    if (!levelBox->GetValue().ToLong(&requestedLevel) || requestedLevel < 1 || requestedLevel > 32) {
        wxMessageBox("Level must be between 1 and 32.");
        return;
    }
    akt_level = (int)requestedLevel;		
	reset_anzcounter();
	moveBox->Clear();	
	infoBox->Clear();
	resBox->Clear();
	resBox->AppendText("Running...");
	
    akt_anztext=0;
	akt_anzinfo=0;
	akt_anzresult=0;
	set_stopflag(0);
	akt_steps = wxAtoi(stepCombo->GetValue());
	if (akt_steps<1) akt_steps=1;
	if (akt_steps>32) akt_steps=32;

	Enable_Items(false);
		
	// The frame owns and joins the worker before another calculation or closing.
	doThread = new MyThread();	
	doThread->set_fen(get_fen(fen_pos));
	doThread->set_level(akt_level);
	doThread->set_steps(akt_steps);
    if (doThread->Create() != wxTHREAD_NO_ERROR || doThread->Run() != wxTHREAD_NO_ERROR) {
        delete doThread;
        doThread = nullptr;
        set_stopflag(1);
        Enable_Items(true);
        resBox->SetValue("Could not start calculation thread.");
    }
}


void MyFrame::OnCopy(wxCommandEvent&)
{
  int erg=0;
	
  if (wxTheClipboard->Open())
  {
    if (wxTheClipboard->IsSupported( wxDF_TEXT ))
    {
      wxTextDataObject data;
      wxTheClipboard->GetData( data );
      wxString str = data.GetText();	  

      const wxScopedCharBuffer ascii = str.ToUTF8();
      char ctx[256];
      if (!ascii.data() || ascii.length()>=sizeof(ctx)) {
          wxMessageBox("FEN input must be shorter than 256 bytes.","Fen incorrect");
          wxTheClipboard->Close();
          return;
      }
      strcpy_s(ctx,ascii.data());
	  erg=Set_Board(ctx);
	  if (erg==0)	  
		  fen_pos=add_fen(ctx);
	  else	  
		  Set_Board(get_fen(fen_pos));
	}
    wxTheClipboard->Close();
  }
}

void MyFrame::OnLeft(wxCommandEvent&)
{		
	if (fen_pos<1) return;
	
	if (strlen(get_fen(fen_pos-1))==0) return;

	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();
	fen_pos--;
	Set_Board(get_fen(fen_pos));
}

void MyFrame::OnRight(wxCommandEvent&)
{
	if (fen_pos>=31) return;
	
	if (strlen(get_fen(fen_pos+1))==0) return;

	reset_anzcounter();
	resBox->Clear();
	infoBox->Clear();
	moveBox->Clear();	
	fen_pos++;
	Set_Board(get_fen(fen_pos));
}

void MyFrame::OnLeft2(wxCommandEvent&)
{
	char ct[12];
	
	if (akt_level>1) akt_level--;
	_itoa(akt_level,ct,10);
	levelBox->SetValue(ct);
}

void MyFrame::OnRight2(wxCommandEvent&)
{
	char ct[12];
	
	if (akt_level<32) akt_level++;
	_itoa(akt_level,ct,10);
	levelBox->SetValue(ct);
}

void MyFrame::OnTimer1(wxTimerEvent& event)
{
    if (!doThread) return;
    Display_Messages();
    if (!doThread->IsAlive()) {
        doThread->Wait();
        delete doThread;
        doThread = nullptr;
        Display_Messages();
        if (get_stopflag()==3) {
            moveBox->Clear(); resBox->Clear(); infoBox->Clear();
        }
        set_stopflag(1);
        Enable_Items(true);
    }
}

void MyFrame::Display_Messages()
{
	int akt2_anztext = get_anztext();
	if (akt2_anztext > akt_anztext)
	{
		for (int i=akt_anztext; i<akt2_anztext; i++)
		{
			moveBox->AppendText(get_text(i));
			moveBox->AppendText("\n");

		}
		akt_anztext = akt2_anztext;
	}
	
	int akt2_anzresult = get_anzresult();
	if (akt2_anzresult > akt_anzresult)
	{
		resBox->Clear();
		resBox->AppendText(get_result());
		akt_anzresult = akt2_anzresult;
	}

	int akt2_anzinfo = get_anzinfo();
	if (akt2_anzinfo > akt_anzinfo)
	{
		infoBox->Clear();
		infoBox->AppendText(get_info());
		akt_anzinfo = akt2_anzinfo;
	}
}
 

int MyFrame::Set_Board(char *fen)
{	  
	int erg = fen_to_board(fen);
  
	if (erg != 0) 
	{
		char ct[64];
		sprintf(ct,"Error: %d",erg);
		wxMessageBox(ct,"Fen incorrect");
		return -1;
	}

	fenBox->SetValue(fen);

    erg=create_grafic_offsets();

    if (erg != 0)
	{
		wxMessageBox("Graphic display","Internal Error");
		return -1;
	}

    int x1,y1;

    for (int i=0; i<64; i++)
    {
	  get_grafic_offset(i,&x1,&y1);
 	  board->set_board_offset(i,x1,y1);   
	}
	 
	board->board_activ=1;	  
	board->Refresh();	

	return 0;
}


void MyFrame::Check_Hash_Menu()
{
    menuBar->Check(ID_1GB,hash_size==1024);
    menuBar->Check(ID_2GB,hash_size==2048);
    menuBar->Check(ID_4GB,hash_size==4096);
    menuBar->Check(ID_8GB,hash_size==8192);
    menuBar->Check(ID_16GB,hash_size==16384);
    menuBar->Check(ID_32GB,hash_size==32768);
}

void MyFrame::Set_Hash(int hsize)
{
    hash_size=hash_set(hsize);
    Check_Hash_Menu();
    if (hash_size!=hsize)
    {
        if (hash_size>0)
            wxMessageBox(wxString::Format("Could not allocate %d GB. Using %d GB.",hsize/1024,hash_size/1024));
        else
            wxMessageBox("Could not allocate hash memory.");
    }
}

void MyFrame::Set_OpMode(int omode, int itemid)
{
    int previous_hash=hash_size;
    menuBar->Check(ID_MOVE,false);
	menuBar->Check(ID_POS,false);
	menuBar->Check(ID_MATE,false);
	menuBar->Check(ID_BENCH,false);

	hash_size = set_opmode(omode);
    Check_Hash_Menu();
    if (hash_size<=0) wxMessageBox("Could not allocate hash memory.");
    else if (hash_size!=previous_hash) wxMessageBox(wxString::Format("Could not retain %d GB in this mode. Using %d GB.",previous_hash/1024,hash_size/1024));

	op_mode=omode;
	menuBar->Check(itemid,true);
}


MyFrame::~MyFrame()
{
    if (doThread) {
        set_stopflag(3);
        doThread->Wait();
        delete doThread;
    }
    delete msgTimer;
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    msgTimer->Stop();
    if (doThread) {
        set_stopflag(3);
        doThread->Wait();
        delete doThread;
        doThread = nullptr;
    }
    event.Skip();
}
