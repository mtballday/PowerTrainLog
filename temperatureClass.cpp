#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/url.h"

#include "temperatureClass.h"

TemperatureClass::TemperatureClass()
{
  m_valid=false;
}

bool TemperatureClass::LookupTemperature()
{
  wxInputStream *is;
  char inbuf[10000];
  wxString strbuf;
  long ii;
  double newTemp;

//  wxURL url("http://www.theweathernetwork.com/weather/USUT0225&switchto=f");
  wxURL url("http://api.wunderground.com/weatherstation/WXCurrentObXML.asp?ID=KUTDRAPE7");
  if(url.GetError() == wxURL_NOERR){
    is = url.GetInputStream();
    is->Read(inbuf, sizeof(inbuf));
    strbuf=wxString(inbuf);
    ii=strbuf.Find("<temp_c>");
    strbuf=strbuf.Mid(ii+8,40);
    ii=strbuf.Find("</temp_c>");
    strbuf=strbuf.Left(ii);
    if(strbuf.ToDouble(&newTemp)==true){
      m_temperature=newTemp;
      m_time=wxDateTime::Now();
      m_valid=true;
      return(true);
    }
  }
  m_valid=false;
  return(false);
}

double TemperatureClass::GetFahrenheit()
{
  if(m_valid==true){
    return(m_temperature*9.0/5.0+32.0);
  }
  return(-9999);
}

double TemperatureClass::GetCelsius()
{
  if(m_valid==true){
    return(m_temperature);
  }
  return(-9999);
}

wxDateTime TemperatureClass::GetTimestamp()
{
  return(m_time);
}
