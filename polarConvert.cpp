#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "powerfile.h"
#include "polarConvert.h"
#include "polarConvert.h"

enum
  {
	 ID_DATECTRL=1,
	 ID_STARTCTRL,
	 ID_TEMPCTRL,
	 ID_POLAR,
	 ID_POWERPLAN,
	 ID_POLARBROWSEBUTTON,
	 ID_PPBROWSEBUTTON,
	 ID_CONVERT
  }
;

BEGIN_EVENT_TABLE(PolarConvert, wxDialog)
  EVT_CALENDAR_SEL_CHANGED(ID_DATECTRL, PolarConvert::OnDate)
  EVT_BUTTON(ID_POLARBROWSEBUTTON, PolarConvert::OnPolarBrowse)
  EVT_BUTTON(ID_PPBROWSEBUTTON, PolarConvert::OnPPBrowse)
  EVT_BUTTON(ID_CONVERT,        PolarConvert::OnConvert)
END_EVENT_TABLE();



PolarConvert::PolarConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxDialog(parent, id, "Polar Conversion", pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{

  // -- Variable Initialization --
  m_currentProfile=up;
  m_converted=0;

  // -- Widgets --
  wxStaticText *dateCtrlLabel=new wxStaticText(this, -1, "Date");
  dateCtrl=new wxCalendarCtrl(this, ID_DATECTRL, wxDateTime::Now(),
														  wxDefaultPosition, wxDefaultSize,
														  wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS|wxCAL_SHOW_SURROUNDING_WEEKS);
  wxStaticText *startTimeLabel=new wxStaticText(this, -1, "Start time");
  timeCtrl=new wxTextCtrl(this, ID_STARTCTRL, "0");

  wxStaticText *temperatureLabel=new wxStaticText(this, -1, "Temperature (F)");
  tempCtrl=new wxTextCtrl(this, ID_TEMPCTRL, "");

  wxStaticText *ptLabel=new wxStaticText(this, -1, "Polar Filename");
  polarCtrl=new wxTextCtrl(this, ID_POLAR, "");
  wxButton *polarBrowse=new wxButton(this, ID_POLARBROWSEBUTTON, "Browse");

  wxStaticText *ppLabel=new wxStaticText(this, -1, "PowerPlan Filename");
  powerplanCtrl=new wxTextCtrl(this, ID_POWERPLAN, "");
  wxButton *powerplanBrowse=new wxButton(this, ID_PPBROWSEBUTTON, "Browse");

  dropZ0Ctrl=new wxCheckBox(this, -1, "Drop Z0");

  wxButton *convertButton=new wxButton(this, ID_CONVERT, "&Convert");
  convertButton->SetDefault();


  wxBoxSizer *dateSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *timeSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *tempSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *polarSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *powerplanSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  dateSizer->Add(dateCtrlLabel, 1, wxGROW, 1);
  dateSizer->Add(dateCtrl, 0, wxGROW|wxALIGN_LEFT, 1);

  timeSizer->Add(startTimeLabel, 0, wxGROW, 1);
  timeSizer->Add(timeCtrl, 1, wxGROW, 1);

  tempSizer->Add(temperatureLabel, 0, wxGROW, 1);
  tempSizer->Add(tempCtrl, 1, wxGROW, 1);

  polarSizer->Add(ptLabel, 0, wxGROW, 1);
  polarSizer->Add(polarCtrl, 1, wxGROW, 1);
  polarSizer->Add(polarBrowse, 0, wxGROW, 1);

  powerplanSizer->Add(ppLabel, 0, wxGROW, 1);
  powerplanSizer->Add(powerplanCtrl, 1, wxGROW, 1);
  powerplanSizer->Add(powerplanBrowse, 0, wxGROW, 1);

  mainSizer->Add(dateSizer,     0, wxGROW, 1);
  mainSizer->Add(timeSizer,     0, wxGROW, 1);
  mainSizer->Add(tempSizer,     0, wxGROW, 1);
  mainSizer->Add(polarSizer, 0, wxGROW, 1);
  mainSizer->Add(powerplanSizer,0, wxGROW, 1);
  mainSizer->Add(dropZ0Ctrl, 0, wxGROW, 1);
  mainSizer->Add(convertButton, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);
}

bool PolarConvert::ConvertFile(wxString ptFilename, wxString powerFilename)
{
  PolarFile ptFile;
  PolarLineList::Node *oneLine;
  PolarLine *dat;
  int temperature;
  Powerfile    powerFile;
  int interval;
  bool intervalInProgress;

  long dropZ0point=-1;

  if(dropZ0Ctrl->GetValue()) dropZ0point=m_currentProfile->zones[1];

  if(ptFile.LoadFile(ptFilename, dropZ0point)==false){
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
	 powerFile.SetWatts(dat->Watts());
	 if(dat->hr<m_currentProfile->hrlimit){
		powerFile.SetHR(dat->hr);
	 }else{
		powerFile.SetHR(0);
	 }
	 powerFile.SetCadence(dat->cadence);
	 powerFile.SetKPH(dat->Kph());
	 powerFile.SetDistanceMeters(dat->Meters());
	 powerFile.SetAltitudeMeters(0);
	 powerFile.SetTempC(temperature);
	 powerFile.SetTorque(dat->Torque());
	 /*
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
	 */

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

void PolarConvert::OnPolarBrowse(wxCommandEvent& WXUNUSED(event))
{
  PolarBrowse(polarCtrl->GetValue());
}

void PolarConvert::OnPPBrowse(wxCommandEvent& WXUNUSED(event))
{
  PPBrowse(powerplanCtrl->GetValue());
}

void PolarConvert::OnConvert(wxCommandEvent& WXUNUSED(event))
{
  wxString polarFn;
  wxString powerplanFn;

  polarFn=polarCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();
  
  if(polarFn.IsEmpty()==true){
	 if(PolarBrowse("")==false) return;
  }
  if(powerplanFn.IsEmpty()==true){
	 if(PPBrowse("")==false) return;
  }

  polarFn=polarCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();

  if(ConvertFile(polarFn, powerplanFn)==true){
	 polarCtrl->SetValue("");
	 powerplanCtrl->SetValue("");
  }
}


bool PolarConvert::PolarBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Polar file",
						m_currentProfile->ptDefDir,		  // default dir
						defname, // default file
						"Polar files (*.hrm)|*.hrm|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  polarCtrl->SetValue(fd.GetPath());
  return(true);
}

bool PolarConvert::PPBrowse(wxString defname)
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

void PolarConvert::OnDate(wxCalendarEvent& WXUNUSED(event))
{
  wxString fn;
  wxString dateString;

  // -- fill polar def filename
  dateString=dateCtrl->GetDate().FormatISODate();
  dateString.Replace("-", "");
  dateString=dateString.Mid(2);

  if(dateString==m_lastDateString){
	 m_converted++;
  }else{
	 m_converted=1;
  }

  m_lastDateString=dateString;

  fn=m_currentProfile->polarDefDir+dateString+
	 wxString::Format("%02d.hrm",m_converted);

  polarCtrl->SetValue(fn);

  // -- fill powerplan def filename
  fn=m_currentProfile->ppDefDir+m_currentProfile->ppPrefix+dateCtrl->GetDate().FormatISODate();
  if(m_converted>1){
	 fn+=wxString::Format("_%d.csv", m_converted);
  }else{
	 fn+=".csv";
  }

  fn.Replace("-", "");
  powerplanCtrl->SetValue(fn);
}
