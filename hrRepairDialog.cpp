#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "timePeriod.h"
#include "powerfile.h"
#include "hrRepairDialog.h"

enum
  {
	 ID_LEFT_PREVIOUSBUTTON=10000,
	 ID_LEFT_LEFTBUTTON,
	 ID_LEFT_RIGHTBUTTON,
	 ID_LEFT_NEXTBUTTON,

	 ID_REPAIRBUTTON,

	 ID_RIGHT_PREVIOUSBUTTON,
	 ID_RIGHT_LEFTBUTTON,
	 ID_RIGHT_RIGHTBUTTON,
	 ID_RIGHT_NEXTBUTTON,

	 ID_REVERTBUTTON,
	 ID_OKBUTTON=wxID_OK,
	 ID_CANCELBUTTON=wxID_CANCEL
  }
;

BEGIN_EVENT_TABLE(HrRepairDialog, wxDialog)
  EVT_BUTTON(ID_LEFT_LEFTBUTTON,        HrRepairDialog::OnLeftLeftButton)
  EVT_BUTTON(ID_LEFT_RIGHTBUTTON,       HrRepairDialog::OnLeftRightButton)
  EVT_BUTTON(ID_RIGHT_LEFTBUTTON,       HrRepairDialog::OnRightLeftButton)
  EVT_BUTTON(ID_RIGHT_RIGHTBUTTON,      HrRepairDialog::OnRightRightButton)

  EVT_BUTTON(ID_LEFT_NEXTBUTTON,        HrRepairDialog::OnLeftNextButton)
  EVT_BUTTON(ID_RIGHT_NEXTBUTTON,       HrRepairDialog::OnRightNextButton)
  EVT_BUTTON(ID_LEFT_PREVIOUSBUTTON,    HrRepairDialog::OnLeftPreviousButton)
  EVT_BUTTON(ID_RIGHT_PREVIOUSBUTTON, HrRepairDialog::OnRightPreviousButton)

  EVT_BUTTON(ID_REPAIRBUTTON,           HrRepairDialog::OnRepair)
  EVT_BUTTON(ID_OKBUTTON,               HrRepairDialog::OnOk)
END_EVENT_TABLE();



HrRepairDialog::HrRepairDialog(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size)
  : wxDialog(parent, id, "Computrainer Conversion", pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{

  // -- Variable Initialization --


  // -- Widgets --
  m_graphPanel=new GraphPanel(this, -1, wxDefaultPosition, wxDefaultSize);
  m_graphPanel->SetSizeHints(1200,800);

  m_leftPositionCtrl       = new wxStaticText(this, -1, "00:00:00 (000.00 Minutes) (0000.00 Seconds)");
  m_leftHR             = new wxStaticText(this, -1, "HR: 000");

  m_rightPositionCtrl      = new wxStaticText(this, -1, "00:00:00 (000.00 Minutes) (0000.00 Seconds)");
  m_rightHR            = new wxStaticText(this, -1, "HR: 000");

  m_left_previousButton= new wxButton(this, ID_LEFT_PREVIOUSBUTTON, "<<");
  m_left_leftButton    = new wxButton(this, ID_LEFT_LEFTBUTTON, "<");
  m_left_rightButton   = new wxButton(this, ID_LEFT_RIGHTBUTTON, ">");
  m_left_nextButton  =   new wxButton(this, ID_LEFT_NEXTBUTTON, ">>");
  
  m_repairButton=        new wxButton(this, ID_REPAIRBUTTON, "&Repair");

  m_right_previousButton=new wxButton(this, ID_RIGHT_PREVIOUSBUTTON, "<<");
  m_right_leftButton=    new wxButton(this, ID_RIGHT_LEFTBUTTON, "<");
  m_right_rightButton=   new wxButton(this, ID_RIGHT_RIGHTBUTTON, ">");
  m_right_nextButton =   new wxButton(this, ID_RIGHT_NEXTBUTTON, ">>");

  m_cancelButton     =   new wxButton(this, wxID_CANCEL, "&Cancel");
  m_revertButton     =   new wxButton(this, ID_REVERTBUTTON, "&Revert");
  m_okButton         =   new wxButton(this, wxID_OK, "&OK");

  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *controlSizer=new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *indicatorSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *dialogSizer=new wxBoxSizer(wxHORIZONTAL);

  // -- indicator sizer --
  indicatorSizer->Add(m_leftPositionCtrl, 0, wxGROW, 1);
  indicatorSizer->Add(m_leftHR, 0, wxGROW, 1);
  indicatorSizer->AddStretchSpacer(1);
  indicatorSizer->Add(m_rightPositionCtrl,0, wxGROW, 1);
  indicatorSizer->Add(m_rightHR,0, wxGROW, 1);

  // -- button sizer --
  buttonSizer->Add(m_left_previousButton, 0, wxGROW, 1);
  buttonSizer->Add(m_left_leftButton, 0, wxGROW, 1);
  buttonSizer->Add(m_left_rightButton,0, wxGROW, 1);
  buttonSizer->Add(m_left_nextButton, 0, wxGROW, 1);

  buttonSizer->Add(m_repairButton, 1, wxGROW, 1);

  buttonSizer->Add(m_right_previousButton, 0, wxGROW, 1);
  buttonSizer->Add(m_right_leftButton, 0, wxGROW, 1);
  buttonSizer->Add(m_right_rightButton, 0, wxGROW, 1);
  buttonSizer->Add(m_right_nextButton, 0, wxGROW, 1);

  // -- dialogSizer --
  dialogSizer->Add(m_cancelButton, 0, wxGROW, 1);
  dialogSizer->Add(m_revertButton, 0, wxGROW, 1);
  dialogSizer->Add(m_okButton,     1, wxGROW, 1);

  // -- controlSizer --
  controlSizer->Add(indicatorSizer, 0, wxGROW, 1);
  controlSizer->Add(buttonSizer, 0, wxGROW, 1);

  // -- mainSizer --
  mainSizer->Add(m_graphPanel, 1, wxGROW, 1);
  mainSizer->Add(controlSizer, 0, wxGROW, 1);
  mainSizer->Add(dialogSizer,  0, wxGROW, 1);

  m_graphPanel->AddLine(0,GP_VERTICAL);
  m_graphPanel->AddLine(0,GP_VERTICAL);

  m_graphPanel->SetLineColour(0,wxColour(255,0,0));
  m_graphPanel->SetLineColour(1,wxColour(255,0,0));

  SetSizerAndFit(mainSizer);
}

void HrRepairDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
  m_powerfile.Save(m_filename.GetFullPath());
  EndModal(wxOK);
}

