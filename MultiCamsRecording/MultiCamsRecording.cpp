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
#include <stdio.h>
#include <shobjidl.h> 
#include <locale>
#include <codecvt>

using namespace cv;
using namespace std;
using namespace std::chrono;

#include "webCam.h"

auto t_start = chrono::steady_clock::now();
bool capturing = false;
Mat currframe;

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
	string outVFile = outFilename + "_video.avi";
	VideoWriter outVideo(outVFile, fourcc, fps, Size(frame_width, frame_height));

	string outTimestampFile = outFilename + "_timestamp.csv";
	std::ofstream timeStampFile(outTimestampFile);
	timeStampFile << "framei" << "," << "time(ms)" << "\n";

	const string showWinName = "cam " + to_string(camID) + ", press ESC to exit";
	namedWindow(showWinName, WINDOW_AUTOSIZE);

	int framei = 0;
	while(capturing)
	{
		cap >> frame;
		long t_elapsed  = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t_start).count();
		framei++;

		frame.copyTo(currframe);

		imshow(showWinName, frame);
		outVideo.write(frame);
		timeStampFile << framei << "," << t_elapsed << "\n";

		char c = (char)waitKey(10);
		if (c == 27)
			capturing = false;
	}
	cap.release();
	cout << "camera " << camID << " released!" << endl;
	timeStampFile.close();

}

bool handleIO8(LPCSTR COMFileName, string outFilename)
{
	HANDLE hComm;

	hComm = CreateFileA(COMFileName, GENERIC_READ | GENERIC_WRITE, //Read/Write
		0,                            // No Sharing
		NULL,                         // No Security
		OPEN_EXISTING,// Open existing port only
		0,            // Non Overlapped I/O
		NULL);        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
	{
		printf("Error openning serial port\n");

		CloseHandle(hComm);
		return false;
	}

	
	printf("opened serial port!\n");

	// setup serial params
	DCB dcb;
	SecureZeroMemory(&dcb, sizeof(DCB)); //  Initialize the DCB structure.

	bool status = GetCommState(hComm, &dcb);

	//  Fill in some DCB values and set the com state: 
	dcb.BaudRate = CBR_115200;     //  baud rate
	dcb.ByteSize = 8;             //  data size, xmit and rcv
	dcb.Parity = NOPARITY;      //  parity bit
	dcb.StopBits = ONESTOPBIT;    //  stop bit
	if (!SetCommState(hComm, &dcb)) {
		std::cout << "!SetCommState" << "\n";
		return false;
	}

	bool fSuccess;

	fSuccess = SetCommMask(hComm, EV_CTS | EV_DSR | EV_RXCHAR);
	if (!fSuccess)
	{
		// Handle the error. 
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return false;
	}

	OVERLAPPED o;
	// Create an event object for use by WaitCommEvent. 
	o.hEvent = CreateEvent(NULL, TRUE, FALSE,  NULL);
	// Initialize the rest of the OVERLAPPED structure to zero.
	o.Internal = 0;
	o.InternalHigh = 0;
	o.Offset = 0;
	o.OffsetHigh = 0;
	assert(o.hEvent);
	
	DWORD dwBytesWritten = 0;
	DWORD dwEvtMask;
	BYTE readbyte;
	DWORD dwBytesTransferred;
	long t_elapsed;
	std::ofstream timeStampFile(outFilename + ".csv");
	timeStampFile << "Binary" << "," << "time(ms)" << "\n";
	while (capturing)
	{
		fSuccess = WriteFile(hComm, "A", 1, &dwBytesWritten, NULL);
		if (WaitCommEvent(hComm, &dwEvtMask, &o))
		{
			if (dwEvtMask & EV_RXCHAR)
			{
				t_elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t_start).count();
				
				fSuccess == ReadFile(hComm, &readbyte, 1, &dwBytesTransferred, 0);
				while (fSuccess && readbyte != '\r')
				{
					if (readbyte == '0' || readbyte == '1')
					{
						timeStampFile << char(readbyte) << ", " << t_elapsed  << endl;
					}
					fSuccess == ReadFile(hComm, &readbyte, 1, &dwBytesTransferred, 0);
				}
			}
		}
	}
	
	timeStampFile.close();
	CloseHandle(hComm);//Closing the Serial Port
	cout << "Closed IO8 COM!" << endl;

	return true;
}

HRESULT chooseSavefolder(string* savefolder)
{
	IFileOpenDialog* pFileOpen{};
	IShellItem* pItem{};
	PWSTR pszFilePath = { 0 };

	HRESULT hr;
	string str;

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (!SUCCEEDED(hr))
		goto done;


	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
	if (!SUCCEEDED(hr))
		goto done;


	hr = pFileOpen->SetOptions(FOS_PICKFOLDERS);
	if (!SUCCEEDED(hr))
		goto done;

	// Show the Open dialog box.
	hr = pFileOpen->Show(NULL);
	if (!SUCCEEDED(hr))
		goto done;
			

	// Get the file name from the dialog box.
	hr = pFileOpen->GetResult(&pItem);
	if (!SUCCEEDED(hr))
		goto done;
		
	
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					
	str = convert.to_bytes(pszFilePath);
	savefolder = &str;

done:
	pItem->Release();
	pFileOpen->Release();
	CoTaskMemFree(pszFilePath);
	CoUninitialize();
	return hr;
}

int main(int argc, char* argv[])
{
	string* savefolder{};
	//HRESULT hr = chooseSavefolder(savefolder);
	

	bool test = true;
	if (test)
	{
		LPCSTR IO8File = "\\\\.\\COM6";


		bool testCams = true;

		// prefix of file name for both.avi and .csv files
		__time64_t t_begin0;
		struct tm curr_tm;
		char timebuff[50];
		_time64(&t_begin0);
		_localtime64_s(&curr_tm, &t_begin0);
		strftime(timebuff, sizeof(timebuff), "%Y%m%d_%H%M%S", &curr_tm);
		string filename_prefix = "v_" + string(timebuff) + "_camera";

		capturing = true;
		thread t_showSave1(showSaveCamStream, 0, filename_prefix + to_string(0));
		thread t_showSave2(showSaveCamStream, 1, filename_prefix + to_string(1));
		thread t_monitorIO8(handleIO8, IO8File, "v_" + string(timebuff) + "_startpad_timestamp");



		t_monitorIO8.join();
		t_showSave1.join();
		t_showSave2.join();

		cout << "main exit" << endl;
	}

	
	return 0;
}