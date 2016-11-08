// dayEntry.cpp
// Class to show a window that displays the data for a given day

// ptapdl.exe r_abbott_pt060515.csv

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include<wx/file.h>
#include<wx/filename.h>
//#include <wx/listimpl.cpp>

#include"userProfile.h"
#include"timePeriod.h"
#include"powerfile.h"
#include"polar.h"
#include"powertap.h"
#include"computrainer.h"
#include"powerplot.h"
#include"bikeDayEntry.h"
#include"hrRepairDialog.h"

enum
  {
	 ID_FILEZONE1=1,
	 ID_FILEZONE2,
	 ID_FILEZONE3,
	 ID_FILEZONE4,
	 ID_FILEZONE5,
	 ID_FILEZONE6,
	 ID_FILEZONE7,
	 ID_MANUALZONE1,
	 ID_MANUALZONE2,
	 ID_MANUALZONE3,
	 ID_MANUALZONE4,
	 ID_MANUALZONE5,
	 ID_MANUALZONE6,
	 ID_MANUALZONE7,

   ID_POWERZONE1,
   ID_POWERZONE2,
   ID_POWERZONE3,
   ID_POWERZONE4,
   ID_POWERZONE5,
   ID_POWERZONE6,
   ID_POWERZONE7,

   ID_POWERPERCENT1,
   ID_POWERPERCENT2,
   ID_POWERPERCENT3,
   ID_POWERPERCENT4,
   ID_POWERPERCENT5,
   ID_POWERPERCENT6,
   ID_POWERPERCENT7,


    ID_KEYEVENT,
    ID_SICK,

	 ID_PPTSS,
	 ID_POLARTSS,
	 ID_CTTSS,
	 ID_PTTSS,

	 ID_PPIF,
	 ID_POLARIF,
	 ID_CTIF,
	 ID_PTIF,

	 ID_PPBROWSE,
	 ID_POLARBROWSE,
	 ID_PTBROWSE,
	 ID_CTBROWSE,

	 ID_VIEWPP,
	 ID_VIEWPOLAR,
	 ID_VIEWPT,
	 ID_VIEWCT,

	 ID_ANALYZEPP,
	 ID_ANALYZEPOLAR,
	 ID_ANALYZEPT,
	 ID_ANALYZECT,

	 ID_REPAIRPP,
	 ID_PP_TSS,

	 ID_INTERVALLIST,

	 ID_OK,
	 ID_CANCEL
  };

BEGIN_EVENT_TABLE(BikeEntryDialog, wxDialog)
  EVT_TEXT(ID_MANUALZONE1, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE2, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE3, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE4, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE5, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE6, BikeEntryDialog::OnManual)
  EVT_TEXT(ID_MANUALZONE7, BikeEntryDialog::OnManual)

  EVT_BUTTON(ID_PPBROWSE,    BikeEntryDialog::OnBrowsePP)
  EVT_BUTTON(ID_POLARBROWSE, BikeEntryDialog::OnBrowsePolar)
  EVT_BUTTON(ID_PTBROWSE,    BikeEntryDialog::OnBrowsePt)
  EVT_BUTTON(ID_CTBROWSE,    BikeEntryDialog::OnBrowseCt)

  EVT_BUTTON(ID_REPAIRPP,    BikeEntryDialog::OnRepairPP)
  EVT_BUTTON(ID_PP_TSS,      BikeEntryDialog::OnPPHRtoTSS)
  EVT_BUTTON(ID_ANALYZEPP,   BikeEntryDialog::OnAnalyzePP)
  EVT_BUTTON(ID_ANALYZEPOLAR,BikeEntryDialog::OnAnalyzePolar)
  EVT_BUTTON(ID_ANALYZEPT,   BikeEntryDialog::OnAnalyzePt)
  EVT_BUTTON(ID_ANALYZECT,   BikeEntryDialog::OnAnalyzeCt)

  EVT_BUTTON(ID_OK,          BikeEntryDialog::OnOK)
  EVT_BUTTON(ID_CANCEL,      BikeEntryDialog::OnCancel)

  EVT_BUTTON(ID_VIEWPP,   BikeEntryDialog::OnViewPP)
  EVT_BUTTON(ID_VIEWPOLAR,   BikeEntryDialog::OnViewPolar)
  EVT_BUTTON(ID_VIEWPT,   BikeEntryDialog::OnViewPT)
  EVT_BUTTON(ID_VIEWCT,   BikeEntryDialog::OnViewCT)

  END_EVENT_TABLE()

  ;

