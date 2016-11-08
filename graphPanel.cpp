#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include"tracePropertiesDialog.h"
#include"graphPanel.h"
#include"wxReadTextFile.h"
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(PointList);
WX_DEFINE_LIST(TraceList);
WX_DEFINE_LIST(LineList);

enum
{
  ID_INNUMWINDOW=1,
  ID_INNUMCTRL,
  ID_OKBUTTON,
  ID_CANCELBUTTON
};

BEGIN_EVENT_TABLE(GraphPanel, wxPanel)
  EVT_CHAR(GraphPanel::OnChar)
  EVT_PAINT(GraphPanel::OnPaint)
  EVT_LEFT_DCLICK(GraphPanel::OnDClick)
  EVT_LEFT_UP(GraphPanel::OnLeftButtonUp)
  EVT_LEFT_DOWN(GraphPanel::OnLeftButtonDown)
  EVT_RIGHT_UP(GraphPanel::OnRightButtonUp)
  EVT_MOTION(GraphPanel::OnMouseMotion)
  EVT_TEXT_ENTER(ID_INNUMWINDOW, GraphPanel::OnEnterKey)
END_EVENT_TABLE()
;

BEGIN_EVENT_TABLE(GetDoubleDialog, wxDialog)
  EVT_BUTTON(ID_OKBUTTON, GetDoubleDialog::OnOK)
  END_EVENT_TABLE();

GetDoubleDialog::GetDoubleDialog(wxWindow *parent, wxString prompt)
  :wxDialog(parent, -1, "Message",
				wxDefaultPosition, wxDefaultSize,
				wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
{
  wxStaticText *promptText=new wxStaticText(this, -1, prompt);
  userCtrl=new wxTextCtrl(this, ID_INNUMCTRL, "");
  wxButton *cancelbutton=new wxButton(this, ID_CANCELBUTTON, "&Cancel");
  wxButton *okbutton=new wxButton(this, ID_OKBUTTON, "&Ok");
  okbutton->SetDefault();

  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);

  buttonSizer->Add(cancelbutton, 0, wxGROW);
  buttonSizer->Add(okbutton, 0, wxGROW);

  mainSizer->Add(promptText, 0, wxGROW, 1);
  mainSizer->Add(userCtrl, 0, wxGROW, 1);
  mainSizer->Add(buttonSizer, 0, wxGROW, 1);
  
  SetSizer(mainSizer);
  mainSizer->Fit(this);
}

double GetDoubleDialog::GetValue()
{
  double dTmp;
  userCtrl->GetValue().ToDouble(&dTmp);
  return(dTmp);
}

void GetDoubleDialog::SetValue(double dTmp)
{
  userCtrl->SetValue(wxString::Format("%lf", dTmp).GetData());
};

void GetDoubleDialog::OnOK(wxCommandEvent&WXUNUSED(event))
{
  EndModal(wxID_OK);
}


#include<wx/brush.h>
#include "math.h"
TraceClass::TraceClass()
{
  pointList.DeleteContents(true);
  offset=0.0; gain=1.0; shift=0.0;
  traceColour=wxColour(0,0,0); // default to black
  mean=0; stdev=0;
  average=1;
  show=true;
  showMean=true; showMin=true; showMax=true; showStdv=true;
}

GraphPanel::GraphPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
							  const wxSize size)
  : wxPanel(parent, id, pos, size)
{
  SetToDefault();
  unfilteredList.DeleteContents(true);
  traceList.DeleteContents(true);
  lineList.DeleteContents(true);
  trackingZoomPosition=false;
  inNumWindow=new wxTextCtrl(this, ID_INNUMWINDOW, "1000.00",
									  wxDefaultPosition,
									  wxDefaultSize, wxTE_PROCESS_ENTER);
  inNumWindow->Hide();
  m_xAxisType=XAXIS_NORMAL;
  m_startDate=wxDateTime::Now();
  m_showLegend=true;
  m_showMean=true;
  m_showMax=true;
  m_showMin=true;
  m_showStdv=true;
  m_crossHairs=false;
}

void GraphPanel::SetToDefault()
{
  m_xlabel=wxString::Format("X");
  m_ylabel=wxString::Format("Y");
  zoomCurrent.m_minX=0;
  zoomCurrent.m_minY=0;
  zoomCurrent.m_maxX=1;
  zoomCurrent.m_maxY=1;
  ticksX=5;
  ticksY=5;
  slope=1.0;
  offset=0.0;
}

void GraphPanel::OnChar(wxKeyEvent& event)
{
  double dtmp, dtmp1;
  wxPoint mousePos;
  GetDoubleDialog *dg;

  switch(event.GetKeyCode()){
  case '+':							  // zoom in
  case '=':
	 dtmp=(zoomCurrent.m_maxX+zoomCurrent.m_minX)/2.0;	  // mean
	 dtmp1=(zoomCurrent.m_maxX-zoomCurrent.m_minX)/2.0*.9; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxX=dtmp+dtmp1;
	 zoomCurrent.m_minX=dtmp-dtmp1;

	 dtmp=(zoomCurrent.m_maxY+zoomCurrent.m_minY)/2.0;	  // mean
	 dtmp1=(zoomCurrent.m_maxY-zoomCurrent.m_minY)/2.0*.9; // 1/2 range 0.9 is zoom factor
	 zoomCurrent.m_maxY=dtmp+dtmp1;
	 zoomCurrent.m_minY=dtmp-dtmp1;
	 Refresh(true);
	 break;
  case '-':							  // zoom out
	 dtmp=(zoomCurrent.m_maxX+zoomCurrent.m_minX)/2.0;	  // mean
	 dtmp1=(zoomCurrent.m_maxX-zoomCurrent.m_minX)/2.0*1.1; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxX=dtmp+dtmp1;
	 zoomCurrent.m_minX=dtmp-dtmp1;

	 dtmp=(zoomCurrent.m_maxY+zoomCurrent.m_minY)/2.0;	  // mean
	 dtmp1=(zoomCurrent.m_maxY-zoomCurrent.m_minY)/2.0*1.1; // 1/2 range 0.9 is zoom factor
	 zoomCurrent.m_maxY=dtmp+dtmp1;
	 zoomCurrent.m_minY=dtmp-dtmp1;
	 Refresh(true);
	 break;
  case WXK_LEFT:				  // pan left
	 dtmp1=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*0.1; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxX-=dtmp1;
	 zoomCurrent.m_minX-=dtmp1;
	 Refresh(true);
	 break;
  case WXK_RIGHT:				  // pan right
	 dtmp1=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*0.1; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxX+=dtmp1;
	 zoomCurrent.m_minX+=dtmp1;
	 Refresh(true);
	 break;
  case WXK_UP:				  // pan up
	 dtmp1=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*0.1; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxY+=dtmp1;
	 zoomCurrent.m_minY+=dtmp1;
	 Refresh(true);
	 break;
  case WXK_DOWN:				  // pan down
	 dtmp1=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*0.1; // 1/2 range. 0.9 is zoom factor
	 zoomCurrent.m_maxY-=dtmp1;
	 zoomCurrent.m_minY-=dtmp1;
	 Refresh(true);
	 break;
  case 'a':
	 ZoomAll();
	 Refresh(true);
	 break;
  case 'p':
	 ZoomPrevious();
	 Refresh(true);
	 break;
  case ' ':							  // record the current zoom setting
	 zoomLast=zoomCurrent;
	 break;
  case 'r':							  // repaint
	 Refresh(true);
	 break;
  case 't':							  // format as time
	 m_xAxisType=(m_xAxisType==XAXIS_NORMAL)?XAXIS_NORMAL:XAXIS_MINUTES;
	 Refresh();
	 break;
  case 'h':							  // add horizontal line
	 dg=new GetDoubleDialog(this, "Enter horizontal line position:");
	 if(dg->ShowModal()==wxID_OK){
		dtmp=dg->GetValue();
		AddLine(dtmp,GP_HORIZONTAL);
		Refresh(true);
	 }
	 delete(dg);
	 break;
  case 'v':							  // add vertical line
	 dg=new GetDoubleDialog(this, "Enter vertical line position:");
	 if(dg->ShowModal()==wxID_OK){
		dtmp=dg->GetValue();
		AddLine(dtmp,GP_VERTICAL);
		Refresh(true);
	 }
	 delete(dg);
	 break;
  default:
	 event.Skip();
  }

  //  wxPostEvent(m_parent, event); // pass event to the parent frame
}

void GraphPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  DrawGraph(&dc, true);
}
// ----------------------------------------------------------------------
// `AddTrace - Add a trace to the data
// ----------------------------------------------------------------------
size_t GraphPanel::AddTrace()					  // returns index of the new trace
{
  int ii;
  traceList.Append(new TraceClass);
  unfilteredList.Append(new TraceClass);
 
  ii=traceList.GetCount();
  if(ii>0){
	 traceList.Item(ii-1)->GetData()->legend=wxString::Format("Trace %d", ii);
	 unfilteredList.Item(ii-1)->GetData()->legend=wxString::Format("Trace %d", ii);
  }
  return((size_t)ii);
}

// ----------------------------------------------------------------------
// `AddPoint
// ----------------------------------------------------------------------
bool GraphPanel::AddPoint(size_t l, wxRealPoint xy)
{
  PointList *pointList;
  if(l>traceList.GetCount())return(false); // can't add to trace that
                                           // isn't there
  pointList=&traceList.Item(l)->GetData()->pointList;
  pointList->Append(new wxRealPoint(xy));

  pointList=&unfilteredList.Item(l)->GetData()->pointList;
  pointList->Append(new wxRealPoint(xy));
  return(true);
}
// ----------------------------------------------------------------------
// `AddPointRealtime
// ----------------------------------------------------------------------
bool GraphPanel::AddPointRealtime(size_t l, wxRealPoint xy)
{
  wxClientDC dc(this);
  AddPoint(l, xy);				  // add the point to the data 
  DrawGraph(&dc, false);		  // don't repaint entire area
  return(true);
}

bool GraphPanel::RemovePoint(int l, int i) // remove a point based on index
{
  return(true);
}

bool GraphPanel::RemovePoint(int l, double x) // remove a point based on x
{
  return(true);
}

bool GraphPanel::SortPoints(int l,  bool forward) // sort the points forward or backward
{
  
  return(true);
}

bool GraphPanel::InsertPoint(int l, double x, double y) // insert a point based on x
{
  return(true);
}

bool GraphPanel::InsertPoint(int l, int i, double y) // insert a point based on 
{
  return(true);
}
TraceClass* GraphPanel::GetTrace(int i){
  return(traceList.Item(i)->GetData());
}

bool GraphPanel::ClearTrace(int l)		  // clear points in a trace
{
  PointList *pointList;
  if(l>traceList.GetCount())return(false); // can't add to trace that
                                           // isn't there
  pointList=&traceList.Item(l)->GetData()->pointList;
  pointList->Clear();

  pointList=&unfilteredList.Item(l)->GetData()->pointList;
  pointList->Clear();
  return(true);
}

int GraphPanel::GetTraceCount()
{
  return(traceList.GetCount());
}
bool GraphPanel::UpdateGraph()  // update entire graph
{
  wxClientDC dc(this);
  DrawGraph(&dc, true);
  return(true);
}

