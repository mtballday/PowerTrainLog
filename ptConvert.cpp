#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ptConvert.h"
#include "powertap.h"
#include "powerfile.h"
#include "temperatureClass.h"

enum
  {
	 ID_DATECTRL=1,
	 ID_STARTCTRL,
	 ID_TEMPCTRL,
   ID_TEMPBUTTON,
	 ID_POWERTAP,
	 ID_POWERPLAN,
	 ID_PTBROWSEBUTTON,
	 ID_PPBROWSEBUTTON,
	 ID_DOWNLOADBUTTON,
	 ID_CONVERT
  }
;

BEGIN_EVENT_TABLE(PtConvert, wxDialog)
  EVT_CALENDAR_SEL_CHANGED(ID_DATECTRL, PtConvert::OnDate)
  EVT_BUTTON(ID_PTBROWSEBUTTON, PtConvert::OnPTBrowse)
  EVT_BUTTON(ID_PPBROWSEBUTTON, PtConvert::OnPPBrowse)
  EVT_BUTTON(ID_DOWNLOADBUTTON, PtConvert::OnDownload)
  EVT_BUTTON(ID_CONVERT,        PtConvert::OnConvert)
  EVT_BUTTON(ID_TEMPBUTTON,     PtConvert::OnTempButton)
END_EVENT_TABLE();



PtConvert::PtConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxDialog(parent, id, "Powertap Conversion", pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{

  // -- Variable Initialization --
  m_currentProfile=up;
  m_sessionNumber=-1;

  // -- Widgets --
  wxStaticText *dateCtrlLabel=new wxStaticText(this, -1, "Date");
  dateCtrl=new wxCalendarCtrl(this, ID_DATECTRL, wxDateTime::Now(),
														  wxDefaultPosition, wxDefaultSize,
														  wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS|wxCAL_SHOW_SURROUNDING_WEEKS);
  wxStaticText *startTimeLabel=new wxStaticText(this, -1, "Start time");
  timeCtrl=new wxTextCtrl(this, ID_STARTCTRL, "0");

  wxStaticText *temperatureLabel=new wxStaticText(this, -1, "Temperature (F)");
  tempCtrl=new wxTextCtrl(this, ID_TEMPCTRL, "");
  temperatureButton=new wxButton(this, ID_TEMPBUTTON, "Get &Temperature");

  wxStaticText *ptLabel=new wxStaticText(this, -1, "Powertap Filename");
  powertapCtrl=new wxTextCtrl(this, ID_POWERTAP, "");
  wxButton *powertapBrowse=new wxButton(this, ID_PTBROWSEBUTTON, "Browse");

  wxStaticText *ppLabel=new wxStaticText(this, -1, "PowerPlan Filename");
  powerplanCtrl=new wxTextCtrl(this, ID_POWERPLAN, "");
  wxButton *powerplanBrowse=new wxButton(this, ID_PPBROWSEBUTTON, "Browse");

  wxButton *downloadButton=new wxButton(this, ID_DOWNLOADBUTTON, "&Download");

  wxButton *convertButton=new wxButton(this, ID_CONVERT, "&Convert");
  convertButton->SetDefault();


  wxBoxSizer *dateSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *timeSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *tempSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *powertapSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *powerplanSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  dateSizer->Add(dateCtrlLabel, 1, wxGROW, 1);
  dateSizer->Add(dateCtrl, 0, wxGROW, 1);

  timeSizer->Add(startTimeLabel, 0, wxGROW, 1);
  timeSizer->Add(timeCtrl, 1, wxGROW, 1);

  tempSizer->Add(temperatureLabel, 0, wxGROW, 1);
  tempSizer->Add(tempCtrl, 1, wxGROW, 1);
  tempSizer->Add(temperatureButton, 0, wxGROW, 1);

  powertapSizer->Add(ptLabel, 0, wxGROW, 1);
  powertapSizer->Add(powertapCtrl, 1, wxGROW, 1);
  powertapSizer->Add(powertapBrowse, 0, wxGROW, 1);

  powerplanSizer->Add(ppLabel, 0, wxGROW, 1);
  powerplanSizer->Add(powerplanCtrl, 1, wxGROW, 1);
  powerplanSizer->Add(powerplanBrowse, 0, wxGROW, 1);

  mainSizer->Add(dateSizer,     0, wxGROW, 1);
  mainSizer->Add(timeSizer,     0, wxGROW, 1);
  mainSizer->Add(tempSizer,     0, wxGROW, 1);
  mainSizer->Add(powertapSizer, 0, wxGROW, 1);
  mainSizer->Add(powerplanSizer,0, wxGROW, 1);
  mainSizer->Add(downloadButton,0, wxGROW, 1);
  mainSizer->Add(convertButton, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);
}

bool PtConvert::ConvertFile(wxString ptFilename, wxString powerFilename)
{
  PowerTapFile ptFile;
  PowerTapLineList::Node *oneLine;
  PowerTapLine *dat;
  int temperature;
  Powerfile    powerFile;
  int interval;
  bool intervalInProgress;

  if(ptFile.LoadFile(ptFilename)==false){
	 return(false);
  }
  
  temperature=atoi(tempCtrl->GetValue().GetData());
  temperature=(int)(5.0*((double)temperature-32.0)/9.0);

  powerFile.startTime=timeCtrl->GetValue();

  oneLine=ptFile.lineList.GetFirst();
  interval=0;
  intervalInProgress=false;

  while(oneLine!=NULL){
	 dat=oneLine->GetData();
	 powerFile.Appendline();
	 powerFile.SetSeconds(dat->Seconds());
	 powerFile.SetWatts(dat->watts);
	 if(dat->hr<m_currentProfile->hrlimit){
		powerFile.SetHR(dat->hr);
	 }else{
		powerFile.SetHR(0);
	 }
	 powerFile.SetCadence(dat->cadence);
	 powerFile.SetKPH(dat->kph);
	 powerFile.SetDistanceMeters(dat->km*1000.0);
	 powerFile.SetAltitudeMeters(0);
	 powerFile.SetTempC(temperature);
	 powerFile.SetTorque(dat->torque);
	 if(dat->id!=interval){
		if(intervalInProgress==true){			  // finishing an interval
		  powerFile.SetIntervalEnd(dat->Seconds());
		  if(dat->id!=0){			  // starting directly into new interval
			 powerFile.AppendInterval();
			 powerFile.SetIntervalStart(dat->Seconds());
		  }else{
			 intervalInProgress=false;	// ending interval
		  }
		}else{						  // Starting new interval
		  powerFile.AppendInterval();
		  powerFile.SetIntervalStart(dat->Seconds());
		  intervalInProgress=true;
		}
	 }
	 interval=dat->id;
	 oneLine=oneLine->GetNext();
  }
  if(intervalInProgress==true){ // never ended last interval
	 powerFile.SetIntervalEnd(dat->Seconds());
	 intervalInProgress=false;
  }
  if(powerFile.Save(powerFilename)==false){
	 return(false);
  }
  return(true);
}

void PtConvert::OnPTBrowse(wxCommandEvent& WXUNUSED(event))
{
  PTBrowse(powertapCtrl->GetValue());
}

void PtConvert::OnPPBrowse(wxCommandEvent& WXUNUSED(event))
{
  PPBrowse(powerplanCtrl->GetValue());
}

void PtConvert::OnConvert(wxCommandEvent& WXUNUSED(event))
{
  wxString powertapFn;
  wxString powerplanFn;

  powertapFn=powertapCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();
  
  if(powertapFn.IsEmpty()==true){
	 if(PTBrowse("")==false) return;
  }
  if(powerplanFn.IsEmpty()==true){
	 if(PPBrowse("")==false) return;
  }

  powertapFn=powertapCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();

  if(ConvertFile(powertapFn, powerplanFn)==true){
	 powertapCtrl->SetValue("");
	 powerplanCtrl->SetValue("");
  }
}


bool PtConvert::PTBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Powertap file",
						m_currentProfile->ptDefDir,		  // default dir
						defname, // default file
						"Powertap files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  powertapCtrl->SetValue(fd.GetPath());
  return(true);
}

