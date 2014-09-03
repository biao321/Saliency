//////////////////////////////////////////////////////////////////////////
// tool function for rgbd video object segmentation
// jiefeng@2014-1-4
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>


namespace visualsearch
{
	namespace common
	{
		namespace tools
		{

			class RGBDTools
			{
			public:

				RGBDTools() {}

				//////////////////////////////////////////////////////////////////////////
				// I/O

				static bool OutputMaskToFile(std::ofstream& out, const cv::Mat& color_img, const cv::Mat& mask, bool hasProb = false);

				static bool LoadBinaryDepthmap(const std::string& filename, cv::Mat& dmap, int w, int h);

				static bool LoadMat(const std::string& filename, cv::Mat& rmat, int w, int h);

				//////////////////////////////////////////////////////////////////////////
				// visualization

				//////////////////////////////////////////////////////////////////////////
				// transformation

				static bool Proj2Dto3D(const cv::Mat& fg_mask, const cv::Mat& dmap, const cv::Mat& w2c_mat, std::vector<cv::Vec3f>& pts3d);
			};
		}
	}
	
}