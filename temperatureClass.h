#ifndef __TEMPERATURECLASS__
#define __TEMPERATURECLASS__

class TemperatureClass
{

private:
  // -- private vars --
  bool          m_valid;
  double        m_temperature;
  wxDateTime    m_time;
  // -- private funcs --


public:
  // -- public vars --

  // -- public funcs --
  TemperatureClass();
  bool          LookupTemperature();
  double        GetFahrenheit();
  double        GetCelsius();
  wxDateTime    GetTimestamp();
};


#endif
