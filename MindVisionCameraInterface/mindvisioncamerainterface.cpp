#include "mindvisioncamerainterface.h"

MindVisionCameraInterface::MindVisionCameraInterface(CameraTriggerType tp) :
	camera_handle(-1),
	is_opened(false),
	out_raw_data_ptr(nullptr),
	out_data_ptr(nullptr),
	reg_str(""),
	type(tp)
{
}
MindVisionCameraInterface::~MindVisionCameraInterface()
{
	if (is_opened)
	{
		try
		{
			CameraSdkStatus status = CameraUnInit(camera_handle);
		}
		catch (...)
		{
		}
	}
}

bool MindVisionCameraInterface::is_open()
{
	return is_opened;
}

void MindVisionCameraInterface::open_camera(const int& idx)
{
	int                     iCameraCounts = 4;
	int                     iStatus = -1;
	tSdkCameraDevInfo       tCameraEnumList[4];

	//sdk��ʼ��  0 English 1����
	CameraSdkInit(1);

	//ö���豸���������豸�б�
	CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);

	//û�������豸
	if (iCameraCounts == 0) {
		throw CameraError(u8"û�����ӵ������");
	}

	//�����ʼ������ʼ���ɹ��󣬲��ܵ����κ����������صĲ����ӿ�
	iStatus = CameraInit(&tCameraEnumList[idx], -1, -1, &camera_handle);

	//��ʼ��ʧ��
	if (iStatus != CAMERA_STATUS_SUCCESS) {
		throw CameraError(u8"��ʼ��ʧ�ܡ�");

	}
	//�����������������ṹ�塣�ýṹ���а�������������õĸ��ֲ����ķ�Χ��Ϣ����������غ����Ĳ���
	CameraGetCapability(camera_handle, &about_camera);

	//g_pRgbBuffer = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax * 3);
	//g_readBuf = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax * 3);
	CameraSdkStatus status = CameraSetTriggerMode(camera_handle, int(type));
	/*��SDK���빤��ģʽ����ʼ��������������͵�ͼ��
	���ݡ������ǰ����Ǵ���ģʽ������Ҫ���յ�
	����֡�Ժ�Ż����ͼ��    */
	CameraPlay(camera_handle);

	out_raw_data_ptr = new unsigned char[about_camera.sResolutionRange.iHeightMax * about_camera.sResolutionRange.iWidthMax * 3];
	out_data_ptr = new unsigned char[about_camera.sResolutionRange.iHeightMax * about_camera.sResolutionRange.iWidthMax * 3];

	/*
	����ͼ����������ʽ����ɫ�ڰ׶�֧��RGB24λ
	*/
	CameraSetIspOutFormat(camera_handle, CAMERA_MEDIA_TYPE_MONO8);

	is_opened = true;
}

cv::Mat MindVisionCameraInterface::get_cvmat(const int &, bool flag)
{
	std::lock_guard<std::mutex> locker(mutex);
	if (type == CameraTriggerType::SOFTTRIGGER)
	{
		CameraSoftTrigger(camera_handle);
	}
	if (CameraGetImageBuffer(camera_handle, &data_head, &out_raw_data_ptr, 2000) == CAMERA_STATUS_SUCCESS)
	{
		CameraImageProcess(camera_handle,out_raw_data_ptr, out_data_ptr, &data_head);
		
		cv::Mat out(int(data_head.iHeight),int(data_head.iWidth), CV_8UC1,out_data_ptr);
		cv::Mat new_out;
		cv::flip(out, new_out, 0);
		CameraReleaseImageBuffer(camera_handle, out_raw_data_ptr);
		//cv::rotate(out, out,);
		return new_out;
	}
	else
	{
		throw CameraError(u8"��ȡ��Ƭʧ�ܡ�");
	}
}

void MindVisionCameraInterface::openWithSn(const std::string & str,const int& idx)
{
	int                     iCameraCounts = 4;
	int                     iStatus = -1;
	tSdkCameraDevInfo       tCameraEnumList[4];

	//sdk��ʼ��  0 English 1����
	CameraSdkInit(1);

	//ö���豸���������豸�б�
	CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);

	//û�������豸
	if (iCameraCounts == 0) {
		throw CameraError(u8"û�����ӵ������");
	}

	//�����ʼ������ʼ���ɹ��󣬲��ܵ����κ����������صĲ����ӿ�
	if (str.empty())
	{
		throw CameraError(u8"��ʼ��ʧ�ܡ�");
	}
	for (int i = 0; i < str.size(); i++)
	{
		if (i >= 32)
			break;
		if (tCameraEnumList[idx].acSn[i] != str[i])
		{
			throw CameraError(u8"��ʼ��ʧ�ܡ�");
		}
	}
	iStatus = CameraInit(&tCameraEnumList[idx], PARAM_MODE_BY_SN,-1, &camera_handle);

	//��ʼ��ʧ��
	if (iStatus != CAMERA_STATUS_SUCCESS) {
		throw CameraError(u8"��ʼ��ʧ�ܡ�");

	}
	//�����������������ṹ�塣�ýṹ���а�������������õĸ��ֲ����ķ�Χ��Ϣ����������غ����Ĳ���
	CameraGetCapability(camera_handle, &about_camera);
	CameraSdkStatus status = CameraSetTriggerMode(camera_handle,int(type));

	//g_pRgbBuffer = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax * 3);
	//g_readBuf = (unsigned char*)malloc(g_tCapability.sResolutionRange.iHeightMax*g_tCapability.sResolutionRange.iWidthMax * 3);

	/*��SDK���빤��ģʽ����ʼ��������������͵�ͼ��
	���ݡ������ǰ����Ǵ���ģʽ������Ҫ���յ�
	����֡�Ժ�Ż����ͼ��    */
	CameraPlay(camera_handle);

	out_raw_data_ptr = new unsigned char[about_camera.sResolutionRange.iHeightMax * about_camera.sResolutionRange.iWidthMax * 3];
	out_data_ptr = new unsigned char[about_camera.sResolutionRange.iHeightMax * about_camera.sResolutionRange.iWidthMax * 3];

	/*
	����ͼ����������ʽ����ɫ�ڰ׶�֧��RGB24λ
	*/
	CameraSetIspOutFormat(camera_handle, CAMERA_MEDIA_TYPE_MONO8);

	is_opened = true;
}

void MindVisionCameraInterface::setRgistedStr(const std::string & str)
{
	reg_str = str;
}

void MindVisionCameraInterface::showCameraSetting()
{
	char title[1024] = "������� - ΢���������Ӿ�ƽ̨";
	CameraCreateSettingPage(camera_handle, NULL,
		 title,NULL, NULL, 0);
	CameraShowSettingPage(camera_handle, TRUE);
}

void MindVisionCameraInterface::setCameraTriggerType(const CameraTriggerType & t)
{
	type = t;
	CameraSetTriggerMode(camera_handle, int(type));
}

std::map<std::string, int> MindVisionCameraInterface::getCameraList()
{
	return std::map<std::string, int>();
}