void PtConvert::OnDownload(wxCommandEvent& WXUNUSED(event))
{
  wxString commandline="c:\\ptapdl.exe \""+powertapCtrl->GetValue()+"\"";
  if(wxShell(commandline)==false){
	 wxMessageBox("Could not download", "Error", wxOK);
  }
}

void PtConvert::OnTempButton(wxCommandEvent& WXUNUSED(event))
{
  if(GetTemperature()==false){
    wxMessageBox("Could not retrieve temperature", "Error", wxOK);
  }
}

bool PtConvert::PPBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Powerplan file",
						m_currentProfile->ppDefDir,		  // default dir
						defname, // default file
						"Powerplan files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  powerplanCtrl->SetValue(fd.GetPath());
  return(true);
}

void PtConvert::OnDate(wxCalendarEvent& WXUNUSED(event))
{
  wxString fn;
  wxString ptFn;

  wxDateTime theDate;
  int      year=0;

  theDate=dateCtrl->GetDate();

  // format powerplan name
  fn=m_currentProfile->ppDefDir+m_currentProfile->ppPrefix+dateCtrl->GetDate().FormatISODate();
  fn.Replace("-", "");

  // format pt filename
  year=theDate.GetYear();
  year-=1900;
  if(year>100) year-=100;
  ptFn=m_currentProfile->ptDefDir+wxString::Format("r_abbott_pt%02d%02d%02d",
																	year, theDate.GetMonth()+1,
																	theDate.GetDay());
  if(m_sessionNumber!=-1 && m_lastDateClicked==theDate){
	 fn+=wxString::Format("_%d", ++m_sessionNumber);
	 ptFn+=wxString::Format("_%d", m_sessionNumber);
  }else{
	 m_lastDateClicked=theDate;
	 m_sessionNumber=1;
  }
  powerplanCtrl->SetValue(fn+".csv");
  powertapCtrl->SetValue(ptFn+".csv");
}

bool PtConvert::GetTemperature()
{
  TemperatureClass tmp;
  bool retval=false;
  
  SetCursor(wxCURSOR_WAIT);
  if(tmp.LookupTemperature()==true){
    tempCtrl->SetValue(wxString::Format("%3.2lf", tmp.GetFahrenheit()));
    retval=true;
  }
  SetCursor(wxNullCursor);
  return(retval);
}