// `DrawGraph
bool GraphPanel::DrawGraph(wxDC *dc, bool repaint)
{
  wxSize clientSize;
  wxRect clientRect;
  wxRect legendRect;
  int    traces;
  int    traceNum;
  int    xpos,ypos;
  wxPoint orig;
  wxPen thePen;
  wxString outtxt;
  TraceClass *theTrace;
  orig=GetClientAreaOrigin();
  double sum, sumsq, ns;  // for statistics calcs
  double maxDisplayed, minDisplayed;
  int maxWidth;
  wxDateTime tDate;
  long xxx,yyy;

  if(repaint) dc->Clear();
  clientSize=GetClientSize();
  clientRect.SetX(orig.x);		  // set up the clientRectangle that we paint in
  clientRect.SetY(orig.y);
  clientRect.SetHeight(clientSize.GetHeight());
  clientRect.SetWidth(clientSize.GetWidth());

  wxCoord width,height;
  dc->GetTextExtent(m_xlabel, &width, &height);	// draw the x label
  xxx=(clientRect.GetWidth()-width)/2+clientRect.GetLeft();
  yyy=clientRect.GetHeight()-height+clientRect.GetTop();
  rPickPoints[RP_ZOOMX]=wxRect(xxx,yyy,width,height);
  if(repaint)dc->DrawText(m_xlabel, xxx, yyy);
  clientRect.SetHeight(clientRect.GetHeight()-4*height);
  clientRect.SetY(clientRect.GetTop()+height);
  
  dc->GetTextExtent(m_ylabel, &width, &height);	// draw the y label
  xxx=clientRect.GetLeft();
  yyy=(clientRect.GetHeight()-width)/2+clientRect.GetTop();
  rPickPoints[RP_ZOOMY]=wxRect(xxx,yyy-width,height,width);
  if(repaint)dc->DrawRotatedText(m_ylabel, xxx, yyy, 90);
  clientRect.SetWidth(clientRect.GetWidth()-2*height);
  clientRect.SetX(clientRect.GetLeft()+height*2);

  // -- find legend width --
  maxWidth=0;
  traces=traceList.GetCount();
  theTrace=NULL;
  for(traceNum=0;traceNum<traces;traceNum++){	// find legend width
	 theTrace=traceList.Item(traceNum)->GetData();
	 dc->GetTextExtent(theTrace->legend, &width, &height);
	 if(width>maxWidth) maxWidth=width;
  }
										  // see if max,min,mean,etc is wider
  outtxt.Empty();
  if(theTrace!=NULL && theTrace->showMax==true){
	 outtxt+=wxString::Format("Max:%3.2lf ", theTrace->maxDisplayed);
  }
  if(theTrace!=NULL && theTrace->showMean==true){
	 outtxt+=wxString::Format("Mean:%3.2lf ", theTrace->mean);
  }
  if(theTrace!=NULL && theTrace->showMin==true){
	 outtxt+=wxString::Format("Min:%3.2lf ", theTrace->minDisplayed);
  }
  if(theTrace!=NULL && theTrace->showStdv==true){
	 outtxt+=wxString::Format("Stdv:%6.4lf ", theTrace->stdev);
  }
  if(theTrace!=NULL && !outtxt.IsEmpty()){
	 dc->GetTextExtent(outtxt, &width, &height);
	 if(width>maxWidth) maxWidth=width;
  }

  if(m_showLegend){
	 outtxt=wxString::Format("3");// get a width for lines
	 dc->GetTextExtent(outtxt, &width, &height);
	 legendRect.SetLeft(clientRect.GetRight()-maxWidth-4*width);
	 legendRect.SetRight(clientRect.GetRight());
	 legendRect.SetTop(height*2);
	 legendRect.SetBottom(clientRect.GetBottom()-height);

	 clientRect.SetWidth(clientRect.GetWidth()-maxWidth-4*width);
  }
			  // draw ticks and labels
  if(ticksY>1){					  // y ticks
	 double loc,yy;
	 int ii;
	 int sx,sy;

	 maxWidth=0;
	 for(ii=0;ii<=ticksY;ii++){  // find max width of y labels
		outtxt=wxString::Format("%0.2lf", (double)ii*(zoomCurrent.m_maxY-zoomCurrent.m_minY)/ticksY+zoomCurrent.m_minY);
		dc->GetTextExtent(outtxt, &width, &height);
		if(width>maxWidth)maxWidth=width;
	 }
	 clientRect.SetWidth(clientRect.GetWidth()-(maxWidth+height/2));
	 clientRect.SetX(clientRect.GetLeft()+(maxWidth+height/2));
	 if(repaint){
		yy=clientRect.GetHeight()/ticksX;
		for(ii=0;ii<=ticksY;ii++){
		  loc=(double)ii*yy;		  // tick mark
		  loc=clientRect.GetBottom()-loc;
		  dc->DrawLine(clientRect.GetLeft()-height/2, (int)loc,
							clientRect.GetLeft(), (int)loc);
		  outtxt=wxString::Format("%0.2lf", (double)ii*(zoomCurrent.m_maxY-zoomCurrent.m_minY)/ticksY+zoomCurrent.m_minY);
		  dc->GetTextExtent(outtxt, &width, &height);
		  sx=(int)(clientRect.GetLeft()-width-height/2);
		  sy=(int)loc-height/2;
		  dc->DrawText(outtxt,sx,sy);
		  if(ii==0)pickPoints[GP_MINY]=wxRect(sx,sy, width,height);
		  if(ii==ticksY)pickPoints[GP_MAXY]=wxRect(sx,sy, width,height);
		}
	 }
  }
  if(ticksX>1){					  // x ticks
    long tLong;
	 double loc,xx;
	 int ii;
	 int sx,sy;
	 double seconds;

	 if(m_xAxisType==XAXIS_MINUTES){	  // label in time format
		outtxt=HHMMSS(zoomCurrent.m_maxX);
	 }else if(m_xAxisType==XAXIS_DATES){
      tDate=m_startDate+wxDateSpan::Days(zoomCurrent.m_maxX);
      outtxt=tDate.FormatISODate();
    }else{							  // label in normal format
		outtxt=wxString::Format("%02lf", zoomCurrent.m_maxX);
	 }
	 dc->GetTextExtent(outtxt, &width, &height);	// adjust right boundary to fit
		// last label
	 if(!m_showLegend || width/2+height>legendRect.GetWidth())
		clientRect.SetWidth(clientRect.GetWidth()-(width/2+height-legendRect.GetWidth()));

	 xx=(double)clientRect.GetWidth()/(double)ticksX;
	 if(repaint){					 
		for(ii=0;ii<=ticksX;ii++){
		  loc=(double)ii*xx;
		  loc+=(double)clientRect.GetLeft();
		  dc->DrawLine((int)loc, clientRect.GetBottom(), // tick mark
							(int)loc,
							clientRect.GetBottom()+height/2);
		  if(m_xAxisType==XAXIS_MINUTES){ // draw the label in time format
			 seconds=(double)ii*(zoomCurrent.m_maxX-zoomCurrent.m_minX)/ticksX+zoomCurrent.m_minX;
			 outtxt=HHMMSS(seconds);
		  }else if(m_xAxisType==XAXIS_DATES){
			 seconds=(double)ii*(zoomCurrent.m_maxX-zoomCurrent.m_minX)/ticksX+zoomCurrent.m_minX;
          tDate=m_startDate+wxTimeSpan::Days(seconds);
          outtxt=tDate.FormatISODate();
        }else{						  // label in normal format
			 outtxt=wxString::Format("%0.2lf",
											 (double)ii*(zoomCurrent.m_maxX-zoomCurrent.m_minX)/ticksX+zoomCurrent.m_minX);
		  }
		  dc->GetTextExtent(outtxt, &width, &height);
		  sx=(int)loc-width/2;
		  sy=(int)(clientRect.GetBottom()+height/2);
		  dc->DrawText(outtxt, sx , sy);
		  if(ii==0)pickPoints[GP_MINX]=wxRect(sx,sy, width,height);
		  if(ii==ticksX)pickPoints[GP_MAXX]=wxRect(sx,sy, width,height);
		}
	 }
  }
  
  if(repaint){						  // draw the axis
	 dc->SetBrush(*wxLIGHT_GREY_BRUSH);
	 dc->SetPen(*wxLIGHT_GREY_PEN);
	 dc->DrawRectangle(clientRect);
	 dc->SetPen(*wxBLACK_PEN);
	 dc->DrawLine(clientRect.GetLeft(),clientRect.GetBottom(),	
					  clientRect.GetRight(),clientRect.GetBottom());
	 dc->DrawLine(clientRect.GetLeft(),clientRect.GetTop(),
					  clientRect.GetLeft(),clientRect.GetBottom());
  }

  // -- Draw Traces and lines--
  dc->SetClippingRegion(clientRect);
  plottingRegion=clientRect;
  traces=traceList.GetCount();

  if(traces>0){
	 int lines;
	 long points, ii;
	 wxRealPoint xy;
	 PointList *pointList;
	 double scaleX, scaleY;
	 double xx,yy, lx, ly;
	 LineList::Node *lineNode;
	 PointList::Node *node;
	 LineClass *currentLine;

	 scaleX=(double)(clientRect.GetWidth())/(zoomCurrent.m_maxX-zoomCurrent.m_minX);
	 scaleY=(double)(clientRect.GetHeight())/(zoomCurrent.m_maxY-zoomCurrent.m_minY);

	 if(repaint){
		m_minXValue=9e9;
		m_minYValue=9e9;
		m_maxXValue=-9e9;
		m_maxYValue=-9e9;

		// draw the lines
		lines=lineList.GetCount();
		lineNode=lineList.GetFirst();
		while(lineNode!=NULL){
		  currentLine=lineNode->GetData();
		  thePen.SetColour(currentLine->lineColour);	// set
		  dc->SetPen(thePen);
		  if(currentLine->vertical==GP_VERTICAL){ // draw a vertical line
			 xy.x=currentLine->pos;
			 xy.x+=theTrace->shift; // apply shift
			 xx=(xy.x-zoomCurrent.m_minX)*scaleX+clientRect.GetLeft(); // new screen x
			 dc->DrawLine((int)xx,(int)plottingRegion.GetTop(),
							  (int)xx,(int)plottingRegion.GetBottom());
		  }else{						  // horizontal line
			 xy.y=currentLine->pos;
			 yy=clientRect.GetBottom()-(xy.y*slope+offset-zoomCurrent.m_minY)*scaleY; // new screen y
			 dc->DrawLine((int)plottingRegion.GetLeft(), (int)yy,
							  (int)plottingRegion.GetRight(), (int)yy);
		  }
		  lineNode=lineNode->GetNext();	// get the second to last
		}

		for(traceNum=0;traceNum<traces;traceNum++){	// loop through the traces
		  maxDisplayed=-9e9;
		  minDisplayed=9e9;
		  sum=0; sumsq=0; ns=0;	  // stats only correct if a full repaint
		  theTrace=traceList.Item(traceNum)->GetData();
		  //		  thePen.SetColour(traceList.Item(traceNum)->GetData()->traceColour);	// set
		  thePen.SetColour(theTrace->traceColour);	// set
		  dc->SetPen(thePen);
		  pointList=&theTrace->pointList;
		  points=pointList->GetCount();
		  node=pointList->GetFirst();
		  lx=0;ly=0;				  // just to avoid a compiler warning
		  for(ii=0;ii<points;ii++){ // loop through points in the trace
			 xy=*node->GetData();
			 xy.x+=theTrace->shift; // apply shift
			 xy.y=xy.y*theTrace->gain+theTrace->offset; // apply offset
			 if(xy.x>=zoomCurrent.m_minX && xy.x<=zoomCurrent.m_maxX){ // calc stats
				sum+=xy.y*slope+offset;
				sumsq+=(xy.y*slope+offset)*(xy.y*slope+offset);
				ns++;
				if(xy.y>maxDisplayed) maxDisplayed=xy.y;
				if(xy.y<minDisplayed) minDisplayed=xy.y;
			 }
			 if(xy.x<m_minXValue) m_minXValue=xy.x;	// find absolute max, min vals
			 if(xy.x>m_maxXValue) m_maxXValue=xy.x;	// find absolute max, min vals
			 if(xy.y*slope+offset<m_minYValue) m_minYValue=xy.y*slope+offset;	// find absolute max, min vals
			 if(xy.y*slope+offset>m_maxYValue) m_maxYValue=xy.y*slope+offset;	// find absolute max, min vals
			 
			 xx=(xy.x-zoomCurrent.m_minX)*scaleX+clientRect.GetLeft(); // new screen x
			 yy=clientRect.GetBottom()-(xy.y*slope+offset-zoomCurrent.m_minY)*scaleY; // new screen y
			 if(ii>0 && theTrace->show) // don't draw at point 0
				dc->DrawLine((int)lx,(int)ly, (int)xx,(int)yy);
			 lx=xx; ly=yy; // save for next line
			 node=node->GetNext();	  // get next node
		  }
		  if(ns>1){					  // store the stats
			 theTrace->stdev=sqrt((ns*sumsq-sum*sum)/ns/(ns-1.0)); 
			 theTrace->mean=sum/ns;
			 theTrace->maxDisplayed=maxDisplayed;
			 theTrace->minDisplayed=minDisplayed;
			 theTrace->max=m_maxYValue;
			 theTrace->min=m_minYValue;
		  }else{
			 theTrace->stdev=0;
			 theTrace->mean=0;
			 theTrace->maxDisplayed=0;
			 theTrace->minDisplayed=0;
			 theTrace->max=1;
			 theTrace->min=0;
		  }
		}
	 }else{										  // single point paint
		for(traceNum=0;traceNum<traces;traceNum++){	// loop through the traces
		  theTrace=traceList.Item(traceNum)->GetData();
		  thePen.SetColour(traceList.Item(traceNum)->GetData()->traceColour);	// set
		  dc->SetPen(thePen);
		  pointList=&traceList.Item(traceNum)->GetData()->pointList;
		  ii=pointList->GetCount()-2;
		  if(ii>0 && theTrace->show==true){
			 node=pointList->Item(ii);	// get the second to last
			 xy=*node->GetData();
			 xy.x+=theTrace->shift; // apply shift
			 xy.y=xy.y*theTrace->gain+theTrace->offset; // apply offset
			 lx=(xy.x-zoomCurrent.m_minX)*scaleX+clientRect.GetLeft(); // new screen x
			 ly=clientRect.GetBottom()-(xy.y*slope+offset-zoomCurrent.m_minY)*scaleY; // new screen y

			 node=node->GetNext();	// get the second to last
			 xy=*node->GetData();
			 xy.x+=theTrace->shift; // apply shift
			 xy.y=xy.y*theTrace->gain+theTrace->offset; // apply offset
			 xx=(xy.x-zoomCurrent.m_minX)*scaleX+clientRect.GetLeft(); // new screen x
			 yy=clientRect.GetBottom()-(xy.y*slope+offset-zoomCurrent.m_minY)*scaleY; // new screen y
			 dc->DrawLine((int)lx,(int)ly, (int)xx,(int)yy);
			 //			 RefreshRect(wxRect(wxPoint((int)xx,(int)yy), wxPoint((int)lx,(int)ly)));
			 //			 m_parent->Update();
		  }
		}
	 }
  }
  rPickPoints[RP_ZOOMBOTH]=clientRect;
  // draw the legend
  if(m_showLegend){
	 dc->DestroyClippingRegion();
	 dc->SetClippingRegion(legendRect);
	 outtxt=wxString::Format("3");// get a width for lines
	 dc->GetTextExtent(outtxt, &width, &height);
	 xpos=legendRect.GetLeft()+(int)(3.5*(double)width);
	 ypos=legendRect.GetTop()+height;
	 for(traceNum=0;traceNum<traces;traceNum++){
		theTrace=traceList.Item(traceNum)->GetData();
		theTrace->legendPick.SetTop(ypos);
		theTrace->legendPick.SetLeft(xpos);
		theTrace->legendPick.SetRight(legendRect.GetRight());
		dc->DrawText(theTrace->legend, xpos,ypos);
		thePen.SetColour(theTrace->traceColour);	// set
		dc->SetPen(thePen);
		dc->DrawLine(legendRect.GetLeft()+width, height/2+ypos,
						 legendRect.GetLeft()+3*width, height/2+ypos);
		ypos+=height;
		outtxt.Empty();
		if(theTrace->showMax==true){
		  outtxt+=wxString::Format("Max:%3.2lf ", theTrace->maxDisplayed);
		}
		if(theTrace->showMean==true){
		  outtxt+=wxString::Format("Mean:%3.2lf ", theTrace->mean);
		}
		if(theTrace->showMin==true){
		  outtxt+=wxString::Format("Min:%3.2lf ", theTrace->minDisplayed);
		}
		if(theTrace->showStdv==true){
		  outtxt+=wxString::Format("Stdv:%6.4lf ", theTrace->stdev);
		}
		if(!outtxt.IsEmpty()){
		  dc->DrawText(outtxt, xpos, ypos);
		  ypos+=height;
		}

		theTrace->legendPick.SetBottom(ypos);
		if(!theTrace->show){		  // cross out legend text
		  dc->SetPen(*wxLIGHT_GREY_PEN);
		  dc->SetBrush(*wxTRANSPARENT_BRUSH);
		  dc->DrawRectangle(theTrace->legendPick);
		  dc->DrawLine(theTrace->legendPick.GetLeft(),theTrace->legendPick.GetTop(),
							theTrace->legendPick.GetRight(),theTrace->legendPick.GetBottom());
		}
	 }
  }
  return(true);
}

