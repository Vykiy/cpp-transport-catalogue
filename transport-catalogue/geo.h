#pragma once

#include <cmath>
#include <numeric>

inline bool IsEqualDouble(double dbl, double otherDbl) {
    double eps = 0.0000001;
    return (fabs(dbl - otherDbl) <= eps);
}

struct Coordinates {
    double lat; //latitude (широта) 
    double lng; //longitude (долгота) 
    bool operator==(const Coordinates &other) const {
        return (IsEqualDouble(lat, other.lat) && IsEqualDouble(lng, other.lng));
    }

    bool operator!=(const Coordinates &other) const {
        return !(*this == other);
    }
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    const double radius_earth = 6371000;
    if (from == to) {
        return 0;
    }
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
           * radius_earth;
}