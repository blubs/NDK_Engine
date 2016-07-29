//
// Created by F1 on 3/29/2016.
//

#ifndef ENGINE_MISC_H
#define ENGINE_MISC_H

#include "common.hpp"

//Sets start_time as current time for later usage as a reference point by time()
void set_start_time();

//Returns time since start_time as a float
float time();

//Prints the elements of a 4x4 matrix
void print_mat4(float* mat);

//Prints the elements of a 3x3 matrix
void print_mat3(float* mat);


//Unused, refer to function def as to why
//float fast_inv_sqrt(float num);


#endif //ENGINE_MISC_H