void GraphPanel::SetXLabel(wxString xLabel)
{
  m_xlabel=xLabel;
}

void GraphPanel::SetYLabel(wxString yLabel)
{
  m_ylabel=yLabel;
}

bool GraphPanel::SetTicksX(int xt)
{
  if(xt>0){
	 ticksX=xt;
	 return(true);
  }else{
	 return(false);
  }
}

bool GraphPanel::SetTicksY(int xt)
{
  if(xt>0){
	 ticksY=xt;
	 return(true);
  }else{
	 return(false);
  }
}

bool GraphPanel::SetXAxisType(int type)
{
  if(type<0) return(false);
  if(type>XAXIS_DATES) return(false);
  m_xAxisType=type;
  return(true);
}


bool GraphPanel::CalculateExtents(int traceNum) // use -1 for all
{
  long ii;
  int startTrace, endTrace;
  long points;
  double xx,yy;

  TraceClass *theTrace;
  PointList *pointList;
  PointList::Node *node;

  
  if(traceNum>=(int)traceList.GetCount()) return(false);

  if(traceNum==-1){				  // will look at all traces
	 startTrace=0;
	 endTrace=traceList.GetCount()-1;
  }else{								  // just going to look at one trace
	 startTrace=traceNum;
	 endTrace=traceNum;
  }

  m_maxXValue=-9e99;
  m_minXValue=9e99;
  m_maxYValue=-9e99;
  m_minYValue=9e99;

  for(traceNum=startTrace; traceNum<=endTrace; traceNum++){
	 theTrace=traceList.Item(traceNum)->GetData();
	 pointList=&theTrace->pointList;
	 node=pointList->GetFirst();
	 points=pointList->GetCount();
	 for(ii=0;ii<points;ii++){	  // now check for extents
		xx=node->GetData()->x;
		yy=node->GetData()->y;
		if(xx>m_maxXValue) m_maxXValue=xx;
		if(xx<m_minXValue) m_minXValue=xx;
		if(yy>m_maxYValue) m_maxYValue=yy;
		if(yy<m_minYValue) m_minYValue=yy;

		node=node->GetNext();	// get the second to last
	 }
  }
  if(m_maxXValue==-9e99) m_maxXValue=1; // don't leave at huge numbers
  if(m_minXValue==9e99)  m_minXValue=0;
  if(m_maxYValue==-9e99) m_maxYValue=1;
  if(m_minYValue==9e99)  m_minYValue=0;
  return(true);
}


