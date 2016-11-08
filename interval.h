#ifndef __INTERVAL__
#define __INTERVAL__

#include<wx/datetime.h>
#include"timePeriod.h"

// ----------------------------------------------------------------------
/// Class for holding the data of a given interval.

/*! This data is stored in the IntervalList.
 */
class Interval
{
public:
  Interval();
  Interval(double start_time, double end_time, wxString a_note);
  double startTime;
  double endTime;
  int averageHr;
  int maxHr;
  int minHr;

  double maxWatts;
  double minWatts;
  double avgWatts;

  wxString note;

  TimePeriod Duration();
  bool IsEmpty();
  Interval &operator=(Interval in);

  Interval *next;
};

class IntervalList
{
public:
  Interval *m_firstInterval;
  Interval *m_currentInterval;

  IntervalList();
  ~IntervalList();

  void First();
  void DeleteAll();
  void Append();
  void Append(Interval inv);
  Interval Index(long index);
};

#endif
