#ifndef __GRAPHPANEL__
#define __GRAPHPANEL__

#include<wx/gdicmn.h>			  // wxRealPoint
#include<wx/datetime.h>

#include"traceClass.h"

WX_DECLARE_LIST(TraceClass, TraceList);	// list of the traces

#define GP_VERTICAL true
#define GP_HORIZONTAL false

#define XAXIS_NORMAL  0
#define XAXIS_MINUTES 1
#define XAXIS_DATES   2

class GetDoubleDialog:public wxDialog
{
  wxTextCtrl *userCtrl;
 public:
  GetDoubleDialog(wxWindow *parent, wxString prompt);
  void SetValue(double);
  double GetValue();
  void OnOK(wxCommandEvent&WXUNUSED(event));
private:
  DECLARE_EVENT_TABLE();
};

class LineClass
{
public:
  double pos;
  bool vertical;					  // true=vert line, false=horiz line
  wxColour lineColour;
  LineClass(){						  // constructor
	 lineColour=wxColour(150,150,150); // default to grey
	 pos=0;
	 vertical=false;				  // default horizontal
  }
  LineClass(bool vert){			  // construct with attitude definition
	 vertical=vert;
	 lineColour=wxColour(150,150,150); // default to grey
	 pos=0;
  }
};

WX_DECLARE_LIST(LineClass, LineList); // list of added lines

enum{									  // enum the grab points
  GP_MINX=0,
	 GP_MAXX,
	 GP_MINY,
	 GP_MAXY,
	 
	 GP_NUMPICKPOINTS
};

enum{
  RP_ZOOMX=0,
	 RP_ZOOMY,
	 RP_ZOOMBOTH,

	 RP_NUMPICKPOINTS
	 };

class ZoomInfo
{
public:
  double m_maxX, m_maxY, m_minX, m_minY;
};

class GraphPanel : public wxPanel
{
private:
  wxString m_xlabel;
  wxString m_ylabel;
  bool    m_showMean, m_showMax, m_showMin, m_showStdv;
  //  double   m_maxX, m_maxY, m_minX, m_minY;
  double   m_minXValue, m_minYValue, m_maxXValue, m_maxYValue;
  int      ticksX, ticksY;
  wxRect   pickPoints[30];
  wxRect   rPickPoints[30];

  wxPoint  zoomStart;			  // remember where mouse zoom started
  wxPoint  zoomTrackLast;
  wxRect   plottingRegion;		  // used to back out zoom coordinates
  bool      trackingZoomPosition; // remembers that we are zooming with mouse
  wxTextCtrl *inNumWindow;		  // window for entering numbers
  double   *inNumWindowValue;	  // pointer to the data that is to be set by inNumWindow
  double   m_average;
  ZoomInfo zoomLast, zoomCurrent;

  bool m_crossHairs;
  wxPoint m_crossHairsLoc;
  wxStaticText *crossHairText;

public:
  int      m_xAxisType;
  wxDateTime m_startDate;
  double   slope,offset;
  bool     m_showLegend;

  TraceList traceList;
  TraceList unfilteredList;
  LineList  lineList;			  // list of extra lines added in

  GraphPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
				 const wxSize size);
  //	 : wxPanel(parent, id, title, pos, size, style);
  void OnChar(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& WXUNUSED(event));

  size_t AddTrace();					  // returns index of the new trace
  void SetToDefault();
  bool AddPoint(size_t l, wxRealPoint xy);
  bool AddPointRealtime(size_t l, wxRealPoint xy);
  bool RemovePoint(int l, int i); // remove a point based on index
  bool RemovePoint(int l, double x); // remove a point based on x
  bool SortPoints(int l,  bool forward); // sort the points forward or backward
  bool InsertPoint(int l, double x, double y); // insert a point based on x
  bool InsertPoint(int l, int i, double y); // insert a point based on i
  int  GetTraceCount();			  // tells how many traces we have
  bool ClearTrace(int l);		  // clear points in a trace

  bool UpdateGraph();			  // update entire graph
  bool DrawGraph(wxDC *dc, bool repaint);	// actually draw the graph
  void SetXLabel(wxString xLabel);
  void SetYLabel(wxString yLabel);
  bool SetTicksY(int xt);
  bool SetTicksX(int xt);
  bool SetXAxisType(int type);

  bool CalculateExtents(int traceNum); /// use -1 for all

  void SetMinX(double mx);
  void SetMaxX(double mx);
  void SetMinY(double my);
  void SetMaxY(double my);
  void SetStartDate(wxDateTime start);
  bool ShowTrace(int trace, bool state);
  bool SetTraceColour(int trace, wxColour newColour);
  void OnLeftButtonUp(wxMouseEvent& event);
  void OnLeftButtonDown(wxMouseEvent& event);
  void OnRightButtonUp(wxMouseEvent& event);
  void OnEnterKey(wxCommandEvent &);
  void OnMouseMotion(wxMouseEvent& event);
  void OnDClick(wxMouseEvent& event);
  void ZoomAll();
  void ZoomPrevious();

  double Mean(unsigned int l);
  double Stdev(unsigned int l);
  bool SetLegend(unsigned int l, wxString newName);
  bool SaveAs(wxString);
  bool Load(wxString fn);

  void Filter(int filtTrace);

  bool AddLine(double pp, bool orientVert);
  bool AddLine(double pp, bool orientVert, wxColour lineColour);
  bool MoveLine(int line, double newVal);

  void SetLineColour(int index, wxColour theColour);
  bool DeleteLine(int line);

  wxString DoubleToTimeString(double ti);
  wxString HHMMSS(double ti);

  TraceClass* GetTrace(int i);
  long GetTraceLength(int traceNum);		  // return number of elements in trace
  wxRealPoint GetPoint(int traceNum, long point);

  ~GraphPanel();

private:
  DECLARE_EVENT_TABLE();
};

#endif
