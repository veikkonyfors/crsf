cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (crsf
	VERSION 0.1
	DESCRIPTION "Lib for crossfire functions")

# This project will output alibrary
add_library(crsf STATIC crsf.c)

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