BikeEntryDialog::BikeEntryDialog(wxWindow *parent, UserProfile *pf, BikeEntryData thedata)
  :wxDialog(parent, -1, "Day Entry",
			  wxDefaultPosition, wxDefaultSize,
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
{

  int ii;
  wxBikeListNode *curNode;

  dialogInitialized=false;

  SetTitle(wxString::Format("Entry for %s - %s: Session #%d",
									 thedata.theDate.FormatISODate().GetData(),
									 thedata.theDate.GetWeekDayName(thedata.theDate.GetWeekDay()
																			  ,wxDateTime::Name_Full).GetData(),
									 thedata.session));
  theDate=thedata.theDate;
  session=thedata.session;
  // ------- Variable Init ------
  userProfile=pf;

  m_planDescription=thedata.m_planDescription;
  m_planNotes=thedata.m_planNotes;
  m_planMesoCycleWeek=thedata.m_mesoCycleWeek;
  m_planPhase=thedata.m_planPhase;
  m_priority=thedata.m_priority;
  m_planTime=thedata.m_planTime;
  m_planIntensity=thedata.m_planIntensity;

  // ------- GUI -------
  getUpTimeCtrl=new wxTextCtrl(this, -1, thedata.arriseTime.FormatTime().GetData());
  sleepTimeCtrl=new wxTextCtrl(this, -1, thedata.sleepTime.HHMM().GetData());
  weightCtrl=   new wxTextCtrl(this, -1, wxString::Format("%3.1lf", thedata.weight));
  fatCtrl=      new wxTextCtrl(this, -1, wxString::Format("%3.1lf", thedata.fat));
  restHrCtrl=   new wxTextCtrl(this, -1, wxString::Format("%d", thedata.restHr));
  keyEventCtrl= new wxCheckBox(this, -1, "Key Event");
  keyEventCtrl->SetValue(thedata.keyEvent);

  sickCtrl= new wxCheckBox(this, -1, "Sick");
  sickCtrl->SetValue(thedata.sick);

  notesCtrl=    new wxTextCtrl(this, -1, thedata.notes, wxDefaultPosition, wxDefaultSize,
										 wxTE_MULTILINE);
  const wxString difficultyList[]={_T("Unrated"),_T("Recovery"),_T("Easy"),_T("Difficult"),
	 _T("Epic")};
  m_recoveryRatingCtrl=new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize,
												5, difficultyList);
  m_recoveryRatingCtrl->SetSelection(0);
  distanceCtrl= new wxTextCtrl(this, -1, wxString::Format("%3.2lf", thedata.miles));
  mtbCheck=     new wxCheckBox(this, -1, "Off-Road");
  mtbCheck->SetValue(thedata.offRoad);

  bikeCtrl=     new wxComboBox(this, -1, thedata.bicycle, wxDefaultPosition,
										 wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_SORT);
  curNode=userProfile->bikeList.GetFirst();
  while(curNode!=NULL){
	 bikeCtrl->Append(curNode->GetData()->GetData());
	 curNode=curNode->GetNext();
  }
  ppFileCtrl=new    wxTextCtrl(this, -1, thedata.ppFile);
  polarFileCtrl=new wxTextCtrl(this, -1, thedata.polarFile);
  ptFileCtrl=new    wxTextCtrl(this, -1, thedata.ptFile);
  ctFileCtrl=new    wxTextCtrl(this, -1, thedata.ctFile);

  ppTssCtrl   =new wxTextCtrl(this, ID_PPTSS,
										wxString::Format("%d", thedata.ppTss));
  ppIfCtrl    =new wxTextCtrl(this, ID_PPIF,
										wxString::Format("%2.3lf", thedata.ppIf));

  polarTssCtrl=new wxTextCtrl(this, ID_POLARTSS,
										wxString::Format("%d", thedata.polarTss));
  polarIfCtrl= new wxTextCtrl(this, ID_POLARIF,
										wxString::Format("%2.3lf", thedata.polarIf));

  ptTssCtrl=   new wxTextCtrl(this, ID_PTTSS,
										wxString::Format("%d", thedata.ptTss));
  ptIfCtrl=    new wxTextCtrl(this, ID_PTIF,
										wxString::Format("%2.3lf", thedata.ptIf));

  ctTssCtrl=   new wxTextCtrl(this, ID_CTTSS,
										wxString::Format("%d", thedata.ctTss));
  ctIfCtrl=    new wxTextCtrl(this, ID_CTIF,
										wxString::Format("%2.3lf", thedata.ctIf));

  ppBrowseButton=new wxButton(this, ID_PPBROWSE, "PP Filename...");
  polarBrowseButton=new wxButton(this, ID_POLARBROWSE, "Polar Filename...");
  ptBrowseButton=new wxButton(this, ID_PTBROWSE, "PT Filename...");
  ctBrowseButton=new wxButton(this, ID_CTBROWSE, "CT Filename...");
  viewPPButton   =new wxButton(this, ID_VIEWPP,    "View PP");
  viewPolarButton=new wxButton(this, ID_VIEWPOLAR, "View Polar");
  viewPtButton   =new wxButton(this, ID_VIEWPT,    "View PT");
  viewCtButton   =new wxButton(this, ID_VIEWCT,    "View CT");
  repairPPButton =new wxButton(this, ID_REPAIRPP,  "Repair HR");
  ppToTssButton  =new wxButton(this, ID_PP_TSS,    "PP HR->TSS");

  ppAnalyzeButton=   new wxButton(this, ID_ANALYZEPP,    "Analyze PP");
  polarAnalyzeButton=new wxButton(this, ID_ANALYZEPOLAR, "Analyze Polar");
  ptAnalyzeButton   =new wxButton(this, ID_ANALYZEPT,    "Analyze PT");
  ctAnalyzeButton   =new wxButton(this, ID_ANALYZECT,    "Analyze CT");


  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sleepRow=new wxBoxSizer(wxHORIZONTAL);

  // body stats box
  sleepRow->Add(new wxStaticText(this, -1, "&Arrise Time:"), 0, 0,1);
  sleepRow->Add(getUpTimeCtrl, 1, 0, 2);
  sleepRow->Add(new wxStaticText(this, -1, "&Sleep Time:"), 0, 0, 1);
  sleepRow->Add(sleepTimeCtrl, 1, 0, 2);
  sleepRow->Add(new wxStaticText(this, -1, "&Weight:"), 0, 0, 1);
  sleepRow->Add(weightCtrl, 1, 0, 2);
  sleepRow->Add(new wxStaticText(this, -1, "&Fat %:"), 0, 0, 1);
  sleepRow->Add(fatCtrl, 1, wxGROW, 1);
  sleepRow->Add(new wxStaticText(this, -1, "&Rest HR:"), 0,0,1);
  sleepRow->Add(restHrCtrl, 1, wxGROW, 1);
  sleepRow->Add(keyEventCtrl, 0, wxGROW, 1);
  sleepRow->Add(sickCtrl, 0, wxGROW, 1);

  mainSizer->Add(sleepRow, 0, wxGROW, 1);
  mainSizer->Add(notesCtrl, 1, wxGROW, 1);
  
  // Distance, etc Sizer
  wxBoxSizer *distanceSizer=new wxBoxSizer(wxHORIZONTAL);
  distanceSizer->Add(new wxStaticText(this, -1, "Recovery Rating:"), 0, wxGROW, 1);
  distanceSizer->Add(m_recoveryRatingCtrl, 0, wxGROW, 1);

  distanceSizer->Add(new wxStaticText(this, -1, "&Distance:"), 0, 0, 1);
  distanceSizer->Add(distanceCtrl, 0, wxGROW, 1);
  
  distanceSizer->Add(mtbCheck, 0, wxGROW/*|wxALIGN_CENTER_VERTICAL*/, 5); 

  distanceSizer->Add(new wxStaticText(this, -1, "&Bicycle:"), 0, 0, 1);
  distanceSizer->Add(bikeCtrl, 1, wxGROW/*|wxALIGN_CENTER_VERTICAL*/, 1);

  mainSizer->Add(distanceSizer, 0, wxGROW, 1);

  // hr zones box
  wxGridSizer *hrZonesSizer=new wxGridSizer(8, 9, 0, 1);
  hrZonesSizer->Add(new wxStaticText(this, -1, ""));
  for(ii=0;ii<4;ii++){
	 hrZonesSizer->Add(new wxStaticText(this, -1, wxString::Format("Zone %d", ii+1)));
  }
  hrZonesSizer->Add(new wxStaticText(this, -1, "Zone 5a"));
  hrZonesSizer->Add(new wxStaticText(this, -1, "Zone 5b"));
  hrZonesSizer->Add(new wxStaticText(this, -1, "Zone 5c"));
  hrZonesSizer->Add(new wxStaticText(this, -1, "Totals"));

  hrZonesSizer->Add(new wxStaticText(this, -1, wxString::Format("HR:")));

  for(ii=1;ii<8;ii++){
	 hrZonesSizer->Add(new wxStaticText(this, -1,
													wxString::Format("%d-%d", userProfile->zones[ii-1],
																		  userProfile->zones[ii]-1)));
  }
  hrZonesSizer->Add(new wxStaticText(this, -1, wxString::Format("")));

  hrZonesSizer->Add(new wxStaticText(this, -1, "From File:"));	// times from the
																					// hr file
  for(ii=0;ii<7;ii++){			  // 8th is the total
	 fileZoneTimes[ii]=new wxStaticText(this, ID_FILEZONE1+ii, thedata.fileZoneTimes[ii].HHMMSSHH());
	 hrZonesSizer->Add(fileZoneTimes[ii]);
  }
  fileZoneTimes[ii]=new wxStaticText(this, ID_FILEZONE1+ii, "0:00:00.0");
  hrZonesSizer->Add(fileZoneTimes[ii]);

  hrZonesSizer->Add(new wxStaticText(this, -1, "Manual:"));	// times manual entered
  for(ii=0;ii<7;ii++){
	 manualZoneTimes[ii]=new wxTextCtrl(this, ID_MANUALZONE1+ii, thedata.manualZoneTimes[ii].HHMMSSHH());
	 hrZonesSizer->Add(manualZoneTimes[ii]);
  }
  totalManualTime=new wxStaticText(this, -1, "0:00:00.0");
  hrZonesSizer->Add(totalManualTime);

  hrZonesSizer->Add(new wxStaticText(this, -1, "Totals:"));
  for(ii=0;ii<8;ii++){
	 totalTimes[ii]=new wxStaticText(this, -1, "0:00:00.0");
	 hrZonesSizer->Add(totalTimes[ii]);
  }
	 
  hrZonesSizer->Add(new wxStaticText(this, -1, "Pwr Zone:"));
  for(ii=1;ii<9;ii++){
    hrZonesSizer->Add(new wxStaticText(this, -1,
                                       wxString::Format("%ld-%ld", userProfile->powerZones[ii-1],
                                                        userProfile->powerZones[ii]-1)));
  }
  //hrZonesSizer->Add(new wxStaticText(this, -1, wxString::Format(""))); // not giving totals on power

  hrZonesSizer->Add(new wxStaticText(this, -1, "Time:"));
  for(ii=0;ii<8;ii++){
	 powerZoneTimes[ii]=new wxStaticText(this, ID_POWERZONE1+ii, "0:00:00.0");
	 hrZonesSizer->Add(powerZoneTimes[ii]);
  }

  hrZonesSizer->Add(new wxStaticText(this, -1, "Percent"));
  for(ii=0;ii<8;ii++){
	 powerZonePercent[ii]=new wxStaticText(this, ID_POWERPERCENT1+ii, "0%");
	 hrZonesSizer->Add(powerZonePercent[ii]);
  }

  mainSizer->Add(hrZonesSizer, 0, wxGROW, 1);
  
  // hr file box
  wxBoxSizer *viewButtonsCol= new  wxBoxSizer(wxVERTICAL);
  wxBoxSizer *browseButtonsCol=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *filenamesCol=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *labelCol= new  wxBoxSizer(wxVERTICAL);
  wxBoxSizer *tssCol  = new  wxBoxSizer(wxVERTICAL);
  wxBoxSizer *ifCol   = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *repairCol =  new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *analyzeCol=  new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hrRow=new wxBoxSizer(wxHORIZONTAL);

  viewButtonsCol->Add(viewPPButton, 1, wxGROW, 1);
  viewButtonsCol->Add(viewPolarButton, 1, wxGROW, 1);
  viewButtonsCol->Add(viewPtButton, 1, wxGROW, 1);
  viewButtonsCol->Add(viewCtButton, 1, wxGROW, 1);

  filenamesCol->Add(ppFileCtrl,    1, wxGROW, 1);
  filenamesCol->Add(polarFileCtrl, 1, wxGROW, 1);
  filenamesCol->Add(ptFileCtrl,    1, wxGROW, 1);
  filenamesCol->Add(ctFileCtrl,    1, wxGROW, 1);

  labelCol->Add(new wxStaticText(this, -1, "Tss/If"), 1, wxGROW, 1);
  labelCol->Add(new wxStaticText(this, -1, "Beats"), 1, wxGROW, 1);
  labelCol->Add(new wxStaticText(this, -1, "Tss/If"), 1, wxGROW, 1);
  labelCol->Add(new wxStaticText(this, -1, "Tss/If"), 1, wxGROW, 1);

  tssCol->Add(ppTssCtrl,    1, wxGROW, 1);
  tssCol->Add(polarTssCtrl, 1, wxGROW, 1);
  tssCol->Add(ptTssCtrl,    1, wxGROW, 1);
  tssCol->Add(ctTssCtrl,    1, wxGROW, 1);

  ifCol->Add(ppIfCtrl,      1, wxGROW, 1);
  ifCol->Add(polarIfCtrl,   1, wxGROW, 1);
  ifCol->Add(ptIfCtrl,      1, wxGROW, 1);
  ifCol->Add(ctIfCtrl,      1, wxGROW, 1);

  browseButtonsCol->Add(ppBrowseButton,    1, wxGROW, 1);
  browseButtonsCol->Add(polarBrowseButton, 1, wxGROW, 1);
  browseButtonsCol->Add(ptBrowseButton, 1, wxGROW, 1);
  browseButtonsCol->Add(ctBrowseButton, 1, wxGROW, 1);

  repairCol->Add(repairPPButton, 1, wxGROW, 1);
  repairCol->Add(ppToTssButton, 1, wxGROW, 1);
  repairCol->AddStretchSpacer(1);
  repairCol->AddStretchSpacer(1);

  analyzeCol->Add(ppAnalyzeButton,    1, wxGROW, 1);
  analyzeCol->Add(polarAnalyzeButton, 1, wxGROW, 1);
  analyzeCol->Add(ptAnalyzeButton, 1, wxGROW, 1);
  analyzeCol->Add(ctAnalyzeButton, 1, wxGROW, 1);

  hrRow->Add(viewButtonsCol, 0, wxGROW, 1);
  hrRow->Add(labelCol, 0, wxGROW, 1);
  hrRow->Add(tssCol, 0, wxGROW, 1);
  hrRow->Add(ifCol, 0, wxGROW, 1);
  hrRow->Add(filenamesCol, 1, wxGROW, 1);
  hrRow->Add(browseButtonsCol, 0, wxGROW, 1);
  hrRow->Add(repairCol,  0, wxGROW, 1);
  hrRow->Add(analyzeCol, 0, wxGROW, 1);
  mainSizer->Add(hrRow, 0, wxGROW, 1);

  // intervals list
  intervalListCtrl=new wxListBox(this, ID_INTERVALLIST,
											wxDefaultPosition, wxDefaultSize,
											0, NULL, wxLB_SINGLE);
  mainSizer->Add(intervalListCtrl, 1, wxGROW, 1);

  // Cancel, Ok
  wxBoxSizer *okcancelrow=new wxBoxSizer(wxHORIZONTAL);
  okcancelrow->Add(new wxButton(this, ID_CANCEL, "&Cancel"), 1, wxGROW, 1);
  okcancelrow->Add(new wxButton(this, ID_OK,     "&Ok"), 1, wxGROW, 1);
  mainSizer->Add(okcancelrow, 0, wxGROW, 1);
  SetSizer(mainSizer);
  mainSizer->Fit(this);

  // move to center of screen
  Centre(wxBOTH);

  // some are disabled if not session 1
  if(session!=1){
	 weightCtrl->Enable(false);
	 fatCtrl->Enable(false);
	 restHrCtrl->Enable(false);
	 getUpTimeCtrl->Enable(false);
	 sleepTimeCtrl->Enable(false);
  }
  dialogInitialized=true;
}

