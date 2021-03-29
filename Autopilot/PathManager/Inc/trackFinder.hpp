#pragma once


/*
    The goal of this function is to return the current track of the plane, given its velocities in the latitude and longitude direction.
    Given the speeds of the aircraft in the latitude and longitude directions in m/s, this function will output the track in degrees.
*/

double getTrack(double latitudeSpeed, double longitudeSpeed);

