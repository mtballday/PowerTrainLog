#ifndef __POLAR__
#define __POLAR__

#include "timePeriod.h"
#include "interval.h"

// class for holding powertap data
class PolarLine{

public:
  int cadence, hr;
  double speed;
  long seconds;

  PolarLine();
  PolarLine(long sec, int hh, double spd, int cad);

  double Kph();
  double Mph();
  double Seconds();
  double Watts();
  double Meters();
  double Torque();
};
WX_DECLARE_LIST(PolarLine, PolarLineList); // list of powertap points

class PolarFile{				  // holds a powertap file

  wxString filename;
  wxString headerInfo;
  
  double totalMiles;
  IntervalList intervalList;

public:
  PolarLineList lineList;
  PolarFile();					  // constructor
  Interval Sections(int ii);
  bool LoadFile(wxString fn, long dropZ0point=-1);
  bool RepairHr();
  bool WriteFile(wxString fn);
  double ZoneTime(int lower, int higher);
  double TotalMiles();
  double TotalKm();
  long m_totalBeats;
};

#endif
