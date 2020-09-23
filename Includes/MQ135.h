/**************************************************************************/
/*!
@file     MQ135.h
@author   G.Krocker (Mad Frog Labs)
@license  GNU GPLv3

First version of an Arduino Library for the MQ135 gas sensor
TODO: Review the correction factor calculation. This currently relies on
the datasheet but the information there seems to be wrong.

@section  HISTORY

v1.0 - First release
*/
/**************************************************************************/
#ifndef MQ135_H
#define MQ135_H

/// Parameters for calculating ppm from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

/// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 412.55 //As of September 2020 https://www.co2.earth/

#endif
