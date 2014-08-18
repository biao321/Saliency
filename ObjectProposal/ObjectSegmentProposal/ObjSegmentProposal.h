//////////////////////////////////////////////////////////////////////////
// main proposal class
// jiefeng@2014-08-15
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "IterativeSegmentor.h"
#include "ObjectRanker.h"

namespace objectproposal
{
	class ObjSegmentProposal
	{
	private:
		IterativeSegmentor iter_segmentor;
		visualsearch::processors::attention::ObjectRanker seg_ranker;


	public:
		ObjSegmentProposal(void);

		bool Run(const Mat& cimg, const Mat& dmap, int topK, vector<SuperPixel>& res);

		//////////////////////////////////////////////////////////////////////////

		bool VisProposals(const Mat& cimg, const vector<SuperPixel>& res);
	};
}



