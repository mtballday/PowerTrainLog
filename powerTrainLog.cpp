
/////////////////////////////////////////////////////////////////////////////
// Name:        powerplot.cpp
// Purpose:     graphPanel sample
// Author:      Richard Abbott
// Modified by:
// Created:     2003-03-27
// RCS-ID:      $Id: powerTrainLog.cpp,v 1.58 2010/01/02 22:59:56 rich Exp $
// Copyright:   (c) Richard Abbott
// Licence:     All rights reserved
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
#include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------
// Other header files
// ----------------------------------------------------------------------
#include "wx/notebook.h"
#include "wx/imaglist.h"

#include"graphPanel.h"
#include"wxReadTextFile.h"
#include"powertap.h"
#include"bikeDayEntry.h"
#include"userProfile.h"
//#include"entryChooser.h"
#include"entryCalendar.h"
#include"powerplot.h"
#include"workoutMenuEditor.h"
#include"weekViewPanel.h"
#include"singleWeekViewPanel.h"
#include"batchEdit.h"
#include"weekview.xpm"
#include"calendarview.xpm"
#include"timePeriod.h"
#include"profileEditor.h"
#include"combineFilesDialog.h"
#include"ptConvert.h"
#include"ctConvert.h"
#include"polarConvert.h"
#include"garminConvert.h"
#include"trainingBalance.h"