void BikeEntryDialog::OnManual(wxCommandEvent& WXUNUSED(event))
{
  int ii;
  double tsecs=0;
  wxWindow *focus=FindFocus();
  bool mod;
  mod=false;
  wxString currentString;

  for(ii=0;ii<7;ii++){			  // don't do this unless on of the windows has
										  // the focus
	 if(focus==manualZoneTimes[ii]) mod=true;
  }
  if(mod==false || dialogInitialized==false){
	 return;
  }

  for(ii=0;ii<7;ii++){
	 TimePeriod tp(manualZoneTimes[ii]->GetValue());
	 tsecs+=tp.Seconds();
	 if(manualZoneTimes[ii]!=focus){
		currentString=manualZoneTimes[ii]->GetValue();
		if(currentString!=tp.HHMMSSHH()) manualZoneTimes[ii]->SetValue(tp.HHMMSSHH());
	 }
  }
  TimePeriod tp1(tsecs);
  totalManualTime->SetLabel(tp1.HHMMSSHH());
}

void BikeEntryDialog::OnBrowsePP(wxCommandEvent& WXUNUSED(event))
{
  wxString defName;


  defName=ppFileCtrl->GetValue();

  if(defName.IsEmpty()){
	 defName=userProfile->ppPrefix+theDate.FormatISODate();
	 if(session!=1) defName+=wxString::Format("_%d", session);
	 defName+=".csv";
	 defName.Replace("-", "");
  }

  wxFileDialog fd(this, "Choose PowerPlan file", userProfile->ppDefDir/*default dir*/,
					defName /*default file*/,
					"Powerplan files (*.csv)|*.csv|All files (*.*)|*.*",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  ppFileCtrl->SetValue(fd.GetFilename());
}

void BikeEntryDialog::OnBrowsePolar(wxCommandEvent& WXUNUSED(event))
{
  wxString defName;
  int year;
  defName=polarFileCtrl->GetValue();
  if(defName.IsEmpty()){
	 year=theDate.GetYear();
	 if(year<2000) year-=1900;
	 if(year>=2000) year-=2000;
	 defName=wxString::Format("%02d%02d%02d%02d.hrm",
									  year,
									  theDate.GetMonth()+1,
									  theDate.GetDay(),
									  session);
  }
  wxFileDialog fd(this, "Choose Polar file", userProfile->polarDefDir/*default dir*/,
					defName /*default file*/,
					"Polar files (*.HRM)|*.HRM|All files (*.*)|*.*",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  polarFileCtrl->SetValue(fd.GetFilename());
}

void BikeEntryDialog::OnBrowsePt(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog fd(this, "Choose Powertap file", userProfile->ptDefDir/*default dir*/,
					ptFileCtrl->GetValue() /*default file*/,
					"Powertap Files (*.csv)|*.csv|All files (*.*)|*.*",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  ptFileCtrl->SetValue(fd.GetFilename());
}

void BikeEntryDialog::OnBrowseCt(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog fd(this, "Choose Computrainer file", userProfile->ctDefDir/*default dir*/,
					ctFileCtrl->GetValue() /*default file*/,
					"Computrainer files (*.txt)|*.txt|All files (*.*)|*.*",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return;
  ctFileCtrl->SetValue(fd.GetFilename());
}

void BikeEntryDialog::OnAnalyzePP(wxCommandEvent& WXUNUSED(event))
{
  wxString outtxt;
  Powerfile pp;
  Interval interval;			  // Used for storing intervals copied out of polar
  int ii;
  double   totalTime;

  outtxt=wxString::Format("%s%s", userProfile->ppDefDir.GetData(), ppFileCtrl->GetValue().GetData());
  if(wxFile::Exists(outtxt)==false){
	 wxMessageBox("Could not load file", "Error", wxOK);
	 return;
  }
  pp.Load(outtxt);
  if(pp.Distance(UNITS_MILES)>0)distanceCtrl->SetValue(wxString::Format("%3.2lf", pp.Distance(UNITS_MILES)));
  if(mtbCheck->GetValue()==false){
    ppTssCtrl->SetValue(wxString::Format("%ld", (long)pp.TSS(userProfile->thresholdPower)));
  }else{
    ppTssCtrl->SetValue(wxString::Format("%ld", (long)pp.TSS(userProfile->mtbThresholdPower)));
  }
  ppIfCtrl->SetValue(wxString::Format("%02lf", pp.m_if));
  pp.Trimps(userProfile->restHr, userProfile->hrlimit);

  totalTime=0;
  for(ii=1;ii<8;ii++){
	 TimePeriod tp(pp.ZoneTime(userProfile->zones[ii-1], userProfile->zones[ii]-1));
	 fileZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());
   totalTime+=tp.Seconds();
  }
  TimePeriod tp(totalTime);
  fileZoneTimes[7]->SetLabel(tp.HHMMSSHH());

  for(ii=1;ii<8;ii++){
	 TimePeriod tp(pp.PowerZoneTime(userProfile->powerZones[ii-1], userProfile->powerZones[ii]-1));
	 powerZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());
   powerZonePercent[ii-1]->SetLabel((wxString::Format("%d%%",(int)(100.0*tp.Seconds()/totalTime))));
  }

  if(m_intervals.m_firstInterval==NULL){	  // only read/add intervals if there are none in
  /*
	 ii=0;
	 do{
		interval=pp.Sections(ii);
		if(!interval.IsEmpty()){
		  m_intervals.Append(new Interval(interval));		  
		}
		ii++;
	 }while(!interval.IsEmpty());
	 SetIntervalList();
  */
	 intervalListCtrl->Append(wxString::Format("KJ: %3.0lf", pp.m_totalJoules/1000.0));
	 intervalListCtrl->Append(wxString::Format("HB: %ld", (long)pp.m_totalHeartBeats));
	 intervalListCtrl->Append(wxString::Format("NP: %ld", (long)pp.m_np));
	 intervalListCtrl->Append(wxString::Format("TRIMPS: %lf", pp.m_trimps));
  }
}

void BikeEntryDialog::OnRepairPP(wxCommandEvent& WXUNUSED(event))
{
  wxFileName fn;

  fn.SetPath(userProfile->ppDefDir);
  fn.SetName(ppFileCtrl->GetValue());

  HrRepairDialog hp(this, -1, wxDefaultPosition, wxSize(1200,800));
  hp.SetFilename(fn);
  hp.ShowModal();
}

void BikeEntryDialog::OnPPHRtoTSS(wxCommandEvent& WXUNUSED(event))
{
  double simulatedTSS;
  wxString outtxt;
  Powerfile pp;
  int ii, mult;

  outtxt=wxString::Format("%s%s", userProfile->ppDefDir.GetData(), ppFileCtrl->GetValue().GetData());
  if(wxFile::Exists(outtxt)==false){
	 wxMessageBox("Could not load file", "Error", wxOK);
	 return;
  }
  pp.Load(outtxt);
  
  simulatedTSS=0;
  for(ii=2;ii<8;ii++){
	 TimePeriod tp(pp.ZoneTime(userProfile->zones[ii-1], userProfile->zones[ii]-1));
	 fileZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());

	 mult=ii;
	 simulatedTSS+=(double)mult*(double)tp.Seconds();
  }
  simulatedTSS/=118.39;
  polarTssCtrl->SetValue(wxString::Format("%ld", (long)simulatedTSS));
}

void BikeEntryDialog::OnAnalyzePolar(wxCommandEvent& WXUNUSED(event))
{
  int ii, mult;
  wxString outtxt;
  PolarFile pt;
  Interval interval;			  // Used for storing intervals copied out of polar
  long simulatedTSS;

  outtxt=wxString::Format("%s%s", userProfile->polarDefDir.GetData(), polarFileCtrl->GetValue().GetData());
  if(wxFile::Exists(outtxt)==false){
	 wxMessageBox("Could not load file", "Error", wxOK);
	 return;
  }
  pt.LoadFile(outtxt);

  if(pt.TotalMiles()>0)distanceCtrl->SetValue(wxString::Format("%3.2lf", pt.TotalMiles()));

  simulatedTSS=0;
  for(ii=1;ii<8;ii++){
	 TimePeriod tp(pt.ZoneTime(userProfile->zones[ii-1], userProfile->zones[ii]-1));
	 fileZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());
	 mult=ii;
	 switch(ii){
	 case 6:
		mult=7;
		break;
	 case 7:
		mult=10;
		break;
	 }
	 simulatedTSS+=(double)mult*(double)tp.Seconds();
  }
  simulatedTSS/=118.39;

  if(m_intervals.m_firstInterval==NULL){	  // only read/add intervals if there are none in
										  // the list already
	 ii=0;
	 do{
		interval=pt.Sections(ii);
		if(!interval.IsEmpty()){
		  m_intervals.Append(interval);		  
		}
		ii++;
	 }while(!interval.IsEmpty());
	 SetIntervalList();
  }
  //  polarTssCtrl->SetValue(wxString::Format("%ld", pt.m_totalBeats));
  polarTssCtrl->SetValue(wxString::Format("%ld", simulatedTSS));
}

