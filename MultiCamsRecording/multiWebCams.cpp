#include "multiWebCams.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

multiWebCams::multiWebCams()
{
}

int multiWebCams::numOfAvailableCameras()
{// return available camera number
	
	int nCams = 0;
	bool noError = true;
	while (noError)
	{
		try
		{
			// Check if camera is available.
			VideoCapture videoCapture(nCams, cv::CAP_DSHOW);

			if (videoCapture.isOpened())
			{
				nCams++;
			}
			else
			{
				noError = false;
			}
			videoCapture.release();
		}
		catch (...)
		{
			noError = false;
		}
	}

	return nCams;

}
