#ifndef __GARMIN__
#define __GARMIN__

#include "interval.h"

// class for holding Garmin data
class GarminLine{

public:
  double        minutes, torque, kph, km;
  double        altitude_meters, watts;
  double        rpm;
  int           hr;
  int           id;             // interval id


  GarminLine();
  GarminLine(double mins, long i/*id*/, long h /*heart rate*/,
             double altitudeMeters, double kph, double longitude, double latitude,
             double meters, double watts, double rpm_cadence);

  double Mph();
  double Miles();
  double Seconds();
  double Watts();
  double Kph();
  double Torque();
  double Meters();
  double Altitude_Meters();
  double Rpm_Cadence();
};

WX_DECLARE_LIST(GarminLine, GarminLineList); // list of powertap points

class GarminFile{				  // holds a powertap file

  wxString filename;

public:
  double m_tss, m_if, m_np;
  double totalKm;
  double m_totalJoules;
  long m_totalHeartBeats;

  GarminLineList lineList;
  GarminLineList filteredList;
  GarminFile();					  // constructor
  wxString ReadTag(wxString line, wxString tag);
  bool LoadFile(wxString fn, wxDateTime ridedate, long dropZ0point=-1, int session=1);
  bool RepairHr();
  bool WriteFile(wxString fn);
  double ZoneTime(int lower, int higher);
  //  void Filter(double average);
  //  double TSS(double pLT);
  double TotalMiles();
  double TotalKm();
  Interval Sections(int ii);
  IntervalList intervalList;
  wxDateTime   m_starttime;
};

#endif
