/**
 Implementation of the DSHOT protocol...
 ect. add more detial on what dshot is here.
 */

#ifndef DSHOT_HPP
#define	DSHOT_HPP

struct DataFrame {
    unsigned int throttle: 11;
    bool telemetryRequest: 1;
    unsigned int CRC:      4;
};


#endif /* DSHOT_HPP */
