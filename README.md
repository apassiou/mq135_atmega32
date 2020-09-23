# mq135_atmega32
MQ135 running on ATMEGA32 with 2x16 LCD

Calibration is required for each sensor, all are different just enough to where calibration cant be used from one to another.
To calibrate:

Run in _DEBUG = 1 and put the unit outdoors when air is clean.

LCD will show various mesurements as well as R0. Take a note of R0 value and set Line 17 RZERO value to it (mine is 60.52). Unit should now be calibrated.
Set _DEBUG = 0
Unit is ready to use.

My implementation uses 2 LEDs (yellow and red) for various levels of PPM.

NOTE: MQ135 does not detect AQI, it detects PPM in the air, it does not differentiate between gases, it will show the highest value one. Most of the time this would be CO2.
When indoors with closed doors/windows PPM should not exceed 1000. When air outside is clean the value should be between 300 and 450.
