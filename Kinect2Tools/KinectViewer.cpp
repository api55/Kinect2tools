#include "stdafx.h"
#include "KinectViewer.h"


KinectViewer::KinectViewer() :
	m_pKinectSensor(NULL),
	m_pCoordinateMapper(NULL),
	m_pMultiSourceFrameReader(NULL)
{
	// create heap storage for the coorinate mapping from depth to color
	m_pColorCoordinates = new ColorSpacePoint[cDepthWidth * cDepthHeight];
	m_pDepthCoordinates = new DepthSpacePoint[cColorWidth * cColorHeight];

	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	min_x = 0;
	min_y = 0;
	min_z = 0;
	max_x = 100.0;
	max_y = 100.0;
	max_z = 100.0;
	initKinect();
}


KinectViewer::~KinectViewer()
{
	// done with frame reader
	SafeRelease(m_pMultiSourceFrameReader);

	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}


HRESULT KinectViewer::initKinect(){

	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the frame reader

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex,
				&m_pMultiSourceFrameReader);
		}
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		// TODO: show message
		return E_FAIL;
	}

	return hr;
}

void KinectViewer::update(){
	if (m_pMultiSourceFrameReader != NULL && !m_pMultiSourceFrameReader)
	{
		return;
	}

	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IColorFrame* pColorFrame = NULL;

	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}

		SafeRelease(pDepthFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}

		SafeRelease(pColorFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		INT64 nDepthTime = 0;
		IFrameDescription* pDepthFrameDescription = NULL;
		int nDepthWidth = 0;
		int nDepthHeight = 0;
		UINT nDepthBufferSize = 0;
		UINT16 *pDepthBuffer = NULL;

		IFrameDescription* pColorFrameDescription = NULL;
		int nColorWidth = 0;
		int nColorHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nColorBufferSize = 0;
		RGBQUAD *pColorBuffer = NULL;

		cv::Mat rgb_im, depth_im, rgb_display, depth_display, rgbd_im;
		// get depth frame data

		hr = pDepthFrame->get_RelativeTime(&nDepthTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);
			depth_im.create(cDepthHeight, cDepthWidth, CV_16U);
			memcpy(depth_im.data, pDepthBuffer, cDepthHeight*cDepthWidth*sizeof(UINT16));

			// get color frame data
		}

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
			}

			if (SUCCEEDED(hr))
			{
				if (imageFormat == ColorImageFormat_Bgra)
				{
					hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
				}
				else if (m_pColorRGBX)
				{
					pColorBuffer = m_pColorRGBX;
					nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
					hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);

					rgb_im.create(cColorHeight, cColorWidth, CV_8UC4);
					memcpy(rgb_im.data, pColorBuffer, 4 * cColorHeight*cColorWidth*sizeof(BYTE));


				}
				else
				{
					hr = E_FAIL;
				}
			}
			if (SUCCEEDED(hr)){

				cv::cvtColor(rgb_im, rgb_display, CV_BGRA2BGR);
				create_rgbd(depth_im, rgb_display, rgbd_im);
				cv::normalize(depth_im, depth_display, 0, 255, CV_MINMAX, CV_8UC1);
				cv::Mat rgbdepth = depth_display.clone();
				cv::cvtColor(rgbdepth, rgbdepth, CV_GRAY2BGR);
				for     (int iii = 0; iii < depth_display.rows; iii++)
				{
					for (int jjj = 0; jjj < depth_display.cols; jjj++)
					{
						if (depth_display.at<uchar>(iii,jjj) == 0)
						{
							rgbdepth.at<cv::Vec3b>(iii, jjj) = cv::Vec3b(0, 0, 255);
						}
					}
				}

				cv::resize(rgb_display, rgb_display, cv::Size(600, 480));
				
				cv::imshow("depth", rgbdepth);
				cv::imshow("rgb (scaled)", rgb_display);
				cv::imshow("rgbd", rgbd_im);
				int key = cv::waitKey(10);
				key_callback(key);
			}
			SafeRelease(pDepthFrameDescription);
			SafeRelease(pColorFrameDescription);
		}

		SafeRelease(pDepthFrame);
		SafeRelease(pColorFrame);
		SafeRelease(pMultiSourceFrame);
}

