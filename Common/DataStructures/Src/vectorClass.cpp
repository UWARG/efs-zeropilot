#include <math.h>
#include "iostream"
#include "vectorClass.hpp"

using namespace std;
int main()
{
    Vector3D one(1,1,1);
    Vector3D two(2,2,2);
    cout << (one+two).x <<(one+two).y<<(one+two).z<<endl;
    cout <<(one-two).x <<(one-two).y<<(one-two).z<<endl;
    cout << (one*10).x << (one*10).y <<(one*10).z<<endl;
    cout << one.dotProduct(two);
    cout << one.norm();
    one = two;
    cout<< one.x<<one.y<<one.z;
    return EXIT_SUCCESS;
}
class Vector3D
{
    public:
        double x,y,z;

        Vector3D()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        Vector3D(double inputX, double inputY, double inputZ)
        {
            x = inputX;
            y = inputY;
            z = inputZ;
        }

        ~Vector3D()
        {
        }

        Vector3D& operator=(const Vector3D &vector)
        {
            x = vector.x;
            y = vector.y;
            z = vector.z;
            return *this;
        }

        Vector3D operator+(const Vector3D &vector2)
        {
            Vector3D returnVector;
            returnVector.x = (*this).x + vector2.x;
            returnVector.y = (*this).y + vector2.y;
            returnVector.z = (*this).z + vector2.z;
            return returnVector;
        }

        Vector3D operator-(const Vector3D &vector2)
        {
            Vector3D returnVector;
            returnVector.x = (*this).x - vector2.x;
            returnVector.y = (*this).y - vector2.y;
            returnVector.z = (*this).z - vector2.z;
            return returnVector;
        }

        Vector3D operator*(double coefficient)
        {
            (*this).x *= coefficient;
            (*this).y *= coefficient;
            (*this).z *= coefficient;
        }

        double dotProduct(const Vector3D &vector2)
        {
            return ((*this).x * vector2.x + (*this).y * vector2.y + (*this).z * vector2.z);
        }

        double norm()
        {
            return sqrt(pow((*this).x,2) + pow((*this).y,2) + pow((*this).z,2));
        }

};

