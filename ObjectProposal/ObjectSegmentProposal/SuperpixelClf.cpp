#include "SuperpixelClf.h"


SuperpixelClf::SuperpixelClf(void)
{
	// init forest params
	DTreeTrainingParams tparams;
	tparams.feature_num = 500;
	tparams.th_num = 70;
	tparams.min_samp_num = 50;
	tparams.MaxLevel = 8;
	
	rfparams.tree_params = tparams;
	rfparams.num_trees = 6;
	rfparams.split_disjoint = false;

	rf.Init(rfparams);

	rf_model_file = "sp_clf.model";
}


bool SuperpixelClf::Train(DatasetName db_name) {

	map<int, ObjectCategory> gt_objs;

	// load ground truth
	DataManagerInterface* db_man = NULL;
	if(db_name == DB_NYU2_RGBD)
		db_man = new NYUDepth2DataMan;
	else return false;

	FileInfos imgfns, dmapfns;
	db_man->GetImageList(imgfns);
	imgfns.erase(imgfns.begin()+30, imgfns.end());
	db_man->GetDepthmapList(imgfns, dmapfns);

	Feature3D feat3d;
	SegmentProcessor seg_processor;
	ImageSegmentor img_segmentor;
	img_segmentor.m_dMinArea = 100;
	img_segmentor.m_dThresholdK = 30;
	img_segmentor.seg_type_ = OVER_SEG_GRAPH;

	int label_cnt = 0;
	map<int, int> label_map;	// label remapping, make sure start from 0

	for(size_t i=0; i<imgfns.size(); i++) {

		Mat cimg = imread(imgfns[i].filepath);
		Size newsz;
		ToolFactory::compute_downsample_ratio(Size(cimg.cols, cimg.rows), 400, newsz);
		resize(cimg, cimg, newsz);
		Mat dmap;
		db_man->LoadDepthData(dmapfns[i].filepath, dmap);
		resize(dmap, dmap, newsz);
		dmap.convertTo(dmap, CV_32F);

		FileInfos tmp_imgfns;
		tmp_imgfns.push_back(imgfns[i]);
		map<string, vector<VisualObject>> gt_masks;
		db_man->LoadGTMasks(tmp_imgfns, gt_masks);

		seg_processor.Init(cimg, dmap);

		img_segmentor.DoSegmentation(cimg);

		for(auto cur_sp : img_segmentor.superPixels) {
			seg_processor.ExtractSegmentBasicFeatures(cur_sp);
			cur_sp.centroid.x /= cimg.cols;
			cur_sp.centroid.y /= cimg.rows;
			// test if overlap with a gt obj
			for(auto& cur_gt : gt_masks[imgfns[i].filename]) {

				if(cur_gt.visual_desc.mask.rows != newsz.height) resize(cur_gt.visual_desc.mask, cur_gt.visual_desc.mask, newsz);

				//imshow("sp mask", cur_sp.mask*255);
				//imshow("gt mask", cur_gt.visual_desc.mask*255);
				//waitKey(10);
				if(countNonZero(cur_sp.mask & cur_gt.visual_desc.mask)*1.f / cur_sp.area < 0.9f)
					continue;

				// extract features
				seg_processor.ExtractSegmentVisualFeatures(cur_sp, SP_COLOR | SP_NORMAL);

				int cnt = 0;
				Mat total_feat(1, cur_sp.namedFeats["color"].cols+cur_sp.namedFeats["normal"].cols+2, CV_32F);
				for(int id=0; id<cur_sp.namedFeats["color"].cols; id++) 
					total_feat.at<float>(cnt++) = cur_sp.namedFeats["color"].at<float>(id);
				for(int id=0; id<cur_sp.namedFeats["normal"].cols; id++) 
					total_feat.at<float>(cnt++) = cur_sp.namedFeats["normal"].at<float>(id);
				total_feat.at<float>(cnt++) = cur_sp.centroid.x;
				total_feat.at<float>(cnt++) = cur_sp.centroid.y;
				cur_gt.visual_desc.img_desc = total_feat;

				// add to set
				if(label_map.find(cur_gt.category_id) == label_map.end()) label_map[cur_gt.category_id] = label_cnt++;
				gt_objs[label_map[cur_gt.category_id]].objects.push_back(cur_gt);
			}
		}

		cout<<"finished image: "<<i<<"/"<<imgfns.size()<<endl;
	}

	delete db_man;
	db_man = NULL;

	// divide into train and test data
	Mat rank_train_data, rank_test_data, rank_train_label, rank_test_label;
	for (auto pi=gt_objs.begin(); pi!=gt_objs.end(); pi++) {
		for (int r=0; r<pi->second.objects.size(); r++) {
			if(r<pi->second.objects.size()*0.8) {
				rank_train_data.push_back(pi->second.objects[r].visual_desc.img_desc);
				rank_train_label.push_back(pi->first);
			}
			else {
				rank_test_data.push_back(pi->second.objects[r].visual_desc.img_desc);
				rank_test_label.push_back(pi->first);
			}
		}
	}

	std::cout<<"Rank training data ready."<<endl;

	std::cout<<"Start to train..."<<endl;
	ofstream out(rf_model_file);
	rf.Train(rank_train_data, rank_train_label);
	rf.Save(out);
	rf.EvaluateRandomForest(rank_test_data, rank_test_label, label_cnt);

	return true;
}

bool SuperpixelClf::Predict(const Mat& cimg, const Mat& dmap) {

	// visualize results
	// pick two samples from the same class, show their prediction histogram
	/*RNG rng_gen(234242);
	int test1 = (int)rng_gen.uniform(0, gt_objs[3].objects.size());
	int test2 = (int)rng_gen.uniform(0, gt_objs[3].objects.size());
	cout<<test1<<" "<<test2<<endl;
	vector<double> res1, res2;
	rforest.Predict(gt_objs[3].objects[test1].visual_desc.img_desc, res1);
	rforest.Predict(gt_objs[3].objects[test2].visual_desc.img_desc, res2);

	Mat testimg = imread(nyu_cfn);
	Mat testdmap = imread(nyu_dfn, CV_LOAD_IMAGE_UNCHANGED);

	ofstream out1("pred.txt");
	for(size_t i=0; i<res1.size(); i++) out1<<res1[i]<<" ";
	out1<<endl;
	for(size_t i=0; i<res2.size(); i++) out1<<res2[i]<<" ";*/

	return true;
}