void KinectViewer::key_callback(int key){
	switch (key)
	{
		//TODO: make the exit better
	case(27) :
		exit(0);
		break;
	default:
		break;
	}
}

//void KinectViewer::create_rgbd(cv::Mat& depth_im, cv::Mat& rgb_im, cv::Mat& rgbd_im){
//	HRESULT hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(cDepthWidth * cDepthHeight, (UINT16*)depth_im.data, cDepthWidth * cDepthHeight, m_pColorCoordinates);
//	rgbd_im = cv::Mat::zeros(depth_im.rows, depth_im.cols, CV_8UC3);
//	double minVal, maxVal;
//	cv::minMaxLoc(depth_im, &minVal, &maxVal);
//	CString t;
//
//	for (int i=0; i < cDepthHeight; i++){
//		for (int j=0; j < cDepthWidth; j++){
//			t.Format(_T("%d\n"), (int)depth_im.at<UINT16>(i, j));
//			OutputDebugString(t);
//			if (depth_im.at<UINT16>(i, j) > 0 && depth_im.at<UINT16>(i, j) < maxVal * (max_z / 100) && depth_im.at<UINT16>(i, j) > maxVal * min_z /100){
//				double a = i * cDepthWidth + j;
//				ColorSpacePoint colorPoint = m_pColorCoordinates[i*cDepthWidth+j];
//				int colorX = (int)(floor(colorPoint.X + 0.5));
//				int colorY = (int)(floor(colorPoint.Y + 0.5));
//				if ((colorX >= 0) && (colorX < cColorWidth) && (colorY >= 0) && (colorY < cColorHeight))
//				{
//					rgbd_im.at<cv::Vec3b>(i, j) = rgb_im.at<cv::Vec3b>(colorY, colorX);
//				}
//			}
//
//		}
//	}
//}
//
void KinectViewer::create_rgbd(cv::Mat& depth_im, cv::Mat& rgb_im, cv::Mat& rgbd_im){
	//HRESULT hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(cDepthWidth * cDepthHeight, (UINT16*)depth_im.data, cDepthWidth * cDepthHeight, m_pColorCoordinates);
	HRESULT hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(cDepthWidth * cDepthHeight, (UINT16*)depth_im.data, cColorWidth * cColorHeight, m_pDepthCoordinates);
	rgbd_im = rgb_im.clone();
	double minVal, maxVal;
	cv::minMaxLoc(depth_im, &minVal, &maxVal);
	for (int i = rgbd_im.rows * (min_y / 100); i < rgbd_im.rows * (max_y / 100); i++){
		for (int j = rgbd_im.cols * (min_x / 100); j < rgbd_im.cols * (max_x / 100); j++){
			//if (depth_im.at<UINT16>(i, j) > 0 && depth_im.at<UINT16>(i, j) < maxVal * (max_z / 100) && depth_im.at<UINT16>(i, j) > maxVal * min_z /100){
			double a = i * cDepthWidth + j;
			DepthSpacePoint depthPoint = m_pDepthCoordinates[i * rgbd_im.cols + j];
			int depthX = (int)(floor(depthPoint.X + 0.5));
			int depthY = (int)(floor(depthPoint.Y + 0.5));
			std::cout << "depth:(" << depthX << "," << depthY << ")" << std::endl;
			if ((depthX >= 0) && (depthX < cDepthWidth) && (depthY >= 0) && (depthY < cDepthHeight))
			{
				if (depth_im.at<UINT16>(depthY, depthX) == 0)
					rgbd_im.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
			}
			//}

		}
	}
}