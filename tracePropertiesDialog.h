#ifndef __TRACEPROPERTIESDIALOG__
#define __TRACEPROPERTIESDIALOG__


#include"traceClass.h"

class TracePropertiesDialog:public wxDialog
{
private:
  wxTextCtrl *offsetTextCtrl, *gainTextCtrl;
  wxTextCtrl *shiftTextCtrl, *averageTextCtrl;
  wxCheckBox *maxCheckbox, *meanCheckbox, *minCheckbox, *stdvCheckbox;

  TraceClass traceData;			  // won't actually bring in the trace itself

public:
  TracePropertiesDialog(wxWindow *parent, TraceClass *tc); // constructor
  void OnOK(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
  bool GetData(TraceClass *tp);

  DECLARE_EVENT_TABLE()
};

#endif
