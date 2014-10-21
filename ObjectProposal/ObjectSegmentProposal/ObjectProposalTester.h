//////////////////////////////////////////////////////////////////////////
// various testing function: higher level task
// jiefeng@2014-10-04
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "ObjectRanker.h"
#include "Segmentor3D.h"
#include "ObjProposalDemo.h"
#include "Common/common_libs.h"
#include "IO/Dataset/RGBDECCV14.h"
#include "IO/Dataset/Berkeley3DDataManager.h"
#include "Processors/Segmentation/SegmentProcessor.h"
#include "ObjSegmentProposal.h"

using namespace visualsearch;
using namespace visualsearch::io;
using namespace visualsearch::processors;

class ObjectProposalTester {

public:
	ObjectProposalTester() {
		nyu_cfn = "E:\\Datasets\\RGBD_Dataset\\NYU\\Depth2\\211.jpg";
		nyu_dfn = "E:\\Datasets\\RGBD_Dataset\\NYU\\Depth2\\211_d.png";
		uw_cfn = "E:\\Datasets\\RGBD_Dataset\\UW\\rgbd-scene-dataset1\\meeting_small\\meeting_small_1\\";
		uw_dfn = "E:\\Datasets\\RGBD_Dataset\\UW\\rgbd-scene-dataset1\\meeting_small\\meeting_small_1\\";
		uw_gt_dir = "E:\\Datasets\\RGBD_Dataset\\UW\\rgbd-scene-dataset1\\meeting_small\\gt\\";
		eccv_cfn = "E:\\Datasets\\RGBD_Dataset\\Saliency\\RGB\\8_08-34-01.jpg";
		eccv_dfn = "E:\\Datasets\\RGBD_Dataset\\Saliency\\Depth\\smoothedDepth\\8_08-34-01_Depth.png";

		save_dir = "E:\\res\\segments\\meeting\\";
	}

	void TestRankerLearner();

	void BatchProposal();

	void TestSegmentor3D();

	void TestBoundaryClf(bool ifTrain);

	void EvaluateOnDataset(DatasetName db_name);

	void Random();

private:

	string nyu_cfn;
	string nyu_dfn;
	string uw_cfn;
	string uw_dfn;
	string uw_gt_dir;
	string eccv_cfn;
	string eccv_dfn;
	string save_dir;

};

//////////////////////////////////////////////////////////////////////////

