#include "Detector.h"



CDetector::~CDetector()
{
}

CDetector::CDetector(const char* model_name) : seeta::FaceDetection(model_name)
{
	this->SetMinFaceSize(40);
	this->SetScoreThresh(2.f);
	this->SetImagePyramidScaleFactor(0.8f);
	this->SetWindowStep(4, 4);
}
