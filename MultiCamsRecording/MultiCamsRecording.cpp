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

/*int main(int argc, char* argv[])
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
	string outFilename = filename_prefix + to_string(wcam.camID);
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


//	return 0;
//}


int showSaveCamStream(int index)
{
	cout << "Initing Camera " << index << "...." << endl;
	VideoCapture cap(index, cv::CAP_DSHOW);
	if (!cap.isOpened())
	{
		cout << "Could not open camera " << index << endl;
		return -1;
	}

	Mat frame;
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');

	cap.set(CAP_PROP_FOURCC, fourcc);

	// prefix of file name for both.aviand .csv files
	__time64_t long_time;
	struct tm curr_tm;
	char timebuff[50];

	_time64(&long_time);
	_localtime64_s(&curr_tm, &long_time);

	strftime(timebuff, sizeof(timebuff), "%Y%m%d_%H%M%S", &curr_tm);


	string filename_prefix = "video_" + string(timebuff) + "_camera" + to_string(index);


	const string showWinName = "cam " + to_string(index) + ", press ESC to exit";
	namedWindow(showWinName, WINDOW_AUTOSIZE);
	moveWindow(showWinName, 400, 0);


	string outVname = filename_prefix + ".avi";

	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	VideoWriter outVideo(outVname, fourcc, 60, Size(frame_width, frame_height));
	for (;;)
	{
		cap >> frame;

		imshow(showWinName, frame);
		outVideo.write(frame);

		char c = (char)waitKey(10);
		if (c == 27)
			break;
	}
	cap.release();
	cout << "camera " << index << " released!" << endl;
}

int main(int argc, char* argv[])
{
	// speed up opencv open camera
	//int env = _putenv("OPENCV_VIDEOIO_MSMF_ENABLE_HW_TRANSFORMS=0");



	thread t_showSave1(showSaveCamStream, 0);
	thread t_showSave2(showSaveCamStream, 1);

	t_showSave1.join(); // Wait for t_showSave to finish
	t_showSave2.join(); // Wait for t_showSave to finish

	cout << "main exit" << endl;
	return 0;
}