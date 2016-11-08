#ifndef __POWERTAP__
#define __POWERTAP__

#include "interval.h"

// class for holding powertap data
class PowerTapLine{

public:
  double minutes, torque, kph, km;
  int watts, cadence, hr;
  int id;							  // interval id

  PowerTapLine();
  PowerTapLine(double min, double torq, double kp,
					int watt, double k, 
					int cad, int h, int i);

  double Mph();
  double Miles();
  double Seconds();
};
WX_DECLARE_LIST(PowerTapLine, PowerTapLineList); // list of powertap points

class PowerTapFile{				  // holds a powertap file

  wxString filename;

public:
  double m_tss, m_if, m_np;
  double totalKm;
  double m_totalJoules;
  long m_totalHeartBeats;

  PowerTapLineList lineList;
  PowerTapLineList filteredList;
  PowerTapFile();					  // constructor
  bool LoadFile(wxString fn);
  bool RepairHr();
  bool WriteFile(wxString fn);
  double ZoneTime(int lower, int higher);
  void Filter(double average);
  double TSS(double pLT);
  double TotalMiles();
  double TotalKm();
  Interval Sections(int ii);
  IntervalList intervalList;
};

#endif
