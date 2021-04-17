#pragma once

#ifndef TRACK_FINDER_HPP
#define TRACK_FINDER_HPP

/**
    The goal of this function is to return the current track of the plane in degrees, given its velocities in the latitude and longitude direction in m/s.

    * @param[in] latitudeSpeed -> load in the speed of the plane in the N/S direction (change in latitude, m/s)
    * @param[in] longitudeSpeed -> load in the speed of the plane in the E/W direction (change in longitude, m/s)
    * @return double - > returns the direction of the resulting velocity/track of the plane
*/
double getTrack(double latitudeSpeed, double longitudeSpeed);

#endif
