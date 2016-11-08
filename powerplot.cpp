/////////////////////////////////////////////////////////////////////////////
// Name:        powerplot.cpp
// Purpose:     graphPanel sample
// Author:      Richard Abbott
// Modified by:
// Created:     2003-03-27
// RCS-ID:      $Id: powerplot.cpp,v 1.20 2009/11/13 14:55:13 rich Exp $
// Copyright:   (c) Richard Abbott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include<math.h>
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include"powerfile.h"
#include"graphPanel.h"
#include"wxReadTextFile.h"
#include"polar.h"
#include"powertap.h"
#include"computrainer.h"
#include"bikeDayEntry.h"
#include"powerplot.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
#define LTPOWER 245

// Define a new frame type: this is going to be our main frame

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  PowerPlot_Quit = 1,
  PowerPlot_FixPowertapHr,
  PowerPlot_TSS,

  PowerPlotMenu_OpenPP,
  PowerPlotMenu_OpenCT,
  PowerPlotMenu_OpenPT,
  PowerPlotMenu_OpenPolar,

  PowerPlotMenu_HRFileRepair,

  PowerPlot_About = wxID_ABOUT,
  MainMenu_UserOpen
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(ppWinFrame, wxFrame)
  EVT_MENU(PowerPlot_TSS,  ppWinFrame::OnCalcTSS)
  EVT_MENU(PowerPlot_FixPowertapHr, ppWinFrame::OnFixPT)
  EVT_MENU(PowerPlot_Quit,  ppWinFrame::OnQuit)
  EVT_MENU(PowerPlot_About, ppWinFrame::OnAbout)

  EVT_MENU(PowerPlotMenu_OpenPP, ppWinFrame::OnOpenPP)
  EVT_MENU(PowerPlotMenu_OpenCT, ppWinFrame::OnOpenCT)
  EVT_MENU(PowerPlotMenu_OpenPT, ppWinFrame::OnOpenPT)
  EVT_MENU(PowerPlotMenu_OpenPolar, ppWinFrame::OnOpenPolar)

  EVT_MENU(PowerPlotMenu_HRFileRepair, ppWinFrame::StartHRRepair)
  END_EVENT_TABLE()

// frame constructor
ppWinFrame::ppWinFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
  : wxFrame(parent, -1, title, pos, size, style)
{
  wxString inText;
  // set the frame icon
  //  SetIcon(wxICON(mondrian)); // Necessary?

  // --- Create the menus
#if wxUSE_MENUS
  wxMenu *menuFile = new wxMenu;
  wxMenu *helpMenu = new wxMenu;

  menuFile->Append(PowerPlotMenu_OpenPP, _T("Open &PP\tAlt-P"),
						 _T("Open PowerPlan File"));
  menuFile->Append(PowerPlotMenu_OpenCT, _T("Open &CT\tAlt-C"),
						 _T("Open Computrainer File"));
  menuFile->Append(PowerPlotMenu_OpenPT, _T("Open P&T\tAlt-T"),
							_T("Open Computrainer File"));
  menuFile->Append(PowerPlotMenu_OpenPolar,
						 _T("Open Po&lar\tAlt-l"),
						 _T("Open Polar File"));

  menuFile->Append(PowerPlot_FixPowertapHr,
						 _T("&Auto Repair PT"),
						 _T("Repair PT HR"));
  menuFile->Append(PowerPlotMenu_HRFileRepair,
						 _T("&Interactive Repair PT"),
						 _T("Repair HR on PT file interactively"));
  helpMenu->Append(PowerPlot_About,
						 _T("&About...\tF1"),
						 _T("Show about dialog"));
  menuFile->Append(PowerPlot_Quit,
						 _T("E&xit\tAlt-X"),
						 _T("Quit this program"));

  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, _T("&File"));
  menuBar->Append(helpMenu, _T("&Help"));
  SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
  // create a status bar just for fun (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText(_T("Powerplot window"));
#endif // wxUSE_STATUSBAR


  graphPanel=new GraphPanel(this, -1, wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  mainSizer->Add(graphPanel, 1, wxGROW, 1);
  //  mainSizer->SetSizeHints(this);
  SetSizer(mainSizer);
//  SetSizerAndFit(mainSizer);
  graphPanel->SetMaxY(350);
  graphPanel->SetMaxX(4000);
  graphPanel->SetXAxisType(XAXIS_MINUTES); // plot x axis in minutes

  // ---------- initialize vars ----------
  m_filesLoaded=0;
}

