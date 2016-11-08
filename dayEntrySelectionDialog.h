#ifndef __DAYENTRYSELECTIONDIALOG__
#define __DAYENTRYSELECTIONDIALOG__
class DayEntrySelectionDialog : public wxDialog
{
  int m_sessions;
public:
  DayEntrySelectionDialog(wxWindow *parent, int numEntries);
  void OnButton(wxCommandEvent &event);
  int GetSessionSelected();


  DECLARE_EVENT_TABLE()
};
#endif
