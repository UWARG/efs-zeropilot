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
    Vector3D vector(1,1,1);
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 1);
    EXPECT_EQ(vector.getY(), 1);
    EXPECT_EQ(vector.getZ(), 1);
}

TEST(VectorClassTest, SettersAndGetters){
    /***SETUP***/
    Vector3D vector;
    vector.setX(5);
    vector.setY(5);
    vector.setZ(5);
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 5);
    EXPECT_EQ(vector.getY(), 5);
    EXPECT_EQ(vector.getZ(), 5);
}

TEST(VectorClassTest, AssignmentOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,2,2);
    vector = vector2;
    /***ASSERTS***/
    EXPECT_EQ(vector.getX(), 2);
    EXPECT_EQ(vector.getY(), 2);
    EXPECT_EQ(vector.getZ(), 2);
}

TEST(VectorClassTest, AdditionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,2,2);
    Vector3D result = vector + vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 3);
    EXPECT_EQ(result.getY(), 3);
    EXPECT_EQ(result.getZ(), 3);
}

TEST(VectorClassTest, SubtractionOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,2,2);
    Vector3D result = vector - vector2;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), -1);
    EXPECT_EQ(result.getY(), -1);
    EXPECT_EQ(result.getZ(), -1);
}

TEST(VectorClassTest, MultiplicationOperator){
    /***SETUP***/
    Vector3D vector(1,1,1);
    double constant = 10;
    Vector3D result = vector * constant;
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 10);
    EXPECT_EQ(result.getY(), 10);
    EXPECT_EQ(result.getZ(), 10);
}

TEST(VectorClassTest, CrossProduct){
    /***SETUP***/
    Vector3D vector(1,1,1);
    Vector3D vector2(2,2,2);
    Vector3D result = vector.crossProduct(vector2);
    /***ASSERTS***/
    EXPECT_EQ(result.getX(), 0);
    EXPECT_EQ(result.getY(), 0);
    EXPECT_EQ(result.getZ(), 0);
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
