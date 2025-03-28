#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <Eigen/Eigen>
using namespace Eigen;
using namespace std;

#define LINE_LEN 100

#define X_dir 0
#define Y_dir 1
#define Z_dir 2

#define Left_side 0
#define Right_side 1

#define ENDSTR ");"

enum BOUNDARY_TYPE
{
    REFLECTIVE,
    VACUUM
};

#endif
