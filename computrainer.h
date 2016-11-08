#ifndef __COMPUTRAINER__
#define __COMPUTRAINER__

// class for holding computrainer data
class ComputrainerLine{

public:
  double ms, mph; int watts;
  int cad, hr, ss, lss, rss;
  int lpwr, rpwr;
  double miles, grade;
  int hrf,  rpmf,  wind,  load;

  ComputrainerLine();
  ComputrainerLine(double MS, double MPH, int WATTS,
						 int CAD, int HR, int SS, int LSS, int RSS,
						 int LPWR, int RPWR, double MILES,
						 int HRF, int RPMF, int WIND, double GRADE, int LOAD);

  double Mph();
  double Kph();
  double Miles();
  double Km();
  double Seconds();
};
WX_DECLARE_LIST(ComputrainerLine, ComputrainerLineList); // list of computrainer points

class ComputrainerFile{				  // holds a computrainer file

  wxString filename;
  double totalMiles;

public:
  bool readMs, readMph, readWatts, readCad, readHr, readSs, readLss, readRss;
  bool readLpwr, readRpwr, readMiles, readHrf, readRpmf, readWind, readGrade, readLoad;

  wxString m_headerText;

  ComputrainerLineList lineList;
  ComputrainerLineList filteredList;

  ComputrainerFile();					  // constructor

  double TotalMiles();
  double TotalKm();

  bool LoadFile(wxString fn, double wattsCorrection);
  bool RepairHr();
  //  bool WriteFile(wxString fn);
  double ZoneTime(int lower, int higher);
  double TSS(double pLT);
  double m_if;
  void Filter(double average);
};

#endif
