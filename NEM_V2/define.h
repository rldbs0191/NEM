#ifndef _DEFINE_H_  
#define _DEFINE_H_  

#include <iostream>  
#include <fstream>  
#include <sstream>
#include <string>  
#include <iomanip>  
#include <vector>
#include <cmath>

using namespace std;  

#define NU 1.0;
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
