# ECVL - European Computer Vision Library
# Version: 0.1
# copyright (c) 2020, UniversitÓ degli Studi di Modena e Reggio Emilia (UNIMORE), AImageLab
# Authors: 
#    Costantino Grana (costantino.grana@unimore.it)
#    Federico Bolelli (federico.bolelli@unimore.it)
#    Michele Cancilla (michele.cancilla@unimore.it)
#    Laura Canalini (laura.canalini@unimore.it)
#    Stefano Allegretti (stefano.allegretti@unimore.it)
# All rights reserved.

target_sources(ECVL_CORE
	PRIVATE
        image.cpp
        datatype.cpp
        imgcodecs.cpp
        imgproc.cpp
        memorymanager.cpp
        support_opencv.cpp
        support_nifti.cpp
        arithmetic.cpp
)
