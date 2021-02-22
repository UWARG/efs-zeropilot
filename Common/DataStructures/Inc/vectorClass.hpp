class Vector3D
{
    public: 
        double x,y,z;
        Vector3D();


        Vector3D(double inputX, double inputY, double inputZ);
        

        ~Vector3D()
        {
        }

        Vector3D& operator=(const Vector3D &vector);
       

        Vector3D operator+(const Vector3D &vector2);

        Vector3D operator-(const Vector3D &vector2);

        Vector3D operator*(double coefficient);
        

        double dotProduct(const Vector3D &vector2);
        

        double norm();

};
