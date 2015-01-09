#include <Kinect.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <pcl\point_types.h>
#include <pcl\io\pcd_io.h>
#include <pcl\io\file_io.h>
#include <iostream>


// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}
#pragma once
class KinectViewer
{
public:
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
	KinectViewer();
	~KinectViewer();
	void update();
	double min_x, min_y, min_z, max_x, max_y, max_z;


private:
	RGBQUAD*                m_pColorRGBX;
	HRESULT initKinect();
	// Current Kinect
	IKinectSensor*           m_pKinectSensor;
	ICoordinateMapper*       m_pCoordinateMapper;
	ColorSpacePoint*        m_pColorCoordinates;
	DepthSpacePoint*        m_pDepthCoordinates;
	// Frame reader
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;
	
	void key_callback(int key);
	void create_rgbd(cv::Mat& depth_im, cv::Mat& rgb_im, cv::Mat& rgbd_im );
};

