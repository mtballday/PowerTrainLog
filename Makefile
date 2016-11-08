#
# File:		Makefile for samples
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# This makefile requires a Unix version of wxWindows
# to be installed on your system. This is most often
# done typing "make install" when using the complete
# sources of wxWindows or by installing the two
# RPM packages wxGTK.XXX.rpm and wxGTK-devel.XXX.rpm
# under Linux.
#

#CXX = $(shell wx-config --cxx)
CXX = g++

INC = -I/home/rich/Builds/wxWidgets/include -I/home/rich/Builds/wxWidgets/Build/lib/wx/include/gtk3-unicode-3.1

PROGRAM = powerTrainLog

OBJECTS = $(PROGRAM).o graphPanel.o wxReadTextFile.o powertap.o bikeDayEntry.o bikeEntryData.o timePeriod.o entryChooser.o userProfile.o polar.o entryCalendar.o tracePropertiesDialog.o computrainer.o powerplot.o workoutMenuEditor.o workouts.o planDisplay.o weekViewPanel.o batchEdit.o dayEntrySelectionDialog.o singleWeekViewPanel.o profileEditor.o combineFilesDialog.o ctConvert.o powerfile.o ptConvert.o interval.o hrRepairDialog.o polarConvert.o temperatureClass.o garmin.o garminConvert.o trainingBalance.o section_record.o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c  $(INC) -g `wx-config --cxxflags` -o $@ $<

all:    $(PROGRAM)
	etags $(OBJECTS)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) `wx-config --libs` $(INC)

clean: 
	rm -f *.o $(PROGRAM)