double ppWinFrame::CalcTss(TraceClass *theTrace)
{
  TraceClass tc;
  PointList::Node *thePoint;
  double yy, sum, ii, fournorm;
  double IF;
  double rawTss, tss;
  //  wxString outtxt;

  // Step 1: Find the 30s rolling average
  CreateSmooth(theTrace,
					&tc,
					30);
  // Step 2,3: Raise to the 4th power
  sum=0; ii=0;
  thePoint=tc.pointList.GetFirst();
  while(thePoint!=NULL){
	 yy=thePoint->GetData()->y;
	 yy=pow(yy,4);
	 sum+=yy;
	 ii++;
	 thePoint->GetData()->y=yy;
	 thePoint=thePoint->GetNext();
  }
  // step 4
  fournorm=pow(sum/ii, .25);
  // step 5
  IF=fournorm/LTPOWER;
  // step 6,7
  rawTss=
	 theTrace->pointList.GetLast()->GetData()->x
	 *theTrace->mean
	 *IF;
  tss=rawTss/36/LTPOWER;
  return(tss);
}

// ----------------------------------------------------------------------
// `CreateSmooth(TraceClass *origTrace, TraceClass *newTrace,
//                double rollingTime)
//  Create a smoothed curve
// ----------------------------------------------------------------------
bool ppWinFrame::CreateSmooth(TraceClass *origTrace, TraceClass *newTrace, double rollingTime)
{
  PointList::Node *windowStart, *windowCenter, *windowEnd, *tnode;
  wxRealPoint *startPoint, *centerPoint, *endPoint, *tPoint;
  double points, sum;

  rollingTime /= 2.0;

  windowStart=origTrace->pointList.GetFirst();
  startPoint=windowStart->GetData();
  windowCenter=windowStart;
  windowEnd=windowCenter;
  endPoint=windowEnd->GetData();

  while(windowCenter!=NULL){
	 // find the start point
	 centerPoint=windowCenter->GetData();
	 while(windowStart!=NULL && startPoint->x<centerPoint->x-rollingTime){
		windowStart=windowStart->GetNext();
		if(windowStart!=NULL) startPoint=windowStart->GetData();
	 }
	 // find the end point
	 while(windowEnd!=NULL && endPoint->x<centerPoint->x+rollingTime){
		windowEnd=windowEnd->GetNext();
		if(windowEnd!=NULL) endPoint=windowEnd->GetData();
	 }
	 // find the average of these points
	 points=0; sum=0;
	 tnode=windowStart;
	 while(tnode!=NULL && tnode!=windowEnd){
		tPoint=tnode->GetData();
		points++;
		sum+=tnode->GetData()->y;
		tnode=tnode->GetNext();
	 }
	 if(points>0)
		newTrace->pointList.Append(new wxRealPoint(centerPoint->x, sum/points));
	 else
		newTrace->pointList.Append(new wxRealPoint(centerPoint->x,
																 centerPoint->y));
	 windowCenter=windowCenter->GetNext();
  }
  return(true);
}

// ----------------------------------------------------------------------
// `OnCalcTSS
// ----------------------------------------------------------------------
void ppWinFrame::OnCalcTSS(wxCommandEvent& WXUNUSED(event))
{
  int traces;
  double tss;
  wxString outtxt;

  traces=graphPanel->GetTraceCount()-1;

  tss=CalcTss(graphPanel->traceList.Item(traces)->GetData());
  outtxt=wxString::Format("%s TSS: %d",
								  graphPanel->traceList.Item(traces)->GetData()->legend.GetData(),
								  (int)tss);
  graphPanel->SetLegend(traces, outtxt);
  graphPanel->UpdateGraph();
}

// event handlers
void ppWinFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void ppWinFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  wxString oneWord;
  //  double dbl;
  //  int i;

  msg.Printf( _T("PowerPlot.\nWritten by Richard Abbott (c)2003")
				  _T("Compiled with %s"), wxVERSION_STRING);

  wxMessageBox(msg, _T("About PowerPlot"), wxOK | wxICON_INFORMATION, this);
  wxReadTextFile tf("test.txt");
}

