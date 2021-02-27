/* 
    Author: Nixon Chan
*/

#include <cmath>
#include "vectorClass.hpp"

Vector3D::Vector3D()
{
    x = 0;
    y = 0;
    z = 0;
}

Vector3D::Vector3D(double inputX, double inputY, double inputZ)
{
    x = inputX;
    y = inputY;
    z = inputZ;
}

Vector3D::~Vector3D()
{
}

Vector3D Vector3D::operator=(const Vector3D &vector2)
{
    this->x = vector2.x;
    this->y = vector2.y;
    this->z = vector2.z;
    return *this;
}

Vector3D Vector3D::operator+(const Vector3D &vector2)
{
    Vector3D returnVector;
    returnVector.x = this->x + vector2.x;
    returnVector.y = this->y + vector2.y;
    returnVector.z = this->z + vector2.z;
    return returnVector;
}

Vector3D Vector3D::operator-(const Vector3D &vector2)
{
    Vector3D returnVector;
    returnVector.x = this->x - vector2.x;
    returnVector.y = this->y - vector2.y;
    returnVector.z = this->z - vector2.z;
    return returnVector;
}

Vector3D Vector3D::operator*(double coefficient)
{
    Vector3D returnVector;
    returnVector.x = this->x*coefficient;
    returnVector.y = this->y*coefficient;
    returnVector.z = this->z*coefficient;
    return returnVector;
}

Vector3D Vector3D::crossProduct(const Vector3D &vector2)
{
    Vector3D returnVector;
    returnVector.x = this->y * vector2.z - this->z * vector2.y;
    returnVector.y = this->z * vector2.x - this->x * vector2.z; //flipped negative
    returnVector.z = this->x * vector2.y - this->y * vector2.x;       
    return returnVector;     
}

double Vector3D::dotProduct(const Vector3D &vector2)
{
    return (this->x * vector2.x + this->y * vector2.y + this->z * vector2.z);
}

double Vector3D::norm()
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
