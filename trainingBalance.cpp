#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
#include "mondrian.xpm"
#endif

#include "trainingBalance.h"

enum
{
  TB_Quit = 1,
  TB_About = wxID_ABOUT,

  ID_STARTCAL,
  ID_ENDCAL,
  ID_CHRONIC,
  ID_ACUTE,
  ID_RECALCBUTTON,

  MainMenu_UserOpen
};

BEGIN_EVENT_TABLE(Trainingbalanceframe, wxFrame)
  EVT_MENU(TB_Quit,  Trainingbalanceframe::OnQuit)
  EVT_MENU(TB_About, Trainingbalanceframe::OnAbout)
  EVT_BUTTON(ID_RECALCBUTTON, Trainingbalanceframe::OnRecalcButton)
  EVT_CALENDAR_SEL_CHANGED(-1, Trainingbalanceframe::OnDateChange)
  END_EVENT_TABLE();


void Trainingbalanceframe::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void Trainingbalanceframe::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  wxString oneWord;
  //  double dbl;
  //  int i;

  msg.Printf( _T("PowerPlot.\nWritten by Richard Abbott (c)2003")
				  _T("Compiled with %s"), wxVERSION_STRING);

  wxMessageBox(msg, _T("About PowerPlot"), wxOK | wxICON_INFORMATION, this);
}

void Trainingbalanceframe::OnDateChange(wxCalendarEvent& evt)
{
  m_parentWindow->CalcBalance(true);
}

void Trainingbalanceframe::OnRecalcButton(wxCommandEvent& WXUNUSED(event))
{
  m_parentWindow->CalcBalance(true);
}