void BikeEntryDialog::OnAnalyzePt(wxCommandEvent& WXUNUSED(event))
{
  int ii;
  wxString outtxt;
  PowerTapFile pt;
  Interval interval;			  // Used for storing intervals copied out of polar

  outtxt=wxString::Format("%s%s", userProfile->ptDefDir.GetData(), ptFileCtrl->GetValue().GetData());
  if(wxFile::Exists(outtxt)==false){
	 wxMessageBox("Could not load file", "Error", wxOK);
	 return;
  }
  pt.LoadFile(outtxt);
  if(pt.TotalMiles()>0)distanceCtrl->SetValue(wxString::Format("%3.2lf", pt.TotalMiles()));
  ptTssCtrl->SetValue(wxString::Format("%ld", (long)pt.TSS(userProfile->thresholdPower)));
  ptIfCtrl->SetValue(wxString::Format("%02lf", pt.m_if));
  for(ii=1;ii<8;ii++){
	 TimePeriod tp(pt.ZoneTime(userProfile->zones[ii-1], userProfile->zones[ii]-1));
	 fileZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());
  }

  if(m_intervals.m_firstInterval==NULL){	  // only read/add intervals if there are none in
										  // the list already
	 ii=0;
	 do{
		interval=pt.Sections(ii);
		if(!interval.IsEmpty()){
		  m_intervals.Append(interval);		  
		}
		ii++;
	 }while(!interval.IsEmpty());
	 SetIntervalList();
	 intervalListCtrl->Append(wxString::Format("KJ: %3.0lf", pt.m_totalJoules/1000.0));
	 intervalListCtrl->Append(wxString::Format("HB: %ld", pt.m_totalHeartBeats));
	 intervalListCtrl->Append(wxString::Format("NP: %ld", (long)pt.m_np));
  }
}

