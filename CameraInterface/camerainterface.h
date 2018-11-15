#ifndef CAMERAINTERFACE_H
#define CAMERAINTERFACE_H

#include <exception>
#include <string>
#include "opencv2/opencv.hpp"
class CameraError : public std::exception
{
public:
	CameraError(const std::string& w) :msg(w) {};
	~CameraError() {};
	const char* what() const { return msg.data(); };
private:
	std::string msg;
};


class CameraInterface
{
public:
	CameraInterface() {};
	virtual ~CameraInterface() {};
	virtual bool is_open() = 0;
	virtual void open_camera(const int&) = 0;
	virtual void openWithSn(const std::string&,const int& = 0) = 0;
	virtual cv::Mat get_cvmat(const int&, bool flag = false) = 0;
	virtual std::map<std::string, int> getCameraList() = 0;
};

#endif // CAMERAINTERFACE_H
