#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"tracePropertiesDialog.h"
enum{ID_OK=1, ID_CANCEL};

BEGIN_EVENT_TABLE(TracePropertiesDialog, wxDialog)
  EVT_BUTTON(ID_OK,          TracePropertiesDialog::OnOK)
  EVT_BUTTON(ID_CANCEL,      TracePropertiesDialog::OnCancel)

  END_EVENT_TABLE()

  ;

TracePropertiesDialog::TracePropertiesDialog(wxWindow *parent, TraceClass *tp)
  :wxDialog(parent, -1, "Trace Properties",
				wxDefaultPosition, wxDefaultSize, wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
{
  // -- gui --
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  // scale
  wxBoxSizer *row1Sizer=new wxBoxSizer(wxHORIZONTAL);
  row1Sizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  gainTextCtrl=new wxTextCtrl(this, -1, wxString::Format("%lf", tp->gain).GetData());
  row1Sizer->Add(gainTextCtrl, 1, wxGROW, 1);

  row1Sizer->Add(new wxStaticText(this, -1, "Offset"), 0, wxGROW, 1);
  offsetTextCtrl=new wxTextCtrl(this, -1, wxString::Format("%lf", tp->offset).GetData());
  row1Sizer->Add(offsetTextCtrl, 1, wxGROW, 1);
  mainSizer->Add(row1Sizer, 0, wxGROW, 1);

  // shift & average sizer
  wxBoxSizer *shiftSizer=new wxBoxSizer(wxHORIZONTAL);
  shiftSizer->Add(new wxStaticText(this, -1, "Shift"), 0, wxGROW, 1);
  shiftTextCtrl=new wxTextCtrl(this, -1, wxString::Format("%lf", tp->shift).GetData());
  shiftSizer->Add(shiftTextCtrl, 1, wxGROW, 1);

  shiftSizer->Add(new wxStaticText(this, -1, "Avg"), 0, wxGROW, 1);
  averageTextCtrl=new wxTextCtrl(this, -1, wxString::Format("%lf", tp->average).GetData());
  shiftSizer->Add(averageTextCtrl, 1, wxGROW, 1);

  mainSizer->Add(shiftSizer, 0, wxGROW, 1);

  // mean/stdv sizer
  wxBoxSizer *meanSizer=new wxBoxSizer(wxHORIZONTAL);
  maxCheckbox= new wxCheckBox(this, -1, "Show Max");
  meanCheckbox=new wxCheckBox(this, -1, "Show Mean");
  minCheckbox =new wxCheckBox(this, -1, "Show Min");
  stdvCheckbox=new wxCheckBox(this,-1, "Show Stdev");
  if(tp->showMax ==true) maxCheckbox->SetValue(true);
  if(tp->showMean==true) meanCheckbox->SetValue(true);
  if(tp->showMin ==true) minCheckbox->SetValue(true);
  if(tp->showStdv==true) stdvCheckbox->SetValue(true);

  meanSizer->Add(maxCheckbox,  0, wxGROW, 3);
  meanSizer->Add(meanCheckbox, 0, wxGROW, 3);
  meanSizer->Add(minCheckbox,  0, wxGROW, 3);
  meanSizer->Add(stdvCheckbox, 0, wxGROW, 3);
  mainSizer->Add(meanSizer, 0, wxGROW, 3);

  // buttons
  wxBoxSizer *buttonSizer=  new wxBoxSizer(wxHORIZONTAL);
  wxButton *cancelbutton=new wxButton(this, ID_CANCEL, "&Cancel");
  wxButton *okbutton=    new wxButton(this, ID_OK, "&Ok");
  //  okbutton->SetDefault();
  buttonSizer->Add(cancelbutton, 1, wxGROW, 1);
  buttonSizer->Add(okbutton,     1, wxGROW, 1);
  mainSizer->Add(buttonSizer, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->Fit(this);
  Centre(wxBOTH);
}

void TracePropertiesDialog::OnOK(wxCommandEvent& event)
{
  double dtmp;
  if(gainTextCtrl->GetValue().ToDouble(&dtmp)==false || // check if values are OK
	  offsetTextCtrl->GetValue().ToDouble(&dtmp)==false ||
	  shiftTextCtrl->GetValue().ToDouble(&dtmp)==false ||
	  averageTextCtrl->GetValue().ToDouble(&dtmp)==false){
	 wxMessageBox("Illegal value entered.", "Error", wxOK);
  }else{
	 EndModal(wxID_OK);
  }
}

void TracePropertiesDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}

bool TracePropertiesDialog::GetData(TraceClass *tp)
{
  long tLong;

  gainTextCtrl->GetValue().ToDouble(&tp->gain);
  offsetTextCtrl->GetValue().ToDouble(&tp->offset);
  shiftTextCtrl->GetValue().ToDouble(&tp->shift);
  averageTextCtrl->GetValue().ToDouble(&tp->average);
  tp->showMax= maxCheckbox->GetValue();
  tp->showMean=meanCheckbox->GetValue();
  tp->showMin= minCheckbox->GetValue();
  tp->showStdv=stdvCheckbox->GetValue();
  return(true);
}
