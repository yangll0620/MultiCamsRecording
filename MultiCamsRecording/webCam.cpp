#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace cv;


#include "webCam.h"


webCam::webCam(int index)
{
	camID = index;
	capturing = false;
}

webCam::~webCam()
{
	cap.release();
}

bool webCam::start(string outFilename)
{
	/*string outVFile = outFilename + ".avi";
	string outTimestampFile = outFilename + ".csv";;
 	capturing = true;
	capturesavethread = std::thread(&webCam::captureFrame, outVFile, outTimestampFile);
	capturesavethread.join();*/

	return true;
}

bool webCam::stop()
{
	capturing = false;
	return true;
}

bool webCam::openCam()
{
	cout << "Initing Camera " << camID << "...." << endl;
	cap.open(camID, cv::CAP_DSHOW);
	if (!cap.isOpened())
	{
		return false;
	}

	return true;
}

double webCam::get_web_frame()
{
	double fps;
	int framei = 0;
	int testframen = 150;
	Mat frame;

	cout << "Testing fps of Camera " << camID << " ......" << endl;
	auto begin = std::chrono::high_resolution_clock::now();
	while (framei < testframen)
	{
		if (cap.read(frame))
		{
			framei++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	fps = (int)((double)testframen / elapsed.count() * 1000 * 100) / 100.0;
	cout <<"Camera " << camID << " fps = " << fps << endl;

	return fps;
}


int webCam::captureFrame(string outVFile, string outTimestampFile)
{
	// videoWriter setup
	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	double fps = get_web_frame();
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
	VideoWriter outVideo(outVFile, fourcc, fps, Size(frame_width, frame_height));

	
	// Start capturing
	Mat frame;
	const string showWinName = "cam " + to_string(camID) + ", press ESC to exit";
	int framei = 0;
	while(capturing)
	{
		cap >> frame;

		imshow(showWinName, frame);
		outVideo.write(frame);

		char c = (char)waitKey(10);
		if (c == 27)
		{
			capturing = false;
			break;
		}

		framei++;	
	}
	outVideo.release();

	return 0;
}


