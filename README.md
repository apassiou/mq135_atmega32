# mq135_atmega32
MQ135 running on ATMEGA32 with 2x16 LCD

Will show: 
 * Current PPM
 * Average PPM for last hour
 * Average PPM for last 24 Hours
 * Lowest and Highest PPM since powered on
 * Utilizes two LEDs (yellow and red) to signal high PPM

IMPORTANT: Calibration is required for each sensor, all are different just enough to where calibration cant be used from one to another.
IMPORTANT2: MQ135 1K ohm resistor needs to be replaced with a 22K ohm resistor, this makes the sensor much more sensitive. 22K resistor is RED|RED|ORANGE. (See included image for location of resistor to replace). You can attempt to run with the stock 1K ohm resistor, it will be not very sensitive and you need to change #define RLOAD 22.0 to RLOAD 1.0

To calibrate:

 - First make sure the sensor is run for 24 to 48 hours of being on. This is initial burn-in that is required, otherwise readings are unreliable.
 - Run in _DEBUG = 1 and put the unit outdoors when air is clean. Leave outside for 15 minutes.

LCD will show various mesurements as well as R0. Take a note of R0 value and set Line 17 RZERO value in main.c to this value (mine is 60.52). Unit should now be calibrated.
Set _DEBUG = 0
Unit is ready to use. Test by putting outside, when air is clean PPM should be between 350 and 420.

NOTE: MQ135 does not detect AQI, it detects PPM in the air, it does not differentiate between gases, it will show the highest value one. Most of the time this would be CO2.
When indoors with closed doors/windows PPM should not exceed 1000. When air outside is clean the value should be between 300 and 450.

PPM REFERENCE:

250 - 400 ppm: background (normal) outdoor air level.
400 - 1,000 ppm: typical level found in occupied spaces with good air exchange.
1,000 - 2,000 ppm: level associated with complaints of drowsiness and poor air.
2,000 - 5,000 ppm: level associated with headaches, sleepiness, and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
5,000 ppm: this indicates unusual air conditions where high levels of other gases could also be present. Toxicity or oxygen deprivation could occur. This is the permissible exposure limit for daily workplace exposures.
40,000 ppm: this level is immediately harmful due to oxygen deprivation.

LICENSE: You are free to use this code as you wish. All I ask is a comment/note with link to this repo.