void BikeEntryDialog::OnAnalyzeCt(wxCommandEvent& WXUNUSED(event))
{
  int ii;
  wxString outtxt;
  ComputrainerFile ct;
  outtxt=wxString::Format("%s%s", userProfile->ctDefDir.GetData(),
								  ctFileCtrl->GetValue().GetData());
  if(wxFile::Exists(outtxt)==false){
	 wxMessageBox("Could not load file", "Error", wxOK);
	 return;
  }
  ct.LoadFile(outtxt, userProfile->ctCorrection);
  if(ct.TotalMiles()>0)distanceCtrl->SetValue(wxString::Format("%3.2lf", ct.TotalMiles()));
  ctTssCtrl->SetValue(wxString::Format("%ld", (long)ct.TSS(userProfile->thresholdPower*userProfile->ctCorrection)));
  ctIfCtrl->SetValue(wxString::Format("%2.2lf", ct.m_if));
  for(ii=1;ii<8;ii++){
	 TimePeriod tp(ct.ZoneTime(userProfile->zones[ii-1], userProfile->zones[ii]-1));
	 fileZoneTimes[ii-1]->SetLabel(tp.HHMMSSHH());
  }
}

void BikeEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_OK);
}

void BikeEntryDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

BikeEntryData BikeEntryDialog::GetData()			  // return the data from the dialog
{
  int ii;
  double dTmp;

  BikeEntryData bd;
  weightCtrl->GetValue().ToDouble(&bd.weight);
  fatCtrl->GetValue().ToDouble(&bd.fat);
  restHrCtrl->GetValue().ToDouble(&dTmp); bd.restHr=(int)dTmp;
  distanceCtrl->GetValue().ToDouble(&dTmp); bd.miles=dTmp;
  bd.keyEvent=keyEventCtrl->GetValue();
  bd.sick=sickCtrl->GetValue();
  bd.offRoad=mtbCheck->GetValue();
  bd.bicycle=bikeCtrl->GetValue();
  bd.ppFile=ppFileCtrl->GetValue();
  bd.polarFile=polarFileCtrl->GetValue();
  bd.ptFile=ptFileCtrl->GetValue();
  bd.ctFile=ctFileCtrl->GetValue();
  bd.theDate=theDate;
  bd.arriseTime.ParseTime(getUpTimeCtrl->GetValue().GetData());
  bd.sleepTime.SetByHHMM(sleepTimeCtrl->GetValue());
  bd.session=session;
  bd.ppTss=atoi(ppTssCtrl->GetValue());
  bd.ctTss=atoi(ctTssCtrl->GetValue());
  bd.ptTss=atoi(ptTssCtrl->GetValue());
  bd.polarTss=atoi(polarTssCtrl->GetValue());
  ppIfCtrl->GetValue().ToDouble(&bd.ppIf);
  polarIfCtrl->GetValue().ToDouble(&bd.polarIf);
  ctIfCtrl->GetValue().ToDouble(&bd.ctIf);
  ptIfCtrl->GetValue().ToDouble(&bd.ptIf);

  for(ii=0;ii<7;ii++){
	 TimePeriod tp(fileZoneTimes[ii]->GetLabel());
	 bd.fileZoneTimes[ii]=  tp;

	 TimePeriod tp1(manualZoneTimes[ii]->GetValue());
	 bd.manualZoneTimes[ii]=tp1;
  }
  bd.notes=notesCtrl->GetValue();

  bd.m_planDescription=m_planDescription;
  bd.m_planNotes=m_planNotes;
  bd.m_mesoCycleWeek=m_planMesoCycleWeek;
  bd.m_planPhase=m_planPhase;
  bd.m_priority=m_priority;
  bd.m_planTime=m_planTime;
  bd.m_planIntensity=m_planIntensity;
  return(bd);
}

