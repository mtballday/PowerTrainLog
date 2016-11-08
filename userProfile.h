#ifndef __USERPROFILE__
#define __USERPROFILE__

#include"bikeEntryData.h"
#include"workouts.h"

WX_DECLARE_LIST(wxString, BikeList); // list of bikes

class UserProfile
{
  wxString filename;
  wxString databaseFilename;
  wxString workoutFilename;

public:
  // vars
  EntryList *mainList;			  // the main list of entries
  WorkoutMenu workoutMenu;

  // funcs
  UserProfile();
  ~UserProfile();

  wxString name;
  int zones[NUMZONES+1];
  int restHr;
  int hrlimit;
  long powerZones[9];

  long thresholdPower;
  long mtbThresholdPower;
  long thresholdHR;
  long hrCutoff;
  long chronicTc;
  long acuteTc;

  wxString ctDefDir;
  wxString ptDefDir;
  wxString ptFormat;
  wxString ppDefDir;
  wxString ppPrefix;
  wxString polarDefDir;
  wxString garminDefFile;
  
  BikeList bikeList;

  TimePeriod shortRideTime;
  TimePeriod longRideTime;
  
  double ctCorrection, ptCorrection, polarCorrection;

  wxString Filename();
  wxString DatabaseFilename();
  bool SetDatabaseFilename(wxString fn);
  bool SetWorkoutsFilename(wxString fn);
  wxString WorkoutFilename();
  bool LoadProfile(wxString filename);
  bool SaveProfile(wxString filename);
  bool LoadData();
};

#endif