void GraphPanel::SetMinX(double mx)
{
  zoomCurrent.m_minX=mx;
}
void GraphPanel::SetMaxX(double mx)
{
  zoomCurrent.m_maxX=mx;
}
void GraphPanel::SetMinY(double my)
{
  zoomCurrent.m_minY=my;
}
void GraphPanel::SetMaxY(double my)
{
  zoomCurrent.m_maxY=my;
}

void GraphPanel::SetStartDate(wxDateTime start)
{
  m_startDate=start;
  start.ResetTime();
}

bool GraphPanel::SetTraceColour(int trace, wxColour newColour)
{
  if((unsigned int)trace>traceList.GetCount())return(false); // can't add to trace that
  // isn't there
  traceList.Item(trace)->GetData()->traceColour=newColour;
  return(true);
}

bool GraphPanel::ShowTrace(int trace, bool state)
{
  if((unsigned int)trace>traceList.GetCount())return(false); // can't add to trace that
  // isn't there
  traceList.Item(trace)->GetData()->show=state;
  Refresh(true);
  return(true);
}

void GraphPanel::OnEnterKey(wxCommandEvent &)
{
  wxMouseEvent event;
  OnLeftButtonUp(event);
}

void GraphPanel::OnLeftButtonUp(wxMouseEvent& event)
{
  double dtmp;
  wxRect rect;
  double x1,y1,x2=0,y2;
  int ii;
  wxString tTxt;
  wxDateTime tDate;

  if(inNumWindow->IsShown()){	  // edited a value
	 inNumWindow->Show(false);
	 if(inNumWindow->GetValue().ToDouble(&dtmp)==true){ // valid number
		*inNumWindowValue=dtmp;
		Refresh(true);
	 }else if(inNumWindow->GetValue().Find(":")>-1){
		tTxt=inNumWindow->GetValue();
		ii=tTxt.Find(":");
		if(tTxt.Left(ii).ToDouble(&x1)==true && // they entered in time format
			tTxt.Mid(ii+1).ToDouble(&x2)==true){
		  dtmp=x1*60.0+x2;
		  *inNumWindowValue=dtmp;
		  Refresh(true);
		}
	 }
  }else if(trackingZoomPosition==true){ // zoomed with mouse
	 trackingZoomPosition=false;
	 rect=wxRect(zoomStart.x, zoomStart.y, event.GetX()-zoomStart.x, event.GetY()-zoomStart.y);
	 
	 y1=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*(double)(rect.GetTop()-plottingRegion.GetBottom())/
		((double)-plottingRegion.GetHeight())+zoomCurrent.m_minY;
	 y2=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*(double)(rect.GetBottom()-plottingRegion.GetBottom())/
		((double)-plottingRegion.GetHeight())+zoomCurrent.m_minY;

	 x1=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*(double)(rect.GetLeft()-plottingRegion.GetLeft())/
		((double)plottingRegion.GetWidth())+zoomCurrent.m_minX;
	 x2=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*(double)(rect.GetRight()-plottingRegion.GetLeft())/
		((double)plottingRegion.GetWidth())+zoomCurrent.m_minX;

	 if(event.m_controlDown){	  // measuring, not zooming
		if(m_xAxisType==XAXIS_NORMAL){
		  tTxt=wxString::Format("(%lf,%lf)-(%lf,%lf)\ndX=%lf\ndY=%lf",
										x1,y1,x2,y2,x2-x1,y2-y1);
		}else{						  // format as time on x axis
		  tTxt=wxString::Format("(%s,%lf)-(%s,%lf)\ndX=%s\ndY=%lf",
										HHMMSS(x1).GetData(),y1,
										HHMMSS(x2).GetData(),y2,
										HHMMSS(x2-x1).GetData(),y2-y1);
		}
		wxMessageBox(tTxt, "Measured", wxOK);
		return;
	 }

	 zoomLast=zoomCurrent;
	 zoomCurrent.m_maxX=x2; zoomCurrent.m_minX=x1;
	 zoomCurrent.m_minY=y2; zoomCurrent.m_maxY=y1;
	 Refresh();
  }else if(m_crossHairs==true){ // drawing a cross-hairs measuring point
	 wxClientDC dc(this);
	 wxSize clSize;
	 double xx,yy;

	 clSize=GetClientSize();
	 dc.SetBrush(*wxTRANSPARENT_BRUSH);

	 dc.DrawLine(0, m_crossHairsLoc.y, clSize.GetWidth(), m_crossHairsLoc.y);
	 dc.DrawLine(m_crossHairsLoc.x, 0, m_crossHairsLoc.x, clSize.GetHeight());

	 yy=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*(double)(m_crossHairsLoc.y-plottingRegion.GetBottom())/
		((double)-plottingRegion.GetHeight())+zoomCurrent.m_minY;
	 xx=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*(double)(m_crossHairsLoc.x-plottingRegion.GetLeft())/
		((double)plottingRegion.GetWidth())+zoomCurrent.m_minX;

	 if(m_xAxisType==XAXIS_MINUTES){
     dc.DrawText(wxString::Format("%s, %3.1lf", HHMMSS(xx).GetData(), xx), m_crossHairsLoc.x, 0);
	 }else if(m_xAxisType==XAXIS_DATES){
      tDate=m_startDate+wxDateSpan::Days(xx+1);
      dc.DrawText(tDate.FormatISODate(), m_crossHairsLoc.x, 0);
    }else{
		dc.DrawText(wxString::Format("%3.2lf", xx), m_crossHairsLoc.x, 0);
	 }
	 dc.DrawText(wxString::Format("%3.2lf", yy), 0, m_crossHairsLoc.y);

	 m_crossHairs=false;
	 m_crossHairsLoc.x=0;
	 m_crossHairsLoc.y=0;

  }else{
	 event.Skip();
  }
}
void GraphPanel::OnLeftButtonDown(wxMouseEvent& event)
{
  if(event.m_shiftDown || event.m_controlDown){
	 zoomStart=wxPoint(event.GetX(), event.GetY());
	 zoomTrackLast=zoomStart;
	 trackingZoomPosition=true;
  }else{
	 m_crossHairs=true;
  }
  event.Skip();
}

