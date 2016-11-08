#ifndef __COMBINEFILESDIALOG__
#define __COMBINEFILESDIALOG__

#include "userProfile.h"

class CombineFilesDialog : public wxDialog
{
  UserProfile *m_userProfile;

  wxListBox *m_filesListbox;
  wxStaticText *m_statusLine;
  long m_fileCount;

public:
  CombineFilesDialog(wxWindow *parent, UserProfile *up);

  // Handlers
  void OnButton(wxCommandEvent &event);

  // Functions
  void AddFile();
  void CombineFiles();

  DECLARE_EVENT_TABLE()
};
#endif
