#pragma once
class webCam
{
public:
	int camID;
	VideoCapture cap;
	VideoWriter writer;
	bool capturing;
	//thread capturesavethread;

	string out, outtimestampfile;

	webCam(int index);
	~webCam();

	bool openCam();
	bool start(string outFilename);
	bool stop();

	double get_web_frame();
	int captureFrame(string outVFile, string outTimestampFile);
};


