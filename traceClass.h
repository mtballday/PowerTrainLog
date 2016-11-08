#ifndef __TRACECLASS__
#define __TRACECLASS__

#include<wx/gdicmn.h>			  // wxRealPoint
WX_DECLARE_LIST(wxRealPoint, PointList); // list of the points

class TraceClass
{
public:
  // vars
  double mean, min, max, stdev; // this mean, max, and min is of the DISPLAYED curve
  bool   showMean, showMin, showMax, showStdv;
  double maxDisplayed, minDisplayed;
  bool   show;
  double gain, offset, shift;
  double average;

  PointList pointList;
  wxColour traceColour;
  
  TraceClass();

  wxRect legendPick;
  wxString legend;
};

#endif