//#include"workouts.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
#define LTPOWER 245
// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
  // override base class virtuals
  // ----------------------------

  // this one is called on application startup and is a good place for the app
  // initialization (doing it here and not in the ctor allows to have an error
  // return: if OnInit() returns false, the application terminates)
  virtual bool OnInit();
};
//`myframe
// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
  // variables
  UserProfile currentProfile;
  //  EntryChooser *entryChooser;

  // ctor(s)
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
			 long style = wxDEFAULT_FRAME_STYLE);
  ~MyFrame();
  // event handlers (these functions should _not_ be virtual)
  void OnFileSave(wxCommandEvent& event);
  void OnProfileEdit(wxCommandEvent& WXUNUSED(event));
  void OnViewPlan(wxCommandEvent& event);
  void OnViewHeartInZone(wxCommandEvent& event);
  void OnTssView(wxCommandEvent& event);
  void OnViewTotals(wxCommandEvent& event);
  bool LoadData();
  void OnFixPT(wxCommandEvent& event);
  void OnCombineFiles(wxCommandEvent &event);
  void OnTest(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnLoadProfile(wxCommandEvent& WXUNUSED(event));
  void OnDayEntry();
  void OnPlotWindow(wxCommandEvent& WXUNUSED(event));
  void OnBalanceWindow(wxCommandEvent& WXUNUSED(event));
  void OnWorkoutEditor(wxCommandEvent& event);
  void OnBatchEdit(wxCommandEvent& event);
  void OnImportPT(wxCommandEvent& WXUNUSED(event));
  void OnImportCT(wxCommandEvent& WXUNUSED(event));
  void OnImportPolar(wxCommandEvent& WXUNUSED(event));
  void OnImportGarmin(wxCommandEvent& WXUNUSED(event));
  void OnSplitPT(wxCommandEvent& event);
  void OnExportWeekCSV(wxCommandEvent &event);
  
private:
  wxNotebook   *m_notebook;
  WeekViewPanel *weekPanel;
  EntryCalendar *calPanel;
  SingleWeekViewPanel *singleWeekPanel;

  wxString     iniFilename;

  bool CreateSmooth(TraceClass *origTrace, TraceClass *newTrace,
						  double rollingTime);

  wxString defDir;
  wxString defPtDir;
  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
  {
	 // menu items
	 PowerPlot_Quit = 1,
	 PowerPlot_FileSave,
	 PowerPlot_FixPowertapHr,
	 PowerPlot_CombineFiles,
	 PowerPlot_PlotWindow,
	 PowerPlot_ProfileEdit,
	 PowerPlot_Test,
	 PowerPlot_MeanText,
	 PowerPlot_TSS,
	 PowerPlot_ViewHrInZone,
	 PowerPlot_ViewTotals,
	 PowerPlot_TSSView,
	 PowerPlot_ViewPlan,
    PowerPlot_TrainingBalancePlot,
	 PowerPlot_WorkoutEditor,
	 PowerPlot_SplitPT,
	 PowerPlot_ImportPT,
	 PowerPlot_ImportCT,
	 PowerPlot_ImportPolar,
    PowerPlot_ImportGarmin,
	 PowerPlot_ExportWeekCSV,

	 PowerPlot_BatchEdit,

	 ID_NOTEBOOK,

	 PowerPlot_About = wxID_ABOUT,
	 MainMenu_UserOpen
  };

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(PowerPlot_Test, MyFrame::OnTest)
  EVT_MENU(PowerPlot_FileSave, MyFrame::OnFileSave)
  EVT_MENU(PowerPlot_FixPowertapHr, MyFrame::OnFixPT)
  EVT_MENU(PowerPlot_CombineFiles, MyFrame::OnCombineFiles)
  EVT_MENU(PowerPlot_PlotWindow, MyFrame::OnPlotWindow)
  EVT_MENU(PowerPlot_ProfileEdit, MyFrame::OnProfileEdit)

  EVT_MENU(PowerPlot_ViewHrInZone, MyFrame::OnViewHeartInZone)
  EVT_MENU(PowerPlot_ViewTotals, MyFrame::OnViewTotals)
  EVT_MENU(PowerPlot_ViewPlan, MyFrame::OnViewPlan)
  EVT_MENU(PowerPlot_TSSView,  MyFrame::OnTssView)
  EVT_MENU(PowerPlot_TrainingBalancePlot, MyFrame::OnBalanceWindow)

  EVT_MENU(PowerPlot_Quit,  MyFrame::OnQuit)
  EVT_MENU(PowerPlot_About, MyFrame::OnAbout)

  EVT_MENU(MainMenu_UserOpen, MyFrame::OnLoadProfile)
  EVT_MENU(PowerPlot_WorkoutEditor, MyFrame::OnWorkoutEditor)

  EVT_MENU(PowerPlot_BatchEdit, MyFrame::OnBatchEdit)

  EVT_MENU(PowerPlot_ImportPT, MyFrame::OnImportPT)
  EVT_MENU(PowerPlot_ImportCT, MyFrame::OnImportCT)
  EVT_MENU(PowerPlot_ImportPolar, MyFrame::OnImportPolar)
  EVT_MENU(PowerPlot_ImportGarmin, MyFrame::OnImportGarmin)
  EVT_MENU(PowerPlot_SplitPT, MyFrame::OnSplitPT)
  EVT_MENU(PowerPlot_ExportWeekCSV, MyFrame::OnExportWeekCSV)

  END_EVENT_TABLE()

  IMPLEMENT_APP(MyApp)
  ;

bool MyApp::OnInit()
{
  // create the main application window
  MyFrame *frame = new MyFrame(_T("Training Log"),
										 wxPoint(50, 50), wxSize(1000, 800),
										 wxDEFAULT_FRAME_STYLE);
  frame->Show(TRUE);
  return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// `myframe
// ----------------------------------------------------------------------------
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
  : wxFrame(NULL, -1, title, pos, size, style)
{
  wxString inText;
  int ii;

  SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
  // create a menu bar
  wxMenu *menuFile=new wxMenu;
  wxMenu *viewMenu=new wxMenu;
  wxMenu *toolsMenu=new wxMenu;
 
  // the "About" item should be in the help menu
  wxMenu *helpMenu = new wxMenu;
  menuFile->Append(PowerPlot_FileSave, _T("&Save\tAlt-S"), _T("Save a file"));
  menuFile->Append(PowerPlot_PlotWindow,_T("P&lot window\tAlt-L"), _T("Plot window"));
  menuFile->Append(MainMenu_UserOpen,  _T("Pr&ofile\tAlt-O"), _T("Open a user profile"));
  menuFile->Append(PowerPlot_ProfileEdit, _T("Profile &Edit\t"), _T("Edit the user profile"));
  menuFile->Append(PowerPlot_FixPowertapHr, _T("&Repair PT"), _T("Repair PT HR"));
  //  menuFile->Append(PowerPlot_TSS,      _T("&TSS\tAlt-T"), _T("Calculate TSS"));
  menuFile->Append(PowerPlot_Test, _T("&Test\tAlt-T"), _T("Test the program"));


  viewMenu->Append(PowerPlot_ViewHrInZone, _T("&Zone View\tAlt-Z"), _T("View the HR zone bars"));
  viewMenu->Append(PowerPlot_ViewTotals, _T("Totals View\tAlt-O"), _T("View the totals screen"));
  viewMenu->Append(PowerPlot_TSSView,    _T("TSS View\tAlt-T"), _T("View TSS values"));
  viewMenu->Append(PowerPlot_ViewPlan, _T("Plan View\tAlt-P"), _T("View the planning screen"));
  viewMenu->Append(PowerPlot_TrainingBalancePlot, _T("Training Balance"), _T("Open a Training Balance window"));
  
  toolsMenu->Append(PowerPlot_ImportPT,      _T("Import Powertap\tAlt-I"), _T("Import a powertap file into Powertrainlog format"));
  toolsMenu->Append(PowerPlot_ImportCT,      _T("Import Computertrainer\tAlt-I"), _T("Import computrainer file into Powertrainlog format"));
  toolsMenu->Append(PowerPlot_ImportPolar,   _T("Import Polar\tAlt-P"), _T("Import polar file into Powertrainlog format"));
  toolsMenu->Append(PowerPlot_ImportGarmin,  _T("Import Garmin\tAlt-G"), _T("Import Garmin history file entry into Powertrainlog format"));
  toolsMenu->Append(PowerPlot_CombineFiles, _T("&Combine Files"), _T("Combine Powerplan files"));

  toolsMenu->Append(PowerPlot_ExportWeekCSV, _T("Export CSV\tAlt-E"), _T("Export week totals to CSV file"));
  toolsMenu->Append(PowerPlot_BatchEdit, _T("&Batch Edit\tAlt-B"), _T("Fill with phases"));
  toolsMenu->Append(PowerPlot_WorkoutEditor, _T("Workout Editor"), _T("Edit the workout menu and workouts"));
  toolsMenu->Append(PowerPlot_SplitPT, _T("Split Powertap File"),
						  _T("Split a powertap file by its segment id's"));

  helpMenu->Append(PowerPlot_About, _T("&About...\tF1"), _T("Show about dialog"));

  menuFile->Append(PowerPlot_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

  // now append the freshly created menu to the menu bar...
  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, _T("&File"));
  menuBar->Append(viewMenu, _T("&View"));
  menuBar->Append(toolsMenu, _T("&Tools"));
  menuBar->Append(helpMenu, _T("&Help"));

  // ... and attach this menu bar to the frame
  SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
  // create a status bar just for fun (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR

  m_notebook=new wxNotebook(this, ID_NOTEBOOK);	// set up the notebook


#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
  wxImageList *imagelist = new wxImageList(20, 20);
  imagelist->Add(wxBitmap(calendarview_xpm));
  imagelist->Add(wxBitmap(weekview_xpm));
  m_notebook->AssignImageList(imagelist);
#endif

  // ----- Daily View -----
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  wxPanel *panel = new wxPanel(m_notebook);

  calPanel=new EntryCalendar(panel, -1, wxDefaultPosition,
									  wxDefaultSize, &currentProfile);
  mainSizer->Add(calPanel, 2, wxGROW, 1);

  //  mainSizer->SetSizeHints(this);
  m_notebook->AddPage(panel, _T("Daily View"), TRUE, 0);
  panel->SetSizer(mainSizer);

  // ----- Week View -----
  wxPanel *weekViewPanel=new wxPanel(m_notebook);
  weekPanel=new WeekViewPanel(weekViewPanel, -1, 
										wxDefaultPosition, wxDefaultSize,
										&currentProfile);
  weekPanel->m_parentFrame=this;
  wxBoxSizer *weekSizer=new wxBoxSizer(wxVERTICAL);
  weekSizer->Add(weekPanel, 1, wxGROW, 1);
  weekViewPanel->SetSizer(weekSizer);
  m_notebook->AddPage(weekViewPanel, _T("6 Month View"), FALSE, 1);

#define ID_WEEKVIEWPANEL 1

  // ----- Single Week View -----
  wxPanel *singleWeekViewPanel=new wxPanel(m_notebook);
  singleWeekPanel=new SingleWeekViewPanel(singleWeekViewPanel, -1, 
										wxDefaultPosition, wxDefaultSize,
										&currentProfile);
  wxBoxSizer *singleWeekSizer=new wxBoxSizer(wxVERTICAL);
  singleWeekSizer->Add(singleWeekPanel, 1, wxGROW, 1);
  singleWeekViewPanel->SetSizer(singleWeekSizer);
  m_notebook->AddPage(singleWeekViewPanel, _T("Single Week View"), FALSE, 1);
#define ID_SINGLEWEEKVIEW 2
  // -- load ini file --
  iniFilename=wxString::Format("%s/powertrainlog.ini",wxGetCwd().GetData());
  if(wxFile::Exists(iniFilename)){
	 wxReadTextFile fi(iniFilename);
	 do{
		fi.ReadLine(&inText);
		ii=inText.Find(">")+1;
		if(inText.Left(ii)=="<profile>"){
		  currentProfile.LoadProfile(inText.Mid(ii));
		}
	 }while(fi.Ready());
  }
  // -- Load User Data --
  if(wxFile::Exists(currentProfile.Filename())){
	 currentProfile.LoadData();
  }
  LoadData();						  // load the data for the user in the ini profile.
#ifndef __WXGTK__
  Maximize();
#endif
}

// ----------------------------------------------------------------------
// `CreateSmooth(TraceClass *origTrace, TraceClass *newTrace,
//                double rollingTime)
//  Create a smoothed curve
// ----------------------------------------------------------------------
bool MyFrame::CreateSmooth(TraceClass *origTrace, TraceClass *newTrace, double rollingTime)
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
// `OnTest
// ----------------------------------------------------------------------
void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
  OnDayEntry();
}

// ----------------------------------------------------------------------
// `OnProfileEdit
// 
// Edit the user's profile
// ----------------------------------------------------------------------
void MyFrame::OnProfileEdit(wxCommandEvent& WXUNUSED(event))
{
  ProfileEditor *pfEditor;
  pfEditor=new ProfileEditor(this, -1, wxDefaultPosition,
									  wxDefaultSize, &currentProfile);
  if(pfEditor->ShowModal()==wxID_OK){ // they want to save their changes
	 currentProfile.SaveProfile(currentProfile.Filename());
  }
  delete(pfEditor);
}

void MyFrame::OnPlotWindow(wxCommandEvent& WXUNUSED(event))
{
  PowerPlotWindow *pp=new PowerPlotWindow(this->GetParent(), "Plotting Window",
														wxDefaultPosition,
														wxDefaultSize,
														&currentProfile);
//  pp->Show(true);
}

void MyFrame::OnBalanceWindow(wxCommandEvent& WXUNUSED(event))
{
  TrainingBalance *tb=new TrainingBalance("Training Balance",
														wxDefaultPosition,
														wxDefaultSize,
														&currentProfile);
//  tb->Show(true);
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  wxString oneWord;
  //  double dbl;
  //  int i;

  msg.Printf( _T("PowerPlot.\nWritten by Richard Abbott (c)2003")
				  _T("\nCompiled with %s on %s %s"), wxVERSION_STRING, __DATE__, __TIME__);

  wxMessageBox(msg, _T("About PowerPlot"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnFixPT(wxCommandEvent& WXUNUSED(event))					  // 
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

// ----------------------------------------------------------------------
// `OnCombineFiles
// Combine several files into one
// ----------------------------------------------------------------------
void MyFrame::OnCombineFiles(wxCommandEvent& WXUNUSED(event))
{
  CombineFilesDialog *combineDialog;
  combineDialog=new CombineFilesDialog(this, &currentProfile);
  combineDialog->ShowModal();
  delete(combineDialog);
}

MyFrame::~MyFrame()
{
  wxString outtxt;		
  wxFile fo;
  fo.Open(iniFilename, wxFile::write);
  outtxt=wxString::Format("<profile>%s\n", currentProfile.Filename().GetData()); // last profile
  fo.Write(outtxt, outtxt.Length());
  fo.Close();
}


void MyFrame::OnLoadProfile(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxFileDialog fd(this, "Choose a profile", defDir/*default dir*/,
					"" /*default file*/,
					"User Profile (*.pfl)|*.pfl",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  defDir=fd.GetDirectory();
  filename=fd.GetPath();
  currentProfile.LoadProfile(filename);
  wxMessageDialog(this, "Loaded", "OK", wxOK);
}

void MyFrame::OnDayEntry()
{
  /*
  BikeEntryDialog *be=new BikeEntryDialog(this, currentProfile, theData);
  if(be->ShowModal()==wxID_OK){
	 theData=be->GetData();
  }
  */;
}

void MyFrame::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
  wxString filename;
  wxString outtxt;
  int ii;
  EntryList::Node *node1;
  FILE *fp;
  BikeEntryData bd;

  filename=currentProfile.DatabaseFilename();
  if(currentProfile.mainList->GetCount()<1) return; // don't do anything if
																	 // there is no data
  fp=fopen(filename, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write file", "Error", wxOK);
	 return;
  }
  node1=currentProfile.mainList->GetFirst();
  while(node1!=NULL){
	 bd=*node1->GetData();
	 fprintf(fp, wxString::Format("<date>%s\n", bd.theDate.FormatISODate().GetData()));
	 fprintf(fp, wxString::Format("<session>%d\n", bd.session));
	 fprintf(fp, wxString::Format("<weight>%lf\n", bd.weight));
	 fprintf(fp, wxString::Format("<fat>%lf\n", bd.fat));
	 fprintf(fp, wxString::Format("<resthr>%d\n", bd.restHr));
	 fprintf(fp, wxString::Format("<arrise>%s\n", bd.arriseTime.FormatTime().GetData()));
	 fprintf(fp, wxString::Format("<sleep>%s\n", bd.sleepTime.HHMMSSHH().GetData()));
	 fprintf(fp, wxString::Format("<miles>%d\n", (int)(bd.miles*100.0)));
	 fprintf(fp, wxString::Format("<bike>%s\n", bd.bicycle.GetData()));
	 fprintf(fp, wxString::Format("<ppfile>%s\n", bd.ppFile.GetData()));
	 fprintf(fp, wxString::Format("<ctfile>%s\n", bd.ctFile.GetData()));
	 fprintf(fp, wxString::Format("<ptfile>%s\n", bd.ptFile.GetData()));
	 fprintf(fp, wxString::Format("<polarfile>%s\n", bd.polarFile.GetData()));

	 fprintf(fp, wxString::Format("<pptss>%d\n", bd.ppTss));
	 fprintf(fp, wxString::Format("<polartss>%d\n", bd.polarTss));
	 fprintf(fp, wxString::Format("<pttss>%d\n", bd.ptTss));
	 fprintf(fp, wxString::Format("<cttss>%d\n", bd.ctTss));

	 fprintf(fp, wxString::Format("<ppif>%lf\n", bd.ppIf));
	 fprintf(fp, wxString::Format("<polarif>%lf\n", bd.polarIf));
	 fprintf(fp, wxString::Format("<ptif>%lf\n", bd.ptIf));
	 fprintf(fp, wxString::Format("<ctif>%lf\n", bd.ctIf));

	 for(ii=0;ii<NUMZONES;ii++){
		fprintf(fp, wxString::Format("<filezone%d>%s\n", ii,bd.fileZoneTimes[ii].HHMMSSHH().GetData()));
	 }
	 for(ii=0;ii<NUMZONES;ii++){
		fprintf(fp, wxString::Format("<manualzone%d>%s\n", ii,bd.manualZoneTimes[ii].HHMMSSHH().GetData()));
	 }
    if(bd.keyEvent==true) fprintf(fp, wxString::Format("<keyevent>%d\n", (int)bd.keyEvent));
    if(bd.sick==true) fprintf(fp, wxString::Format("<sick>%d\n", (int)bd.sick));
    if(bd.offRoad==true) fprintf(fp, wxString::Format("<offroad>%d\n", (int)bd.offRoad));

	 outtxt=bd.notes;
	 outtxt.Replace("\n", "~", true);
	 fprintf(fp, wxString::Format("<notes>%s\n", outtxt.GetData()));
	 fprintf(fp, wxString::Format("<planDescription>%s\n", bd.m_planDescription.GetData()));
	 fprintf(fp, wxString::Format("<planMesoCycle>%s\n", bd.m_planPhase.GetData()));
	 fprintf(fp, wxString::Format("<planMesoCycleWeek>%d\n", bd.m_mesoCycleWeek));
	 fprintf(fp, wxString::Format("<planTime>%s\n", bd.m_planTime.HHMMSSHH().GetData()));
	 fprintf(fp, wxString::Format("<planIntensity>%d\n", bd.m_planIntensity));
	 fprintf(fp, wxString::Format("<planNotes>%s\n", bd.m_planNotes.GetData()));
	 fprintf(fp, wxString::Format("<racepriority>%d\n", bd.m_priority));
	 fprintf(fp, wxString::Format("<end>\n"));
	 node1=node1->GetNext();
  }
  fclose(fp);
}

bool MyFrame::LoadData()
{
  wxString filename;
  wxString inText, outtxt;
  int ii, jj;
  BikeEntryData *bd=new BikeEntryData();
  double dTmp;
  long lTmp;

  filename=currentProfile.DatabaseFilename();
  if(wxFile::Exists(filename)==false){
	 delete(bd);
	 return(false);
  }
  currentProfile.mainList->Clear();	// empty the list
  // there is no data
  wxReadTextFile fi(filename);
  do{
	 fi.ReadLine(&inText);
	 ii=inText.Find(">")+1;
	 if(ii>1){
		if(inText.Left(ii)=="<date>"){
		  bd->theDate.ParseDate(inText.Mid(ii).GetData());
		  outtxt=bd->theDate.FormatISODate();
		}
		if(inText.Left(ii)=="<session>"){
		  bd->session=atoi(inText.Mid(ii).GetData());
		}
		if(inText.Left(ii)=="<weight>"){
		  inText.Mid(ii).ToDouble(&bd->weight);
		}
		if(inText.Left(ii)=="<fat>"){
		  inText.Mid(ii).ToDouble(&bd->fat);
		}
		if(inText.Left(ii)=="<resthr>"){
		  inText.Mid(ii).ToDouble(&dTmp);
		  bd->restHr=(int)dTmp;
		}
		if(inText.Left(ii)=="<arrise>"){
		  bd->arriseTime.ParseTime(inText.Mid(ii));
		}
		if(inText.Left(ii)=="<sleep>"){
		  bd->sleepTime=TimePeriod(inText.Mid(ii));
		}
		if(inText.Left(ii)=="<miles>"){
		  inText.Mid(ii).ToDouble(&dTmp);
		  bd->miles=dTmp/100.0;
		}
		if(inText.Left(ii)=="<bike>"){
		  bd->bicycle=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<pptss>"){
		  bd->ppTss=atoi(inText.Mid(ii).GetData());
		}
		if(inText.Left(ii)=="<polartss>"){
		  bd->polarTss=atoi(inText.Mid(ii).GetData());
		}
		if(inText.Left(ii)=="<cttss>"){
		  bd->ctTss=atoi(inText.Mid(ii).GetData());
		}
		if(inText.Left(ii)=="<pttss>"){
		  bd->ptTss=atoi(inText.Mid(ii).GetData());
		}
		if(inText.Left(ii)=="<ppif>"){
		  inText.Mid(ii).ToDouble(&bd->ppIf);
		}
		if(inText.Left(ii)=="<polarif>"){
		  inText.Mid(ii).ToDouble(&bd->polarIf);
		}
		if(inText.Left(ii)=="<ctif>"){
		  inText.Mid(ii).ToDouble(&bd->ctIf);
		}
		if(inText.Left(ii)=="<ptif>"){
		  inText.Mid(ii).ToDouble(&bd->ptIf);
		}
		if(inText.Left(ii)=="<keyevent>"){
		  inText.Mid(ii).ToDouble(&dTmp);
        if(dTmp>0) bd->keyEvent=true;
		}
		if(inText.Left(ii)=="<sick>"){
		  inText.Mid(ii).ToDouble(&dTmp);
        if(dTmp>0) bd->sick=true;
		}
		if(inText.Left(ii)=="<offroad>"){
		  inText.Mid(ii).ToDouble(&dTmp);
        if(dTmp>0) bd->offRoad=true;
		}

		if(inText.Left(ii)=="<ppfile>"){
		  bd->ppFile=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<ctfile>"){
		  bd->ctFile=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<ptfile>"){
		  bd->ptFile=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<polarfile>"){
		  bd->polarFile=inText.Mid(ii);
		}
		for(jj=0;jj<NUMZONES;jj++){
		  if(inText.Left(ii)==wxString::Format("<filezone%d>", jj)){
			 bd->fileZoneTimes[jj]=TimePeriod(inText.Mid(ii));
		  }
		  if(inText.Left(ii)==wxString::Format("<manualzone%d>", jj)){
			 bd->manualZoneTimes[jj]=TimePeriod(inText.Mid(ii));
		  }
		}
		if(inText.Left(ii)=="<notes>"){
		  outtxt=inText.Mid(ii);
		  outtxt.Replace("~", "\n", true);
		  bd->notes=outtxt;
		}
		if(inText.Left(ii)=="<end>"){ // done reading this entry. append to list
		  currentProfile.mainList->Append(bd);
		  bd=new BikeEntryData();
		}
		if(inText.Left(ii)=="<planDescription>"){
		  bd->m_planDescription=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<planMesoCycle>"){
		  bd->m_planPhase=inText.Mid(ii);		  
		}
		if(inText.Left(ii)=="<planTime>"){
		  bd->m_planTime=TimePeriod(inText.Mid(ii));
		}
		if(inText.Left(ii)=="<planIntensity>"){
		  bd->m_planIntensity=atoi(inText.Mid(ii));
		}
		if(inText.Left(ii)=="<planNotes>"){
		  bd->m_planNotes=inText.Mid(ii);
		}
		if(inText.Left(ii)=="<planMesoCycleWeek>"){
		  outtxt=inText.Mid(ii);
		  outtxt.ToLong(&lTmp);
		  bd->m_mesoCycleWeek=lTmp;
		}
		if(inText.Left(ii)=="<racepriority>"){
		  outtxt=inText.Mid(ii);
		  outtxt.ToLong(&lTmp);
		  bd->m_priority=lTmp;
		}
	 }
  }while(fi.Ready());
  //  entryChooser->FillList();
  delete(bd);
  return(true);
}

void MyFrame::OnViewPlan(wxCommandEvent& WXUNUSED(event))
{
  calPanel->OnViewPlan();
}

void MyFrame::OnViewHeartInZone(wxCommandEvent& WXUNUSED(event))
{
  calPanel->OnViewHeartInZone();
}

void MyFrame::OnTssView(wxCommandEvent& WXUNUSED(event))
{
  calPanel->OnTssView();
}

void MyFrame::OnViewTotals(wxCommandEvent& WXUNUSED(event))
{
  calPanel->OnViewTotals();
}

void MyFrame::OnWorkoutEditor(wxCommandEvent& WXUNUSED(event))
{
  WorkoutMenuEditor we(this, &currentProfile);
  we.ShowModal();
}

void MyFrame::OnExportWeekCSV(wxCommandEvent & WXUNUSED(event))
{
  wxString fn;
  wxFileDialog fd(this, "Choose an output filename", defDir/*default dir*/,
					"" /*default file*/,
					"csv (*.csv)|*.csv",
					wxFD_SAVE);
  if(fd.ShowModal()==wxID_CANCEL) return;
  fn=fd.GetPath();
  calPanel->ExportWeekCSV(fn);
}


void MyFrame::OnBatchEdit(wxCommandEvent& WXUNUSED(event))
{
  int currentPanel;
  wxDateTime startDate;
  wxDateTime endDate;

  BatchEditDialog bd(this, &currentProfile);
  // set the dates to the current week
  currentPanel=m_notebook->GetSelection(); // which panel are we looking at?
  if(currentPanel==ID_WEEKVIEWPANEL){ // 6 month view
	 startDate=weekPanel->m_currentDate;
	 endDate=weekPanel->m_currentDate;
	 startDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
	 endDate.SetToWeekDayInSameWeek(wxDateTime::Sun, wxDateTime::Monday_First);
	 bd.weekViewParent=weekPanel;
  }else if(currentPanel==ID_SINGLEWEEKVIEW){	// plan view
	 startDate=singleWeekPanel->m_calStart;
	 endDate=singleWeekPanel->m_calStart;
	 startDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
	 endDate.SetToWeekDayInSameWeek(wxDateTime::Sun, wxDateTime::Monday_First);
  }else{								  // others (calendar)
	 startDate=calPanel->currentDate;
	 endDate=calPanel->currentDate;
	 startDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
	 endDate.SetToWeekDayInSameWeek(wxDateTime::Sun, wxDateTime::Monday_First);
  }
  bd.startDateCalendar->SetDate(startDate);
  bd.endDateCalendar->SetDate(endDate);
  bd.ShowModal();
  calPanel->Refresh(true);
  weekPanel->Refresh(true);
}

// ----------------------------------------------------------------------
// OnImportPT
/// Import a powertap file into the powerTrainLog format
void MyFrame::OnImportPT(wxCommandEvent& WXUNUSED(event))
{
  PtConvert ptConvert(this, -1, wxDefaultPosition, wxDefaultSize, &currentProfile);
  ptConvert.ShowModal();
}

// ----------------------------------------------------------------------
// OnImportCT
/// Import a powertap file into the powerTrainLog format
void MyFrame::OnImportCT(wxCommandEvent& WXUNUSED(event))
{
  CtConvert ctConvert(this, -1, wxDefaultPosition, wxDefaultSize, &currentProfile);
  ctConvert.ShowModal();
}

// ----------------------------------------------------------------------
// OnImportPolar
/// Import a power file into powerplan format
void MyFrame::OnImportPolar(wxCommandEvent& WXUNUSED(event))
{
  PolarConvert polarConvert(this, -1, wxDefaultPosition, wxDefaultSize, &currentProfile);
  polarConvert.ShowModal();
}

// ----------------------------------------------------------------------
// OnImportGarmin
/// Import a power file into powerplan format
void MyFrame::OnImportGarmin(wxCommandEvent& WXUNUSED(event))
{
  GarminConvert garminConvert(this, -1, wxDefaultPosition, wxDefaultSize, &currentProfile);
  garminConvert.ShowModal();
}

//----------------------------------------------------------------------
// OnSplitPT
/// Split PT File
void MyFrame::OnSplitPT(wxCommandEvent& WXUNUSED(event))
{
  int lastId;
  wxString filename;
  PowerTapLineList::Node *oneLine;
  PowerTapLineList::Node *thisLine,*nextLine;
  PowerTapLine *dat;
  double splitTime;
  double splitDistance;
  wxMessageDialog *msgDlg;

  wxFileDialog fd(this, "Choose a Powertap file",
						currentProfile.ptDefDir,		  // default dir
						"", // default file
						"Powertap files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  filename=fd.GetPath();

  PowerTapFile ptFile, ptFileCopy;
  ptFile.LoadFile(filename);
  ptFileCopy.LoadFile(filename);
  ptFile.WriteFile(filename+".bak");

  lastId=0;
  oneLine=ptFile.lineList.GetFirst();
  splitTime=0;
  splitDistance=0;
  while(oneLine!=NULL && splitTime==0){
	 dat=oneLine->GetData();
	 if(dat->id!=lastId){
		msgDlg=new wxMessageDialog(this,
											wxString::Format("Split at %s?",
																  TimePeriod::HHMMSSHH(dat->Seconds()).GetData()),
											"", wxYES_NO);
		if(msgDlg->ShowModal()==wxID_YES){	// found the splitpoint
		  splitTime=dat->minutes;
		  splitDistance=dat->km;
		}
		delete(msgDlg);
	 }
	 if(splitTime==0){
		lastId=dat->id;
		oneLine=oneLine->GetNext();
	 }
  }
  if(splitTime==0) return;				  // didn't split

  thisLine=oneLine;
  while(thisLine!=NULL){		  // delete the rest of this list
	 nextLine=thisLine->GetNext();
	 ptFile.lineList.DeleteNode(thisLine);
	 thisLine=nextLine;
  }

  thisLine=ptFileCopy.lineList.GetFirst();
  while(thisLine!=NULL && thisLine->GetData()->minutes<splitTime){	  // delete the beginning of the list
	 nextLine=thisLine->GetNext();
	 ptFileCopy.lineList.DeleteNode(thisLine);
	 thisLine=nextLine;
  }
  while(thisLine!=NULL){		  // subtract the time off the rest
	 dat=thisLine->GetData();
	 dat->minutes-=splitTime;
	 dat->km-=splitDistance;
	 thisLine=thisLine->GetNext();
  }
  // Write first file
  wxFileDialog fd1(this, "Choose name to save first half",
						currentProfile.ptDefDir,		  // default dir
						filename, // default file
						"Powertap files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_SAVE);
  if(fd1.ShowModal()==wxID_CANCEL) return;
  filename=fd1.GetPath();
  if(ptFile.WriteFile(filename)==true)
	 wxMessageBox("Wrote "+filename, "", wxOK);

  wxFileDialog fd2(this, "Choose name to save second half",
						currentProfile.ptDefDir,		  // default dir
						filename, // default file
						"Powertap files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_SAVE);
  if(fd2.ShowModal()==wxID_CANCEL) return;
  filename=fd2.GetPath();
  if(ptFileCopy.WriteFile(filename)==true)
	 wxMessageBox("Wrote "+filename, "", wxOK);
}
