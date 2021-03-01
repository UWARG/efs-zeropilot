/*
    Author: Nixon Chan
*/

#ifndef VECTORCLASS_HPP
#define VECTORCLASS_HPP

class Vector3D
{
    public: 

        double x,y,z;

        Vector3D();

        Vector3D(double inputX, double inputY, double inputZ);
        
        ~Vector3D();

        Vector3D operator=(const Vector3D &vector);
       
        Vector3D operator+(const Vector3D &vector2);

        Vector3D operator-(const Vector3D &vector2);

        Vector3D operator*(double coefficient);

        Vector3D crossProduct(const Vector3D &vector2);
        
        double dotProduct(const Vector3D &vector2);
        
        double norm();

};

#endif
