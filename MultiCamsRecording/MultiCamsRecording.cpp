// MultiCamsRecording.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <thread> 
#include <chrono>
#include <cstdlib>


using namespace cv;
using namespace std;
using namespace std::chrono;

#include "multiWebCams.h"

int showSaveCamStream(int index)
{
	VideoCapture cap(index, cv::CAP_DSHOW);
	if (!cap.isOpened())
	{
		cout << "Could not open camera " << index << endl;
		return -1;
	}

	Mat frame;
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');

	cap.set(CAP_PROP_FOURCC, fourcc);
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);


	const string showWinName = "cam " + to_string(index) + ", press ESC to exit";
	namedWindow(showWinName, WINDOW_AUTOSIZE);
	moveWindow(showWinName, 400, 0);

	
	string outVname = "live.avi";
	
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
	cout << "camera " << index  << " released!" << endl;
}

int main(int argc, char *argv[])
{
	// speed up opencv open camera
	//int env = _putenv("OPENCV_VIDEOIO_MSMF_ENABLE_HW_TRANSFORMS=0");
	
	multiWebCams mwebcams;
	int nCams = mwebcams.numOfAvailableCameras();

	cout << "There are " << nCams << " cameras" << endl;


	thread t_showSave(showSaveCamStream, 1);
	t_showSave.join(); // Wait for t_showSave to finish
	
	cout << "main exit" << endl;
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
