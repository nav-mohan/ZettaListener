cmake_minimum_required(VERSION 3.11)

#INSTEAD OF add_executable WE USE add_library TO CREATE "glad.a" LIB (gcc -lglad FLAG)
add_library(glad src/glad.c)

#THE OUTPUT glad.a NEEDS THE KHR/khrplatform.h glad/glad.h HEADER FILES
target_include_directories(glad PUBLIC include/)
