#include <gtest/gtest.h>
#include "math.h"
#include "vectorClass.hpp"


using namespace std; 
using ::testing::Test;

TEST(VectorClassTest, DefaultConstructor){
    /***SETUP***/
    Vector3D vector;
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 0);
    EXPECT_EQ(vector.getY(), 0);
    EXPECT_EQ(vector.getZ(), 0);
}

TEST(VectorClassTest, DefinedConstructor){
    /***SETUP***/
    Vector3D vector(1,2,3);
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 1);
    EXPECT_EQ(vector.getY(), 2);
    EXPECT_EQ(vector.getZ(), 3);
}

TEST(VectorClassTest, SettersAndGetters){
    /***SETUP***/
    Vector3D vector;
    vector.setX(1);
    vector.setY(2);
    vector.setZ(3);
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 1);
    EXPECT_EQ(vector.getY(), 2);
    EXPECT_EQ(vector.getZ(), 3);
}

TEST(VectorClassTest, AssignmentOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(3,4,5);
    vector = vector2;
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 3);
    EXPECT_EQ(vector.getY(), 4);
    EXPECT_EQ(vector.getZ(), 5);
}

TEST(VectorClassTest, AdditionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(1,2,3);
    Vector3D result = vector + vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 2);
    EXPECT_EQ(result.getY(), 3);
    EXPECT_EQ(result.getZ(), 4);
}

TEST(VectorClassTest, SubtractionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,3,4);
    Vector3D result = vector - vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), -1);
    EXPECT_EQ(result.getY(), -2);
    EXPECT_EQ(result.getZ(), -3);
}

TEST(VectorClassTest, MultiplicationOperator){
    /***SETUP***/
    Vector3D vector(1,2,3);
    double constant = 10;
    Vector3D result = vector * constant;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 10);
    EXPECT_EQ(result.getY(), 20);
    EXPECT_EQ(result.getZ(), 30);
}

TEST(VectorClassTest, CrossProduct){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(1,2,3);
    Vector3D result = vector.crossProduct(vector2);
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 1);
    EXPECT_EQ(result.getY(), -2);
    EXPECT_EQ(result.getZ(), 1);
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
    EXPECT_EQ(result, sqrt(3));
}
