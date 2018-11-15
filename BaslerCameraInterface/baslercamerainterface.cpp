#include "baslercamerainterface.h"
#include "pylon/PylonIncludes.h"

using namespace Pylon;
BaslerCameraInterface::BaslerCameraInterface(const BaslerType& type) :
	camera_statu(false),
	camera_type(type)
{
	PylonInitialize();

}

BaslerCameraInterface::~BaslerCameraInterface()
{
	if (camera_statu)
	{
		camera->StopGrabbing();
		camera->Close();
		camera->DetachDevice();
	}

	PylonTerminate();

}

void BaslerCameraInterface::open_camera(const int& idx)
{
	CTlFactory& fac = CTlFactory::GetInstance();
	DeviceInfoList_t info_list;
	ITransportLayer* pTl(nullptr);

	switch (camera_type)
	{
	case BaslerCameraInterface::PYLON_GIGE:
		pTl = fac.CreateTl("BaslerGigE");
		break;
	case BaslerCameraInterface::PYLON_1394:
		pTl = fac.CreateTl("Basler1394");
		break;
	case BaslerCameraInterface::PYLON_USB:
		pTl = fac.CreateTl("BaslerUsb");
		break;
	case BaslerCameraInterface::PYLON_CLSER:
		pTl = fac.CreateTl("BaslerCLSer");
		break;
	}
	pTl->EnumerateDevices(info_list);

	if (info_list.empty())
	{
		fac.ReleaseTl(pTl);
		throw CameraError("这台电脑上没有相机。");
		return;
	}
	if (info_list.size() <= idx)
	{
		fac.ReleaseTl(pTl);
		throw CameraError("未找到对应的相机。");
		return;
	}
	try
	{
		camera = std::make_shared<Camera_t>(pTl->CreateDevice(info_list[idx]));
		camera->Open();
	}
	catch (GenICam::GenericException & e)
	{
		throw CameraError("无法打开相机。");
	}
	camera_statu = true;
	camera->StartGrabbing(GrabStrategy_LatestImageOnly);
}


cv::Mat BaslerCameraInterface::get_cvmat(const int & format, bool flag)
{
	try
	{
		CGrabResultPtr res;
		cv::Mat out;
		camera->RetrieveResult(5000, res, TimeoutHandling_Return);

		uchar* data = static_cast<uchar*>(res->GetBuffer());
		if (format == CV_8UC3)
		{
			cv::Mat img(res->GetHeight(), res->GetWidth(), CV_8UC1);
			img.data = data;
			out = img.clone();
			cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
			return out;
		}
		else
		{
			cv::Mat img(res->GetHeight(), res->GetWidth(), CV_8UC1);
			img.data = data;
			out = img.clone();
			return out;
		}
	}
	catch (GenICam::GenericException& e)
	{
		throw CameraError(std::string("BaslerError:") + e.what());
	}

}

bool BaslerCameraInterface::is_open()
{
	return camera_statu;
}

void BaslerCameraInterface::openWithSn(const std::string & sn, const int & idx)
{
	CTlFactory& fac = CTlFactory::GetInstance();
	DeviceInfoList_t info_list;
	ITransportLayer* pTl(nullptr);

	switch (camera_type)
	{
	case BaslerCameraInterface::PYLON_GIGE:
		pTl = fac.CreateTl("BaslerGigE");
		break;
	case BaslerCameraInterface::PYLON_1394:
		pTl = fac.CreateTl("Basler1394");
		break;
	case BaslerCameraInterface::PYLON_USB:
		pTl = fac.CreateTl("BaslerUsb");
		break;
	case BaslerCameraInterface::PYLON_CLSER:
		pTl = fac.CreateTl("BaslerCLSer");
		break;
	}
	pTl->EnumerateDevices(info_list);

	if (info_list.empty())
	{
		fac.ReleaseTl(pTl);
		throw CameraError(u8"这台电脑上没有相机。");
		return;
	}
	if (info_list.size() <= idx)
	{
		fac.ReleaseTl(pTl);
		throw CameraError(u8"未找到对应的相机。");
		return;
	}
	try
	{
		auto create_info = info_list[idx];
		auto sn_str = create_info.GetSerialNumber();
		if (sn != sn_str.c_str())
		{
			throw CameraError(u8"无法打开相机。");
		}
		camera = std::make_shared<Camera_t>(pTl->CreateDevice(info_list[idx]));
		camera->Open();
	}
	catch (GenICam::GenericException & e)
	{
		throw CameraError(u8"无法打开相机。");
	}
	camera_statu = true;
	camera->StartGrabbing(GrabStrategy_LatestImageOnly);
}

std::map<std::string, int> BaslerCameraInterface::getCameraList()
{
	return std::map<std::string, int>();
}