// ----------------------------------------------------------------------
Trainingbalanceframe::Trainingbalanceframe(wxWindow *par, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
  : wxFrame(par, -1, title, pos, size, style)
{
  wxString inText;
  wxDateTime startDate;

  m_parentWindow=(TrainingBalance*)par;

  // set the frame icon
  SetIcon(wxICON(mondrian)); // Necessary?

  // --- Create the menus
  wxMenu *menuFile = new wxMenu;
  wxMenu *helpMenu = new wxMenu;

  helpMenu->Append(TB_About,
						 _T("&About...\tF1"),
						 _T("Show about dialog"));
  menuFile->Append(TB_Quit,
						 _T("E&xit\tAlt-X"),
						 _T("Quit this program"));

  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, _T("&File"));
  menuBar->Append(helpMenu, _T("&Help"));
  SetMenuBar(menuBar);

  CreateStatusBar(2);
  SetStatusText(_T("Training Balance"));

  graphPanel=new GraphPanel(this, -1, wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *calSizer= new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *tcSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *acuteSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *chronicSizer=new wxBoxSizer(wxHORIZONTAL);

  startDate=wxDateTime::Now()-wxTimeSpan::Days(365);
  startDate.SetMonth(wxDateTime::Jan);
  startDate.SetDay(1);
  m_startCal=new wxCalendarCtrl(this, ID_STARTCAL, startDate);
  m_endCal  = new wxCalendarCtrl(this, ID_ENDCAL, wxDateTime::Now());

  wxStaticText *acuteLabel=new wxStaticText(this, -1, "&Acute TC");
  m_acuteTc=  new wxTextCtrl(this, ID_ACUTE, "5");

  wxStaticText *chronicLabel=new wxStaticText(this, -1, "&Chronic TC");
  m_chronicTc=new wxTextCtrl(this, ID_CHRONIC, "40");
  m_recalcButton=new wxButton(this, ID_RECALCBUTTON, "&Recalculate");


  acuteSizer->Add(acuteLabel, 0, wxGROW, 1);
  acuteSizer->Add(m_acuteTc, 1, wxGROW, 1);
  
  chronicSizer->Add(chronicLabel, 0, wxGROW, 1);
  chronicSizer->Add(m_chronicTc, 1, wxGROW, 1);

  tcSizer->Add(acuteSizer, 0, wxGROW, 1);
  tcSizer->Add(chronicSizer, 0, wxGROW, 1);
  tcSizer->Add(m_recalcButton, 1, wxGROW, 1);

  calSizer->Add(m_startCal, 0, wxGROW, 1);
  calSizer->Add(m_endCal,   0, wxGROW, 1);
  calSizer->Add(tcSizer, 1, wxGROW, 1);

  mainSizer->Add(graphPanel, 1, wxGROW, 1);
  mainSizer->Add(calSizer, 0, wxGROW, 1);
  //  mainSizer->SetSizeHints(this);
  SetSizer(mainSizer);
  graphPanel->SetMaxY(350);
  graphPanel->SetMaxX(4000);
  graphPanel->SetXAxisType(XAXIS_DATES); // plot x axis in minutes

  // ---------- initialize vars ----------
}

Trainingbalanceframe::~Trainingbalanceframe()
{
  ;
}


// ----------------------------------------------------------------------
// TrainingBalance
TrainingBalance::TrainingBalance(const wxString& title, 
											const wxPoint& pos, const wxSize& size,
											UserProfile *pf){
  frame=new Trainingbalanceframe(this, _T("Training Balance Window"), 
									  wxPoint(50,50), wxSize(450, 640),
									  wxDEFAULT_FRAME_STYLE);
  frame->theProfile=pf;

  frame->graphPanel->AddTrace();
  frame->graphPanel->SetLegend(0, "Acute");
  frame->graphPanel->SetTraceColour(0, wxColour(*wxRED));

  frame->graphPanel->AddTrace();
  frame->graphPanel->SetLegend(1, "Chronic");
  frame->graphPanel->SetTraceColour(1, wxColour(*wxBLUE));

  frame->graphPanel->AddTrace();
  frame->graphPanel->SetLegend(2, "Balance");
  frame->graphPanel->SetTraceColour(2, wxColour(*wxGREEN));
  frame->graphPanel->AddLine(0, false);

  frame->m_acuteTc->SetValue(wxString::Format("%ld", pf->acuteTc));
  frame->m_chronicTc->SetValue(wxString::Format("%ld", pf->chronicTc));

  CalcBalance(true);
  frame->graphPanel->ZoomAll();
  frame->Show(TRUE);
}

bool TrainingBalance::CalcBalance(bool repaint)
{
  EntryList::Node *node1;
  BikeEntryData bikedata;

  wxDateTime calcDate, startDate, endDate;

  double tc,a1,b0,tss;

  double chronicTc, acuteTc;
  double sum_short, um1_short, am1_short;
  double sum_long, um1_long, am1_long;

  long ii=0;
  bool keyevent;
  bool sick;

  sum_short=0; um1_short=0; am1_short=0;
  sum_long=0; um1_long=0; am1_long=0;

  tss=100;

  frame->m_acuteTc->GetValue().ToDouble(&acuteTc);
  frame->m_chronicTc->GetValue().ToDouble(&chronicTc);

  if(acuteTc<1 || chronicTc<1){
    wxMessageBox("Must Enter Valid TC Values", "Error", wxOK);
    return(false);
  }

  endDate=frame->m_endCal->GetDate();
  endDate.ResetTime();

  startDate=frame->m_startCal->GetDate();
  startDate.ResetTime();

  node1=frame->theProfile->mainList->GetFirst(); // set up the date ptr
  if(node1==NULL) return(false);
  bikedata=*node1->GetData();

  frame->graphPanel->ClearTrace(0);
  frame->graphPanel->ClearTrace(1);
  frame->graphPanel->ClearTrace(2);
  frame->graphPanel->AddLine(0, false);

  calcDate=bikedata.theDate;
  calcDate.ResetTime();
  frame->graphPanel->SetStartDate(calcDate);

  while(node1!=NULL || calcDate.IsEarlierThan(endDate)){           // calc the whole file
    tss=0;
    while(node1!=NULL && bikedata.theDate.IsEarlierThan(calcDate)){ // move forward to calc date
      node1=node1->GetNext();
      if(node1!=NULL)bikedata=*node1->GetData();
    }

    keyevent=false;
    sick=false;
    while(node1!=NULL && bikedata.theDate.IsSameDate(calcDate)){ // total of day's tss values
      if(bikedata.ppTss!=0)      tss+=bikedata.ppTss;
      else if(bikedata.ptTss!=0) tss+=bikedata.ptTss;
      else if(bikedata.ctTss!=0) tss+=bikedata.ctTss;
      else if(bikedata.polarTss!=0) tss+=bikedata.polarTss;
      if(bikedata.keyEvent==true)keyevent=true;
      if(bikedata.sick==true) sick=true;
      node1=node1->GetNext();
      if(node1!=NULL)bikedata=*node1->GetData();
    }
    // calc acute
    a1 = exp(-1.0 / acuteTc);
    b0 = 1.0-a1; 
    sum_short = a1*um1_short+b0*tss;
    um1_short = sum_short; am1_short = tss;	// Age data

    // calc chronic
    a1 = exp(-1.0 / chronicTc);
    b0 = 1.0-a1; 
    sum_long = a1*um1_long+b0*tss;
    um1_long = sum_long; am1_long = tss;	// Age data

    if(calcDate.IsLaterThan(startDate) && calcDate.IsEarlierThan(endDate)){
      if(keyevent) frame->graphPanel->AddLine(ii, true);
      if(sick) frame->graphPanel->AddLine(ii, true, wxColour(255,150,150));
    }
    ii++;
    calcDate+=wxTimeSpan::Day()+wxTimeSpan::Hours(2);
	 calcDate.ResetTime();
    if(calcDate.IsLaterThan(startDate) && calcDate.IsEarlierThan(endDate)){
      frame->graphPanel->AddPoint(0, wxRealPoint(ii,sum_short));
      frame->graphPanel->AddPoint(1, wxRealPoint(ii,sum_long));
      frame->graphPanel->AddPoint(2, wxRealPoint(ii,sum_long-sum_short));
    }
  }
  frame->graphPanel->UpdateGraph();

  return(true);
}
