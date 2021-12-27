// MultiCamsRecording.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <thread> 
#include <chrono>
#include <cstdlib>
#include <time.h>
#include<list>


using namespace cv;
using namespace std;
using namespace std::chrono;

#include "webCam.h"

int main(int argc, char* argv[])
{
	__time64_t t_begin0;
	struct tm curr_tm;
	char timebuff[50];
	_time64(&t_begin0);
	_localtime64_s(&curr_tm, &t_begin0);
	strftime(timebuff, sizeof(timebuff), "%Y%m%d_%H%M%S", &curr_tm);


	// openning cameras iteratively
	int camId = 0;
	webCam wcam(camId);
	wcam.openCam();
	string filename_prefix = "video_" + string(timebuff) + "_camera";
	string outFilename = filename_prefix + to_string(wcam.camID) + ".avi";
	wcam.capturing = true;
	wcam.captureFrame(outFilename + ".avi", outFilename + ".csv");
	/*std::list<webCam> wcams;
	while (true)
	{
		webCam wcam(camId);
		if (!wcam.openCam())
		{
			cout << "There are " << camId << " cameras opened!" << endl;
			break;
		}
		wcams.push_back(wcam);
		camId++;
	}
	

	// open each capture Frame thread
	string filename_prefix = "video_" + string(timebuff) + "_camera";

	std::list<webCam>::iterator it;
	for (it = wcams.begin(); it != wcams.end(); it++)
	{
		string outFilename = filename_prefix + to_string(it->camID) + ".avi";
		it->captureFrame(outFilename + ".avi", outFilename + ".csv");
	}*/


	return 0;
}
