#ifndef __POWERLINE__
#define __POWERLINE__

#define UNITS_METERS 2
#define UNITS_MILES 1
#define UNITS_KM    0

/*! \file Powerfile Class
This file holds the data from a power file.
File Attributes:
-Date/time stamp
-Threshold wattage

Line Attributes
-Seconds (seconds)
-Power (watts)
-HR (hr_bpm)
-Cadence (cadence_rpm)
-Speed (speed_mps)
-Distance (distance_meters)
-Altitude (alititude_meters)
-Temperature (degrees_c)
-Torque (torque_nm)

*/

#include "wxReadTextFile.h"
#include <wx/datetime.h>

// ----------------------------------------------------------------------
/// Holds the intervals
class PowerInterval
{
public:

  double starttime; // (secs)
  double endtime; // (secs)
  PowerInterval *next;
  PowerInterval(){
	 starttime=0;
	 endtime=0;
	 next=NULL;
  }
  PowerInterval& operator=(PowerInterval pl);
};


// ----------------------------------------------------------------------
/// Holds each individual line of a power file
class Powerline
{
public:

  // -- member vars --
  double seconds;
  double watts;
  unsigned char hr_bpm;
  unsigned char cadence_rpm;
  double speed_kph;
  double distance_meters;
  double altitude_meters;
  int degrees_c;
  double torque_nm;

  Powerline *next;
  Powerline *previous;

  // -- member functions --
  Powerline();
  Powerline(Powerline *prev);
  Powerline& operator=(Powerline pl);

};

// ----------------------------------------------------------------------
/// Holds entire ride
class Powerfile
{
  // -- Member Variables --
  wxString m_filename;
  double m_totalMeters;

  Powerline *m_firstPowerline;

  PowerInterval *m_firstInterval;
  PowerInterval *m_currentInterval;

  double m_thresholdWattage;

  void DeleteAll();

public:
  Powerline *m_currentLine;

  Powerline *m_firstFilteredline;
  Powerline *m_currentFilteredline;


  wxDateTime m_rideTimestamp;
  wxString startTime;

  double m_np, m_if, m_tss, m_totalJoules, m_totalHeartBeats, m_factoredHR;
  double m_trimps;

  // -- Member Functions --
  Powerfile();
  ~Powerfile();

  void FirstInterval();
  bool NextInterval();
  bool AppendInterval();
  double IntervalStartSeconds();
  double IntervalEndSeconds();
  bool SetIntervalStart(double seconds);
  bool SetIntervalEnd(double seconds);
  PowerInterval CurrentInterval();
  bool AppendInterval(PowerInterval interval);
  bool OnAInterval(){
	 if(m_currentInterval!=NULL) return(true);
	 return(false);
  }


  void Firstline();
  bool Nextline();
  Powerline Currentline();
  bool OnALine();
  bool Appendline();
  bool Appendline(Powerline pl);

  bool FixSpeedFromDistance();
  void SetSeconds(double secs);
  void SetWatts(double w);
  void SetHR(unsigned int hr);
  void SetCadence(unsigned char cad);
  void SetKPH(double kph);
  void SetDistanceMeters(double m);
  void SetAltitudeMeters(double m);
  void SetTempC(int deg);
  void SetTorque(double tq);
  
  bool Save(wxString fn);
  bool Load(wxString fn);

  void Filter(double average);
  double TSS(double pLT);
  double CurrentDistance(int units);
  double Distance(int units);
  double Seconds();

  double ZoneTime(int lower, int higher);
  double Trimps(int restHr, int maxHr);
  double PowerZoneTime(int lower, int higher);
};

#endif
