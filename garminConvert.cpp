#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "powerfile.h"
#include "garminConvert.h"
#include "garminConvert.h"
#include "temperatureClass.h"

enum
  {
	 ID_DATECTRL=1,
	 ID_STARTCTRL,
	 ID_TEMPCTRL,
   ID_TEMPBUTTON,
   ID_SESSIONCTRL,
	 ID_GARMIN,
	 ID_POWERPLAN,
	 ID_GARMINBROWSEBUTTON,
	 ID_PPBROWSEBUTTON,
	 ID_CONVERT
  }
;

BEGIN_EVENT_TABLE(GarminConvert, wxDialog)
  EVT_CALENDAR_SEL_CHANGED(ID_DATECTRL, GarminConvert::OnDate)
  EVT_BUTTON(ID_GARMINBROWSEBUTTON, GarminConvert::OnGarminBrowse)
  EVT_BUTTON(ID_PPBROWSEBUTTON, GarminConvert::OnPPBrowse)
  EVT_BUTTON(ID_CONVERT,        GarminConvert::OnConvert)
  EVT_BUTTON(ID_TEMPBUTTON,     GarminConvert::OnTempButton)
END_EVENT_TABLE();


GarminConvert::GarminConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxDialog(parent, id, "Garmin Conversion", pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{

  // -- Variable Initialization --
  m_currentProfile=up;
  m_converted=0;

  // -- Widgets --
  wxStaticText *dateCtrlLabel=new wxStaticText(this, -1, "Date");
  dateCtrl=new wxCalendarCtrl(this, ID_DATECTRL, wxDateTime::Now(),
														  wxDefaultPosition, wxDefaultSize,
														  wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS|wxCAL_SHOW_SURROUNDING_WEEKS);

  wxStaticText *temperatureLabel=new wxStaticText(this, -1, "Temperature (F)");
  tempCtrl=new wxTextCtrl(this, ID_TEMPCTRL, "");
  temperatureButton=new wxButton(this, ID_TEMPBUTTON, "Get &Temperature");

  wxStaticText *sessionLabel=new wxStaticText(this, -1, "Session: ");
  sessionNumberCtrl=new wxStaticText(this, ID_SESSIONCTRL, "1");

  wxStaticText *ptLabel=new wxStaticText(this, -1, "Garmin Filename");
  garminCtrl=new wxTextCtrl(this, ID_GARMIN, "");
  wxButton *garminBrowse=new wxButton(this, ID_GARMINBROWSEBUTTON, "Browse");

  wxStaticText *ppLabel=new wxStaticText(this, -1, "PowerPlan Filename");
  powerplanCtrl=new wxTextCtrl(this, ID_POWERPLAN, "");
  wxButton *powerplanBrowse=new wxButton(this, ID_PPBROWSEBUTTON, "Browse");

  dropZ0Ctrl=new wxCheckBox(this, -1, "Drop Z0");

  wxButton *convertButton=new wxButton(this, ID_CONVERT, "&Convert");
  convertButton->SetDefault();


  wxBoxSizer *dateSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *timeSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *tempSizer    =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *sessionSizer =new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *garminSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *powerplanSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  dateSizer->Add(dateCtrlLabel, 1, wxGROW, 1);
  dateSizer->Add(dateCtrl, 0, wxGROW|wxALIGN_LEFT, 1);

  tempSizer->Add(temperatureLabel, 0, wxGROW, 1);
  tempSizer->Add(tempCtrl, 1, wxGROW, 1);
  tempSizer->Add(temperatureButton, 0, wxGROW, 1);

  sessionSizer->Add(sessionLabel, 0,wxGROW, 1);
  sessionSizer->Add(sessionNumberCtrl, 1, wxGROW, 1);

  garminSizer->Add(ptLabel, 0, wxGROW, 1);
  garminSizer->Add(garminCtrl, 1, wxGROW, 1);
  garminSizer->Add(garminBrowse, 0, wxGROW, 1);

  powerplanSizer->Add(ppLabel, 0, wxGROW, 1);
  powerplanSizer->Add(powerplanCtrl, 1, wxGROW, 1);
  powerplanSizer->Add(powerplanBrowse, 0, wxGROW, 1);

  mainSizer->Add(dateSizer,     0, wxGROW, 1);
  mainSizer->Add(timeSizer,     0, wxGROW, 1);
  mainSizer->Add(tempSizer,     0, wxGROW, 1);
  mainSizer->Add(sessionSizer,  0, wxGROW, 1);
  mainSizer->Add(garminSizer, 0, wxGROW, 1);
  mainSizer->Add(powerplanSizer,0, wxGROW, 1);
  mainSizer->Add(dropZ0Ctrl, 0, wxGROW, 1);
  mainSizer->Add(convertButton, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->SetSizeHints(this);
}

bool GarminConvert::ConvertFile(wxString grFilename, wxString powerFilename, int session)
{
  GarminFile                    grFile;
  GarminLineList::Node          *oneLine;
  GarminLine                    *dat;
  GarminLine                    *lastLine;
  int                           temperature;
  Powerfile                     powerFile;
  int                           interval;
  bool                          intervalInProgress;
  double                        droppedTime;


  long dropZ0point=-1;
  droppedTime=0.0;
  lastLine=NULL;

  if(dropZ0Ctrl->GetValue()) dropZ0point=m_currentProfile->zones[1];

  SetCursor(wxCURSOR_WAIT);
  if(grFile.LoadFile(grFilename, dateCtrl->GetDate(), dropZ0point, session)==false){
    SetCursor(wxNullCursor);
    wxMessageBox("Could not find Entry", "Error", wxOK);
    return(false);
  }
  
  temperature=atoi(tempCtrl->GetValue().GetData());
  temperature=(int)(5.0*((double)temperature-32.0)/9.0);

  powerFile.startTime=(grFile.m_starttime).FormatISOTime();

  oneLine=grFile.lineList.GetFirst();
  interval=0;
  intervalInProgress=false;

  while(oneLine!=NULL){
	 dat=oneLine->GetData();
   if(dropZ0point>0 && lastLine!=NULL && dat!=NULL &&
      dat->Seconds()-lastLine->Seconds()>300.0){ // delete any breaks longer than
                                               // 5 minutes
     droppedTime+=(dat->Seconds()-lastLine->Seconds()-1.0);
   }
	 powerFile.Appendline();
	 powerFile.SetSeconds(dat->Seconds()-droppedTime);
	 powerFile.SetWatts(dat->Watts());
	 if(dat->hr<m_currentProfile->hrlimit){
		powerFile.SetHR(dat->hr);
	 }else{
		powerFile.SetHR(0);
	 }
	 powerFile.SetCadence(dat->Rpm_Cadence());
	 powerFile.SetKPH(dat->Kph());
	 powerFile.SetDistanceMeters(dat->Meters());
	 powerFile.SetAltitudeMeters(dat->Altitude_Meters());
	 powerFile.SetTempC(temperature);
	 powerFile.SetTorque(dat->Torque());

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
   lastLine=dat;
	 oneLine=oneLine->GetNext();
  }
  if(intervalInProgress==true){ // never ended last interval
	 powerFile.SetIntervalEnd(dat->Seconds());
	 intervalInProgress=false;
  }
  powerFile.FixSpeedFromDistance();
  if(powerFile.Save(powerFilename)==false){
    SetCursor(wxNullCursor);
    wxMessageBox("Could not save file", "Error", wxOK);
    return(false);
  }
  SetCursor(wxNullCursor);
  return(true);
}



void GarminConvert::OnGarminBrowse(wxCommandEvent& WXUNUSED(event))
{
  GarminBrowse(garminCtrl->GetValue());
}

void GarminConvert::OnPPBrowse(wxCommandEvent& WXUNUSED(event))
{
  PPBrowse(powerplanCtrl->GetValue());
}

void GarminConvert::OnConvert(wxCommandEvent& WXUNUSED(event))
{
  wxString garminFn;
  wxString powerplanFn;

  garminFn=garminCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();
  
  if(garminFn.IsEmpty()==true){
	 if(GarminBrowse("")==false) return;
  }
  if(powerplanFn.IsEmpty()==true){
	 if(PPBrowse("")==false) return;
  }

  garminFn=garminCtrl->GetValue();
  powerplanFn=powerplanCtrl->GetValue();

  if(ConvertFile(garminFn, powerplanFn, m_converted)==true){
	 garminCtrl->SetValue("");
	 powerplanCtrl->SetValue("");
  }
}


void GarminConvert::OnTempButton(wxCommandEvent& WXUNUSED(event))
{
  if(GetTemperature()==false){
    wxMessageBox("Could not retrieve temperature", "Error", wxOK);
  }
}

bool GarminConvert::GarminBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Garmin file",
						m_currentProfile->garminDefFile,		  // default dir
						defname, // default file
						"Garmin files (*.tcx)|*.tcx|Garmin files (*.hst)|*.hst|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  garminCtrl->SetValue(fd.GetPath());
  return(true);
}

bool GarminConvert::PPBrowse(wxString defname)
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

void GarminConvert::OnDate(wxCalendarEvent& WXUNUSED(event))
{
  wxString fn;
  wxString dateString;

  // -- fill garmin def filename
  dateString=dateCtrl->GetDate().FormatISODate();
  dateString.Replace("-", "");
  dateString=dateString.Mid(2);

  if(dateString==m_lastDateString){
	 m_converted++;
  }else{
	 m_converted=1;
  }

  sessionNumberCtrl->SetLabel(wxString::Format("%d", m_converted));

  m_lastDateString=dateString;

  fn=m_currentProfile->garminDefFile;

  garminCtrl->SetValue(fn);

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

bool GarminConvert::GetTemperature()
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
