#ifndef __WORKOUTENTRY__
#define __WORKOUTENTRY__

class WorkoutEntry
{
public:
  wxString m_title;
  wxString m_abbreviation;
  wxString m_description;
  wxString m_phases;

  WorkoutEntry();
  WorkoutEntry(wxString title, wxString abbrev, wxString description);
  WorkoutEntry(wxString title, wxString abbrev, wxString description, wxString phases);
};

class WorkoutPhase
{
public:
  wxString m_phase;				  // for now, use a class for future expansion
  wxColour m_phaseColour;

  WorkoutPhase(wxString istr)
  {
	 m_phase=istr;
	 m_phaseColour=wxColour(0,0,0);
  }
};

WX_DECLARE_LIST(WorkoutEntry, WorkoutList); // list of the points
WX_DECLARE_LIST(WorkoutPhase, PhaseList);

class WorkoutMenu
{
  wxString m_filename;

public:
  WorkoutList *workoutList;		  // the main list of entries
  PhaseList   *phaseList;

  WorkoutMenu();
  bool LoadWorkouts(wxString fn);
  bool SaveWorkouts();			  // saves to the same file it was loaded from
};
#endif
