//////////////////////////////////////////////////////////////////////////
// tester class
// jiefeng 2014-7-26
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "FixationSegmentor.h"
#include "ViewSearcher.h"
#include "ImgVisualizer.h"
#include "Kinect/KinectDataMan.h"

class Tester
{
public:
	Tester(void);

	void TestFixationSegmentation();

	void TestViewSearch();

	void TestKinectStream();

};

