/*
 * tester.cpp
 *
 *  Created on: 19 May 2016
 *      Author: junior
 */

#include "hdbscan.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/optflow.hpp>
#include <chrono>

using namespace std;
using namespace clustering;
using namespace cv;
using namespace std::chrono;
//using namespace cv::ocl;
#define edgeThresh 30

class HDBSCANStarParameters {
	string inputFile;
	string constraintsFile;
	int minPoints;
	int minClusterSize;
	bool compactHierarchy;
	//DistanceCalculator distanceFunction(EUCLIDEAN);

	string hierarchyFile;
	string clusterTreeFile;
	string partitionFile;
	string outlierScoreFile;
	string visualizationFile;
};

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
UMat _prev, prevgray, fdiff, 	// previous two frames
		 diff, 	// previous image differences
		 edge, 	// previous edge difference edges
		 //fEdge[2],
		 flow, cflow, uflow,		// Optical flow as a colour image
		 frame,		// image version of flow
		gray,	// previous optical flows
		 image,		// the current image
		 cImage,	// contour image
		 candidates,// candidates computed from the contours
		 iEdge;		// edges for the current image

vector<vector<double> >* prepareHDBSCANData(int idx) {
	vector<Point> contour = contours[idx];
	vector<vector<double> >* h = new vector<vector<double> >();

	for (unsigned int i = 0; i < contour.size() && !flow.empty(); i++) {
		Point2f p = flow.getMat(ACCESS_RW).at<Point2f>(contour[i].y, contour[i].x);
		vector<double>* np = new vector<double>();

		np->push_back(contour[i].x);
		np->push_back(contour[i].y);
		np->push_back(p.x);
		np->push_back(p.y);
		//printf("Internal size is %d : %d\n", i, np->size());
		h->push_back(*np);
	}

	//printf("Final size is %d\n", h->size());

	return h;
}

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                    double, const Scalar& color)
{
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
}

vector<vector<Point> >* getSegmentedContours(vector<int>* c, int idx){
	vector<Point> contour = contours[idx];

	set<int> s;

	for(uint i = 0; i < c->size(); i++){
		s.insert((*c)[i]);
	}

	// the new segemented contour
	vector<vector<Point> >* cs = new vector<vector<Point> >();

	if(s.size() == 1){ //If we have only one label there is no need to traverse
		cs->push_back(contour);
	} else{
		for(set<int>::iterator it = s.begin(); it != s.end(); ++it){
			vector<Point>* _c = new vector<Point>();
			for(uint i = 0; i < c->size(); i++){

				if((*c)[i] == *it){
					_c->push_back(contour[i]);
				}

			}

			cs->push_back(*_c);
		}
	}

	return cs;
}

int segmentContours() {
	vector<vector<Point> > *new_contours = new vector<vector<Point> >(); // new segments contours
	for (uint i = 0; i < contours.size(); i++) {
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		vector<vector<double> >* contour = prepareHDBSCANData(i);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>( t2 - t1 ).count();

		cout << "prepareHDBSCANData time : " << duration << endl;
		/**
		 * Cluster the contours
		 */
		HDBSCAN scan(contour, _EUCLIDEAN, 4, 4);
		scan.run(true);
		vector<vector<Point> > s_contours = *getSegmentedContours(scan.getClusterLabels(), i);
		new_contours->insert(new_contours->end(), s_contours.begin(), s_contours.end());
	}

	// Set contours to the new segmented contours
	std::swap(contours, *new_contours);

	return 0;
}

int main(int argc, char** argv) {

	ocl::setUseOpenCL(true);
    /*cv::CommandLineParser parser(argc, argv, "{help h||}");
    if (parser.has("help"))
    {
        //help();
        return 0;
    }*/

    string filename = "/home/junior/Dropbox/School/phd/code/resources/starlings.mp4";

    VideoCapture cap(argv[1]);
    /*if(argc == 0){
    	VideoCapture c(0);
    	cap = c;
    } else{
    	printf(argv[0]);
    	VideoCapture c(argv[1]);
    	cap = c;
    }*/
    //help();
    if( !cap.isOpened() ){
        printf("Could not open stream\n");
    	return -1;
    }
    printf("creating named window\n");
    namedWindow("flow", 1);
    printf("created named window\n");

    Ptr<DenseOpticalFlow> algorithm;
    printf("optical flow algorithm declaration\n");
    algorithm = optflow::createOptFlow_Farneback();
    printf("Created farneback algorithm calculator\n");

    for(;;)
    {

        cap >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        if( !prevgray.empty() )
        {
			absdiff(frame, _prev, fdiff);
        	//void calcOpticalFlowFarneback(InputArray prev,
        	//								InputArray next,
        	//								InputOutputArray flow,
        	//								double pyr_scale, int levels,
        	//								int winsize, int iterations,
        	//								int poly_n, double poly_sigma,
        	//								int flags)
            /*calcOpticalFlowFarneback(prevgray,
            						gray,
            						uflow,
            						0.7, 10,
            						50, 10,
            						9, 1.2,
            						0);*/

            algorithm->calc(gray, prevgray, uflow);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            uflow.copyTo(flow);

            Canny(gray, edge, edgeThresh, edgeThresh * 3, 3);
            findContours(edge, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE,
        			Point(0, 0));
        	if (!flow.empty()) {
        		segmentContours();
        	}

            Mat m1 = flow.getMat(ACCESS_READ);
            Mat m2 = cflow.getMat(ACCESS_READ);

            drawOptFlowMap(m1, m2, 16, 1.5, Scalar(0, 255, 0));
            imshow("flow", cflow);
        }
        if(waitKey(30)>=0)
            break;
        std::swap(prevgray, gray);
        std::swap(_prev, frame);
    }
	//Parse input parameters from program arguments:
	//HDBSCANStarParameters parameters; // = checkInputParameters(args);

	/*System.out.println(
	 "Running HDBSCAN* on " + parameters.inputFile + " with minPts="
	 + parameters.minPoints + ", minClSize="
	 + parameters.minClusterSize + ", constraints="
	 + parameters.constraintsFile + ", compact="
	 + parameters.compactHierarchy + ", dist_function="
	 + parameters.distanceFunction.getName());
	 */
	//if(argc < 2){
	//	printf("Missing argument : example ./hdbscan myfile.csv\n\n");

	//	return -1;
	//}
	//Read in input file:
	//HDBSCAN hdbscan(argv[1], EUCLIDEAN, 3, 3);

	//Read in constraints:
	//hdbscan.readInConstraints("example_constraints.csv");

	/**hdbscan.run(true);

	vector<Cluster*> clusters = *(hdbscan.getClusters());
	printf("Found %d labels.\n[", clusters.size());

	vector<int>* labels = hdbscan.getClusterLabels();

	for(int i = 0; i < labels->size() - 1; i++){
		printf("%d, ", labels[0][i]);
	}

	printf("%d]\n", labels[0][labels->size() - 1]);

	*/



	return 0;
}