void HrRepairDialog::OnLeftLeftButton(wxCommandEvent& WXUNUSED(event))
{
  if(--m_leftPosition<0) m_leftPosition=0;
  SetPointers();
}

void HrRepairDialog::OnLeftRightButton(wxCommandEvent& WXUNUSED(event))
{
  if(++m_leftPosition>m_graphPanel->GetTraceLength(0)) m_leftPosition=m_graphPanel->GetTraceLength(0);
  SetPointers();
}

void HrRepairDialog::OnRightLeftButton(wxCommandEvent& WXUNUSED(event))
{
  if(--m_rightPosition<0) m_rightPosition=0;
  SetPointers();
}

void HrRepairDialog::OnRightRightButton(wxCommandEvent& WXUNUSED(event))
{
  if(++m_rightPosition>m_graphPanel->GetTraceLength(0)) m_rightPosition=m_graphPanel->GetTraceLength(0);
  SetPointers();
}

// ----------------------------------------------------------------------
// OnLeftNextButton
/// Find the next HR Jump
void HrRepairDialog::OnLeftNextButton(wxCommandEvent& WXUNUSED(event))
{
  double lastHr;

  long newPosition=m_leftPosition;

  wxRealPoint point;

  point=m_graphPanel->GetPoint(0, newPosition);
  
  do{
	 lastHr=point.y;
	 newPosition++;
	 point=m_graphPanel->GetPoint(0, newPosition);
  }while((point.x!=-1) && (fabs(point.y-lastHr)<10));
  if(point.x!=-1) m_leftPosition=newPosition;
  SetPointers();
}

// ----------------------------------------------------------------------
// OnLeftPreviousButton
/// Find the next HR Jump
void HrRepairDialog::OnLeftPreviousButton(wxCommandEvent& WXUNUSED(event))
{
  double lastHr;

  long newPosition=m_leftPosition;

  wxRealPoint point;

  point=m_graphPanel->GetPoint(0, newPosition);
  
  do{
	 lastHr=point.y;
	 newPosition--;
	 point=m_graphPanel->GetPoint(0, newPosition);
  }while((point.x>0) && (fabs(point.y-lastHr)<10));
  if(point.x>0) m_leftPosition=newPosition;
  SetPointers();
}

// ----------------------------------------------------------------------
// OnRightPreviousButton
/// Find the next HR Jump
void HrRepairDialog::OnRightPreviousButton(wxCommandEvent& WXUNUSED(event))
{
  double lastHr;

  long newPosition=m_rightPosition;

  wxRealPoint point;

  point=m_graphPanel->GetPoint(0, newPosition);
  
  do{
	 lastHr=point.y;
	 newPosition--;
	 point=m_graphPanel->GetPoint(0, newPosition);
  }while((point.x>0) && (fabs(point.y-lastHr)<10));
  if(point.x>0) m_rightPosition=newPosition;
  SetPointers();
}


