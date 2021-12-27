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
#include <fstream>
#include<windows.h>


using namespace cv;
using namespace std;
using namespace std::chrono;

#include "webCam.h"

auto t_start = chrono::steady_clock::now();

double get_web_frame(VideoCapture cap, int camID)
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
	cout << "Camera " << camID << " fps = " << fps << endl;

	return fps;
}

int showSaveCamStream(int camID, string outFilename)
{
	cout << "Initing Camera " << camID << "...." << endl;
	VideoCapture cap(camID, cv::CAP_DSHOW);
	if (!cap.isOpened())
	{
		cout << "Could not open camera " << camID << endl;
		return -1;
	}

	Mat frame;
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');

	cap.set(CAP_PROP_FOURCC, fourcc);


	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	double fps = get_web_frame(cap, camID);
	string outVFile = outFilename + ".avi";
	VideoWriter outVideo(outVFile, fourcc, fps, Size(frame_width, frame_height));

	string outTimestampFile = outFilename + ".csv";
	std::ofstream timeStampFile(outTimestampFile);
	timeStampFile << "framei" << "," << "time(ms)" << "\n";

	const string showWinName = "cam " + to_string(camID) + ", press ESC to exit";
	namedWindow(showWinName, WINDOW_AUTOSIZE);

	int framei = 0;
	for (;;)
	{
		cap >> frame;
		long t_elapsed  = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t_start).count();
		framei++;

		imshow(showWinName, frame);
		outVideo.write(frame);
		timeStampFile << framei << "," << t_elapsed << "\n";

		char c = (char)waitKey(10);
		if (c == 27)
			break;
	}
	cap.release();
	cout << "camera " << camID << " released!" << endl;
	timeStampFile.close();

}

int main(int argc, char* argv[])
{

	HANDLE hComm;

	hComm = CreateFileA("\\\\.\\COM6",                //port name
		GENERIC_READ | GENERIC_WRITE, //Read/Write
		0,                            // No Sharing
		NULL,                         // No Security
		OPEN_EXISTING,// Open existing port only
		0,            // Non Overlapped I/O
		NULL);        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
		printf("Error in opening serial port");
	else
		printf("opening serial port successful");

	CloseHandle(hComm);//Closing the Serial Port

	// prefix of file name for both.avi and .csv files
	__time64_t t_begin0;
	struct tm curr_tm;
	char timebuff[50];
	_time64(&t_begin0);
	_localtime64_s(&curr_tm, &t_begin0);
	strftime(timebuff, sizeof(timebuff), "%Y%m%d_%H%M%S", &curr_tm);
	string filename_prefix = "video_" + string(timebuff) + "_camera";

	

	thread t_showSave1(showSaveCamStream, 0, filename_prefix + to_string(0));
	thread t_showSave2(showSaveCamStream, 1, filename_prefix + to_string(1));

	t_showSave1.join(); 
	t_showSave2.join();

	cout << "main exit" << endl;
	return 0;
}