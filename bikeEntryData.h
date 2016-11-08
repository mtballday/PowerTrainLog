#ifndef __BIKEENTRYDATA__
#define __BIKEENTRYDATA__

#define NUMZONES 7

#include<wx/datetime.h>
#include"timePeriod.h"
#include "interval.h"

/// Class for holding data for each exercise entry
class BikeEntryData
{
public:
  BikeEntryData();
  wxDateTime theDate;
  wxDateTime arriseTime;
  TimePeriod sleepTime;
  TimePeriod fileZoneTimes[NUMZONES];
  TimePeriod manualZoneTimes[NUMZONES];
  double miles;
  wxString bicycle;
  double weight, fat;
  int    session;
  int    restHr;

  bool   keyEvent;
  bool   sick;
  bool   offRoad;
  int    ppTss, polarTss, ptTss, ctTss;
  double ppIf, polarIf, ptIf, ctIf;
  wxString ppFile, ctFile, ptFile, polarFile, notes;
  IntervalList intervalList;

  // plan variables
  wxString m_planPhase;
  int m_mesoCycleWeek;
  wxString m_planDescription;
  wxString m_planNotes;
  int m_priority;
  TimePeriod m_planTime;
  int m_planIntensity;
  //  BikeEntryData operator=(BikeEntryData);
};
WX_DECLARE_LIST(BikeEntryData, EntryList); // list of the points
#endif

