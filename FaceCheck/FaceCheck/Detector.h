#pragma once
#include "face_detection.h"
class CDetector: public seeta::FaceDetection
{
public:
	~CDetector();
	CDetector(const char * model_name);
};

