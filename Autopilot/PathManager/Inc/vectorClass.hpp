class Vector3D
{
    public: 
        double x,y,z;

        Vector3D();

        Vector3D(double inputX, double inputY, double inputZ);
        
        ~Vector3D();

        Vector3D& operator=(const Vector3D &vector2); //assigns the vector on the rhs to the lhs

        Vector3D operator+(const Vector3D &vector2); //returns a sum vector

        Vector3D operator-(const Vector3D &vector2); //returns a difference vector

        Vector3D operator*(double coefficient); //returns a multiple vector

        Vector3D crossProduct(const Vector3D &vector2); //returns the cross product of two vectors
    
        double dotProduct(const Vector3D &vector2); //returns the dot product
    
        double norm(); //returns the norm

};
