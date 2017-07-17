#pragma once
#include <string>
#include<iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "face_detection.h"
#include "face_alignment.h"
#include "face_identification.h"
#include "Detector.h"

using std::string;

class CSeetaFace
{
public:
	CSeetaFace();
	~CSeetaFace();

	CDetector* detector;
	seeta::FaceAlignment* point_detector;
	seeta::FaceIdentification* face_recognizer;
	bool GetFeature(string filename, float* feat);
	float* NewFeatureBuffer();
	float FeatureCompare(float* feat1, float* feat2);
	int GetFeatureDims();
};

