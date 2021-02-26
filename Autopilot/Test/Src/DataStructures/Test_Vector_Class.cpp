#include <gtest/gtest.h>
#include <cmath>
#include "vectorClass.hpp"


using namespace std; 
using ::testing::Test;

TEST(VectorClassTest, DefaultConstructor){
    /***SETUP***/
    Vector3D vector;
    /***ASSERTS***/
    EXPECT_EQ(vector.x, 0);
    EXPECT_EQ(vector.y, 0);
    EXPECT_EQ(vector.z, 0);
}

TEST(VectorClassTest, DefinedConstructor){
    /***SETUP***/
    Vector3D vector(1,2,3);
    /***ASSERTS***/
    EXPECT_EQ(vector.x, 1);
    EXPECT_EQ(vector.y, 2);
    EXPECT_EQ(vector.z, 3);
}

TEST(VectorClassTest, AssignmentOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(3,4,5);
    vector = vector2;
    /***ASSERTS***/
    EXPECT_EQ(vector.x, 3);
    EXPECT_EQ(vector.y, 4);
    EXPECT_EQ(vector.z, 5);
}

TEST(VectorClassTest, AdditionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(1,2,3);
    Vector3D result = vector + vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.x, 2);
    EXPECT_EQ(result.y, 3);
    EXPECT_EQ(result.z, 4);
}

TEST(VectorClassTest, SubtractionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,3,4);
    Vector3D result = vector - vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.x, -1);
    EXPECT_EQ(result.y, -2);
    EXPECT_EQ(result.z, -3);
}

TEST(VectorClassTest, MultiplicationOperator){
    /***SETUP***/
    Vector3D vector(1,2,3);
    double constant = 10;
    Vector3D result = vector * constant;
    /***ASSERTS***/
    EXPECT_EQ(result.x, 10);
    EXPECT_EQ(result.y, 20);
    EXPECT_EQ(result.z, 30);
}

TEST(VectorClassTest, CrossProduct){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(1,2,3);
    Vector3D result = vector.crossProduct(vector2);
    /***ASSERTS***/
    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, -2);
    EXPECT_EQ(result.z, 1);
}

TEST(VectorClassTest, DotProduct){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,2,2);
    double result = vector.dotProduct(vector2);
    /***ASSERTS***/
    EXPECT_EQ(result, 6);
}

TEST(VectorClassTest, Norm){
    /***SETUP***/
    Vector3D vector(1,1,1);
    double result = vector.norm();
    /***ASSERTS***/
    EXPECT_EQ(result, std::sqrt(3));
}