void GraphPanel::OnDClick(wxMouseEvent& event)
{
  int ii;
  TraceList::Node *node;
  TraceClass *theTrace;
  int traceNum;


  for(ii=0;ii<GP_NUMPICKPOINTS;ii++){
	 if(pickPoints[ii].Contains(event.GetX(), event.GetY())){
		switch(ii){
		case GP_MINX:
		  inNumWindow->Move(pickPoints[GP_MINX].GetX(),
								  pickPoints[GP_MINX].GetY());
		  inNumWindow->SetValue(wxString::Format("%lf", zoomCurrent.m_minX));
		  inNumWindowValue=&zoomCurrent.m_minX;
		  inNumWindow->SetSelection(-1,-1);	// select the text
		  inNumWindow->SetFocus();
		  inNumWindow->Show(true);
		  break;
		case GP_MINY:
		  inNumWindow->Move(pickPoints[GP_MINY].GetX(),
								  pickPoints[GP_MINY].GetY());
		  inNumWindow->SetValue(wxString::Format("%lf", zoomCurrent.m_minY));
		  inNumWindowValue=&zoomCurrent.m_minY;
		  inNumWindow->SetSelection(-1,-1);	// select the text
		  inNumWindow->SetFocus();
		  inNumWindow->Show(true);
		  break;
		case GP_MAXX:
		  inNumWindow->Move(pickPoints[GP_MAXX].GetX(),
								  pickPoints[GP_MAXX].GetY());
		  inNumWindow->SetValue(wxString::Format("%lf", zoomCurrent.m_maxX));
		  inNumWindowValue=&zoomCurrent.m_maxX;
		  inNumWindow->SetSelection(-1,-1);	// select the text
		  inNumWindow->SetFocus();
		  inNumWindow->Show(true);
		  break;
		case GP_MAXY:
		  inNumWindow->Move(pickPoints[GP_MAXY].GetX(),
								  pickPoints[GP_MAXY].GetY());
		  inNumWindow->SetValue(wxString::Format("%lf", zoomCurrent.m_maxY));
		  inNumWindowValue=&zoomCurrent.m_maxY;
		  inNumWindow->SetSelection(-1,-1);	// select the text
		  inNumWindow->SetFocus();
		  inNumWindow->Show(true);
		  break;
		};
	 }
  }
  node=traceList.GetFirst();
  traceNum=0;						  // use this to count up to the actual trace
  while(node!=NULL){
	 long ret;

	 theTrace=node->GetData();
	 if(theTrace->legendPick.Contains(event.GetX(), event.GetY())){	// dclicked trace legend
		TracePropertiesDialog di(this, theTrace);
		ret=di.ShowModal();
		if(ret==wxID_OK){
		  di.GetData(theTrace);
		  Filter(traceNum);
		  Refresh();
		}
		node=NULL;					  // end the while loop
	 }else{
		node=node->GetNext();
		traceNum++;
	 }
  }
}

double GraphPanel::Mean(unsigned int l)
{
  if(l>traceList.GetCount())return(false); // trace exists?
  return(traceList.Item(l)->GetData()->mean);
}
double GraphPanel::Stdev(unsigned int l)
{
  if(l>traceList.GetCount())return(false); // trace exists?
  return(traceList.Item(l)->GetData()->stdev);
}
bool GraphPanel::SetLegend(unsigned int l, wxString newName)
{
  if(l>traceList.GetCount())return(false); // trace exists?
  traceList.Item(l)->GetData()->legend=newName;
  return(true);
}

