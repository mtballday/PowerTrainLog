// This repeats some functionality of wxTimeSpan, but oh well.
#ifndef __TIMEPERIOD__
#define __TIMEPERIOD__

class TimePeriod{
private:
  double seconds;

public:
  TimePeriod();
  TimePeriod(double);
  TimePeriod(wxString);			  // decode the time

  void Set(double secs);
  wxString SetByHHMMSSHH(wxString timeStr);
  wxString SetByHHMM(wxString timeStr);
  wxString HHMMSSHH();
  wxString SSHH();
  wxString HHMM(double theSeconds);
  wxString HHMM();
  double Seconds();
  static wxString HHMMSSHH(double inSecs);
  bool operator>(TimePeriod tp);
  bool operator>=(TimePeriod tp);
  bool operator<(TimePeriod tp);
  bool operator<=(TimePeriod tp);
  TimePeriod operator+(TimePeriod tp);
};
#endif