void BikeEntryDialog::OnViewPP(wxCommandEvent& WXUNUSED(event))
{
  wxBusyCursor waitCursor;
  wxFileName fn(userProfile->ppDefDir, ppFileCtrl->GetValue(), wxPATH_NATIVE);

  PowerPlotWindow *pp=new PowerPlotWindow(this->GetParent(), "Plotting Window",
														wxDefaultPosition,
														wxDefaultSize,
														userProfile);
  pp->OpenPP(fn.GetFullPath());
  EndModal(wxID_OK);
}

void BikeEntryDialog::OnViewPolar(wxCommandEvent& WXUNUSED(event))
{
  wxBusyCursor waitCursor;
  wxFileName fn(userProfile->polarDefDir, polarFileCtrl->GetValue());
  PowerPlotWindow *pp=new PowerPlotWindow(this->GetParent(), "Plotting Window",
														wxDefaultPosition,
														wxDefaultSize,
														userProfile);
  pp->OpenPolar(fn.GetFullPath());
  EndModal(wxID_OK);
}

void BikeEntryDialog::OnViewPT(wxCommandEvent& WXUNUSED(event))
{
  wxBusyCursor waitCursor;
  wxFileName fn(userProfile->ptDefDir, ptFileCtrl->GetValue());

  PowerPlotWindow *pp=new PowerPlotWindow(this->GetParent(), "Plotting Window",
														wxDefaultPosition,
														wxDefaultSize,
														userProfile);
  pp->OpenPT(fn.GetFullPath());
  EndModal(wxID_OK);
}

