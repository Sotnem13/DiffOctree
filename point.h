//
// Created by sotnem on 13.04.17.
//

#ifndef OCTEE_POINT_H
#define OCTEE_POINT_H


#include <array>
#include <cmath>

template <typename T>
struct Point {
    union {
        struct { T x, y, z; };
        std::array<T,3> coords;
    };
    inline T volume(){
        return x*y*z;
    }

    template <typename T1>
    inline Point<T> operator=(Point<T1> p){
        for(int i = 0; i < 3; i++)
            coords[i] = (T)p.coords[i];
    };

    template <typename T1>
    inline Point<T> operator+=(Point<T1> p){
        for(int i = 0; i < 3; i++)
            coords[i] += (T)p.coords[i];
    };

    // computes L2 norm of input array
    T normal(){ return std::sqrt(x*x + y*y + z*z); };


    // returns distance (L2 norm) between two positions in 3D
    T getL2Distance(const Point<T>& v) { return ((*this) - v).normal(); }


    static Point<T> random(){
        T x = rand() * 1.f / RAND_MAX;
        T y = rand() * 1.f / RAND_MAX;
        T z = rand() * 1.f / RAND_MAX;
        return {x,y,z};
    }


};

template <typename T>
bool operator < (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] < rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
bool operator > (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] > rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
bool operator <= (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] <= rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
bool operator >= (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] >= rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
bool operator == (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] == rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
bool operator != (Point<T> lhs,Point<T> rhs){
    for( int i = 0; i < lhs.coords.size(); i++){
        if(!(lhs.coords[i] != rhs.coords[i]) ) return false;
    }
    return true;
};

template <typename T>
Point<T> operator+(Point<T> lhs,Point<T> rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] + rhs.coords[i];
    }
    return result;
};

template <typename T>
Point<T> operator-(Point<T> lhs,Point<T> rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] - rhs.coords[i];
    }
    return result;
};

template <typename T>
Point<T> operator/(Point<T> lhs,Point<T> rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] / rhs.coords[i];
    }
    return result;
};

template <typename T, typename T1>
Point<T> operator/(Point<T> lhs,T1 rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] / rhs;
    }
    return result;
};

template <typename T, typename T1>
Point<T> operator*(Point<T> lhs,Point<T1> rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] * rhs.coords[i];
    }
    return result;
};
template <typename T, typename T1>
Point<T> operator*(Point<T> lhs,T1 rhs){
    Point<T> result;
    for( int i = 0; i < lhs.coords.size(); i++){
        result.coords[i] = lhs.coords[i] * rhs;
    }
    return result;
};

template <typename T, typename T1>
Point<T> operator*(T1 lhs,Point<T> rhs){
    Point<T> result;
    for( int i = 0; i < rhs.coords.size(); i++){
        result.coords[i] = lhs * rhs.coords[i];
    }
    return result;
};

#endif //OCTEE_POINT_H