// `OnRightButtonUp
void GraphPanel::OnRightButtonUp(wxMouseEvent& event)
{
  int ii;
  int traces;
  TraceClass *theTrace;

  for(ii=0;ii<RP_NUMPICKPOINTS;ii++){
	 if(rPickPoints[ii].Contains(event.GetX(), event.GetY())){
		switch(ii){
      case RP_ZOOMX:
		  if(traceList.GetCount()>0){	// don't zoom if no curves
			 zoomCurrent.m_minX=m_minXValue;
			 zoomCurrent.m_maxX=m_maxXValue;
			 Refresh();
		  }
        break;
      case RP_ZOOMY:
		  if(traceList.GetCount()>0){	// don't zoom if no curves
			 zoomCurrent.m_minY=m_minYValue;
			 zoomCurrent.m_maxY=m_maxYValue;
			 Refresh();
		  }
        break;
		case RP_ZOOMBOTH:
		  if(traceList.GetCount()>0){	// don't zoom if no curves
			 zoomCurrent.m_minX=m_minXValue;
			 zoomCurrent.m_maxX=m_maxXValue;
			 zoomCurrent.m_minY=m_minYValue;
			 zoomCurrent.m_maxY=m_maxYValue;
			 Refresh();
		  }
		  break;
		};
	 }
  }
  traces=traceList.GetCount();
  for(ii=0;ii<traces;ii++){
	 theTrace=traceList.Item(ii)->GetData();
	 if(theTrace->legendPick.Contains(event.GetX(), event.GetY())){
		theTrace->show=!theTrace->show;
		Refresh();
	 }
  }
}

// `OnMouseMotion
void GraphPanel::OnMouseMotion(wxMouseEvent& event)
{
  if(trackingZoomPosition){
	 wxClientDC dc(this);
	 dc.SetLogicalFunction(wxINVERT);
	 dc.SetBrush(*wxTRANSPARENT_BRUSH);
	 dc.DrawRectangle(zoomStart.x, zoomStart.y,
							zoomTrackLast.x-zoomStart.x, zoomTrackLast.y-zoomStart.y);
	 zoomTrackLast.x=event.GetX();
	 zoomTrackLast.y=event.GetY();
	 dc.DrawRectangle(zoomStart.x, zoomStart.y,
							zoomTrackLast.x-zoomStart.x, zoomTrackLast.y-zoomStart.y);
  }else if(m_crossHairs){
	 wxClientDC dc(this);
	 wxSize clSize;
	 double xx,yy;

	 clSize=GetClientSize();
	 dc.SetLogicalFunction(wxINVERT);
	 dc.SetBrush(*wxTRANSPARENT_BRUSH);
	 if(m_crossHairsLoc.x!=0 && m_crossHairsLoc.y!=0){	// don't draw these the
		// first time
		dc.DrawLine(0, m_crossHairsLoc.y, clSize.GetWidth(), m_crossHairsLoc.y);
		dc.DrawLine(m_crossHairsLoc.x, 0, m_crossHairsLoc.x, clSize.GetHeight());
		yy=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*(double)(m_crossHairsLoc.y-plottingRegion.GetBottom())/
		  ((double)-plottingRegion.GetHeight())+zoomCurrent.m_minY;
		xx=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*(double)(m_crossHairsLoc.x-plottingRegion.GetLeft())/
		  ((double)plottingRegion.GetWidth())+zoomCurrent.m_minX;
	 }
	 m_crossHairsLoc.x=event.GetX();
	 m_crossHairsLoc.y=event.GetY();
	 dc.DrawLine(0, m_crossHairsLoc.y, clSize.GetWidth(), m_crossHairsLoc.y);
	 dc.DrawLine(m_crossHairsLoc.x, 0, m_crossHairsLoc.x, clSize.GetHeight());
	 yy=(zoomCurrent.m_maxY-zoomCurrent.m_minY)*(double)(m_crossHairsLoc.y-plottingRegion.GetBottom())/
		((double)-plottingRegion.GetHeight())+zoomCurrent.m_minY;
	 xx=(zoomCurrent.m_maxX-zoomCurrent.m_minX)*(double)(m_crossHairsLoc.x-plottingRegion.GetLeft())/
		((double)plottingRegion.GetWidth())+zoomCurrent.m_minX;
  }
  event.Skip();
}

bool GraphPanel::SaveAs(wxString fn)
{
  TraceClass *theTrace;
  PointList::Node *xyNode;
  wxRealPoint *xy;
  int ii;
  int traces=traceList.GetCount();
  if(traces==0)return(false);	  // no data to save
  FILE *fp=fopen(fn.GetData(), "w");
  if(fp==NULL)return(false);
  for(ii=0;ii<traces;ii++){
	 theTrace=traceList.Item(ii)->GetData(); // get the trace
	 xyNode=theTrace->pointList.GetFirst();
	 fprintf(fp,"<trace>\n");
	 fprintf(fp,"<legend>%s\n", (char*)theTrace->legend.char_str());
	 fprintf(fp,"<color>%d,%d,%d\n", theTrace->traceColour.Red(),
				theTrace->traceColour.Green(),
				theTrace->traceColour.Blue());
	 fprintf(fp,"<showmean>%d\n", theTrace->showMean);
	 fprintf(fp,"<showmax>%d\n",  theTrace->showMax);
	 fprintf(fp,"<showmin>%d\n",  theTrace->showMin);
	 fprintf(fp,"<showstdv>%d\n", theTrace->showStdv);
	 fprintf(fp,"<gain>%lf\n", theTrace->gain);
	 fprintf(fp,"<offset>%lf\n", theTrace->offset);
	 fprintf(fp,"<shift>%lf\n", theTrace->shift);
	 while(xyNode!=NULL){
		xy=xyNode->GetData();
		fprintf(fp, "<p>%lf,%lf\n", xy->x, xy->y);
		xyNode=xyNode->GetNext();
	 }
  }
  fclose(fp);
  return(true);
}

