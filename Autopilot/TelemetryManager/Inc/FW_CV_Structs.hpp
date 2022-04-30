#ifndef FW_CV_STRUCTS_HPP
#define FW_CV_STRUCTS_HPP
//Creates the fijo and foji struct

//Should be 36 bytes but because of padding its really 40 bytes
//Flight Controller Out Jetson In
 struct foji{
	double lattitude;
	double longtitude;
	double altitude;
	float yaw;
	float pitch;
	float roll;
};

//Should be 19 bytes but because of padding its really 24 bytes
//Flight Controller In Jetson Out
struct fijo{
	bool detectFlag;
	bool qrScanFlag;
	bool takeoffCommand;
	double lattitude;
	double longtitude;
};

#endif