// ----------------------------------------------------------------------
// OnRightNextButton
/// Find the next HR Jump
void HrRepairDialog::OnRightNextButton(wxCommandEvent& WXUNUSED(event))
{
  double lastHr;

  long newPosition=m_rightPosition;

  wxRealPoint point;

  point=m_graphPanel->GetPoint(0, newPosition);
  
  do{
	 lastHr=point.y;
	 newPosition++;
	 point=m_graphPanel->GetPoint(0, newPosition);
  }while((point.x!=-1) && (fabs(point.y-lastHr)<10));
  if(point.x!=-1) m_rightPosition=newPosition;
  SetPointers();
}

void HrRepairDialog::OnRepair(wxCommandEvent& WXUNUSED(event))
{
  Extrapolate();
}

// ----------------------------------------------------------------------
// Extrapolate
void HrRepairDialog::Extrapolate()
{
  wxRealPoint pt;

  double dtmp;
  
  double startTime, endTime;
  double startHr, endHr;
  double theTime;

  pt=m_graphPanel->GetPoint(0, m_leftPosition);
  startTime=pt.x;
  startHr=pt.y;

  pt=m_graphPanel->GetPoint(0, m_rightPosition);
  endTime=pt.x;
  endHr=pt.y;

  if(endTime<startTime){				  // make startTime lower by swapping
	 dtmp=endTime;
	 endTime=startTime;
	 startTime=dtmp;

	 dtmp=endHr;
	 endHr=startHr;
	 startHr=dtmp;
  }
  m_powerfile.Firstline();
  while(m_powerfile.OnALine()==true && m_powerfile.Currentline().seconds<startTime) m_powerfile.Nextline();
  while(m_powerfile.OnALine() && m_powerfile.Currentline().seconds<endTime){
	 theTime=m_powerfile.Currentline().seconds;
	 m_powerfile.SetHR(startHr+(endHr-startHr)*(theTime-startTime)/(endTime-startTime));
	 m_powerfile.Nextline();
  }
  Plot();
}

void HrRepairDialog::OnPPBrowse(wxCommandEvent& WXUNUSED(event))
{
  PPBrowse(m_filename.GetFullPath());
}

bool HrRepairDialog::PPBrowse(wxString defname)
{
  wxFileDialog fd(this, "Choose a Powerplan file",
						m_filename.GetPath(), // default dir
						m_filename.GetFullName(), // default file
						"Powerplan files (*.csv)|*.csv|All files (*.*)|*.*",
						wxFD_OPEN);
  if(fd.ShowModal()==wxID_CANCEL) return(false);
  //  powerplanCtrl->SetValue(fd.GetPath());
  return(true);
}

bool HrRepairDialog::SetFilename(wxFileName fn)
{
  m_filename=fn;
  if(!m_powerfile.Load(m_filename.GetFullPath())){
	 wxMessageBox("Could not open file", "Error", wxOK);
	 return(false);
  }
  m_graphPanel->AddTrace();
  m_graphPanel->m_showLegend=false;
  
  Plot();
  
  m_leftPosition=0;
  m_rightPosition=0;

  m_graphPanel->ZoomAll();
  return(true);
}

void HrRepairDialog::Plot()
{
  m_graphPanel->ClearTrace(0);
  m_powerfile.Firstline();
  while(m_powerfile.OnALine()){
	 m_graphPanel->AddPointRealtime(0,
											  wxRealPoint(m_powerfile.Currentline().seconds,
															  m_powerfile.Currentline().hr_bpm));
	 m_powerfile.Nextline();
  }
  m_graphPanel->Refresh(true);
}

void HrRepairDialog::SetPointers()
{
  wxRealPoint point;
  TimePeriod tp;

  m_graphPanel->MoveLine(RIGHTPOINTER, m_rightPosition);

  //"00:00:00 (000.00 Minutes) (0000.00 Seconds)");
  point=m_graphPanel->GetPoint(0, m_leftPosition);
  m_graphPanel->MoveLine(LEFTPOINTER, point.x);
  tp.Set(point.x);

  m_leftPositionCtrl->SetLabel(tp.HHMMSSHH()
										 +wxString::Format(" (%3.2lf Minutes) ", point.x/60.0)
										 +wxString::Format(" (%3.2lf Seconds) ", point.x));
  m_leftHR->SetLabel(wxString::Format("HR: %d", (int)point.y));

  point=m_graphPanel->GetPoint(0, m_rightPosition);
  m_graphPanel->MoveLine(RIGHTPOINTER, point.x);
  tp.Set(point.x);
  m_rightPositionCtrl->SetLabel(tp.HHMMSSHH()
										 +wxString::Format(" (%3.2lf Minutes) ", point.x/60.0)
										 +wxString::Format(" (%3.2lf Seconds) ", point.x));
  m_rightHR->SetLabel(wxString::Format("HR: %d", (int)point.y));
  m_graphPanel->Refresh();
}