bool GraphPanel::Load(wxString fn)
{
  int traceNum=0;
  wxString inText;
  wxString keyWord;
  wxString dat;
  double xx,yy;
  int ii;
  long aa,bb,cc;

  if(!wxFile::Exists(fn)) return(false);
  wxReadTextFile infile(fn);
  while(infile.Ready()){
	 if(infile.ReadLine(&inText)==true){
		ii=inText.Find(">")+1;
		if((int)inText.Length()>ii){
		  dat=inText.Mid(ii);
		}else{
		  dat.Empty();
		}
		keyWord=inText.Left(ii);
		if(keyWord=="<trace>"){
		  traceNum=AddTrace()-1;
		}
		if(keyWord=="<color>"){	  // set the trace color
		  ii=dat.Find(",");
		  dat.Left(ii).ToLong(&aa);
		  dat=dat.Mid(ii+1);
		  ii=dat.Find(",");
		  dat.Left(ii).ToLong(&bb);
		  dat=dat.Mid(ii+1);
		  dat.ToLong(&cc);
		  SetTraceColour(traceNum, wxColour(aa,bb,cc));
		}
		if(keyWord=="<showmean>"){
		  if(dat=="1")
			 GetTrace(traceNum)->showMean=true; else GetTrace(traceNum)->showMean=false;
		}
		if(keyWord=="<showmax>"){
		  if(dat=="1")
			 GetTrace(traceNum)->showMax=true; else GetTrace(traceNum)->showMax=false;
		}
		if(keyWord=="<showmin>"){
		  if(dat=="1")
			 GetTrace(traceNum)->showMin=true; else GetTrace(traceNum)->showMin=false;
		}
		if(keyWord=="<showstdv>"){
		  if(dat=="1")
			 GetTrace(traceNum)->showStdv=true; else GetTrace(traceNum)->showStdv=false;
		}
		if(keyWord=="<gain>"){
		  dat.ToDouble(&GetTrace(traceNum)->gain);
		}
		if(keyWord=="<offset>"){
		  dat.ToDouble(&GetTrace(traceNum)->offset);
		}
		if(keyWord=="<shift>"){
		  dat.ToDouble(&GetTrace(traceNum)->shift);
		}
		if(keyWord=="<legend>"){
		  GetTrace(traceNum)->legend=dat;
		}
		if(keyWord=="<p>"){		  // get the actual point
		  ii=dat.Find(",");
		  dat.Left(ii).ToDouble(&xx);
		  dat.Mid(ii+1).ToDouble(&yy);
		  AddPoint(traceNum, wxRealPoint(xx,yy));
		}
	 }
  }
  return(true);
}

void GraphPanel::ZoomAll(){
  if(traceList.GetCount()>0){	// don't zoom if no curves
	 CalculateExtents(-1);

	 zoomCurrent.m_minX=m_minXValue;
	 zoomCurrent.m_maxX=m_maxXValue;
	 zoomCurrent.m_minY=m_minYValue;
	 zoomCurrent.m_maxY=m_maxYValue;
	 Refresh(true);
  }
}

void GraphPanel::ZoomPrevious()
{
  ZoomInfo tmpZoom;

  tmpZoom=zoomLast;
  zoomLast=zoomCurrent;
  zoomCurrent=tmpZoom;
}

// GraphPanel::Filter()
void GraphPanel::Filter(int filtTrace)
{
  double total=0;

  TraceClass *theTrace;
  TraceClass *unfTrace;

  PointList::Node *xyNode;
  PointList::Node *startNode;
  PointList::Node *endNode;

  double tx;
  double halfWidth;
  long pointCount;

  int traces=traceList.GetCount();
  if(filtTrace>=traces)return;	  // doesn't exist

  theTrace=traceList.Item(filtTrace)->GetData(); // get the trace
  unfTrace=unfilteredList.Item(filtTrace)->GetData(); // get unfiltered trace

  //  numPoints=theTrace->average;
  halfWidth=theTrace->average/2.0;

  xyNode=theTrace->pointList.GetFirst();
  startNode=unfTrace->pointList.GetFirst();
  endNode=unfTrace->pointList.GetFirst();

  total=endNode->GetData()->y;  // prime total with first number

  pointCount=1;
  while(xyNode!=NULL){			  // xyNode has the node we're counting to
	 while(endNode->GetNext()!=NULL &&
			 endNode->GetNext()->GetData()->x <= xyNode->GetData()->x+halfWidth){ // add end points
		endNode=endNode->GetNext();
		total+=endNode->GetData()->y;
		pointCount++;
	 }
	 while(startNode!=NULL && startNode->GetData()->x <= xyNode->GetData()->x-halfWidth){ // remove beginnings
		total-=startNode->GetData()->y;
		pointCount--;
		startNode=startNode->GetNext();
	 }

	 xyNode->GetData()->y=total/(double)(pointCount);
	 xyNode=xyNode->GetNext();
  }
}

// ---------------------------------------------------------------------------
// `AddLine
// Add a line (NOT a trace
// ---------------------------------------------------------------------------
bool GraphPanel::AddLine(double pp, bool orientVert)
{
  LineList::Node *lc;

  lc=lineList.Append(new LineClass(orientVert));
  if(lc==NULL){
	 return(false);
  }else{
	 lc->GetData()->pos=pp;
	 return(true);
  }
}

// ---------------------------------------------------------------------------
// `AddLine
// Add a line (NOT a trace
// ---------------------------------------------------------------------------
bool GraphPanel::AddLine(double pp, bool orientVert, wxColour lineColour)
{
  LineList::Node *lc;

  lc=lineList.Append(new LineClass(orientVert));
  if(lc==NULL){
	 return(false);
  }else{
	 lc->GetData()->pos=pp;
	 lc->GetData()->lineColour=lineColour;
	 return(true);
  }
}

// ----------------------------------------------------------------------
// MoveLine
// Move a line (Not a trace)
bool GraphPanel::MoveLine(int line, double newVal)
{
  int ii;
  ii=lineList.GetCount();
  if(line>=ii) return(false);
  lineList.Item(line)->GetData()->pos=newVal;
  return(true);
}



void GraphPanel::SetLineColour(int index, wxColour theColour)
{
  LineList::Node *lc;
  lc=lineList.Item(index);
  lc->GetData()->lineColour=theColour;
}

// ---------------------------------------------------------------------------
// `DeleteLine
// Remove a line (NOT a trace)
// ---------------------------------------------------------------------------
bool GraphPanel::DeleteLine(int line)
{
  int ii;
  ii=lineList.GetCount();
  if(line>=ii) return(false);
  lineList.DeleteNode(lineList.Item(ii));
  return(true);
}

wxString GraphPanel::HHMMSS(double ti)
{
  wxString ret;
  long hours=0, minutes=0, seconds=0, hunds=0;

  if((long)ti>3600){
	 hours=(long)ti/3600;
	 ti=ti-3600*(double)hours;
  }
  if(ti>60){
	 minutes=(long)ti/60;
	 ti=ti-60*(double)minutes;
  }
  if(ti>0.0){
	 seconds=(long)ti;
	 ti=ti-(double)seconds;
  }
  hunds=(long)(ti*100.0);

  ret=wxString::Format("%02ld:%02ld:%02ld.%02ld", hours, minutes, seconds, hunds);
  return(ret);
}

long GraphPanel::GetTraceLength(int traceNum)		  // return number of elements in trace
{
  if(traceNum>=(int)traceList.GetCount()) return(-1);
  return(traceList.Item(traceNum)->GetData()->pointList.GetCount());
}

wxRealPoint GraphPanel::GetPoint(int traceNum, long point)
{
  if(traceNum>=(int)traceList.GetCount()) return(wxRealPoint(-1,-1));
  if(point>=traceList.Item(traceNum)->GetData()->pointList.GetCount()) return(wxRealPoint(-1,-1));
  return(*traceList.Item(traceNum)->GetData()->pointList.Item(point)->GetData());
}


GraphPanel::~GraphPanel()		  // object deletion
{
  //  inNumWindow->Destroy();
  //  inNumWindow=NULL;
}

