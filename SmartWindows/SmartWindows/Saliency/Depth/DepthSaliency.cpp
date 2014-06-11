#include "DepthSaliency.h"


DepthSaliency::DepthSaliency(void)
{
}

//////////////////////////////////////////////////////////////////////////

double DepthSaliency::CompDepthVariance(const Mat& dmap, ImgWin win)
{
	// assume the dmap has been normalized to 0~255
	cv::Scalar mean, stddev;
	meanStdDev(dmap(win), mean, stddev);
	return (255-mean.val[0]) / (stddev.val[0]+0.000005f);
}

//#define VERBOSE

bool DepthSaliency::CompWinDepthSaliency(const Mat& dmap, ImgWin& win)
{
	// compute center-surround depth difference
	double sal = 0;
	cv::Scalar in_mean, in_stddev;
	meanStdDev(dmap(win), in_mean, in_stddev);
	sal = (255-in_mean.val[0]) / (in_stddev.val[0]+0.000001f);
	
	double context_sal = 0;
	Scalar out_mean, out_stddev;
	ImgWin contextWin = ToolFactory::GetContextWin(dmap.cols, dmap.rows, win, 1.5);

#ifdef VERBOSE
	// draw context win
	vector<ImgWin> drawwins;
	drawwins.push_back(win);
	drawwins.push_back(contextWin);
	visualsearch::ImgVisualizer::DrawImgWins("context", dmap, drawwins);
	waitKey(10);
#endif

	meanStdDev(dmap(contextWin), out_mean, out_stddev);
	context_sal = (255-out_mean.val[0]) / (out_stddev.val[0]+0.000001f);

	double bordermean = (out_mean.val[0]*contextWin.area() - in_mean.val[0]*win.area()) / (contextWin.area()-win.area());
	win.score = fabs(bordermean-in_mean.val[0]) / in_stddev.val[0];
	win.tempvals.push_back(sal);
	win.tempvals.push_back(context_sal);

	//return true;

	//////////////////////////////////////////////////////////////////////////
	// do local normalization first
	double minval, maxval;
	minMaxLoc(dmap(contextWin), &minval, &maxval);
	// center-surround histogram
	int binnum = 10;
	int bin_step = 255 / binnum;
	Mat in_hist(1, binnum, CV_32F);
	in_hist.setTo(0);
	Mat out_hist(1, binnum, CV_32F);
	out_hist.setTo(0);
	for (int r=contextWin.tl().y; r<contextWin.br().y; r++)
	{
		for(int c=contextWin.tl().x; c<contextWin.br().x; c++)
		{
			float dval = dmap.at<float>(r, c);
			// normalize
			//dval = (dval-minval)*255 / (maxval-minval);
			int bin_id = MIN(binnum-1, (int)(dval/bin_step));
			if(win.contains(Point(c, r)))
				in_hist.at<float>(bin_id)++;
			
				out_hist.at<float>(bin_id)++;
		}
	}

	normalize(in_hist, in_hist, 1, 0, NORM_L1);
	normalize(out_hist, out_hist, 1, 0, NORM_L1);
	float contrast = 1 - compareHist(in_hist, out_hist, CV_COMP_INTERSECT);
	float inner_smooth = (255-in_stddev.val[0]) / 255;
	win.score = contrast; //contrast * inner_smooth;

#ifdef VERBOSE
	cout<<"center-surround: "<<contrast<<"; in_stddev: "<<in_stddev.val[0]<<"; in_smooth: "<<inner_smooth<<"; final: "<<win.score<<endl;

	Mat can1, can2;
	ToolFactory::DrawHist(can1, Size(300, 300), 250, in_hist);
	ToolFactory::DrawHist(can2, Size(300, 300), 250, out_hist);
	imshow("inhist", can1);
	imshow("outhist", can2);
	waitKey(0);
#endif
	

	return true;
}

void DepthSaliency::RankWins(const Mat& dmap, vector<ImgWin>& wins)
{
	// normalize
	Mat ndmap;
	normalize(dmap, ndmap, 0, 255, NORM_MINMAX);

	for (size_t i=0; i<wins.size(); i++)
	{
		CompWinDepthSaliency(ndmap, wins[i]);
	}

	sort(wins.begin(), wins.end(), [](const ImgWin& a, const ImgWin& b) { return a.score > b.score; } );

}

bool DepthSaliency::DepthToCloud(const Mat& dmap, Mat& cloud)
{
	// dmap is millimeter

	float coeff = 585.6f;
	float MM_PER_M = 1000;
	Size dmap_sz(dmap.cols, dmap.rows);
	Point centerPt(dmap.cols/2, dmap.rows/2);

	cloud.create(dmap_sz.height, dmap_sz.width, CV_32FC3);
	vector<Mat> cloudchs;
	split(cloud, cloudchs);
	Mat widthrow(1, dmap.cols, CV_32F);
	for(int i=0; i<dmap.cols; i++) widthrow.at<float>(i) = i+1;
	Mat heightcol(dmap.rows, 1, CV_32F);
	for(int i=0; i<dmap.rows; i++) heightcol.at<float>(i) = i+1;
	Mat onecol(dmap.rows, 1, CV_32F);
	onecol.setTo(1);
	Mat onerow(1, dmap.cols, CV_32F);
	onerow.setTo(1);
	Mat xgrid = onecol * widthrow - centerPt.y;
	Mat ygrid = heightcol * onerow - centerPt.x;
	cloudchs[0] = xgrid.mul(dmap) / coeff / MM_PER_M;
	cloudchs[1] = ygrid.mul(dmap) / coeff / MM_PER_M;
	cloudchs[2] = dmap / MM_PER_M;
	merge(cloudchs, cloud);

	return true;
}

bool DepthSaliency::OutputToOBJ(const Mat& cloud, string objfile)
{
	ofstream out(objfile);
	for (int r=0; r<cloud.rows; r++)
	{
		for(int c=0; c<cloud.cols; c++)
		{
			Vec3f curpt = cloud.at<Vec3f>(r, c);
			out<<"v "<<curpt.val[0]<<" "<<curpt.val[1]<<" "<<curpt.val[2]<<endl;
		}
	}

	return true;
}