void ppWinFrame::OnFixPT(wxCommandEvent& WXUNUSED(event))					  // 
{
  PowerTapFile ptfile;

  wxString filename;
  wxFileDialog fd(this, "Choose a Powertap file", defDir/*default dir*/,
					"" /*default file*/,
					"PT Files (*.csv)|*.csv|All files (*.*)|*.*",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  defPtDir=fd.GetDirectory();
  filename=fd.GetPath();
  ptfile.LoadFile(filename);
  ptfile.WriteFile(filename+".old");
  ptfile.RepairHr();
  ptfile.WriteFile(filename);
}

ppWinFrame::~ppWinFrame()
{
  ;
}

// ---- OpenPolar ----
void ppWinFrame::OnOpenPolar(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxFileDialog fd(this, "Choose a Polar HR file",
						theProfile->polarDefDir,		  // default dir
						"", // default file
						"Polar Files (*.HRM)|*.HRM|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  filename=fd.GetPath();
  OpenPolar(filename);
}

// -- Open Powerplan --
void ppWinFrame::OpenPP(wxString filename)
{
  wxString shortName;

  Powerfile pf;

  int ii;
  int traces;
  wxColour traceColour;

  shortName=filename;
  do{
	 ii=shortName.Find("\\");
	 if(ii==-1) ii=shortName.Find("/");
	 if(ii>=0) shortName=shortName.Mid(ii+1);
  }while(ii>=0);

  pf.Load(filename);

  graphPanel->AddTrace();
  graphPanel->AddTrace();
  graphPanel->AddTrace();
  graphPanel->AddTrace();
  traces=graphPanel->GetTraceCount()-1;
  graphPanel->SetLegend(traces-3, wxString::Format("Power: %s", shortName.GetData()));
  graphPanel->SetLegend(traces-2, wxString::Format("MPH: %s", shortName.GetData()));
  graphPanel->SetLegend(traces-1, wxString::Format("HR:%s",shortName.GetData()));
  graphPanel->SetLegend(traces,   wxString::Format("Altitude:%s",shortName.GetData()));
  traceColour=wxColor(0,0,0);
  if(m_filesLoaded==1)traceColour= wxColor(255,0,0);
  if(m_filesLoaded==2)traceColour= wxColor(0,255,0);
  if(m_filesLoaded==3)traceColour= wxColor(0,0,255);
  if(m_filesLoaded==4)traceColour= wxColor(255,0,255);
  if(m_filesLoaded==5)traceColour= wxColor(255,255,0);
  if(m_filesLoaded==6)traceColour= wxColor(0,255,255);
  graphPanel->SetTraceColour(traces, traceColour);
  graphPanel->SetTraceColour(traces-1, traceColour);
  graphPanel->SetTraceColour(traces-2, traceColour);
  graphPanel->SetTraceColour(traces-3, traceColour);
  pf.Firstline();
  pf.m_currentFilteredline=pf.m_firstFilteredline;
  pf.Filter(30);
  while(pf.m_currentLine!=NULL){
	 //	 if(dat->id!=lastId) graphPanel->AddLine(dat->Seconds(), true, wxColour(255,255,0));
	 graphPanel->AddPoint(traces-3, wxRealPoint(pf.m_currentLine->seconds, pf.m_currentLine->watts));
	 graphPanel->AddPoint(traces-2, wxRealPoint(pf.m_currentLine->seconds, pf.m_currentLine->speed_kph));
	 graphPanel->AddPoint(traces-1, wxRealPoint(pf.m_currentLine->seconds, pf.m_currentLine->hr_bpm));
	 graphPanel->AddPoint(traces  , wxRealPoint(pf.m_currentLine->seconds, pf.m_currentLine->altitude_meters*3.28084));

	 //	 graphPanel->AddPoint(traces  , wxRealPoint(pf.m_currentLine->seconds, pf.m_currentFilteredline->watts));
	 pf.m_currentFilteredline=pf.m_currentFilteredline->next;
	 pf.Nextline();
  }
  pf.FirstInterval();
  while(pf.OnAInterval()==true){	 
	 graphPanel->AddLine(pf.IntervalStartSeconds(), true, wxColour(255,255,0));
	 graphPanel->AddLine(pf.IntervalEndSeconds(),   true, wxColour(255,255,100));
	 pf.NextInterval();
  }

  graphPanel->Refresh();
  m_filesLoaded++;
}

void ppWinFrame::OpenPolar(wxString filename)
{
  int traces;
  wxColour traceColour;
  PolarLineList::Node *oneLine;
  PolarLine *dat;

  wxString shortName;
  int ii;

  shortName=filename;
  do{
	 ii=shortName.Find("\\");
	 if(ii==-1) ii=shortName.Find("/");
	 if(ii>=0) shortName=shortName.Mid(ii+1);
  }while(ii>=0);

  PolarFile pf;
  pf.LoadFile(filename);

  graphPanel->AddTrace();		  // hr
  graphPanel->AddTrace();		  // speed
  graphPanel->AddTrace();		  // cadence
  traces=graphPanel->GetTraceCount()-1;
  graphPanel->SetLegend(traces-2, wxString::Format("HR: %s", shortName.GetData()));
  graphPanel->SetLegend(traces-1, wxString::Format("Speed: %s", shortName.GetData()));
  graphPanel->SetLegend(traces,wxString::Format("Cad:%s", shortName.GetData()));
  traceColour=wxColor(0,0,0);
  if(m_filesLoaded==1)traceColour= wxColor(255,0,0);
  if(m_filesLoaded==2)traceColour= wxColor(0,255,0);
  if(m_filesLoaded==3)traceColour= wxColor(0,0,255);
  if(m_filesLoaded==4)traceColour= wxColor(255,0,255);
  if(m_filesLoaded==5)traceColour= wxColor(255,255,0);
  if(m_filesLoaded==6)traceColour= wxColor(0,255,255);
  graphPanel->SetTraceColour(traces, traceColour);
  graphPanel->SetTraceColour(traces-1, traceColour);
  graphPanel->SetTraceColour(traces-2, traceColour);

  oneLine=pf.lineList.GetFirst();
  while(oneLine!=NULL){
	 dat=oneLine->GetData();
	 graphPanel->AddPoint(traces-2, wxRealPoint(dat->Seconds(), dat->hr));
	 graphPanel->AddPoint(traces-1, wxRealPoint(dat->Seconds(), dat->speed));
	 graphPanel->AddPoint(traces  , wxRealPoint(dat->Seconds(), dat->cadence));
	 oneLine=oneLine->GetNext();
  }
  graphPanel->Refresh();
  m_filesLoaded++;
}

// ---- Open Powertap ----
void ppWinFrame::OnOpenPT(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxFileDialog fd(this, "Choose a Powertap file",
						theProfile->ptDefDir,		  // default dir
						"", // default file
						"Powertap files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  filename=fd.GetPath();
  OpenPT(filename);
}

void ppWinFrame::OpenPT(wxString filename)
{
  wxString shortName;
  int ii;
  int traces;
  wxColour traceColour;
  PowerTapLineList::Node *oneLine;
  PowerTapLine *dat;
  int lastId;

  shortName=filename;
  do{
	 ii=shortName.Find("\\");
	 if(ii==-1) ii=shortName.Find("/");
	 if(ii>=0) shortName=shortName.Mid(ii+1);
  }while(ii>=0);

  PowerTapFile ptFile;
  ptFile.LoadFile(filename);
  graphPanel->AddTrace();
  graphPanel->AddTrace();
  graphPanel->AddTrace();
  traces=graphPanel->GetTraceCount()-1;
  graphPanel->SetLegend(traces-2, wxString::Format("Power: %s", shortName.GetData()));
  graphPanel->SetLegend(traces-1, wxString::Format("MPH: %s", shortName.GetData()));
  graphPanel->SetLegend(traces,wxString::Format("HR:%s",shortName.GetData()));
  traceColour=wxColor(0,0,0);
  if(m_filesLoaded==1)traceColour= wxColor(255,0,0);
  if(m_filesLoaded==2)traceColour= wxColor(0,255,0);
  if(m_filesLoaded==3)traceColour= wxColor(0,0,255);
  if(m_filesLoaded==4)traceColour= wxColor(255,0,255);
  if(m_filesLoaded==5)traceColour= wxColor(255,255,0);
  if(m_filesLoaded==6)traceColour= wxColor(0,255,255);
  graphPanel->SetTraceColour(traces, traceColour);
  graphPanel->SetTraceColour(traces-1, traceColour);
  graphPanel->SetTraceColour(traces-2, traceColour);

  oneLine=ptFile.lineList.GetFirst();
  lastId=0;
  while(oneLine!=NULL){
	 dat=oneLine->GetData();
	 if(dat->id!=lastId) graphPanel->AddLine(dat->Seconds(), true, wxColour(255,255,0));
	 graphPanel->AddPoint(traces-2, wxRealPoint(dat->Seconds(), dat->watts));
	 graphPanel->AddPoint(traces-1, wxRealPoint(dat->Seconds(), dat->Mph()));
	 graphPanel->AddPoint(traces  , wxRealPoint(dat->Seconds(), dat->hr));
	 lastId=dat->id;
	 oneLine=oneLine->GetNext();
  }
  graphPanel->Refresh();
  m_filesLoaded++;
}

// ---- Open PowerPlan ----
void ppWinFrame::OnOpenPP(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxFileDialog fd(this, "Choose a powerplan file",
						theProfile->ppDefDir,	// default dir
						"",			  // default file
						"Powerplan Files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  filename=fd.GetPath();
  OpenPP(filename);
}

// ---- Open Computrainer ----
void ppWinFrame::OnOpenCT(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxFileDialog fd(this, "Choose a computrainer file",
						theProfile->ctDefDir,	// default dir
						"",			  // default file
						"Computrainer Files (*.txt)|*.txt|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  filename=fd.GetPath();
  OpenCT(filename);
}

void ppWinFrame::OpenCT(wxString filename)
{
  wxString shortName;
  int ii;
  int traces;
  wxColour traceColour;
  ComputrainerLineList::Node *oneLine;
  ComputrainerLine *dat;

  shortName=filename;
  do{
	 ii=shortName.Find("\\");
	 if(ii==-1) ii=shortName.Find("/");
	 if(ii>=0) shortName=shortName.Mid(ii+1);
  }while(ii>=0);

  ComputrainerFile ctFile;
  ctFile.LoadFile(filename, theProfile->ctCorrection);
  graphPanel->AddTrace();
  graphPanel->AddTrace();
  traces=graphPanel->GetTraceCount()-1;
  graphPanel->SetLegend(traces-1, wxString::Format("Power: %s", shortName.GetData()));
  graphPanel->SetLegend(traces,wxString::Format("HR:%s",shortName.GetData()));
  traceColour=wxColor(0,0,0);
  if(m_filesLoaded==1)traceColour= wxColor(255,0,0);
  if(m_filesLoaded==2)traceColour= wxColor(0,255,0);
  if(m_filesLoaded==3)traceColour= wxColor(0,0,255);
  if(m_filesLoaded==4)traceColour= wxColor(255,0,255);
  if(m_filesLoaded==5)traceColour= wxColor(255,255,0);
  if(m_filesLoaded==6)traceColour= wxColor(0,255,255);
  graphPanel->SetTraceColour(traces, traceColour);
  graphPanel->SetTraceColour(traces-1, traceColour);

  oneLine=ctFile.lineList.GetFirst();
  while(oneLine!=NULL){
	 dat=oneLine->GetData();
	 graphPanel->AddPoint(traces-1, wxRealPoint(dat->Seconds(), dat->watts));
	 graphPanel->AddPoint(traces  , wxRealPoint(dat->Seconds(), dat->hr));
	 oneLine=oneLine->GetNext();
  }
  graphPanel->Refresh();
  m_filesLoaded++;
}

void ppWinFrame::StartHRRepair(wxCommandEvent& WXUNUSED(event))
{
  if(m_repairingHR==true) return; // don't start doing this twice
  m_repairingHR=true;
  graphPanel->AddLine(10, true);
  graphPanel->AddLine(100, true);
  graphPanel->SetLineColour(0, wxColour(255,255,0));
  graphPanel->SetLineColour(1, wxColour(255,255,0));
}

// ----------------------------------------------------------------------
// PowerPlotWindow
// ----------------------------------------------------------------------
PowerPlotWindow::PowerPlotWindow(wxWindow* parent, const wxString& title, 
											const wxPoint& pos, const wxSize& size,
                                 UserProfile *pf){
  
  frame=new ppWinFrame(parent, _T("Plotting Window"), 
									  wxPoint(50,50), wxSize(450, 640),
									  wxDEFAULT_FRAME_STYLE);
  frame->theProfile=pf;
  frame->Show(true);
}

