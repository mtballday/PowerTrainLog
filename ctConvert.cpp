#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ctConvert.h"
#include "computrainer.h"
#include "powerfile.h"

enum
  {
	 ID_DATECTRL=1,
	 ID_STARTCTRL,
	 ID_TEMPCTRL,
	 ID_COMPUTRAINER,
	 ID_POWERPLAN,
	 ID_CTBROWSEBUTTON,
	 ID_PPBROWSEBUTTON,
	 ID_CONVERT
  }
;

BEGIN_EVENT_TABLE(CtConvert, wxDialog)
  EVT_CALENDAR_SEL_CHANGED(ID_DATECTRL, CtConvert::OnDate)
  EVT_BUTTON(ID_CTBROWSEBUTTON, CtConvert::OnCTBrowse)
  EVT_BUTTON(ID_PPBROWSEBUTTON, CtConvert::OnPPBrowse)
  EVT_BUTTON(ID_CONVERT,        CtConvert::OnConvert)
END_EVENT_TABLE();



CtConvert::CtConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxDialog(parent, id, "Computrainer Conversion", pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{

  // -- Variable Initialization --
  m_currentProfile=up;

  // -- Widgets --
  wxStaticText *dateCtrlLabel=new wxStaticText(this, -1, "Date");
  dateCtrl=new wxCalendarCtrl(this, ID_DATECTRL, wxDateTime::Now(),
														  wxDefaultPosition, wxDefaultSize,
														  wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS|wxCAL_SHOW_SURROUNDING_WEEKS);
  wxStaticText *startTimeLabel=new wxStaticText(this, -1, "Start time");
  timeCtrl=new wxTextCtrl(this, ID_STARTCTRL, "0");

  wxStaticText *temperatureLabel=new wxStaticText(this, -1, "Temperature (F)");
  tempCtrl=new wxTextCtrl(this, ID_TEMPCTRL, "68");

  wxStaticText *ctLabel=new wxStaticText(this, -1, "Computrainer Filename");
  computrainerCtrl=new wxTextCtrl(this, ID_COMPUTRAINER, "");
  wxButton *computrainerBrowse=new wxButton(this, ID_CTBROWSEBUTTON, "Browse");

  wxStaticText *ppLabel=new wxStaticText(this, -1, "PowerPlan Filename");
  powerplanCtrl=new wxTextCtrl(this, ID_POWERPLAN, "");
  wxButton *powerplanBrowse=new wxButton(this, ID_PPBROWSEBUTTON, "Browse");

  wxButton *convertButton=new wxButton(this, ID_CONVERT, "&Convert");
  convertButton->SetDefault();


  wxBoxSizer *dateSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *timeSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *tempSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *computrainerSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *powerplanSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  dateSizer->Add(dateCtrlLabel, 1, wxGROW, 1);
  dateSizer->Add(dateCtrl, 0, wxGROW, 1);

  timeSizer->Add(startTimeLabel, 0, wxGROW, 1);
  timeSizer->Add(timeCtrl, 1, wxGROW, 1);

  tempSizer->Add(temperatureLabel, 0, wxGROW, 1);
  tempSizer->Add(tempCtrl, 1, wxGROW, 1);

  computrainerSizer->Add(ctLabel, 0, wxGROW, 1);
  computrainerSizer->Add(computrainerCtrl, 1, wxGROW, 1);
  computrainerSizer->Add(computrainerBrowse, 0, wxGROW, 1);

  powerplanSizer->Add(ppLabel, 0, wxGROW, 1);
  powerplanSizer->Add(powerplanCtrl, 1, wxGROW, 1);
  powerplanSizer->Add(powerplanBrowse, 0, wxGROW, 1);

  mainSizer->Add(dateSizer,     0, wxGROW, 1);
  mainSizer->Add(timeSizer,     0, wxGROW, 1);
  mainSizer->Add(tempSizer,     0, wxGROW, 1);
  mainSizer->Add(computrainerSizer, 0, wxGROW, 1);
  mainSizer->Add(powerplanSizer,0, wxGROW, 1);
  mainSizer->Add(convertButton, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);
}

bool CtConvert::ConvertFile(wxString ctFilename, wxString powerFilename)
{
  ComputrainerFile ctFile;
  ComputrainerLineList::Node *oneLine;
  ComputrainerLine *dat;
  int temperature;
  Powerfile    powerFile;
  int interval;
  bool intervalInProgress;

  if(ctFile.LoadFile(ctFilename, m_currentProfile->ctCorrection)==false){
	 return(false);
  }
  
  temperature=atoi(tempCtrl->GetValue().GetData());
  temperature=(int)(5.0*((double)temperature-32.0)/9.0);

  powerFile.startTime=timeCtrl->GetValue();

  oneLine=ctFile.lineList.GetFirst();
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
	 powerFile.SetCadence(dat->cad);
	 powerFile.SetKPH(dat->Kph());
	 powerFile.SetDistanceMeters(dat->Km()*1000.0);
	 powerFile.SetAltitudeMeters(0);
	 powerFile.SetTempC(temperature);
	 //	 powerFile.SetTorque(dat->torque); // computrainer doesn't have torque?
	 /*
	 if(dat->id!=interval){		  // no intervals on CT
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
  /*
  if(intervalInProgress==true){ // never ended last interval
	 powerFile.SetIntervalEnd(dat->Seconds());
	 intervalInProgress=false;
  }
  */
  if(powerFile.Save(powerFilename)==false){
	 return(false);
  }
  return(true);
}

void CtConvert::OnCTBrowse(wxCommandEvent& WXUNUSED(event))
{
  CTBrowse(computrainerCtrl->GetValue());
}

void CtConvert::OnPPBrowse(wxCommandEvent& WXUNUSED(event))
{
  PPBrowse(powerplanCtrl->GetValue());
}

void CtConvert::OnConvert(wxCommandEvent& WXUNUSED(event))
{
  wxString computrainerFn;
  wxString powerplanFn;

  computrainerFn=computrainerCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();
  
  if(computrainerFn.IsEmpty()==true){
	 if(CTBrowse("")==false) return;
  }
  if(powerplanFn.IsEmpty()==true){
	 if(PPBrowse("")==false) return;
  }

  computrainerFn=computrainerCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();

  if(ConvertFile(computrainerFn, powerplanFn)==true){
	 computrainerCtrl->SetValue("");
	 powerplanCtrl->SetValue("");
  }
}


bool CtConvert::CTBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Computrainer file",
						m_currentProfile->ctDefDir,		  // default dir
						defname, // default file
						"Computrainer files (*.txt)|*.txt|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  computrainerCtrl->SetValue(fd.GetPath());
  return(true);
}

bool CtConvert::PPBrowse(wxString defname)
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

void CtConvert::OnDate(wxCalendarEvent& WXUNUSED(event))
{
  wxString fn;

  fn=m_currentProfile->ppDefDir+m_currentProfile->ppPrefix+dateCtrl->GetDate().FormatISODate()+".csv";
  fn.Replace("-", "");

  powerplanCtrl->SetValue(fn);
}
