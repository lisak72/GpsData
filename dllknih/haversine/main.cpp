#include "math.h"
#include <stdexcept>
#include <iostream>

//Jiri Liska, PrF JcU, 2015
//knihovna pouzivajici haversine funkce pro vypocet vzdalenosti bodu podle souradnic
// na geosfere

using namespace std;

#define R 6371000 //polomer Zeme v m
#define TO_RAD (3.1415926536 / 180) // stupne na radiany

extern "C"
{


double __declspec(dllexport) havdistance(double lat1, double lon1, double lat2, double lon2) {
    // funkce vrati vzdalenost v m 
    //exportovany nazev je havdistance
    //poradi zadavani latitude(+N, -S), longitude(+E, -W)
    // from http://rosettacode.org/wiki/Haversine_formula
    
	double dx, dy, dz;
	lon1 -= lon2;
	lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;
 
	dz = sin(lat1) - sin(lat2);
	dx = cos(lon1) * cos(lat1) - cos(lat2);
	dy = sin(lon1) * cos(lat1);
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
}

void __declspec(dllexport) midpoint(double th1, double ph1, double th2, double ph2, double &lat3, double &lon3) {
    // Input values as degrees
	//predava se midpoint(latit1,longit1,latit2,longit2,latitmid, longitmid)
    //midpoint bod na puli cesty
    //exportovany nazev je midpoint
    //parametry: souradnice 1. bodu, souradnice 2. bodu, odkaz na lat a lon mezibodu
    //pocitaci cast prevzata z http://code.activestate.com/recipes/577713-midpoint-of-two-gps-points/
    //z pythonu do cpp
    //musi se vratit 2 hodnoty (latmid, lonmid)
    // opet konverze na radiany a zpet
    double lat1, lon1, lat2, lon2, bx, by;
     lat1 = th1 * TO_RAD; lon1 = ph1 * TO_RAD; lat2= th2 * TO_RAD; lon2= ph2 * TO_RAD; 
     
    
     
    bx = cos(lat2) * cos(lon2 - lon1);
    by = cos(lat2) * sin(lon2 - lon1);
    lat3 = atan2(sin(lat1) + sin(lat2), sqrt((cos(lat1) + bx) * (cos(lat1)+ bx) + pow(by,2)));
    lon3 = lon1 + atan2(by, cos(lat1) + bx);
    lat3 /= TO_RAD; //prevedeni zpet z rad na stupne
    lon3 /= TO_RAD;
    //atan2 je atan2, atan ze 2 argumentu, v cpp stejne jako v pythonu
        
}

} // tady konci extern "C"

/* //dll nema main
int main(int argc, char **argv)
{
    return 0;
}
*/