#pragma once
#include "../CameraInterface/camerainterface.h"
#include <mutex>
#include "Windows.h"
#include "CameraApi.h"
enum CameraTriggerType
{
	AUTOTRIGGER = 0,
	SOFTTRIGGER = 1,
	HARDTRIGGER = 2
};

class MindVisionCameraInterface :public CameraInterface
{

public:
	MindVisionCameraInterface(CameraTriggerType = AUTOTRIGGER);
	virtual ~MindVisionCameraInterface() override;
	virtual bool is_open();
	virtual void open_camera(const int&);
	virtual cv::Mat get_cvmat(const int&, bool flag = false);
	void openWithSn(const std::string&,const int& = 0);
	void setRgistedStr(const std::string&);
	void showCameraSetting();
	void setCameraTriggerType(const CameraTriggerType&);
	std::map<std::string, int> getCameraList();
private:
	bool is_opened;
	int camera_handle;
	tSdkCameraCapbility about_camera;
	unsigned char* out_raw_data_ptr;
	unsigned char* out_data_ptr;
	tSdkFrameHead data_head;
	std::mutex mutex;
	std::string reg_str;
	CameraTriggerType type;
};