void BikeEntryDialog::OnViewCT(wxCommandEvent& WXUNUSED(event))
{
  wxBusyCursor waitCursor;
  wxFileName fn(userProfile->ctDefDir, ctFileCtrl->GetValue());

  PowerPlotWindow *pp=new PowerPlotWindow(this->GetParent(), "Plotting Window",
														wxDefaultPosition,
														wxDefaultSize,
														userProfile);
  pp->OpenCT(fn.GetFullPath());
  EndModal(wxID_OK);
}

// ----------------------------------------------------------------------
// SetIntervalList
/// Fills the interval list control

/// This function uses the data in the m_intervals list to fill
/// the control. It clears the list and then copies the data over.
// ----------------------------------------------------------------------
void BikeEntryDialog::SetIntervalList()
{
  Interval *interval;

  intervalListCtrl->Clear();
  m_intervals.First();

  while(m_intervals.m_currentInterval!=NULL){
	 interval=m_intervals.m_currentInterval;
	 intervalListCtrl->Append(wxString::Format("Start:%s  End:%s  (%s) %s  Watts_Mean:%3.0lf HR_Mean:%d",
															 TimePeriod(interval->startTime).HHMMSSHH().GetData(),
															 TimePeriod(interval->endTime).HHMMSSHH().GetData(),
															 interval->Duration().HHMMSSHH().GetData(),
															 interval->note.GetData(),
															 interval->avgWatts,
															 interval->averageHr));
	 m_intervals.m_currentInterval=m_intervals.m_currentInterval->next;
  }
}
