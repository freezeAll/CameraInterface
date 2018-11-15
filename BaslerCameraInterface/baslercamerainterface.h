#ifndef BASLERCAMERAINTERFACE_H
#define BASLERCAMERAINTERFACE_H

#pragma comment (lib,"GCBase_MD_VC120_v3_0_Basler_pylon_v5_0")
#pragma comment (lib,"GenApi_MD_VC120_v3_0_Basler_pylon_v5_0")
#pragma comment (lib,"PylonBase_MD_VC120_v5_0")
#pragma comment (lib,"PylonC_MD_VC120")
#pragma comment (lib,"PylonGUI_MD_VC120_v5_0")
#pragma comment (lib,"PylonUtility_MD_VC120_v5_0")


// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Namespace for using pylon objects.
using namespace Pylon;

// Settings to use Basler GigE cameras.
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;

// The camera specific grab result smart pointer.
typedef Camera_t::GrabResultPtr_t GrabResultPtr_t;


#include "../CameraInterface/camerainterface.h"

#include <memory>
#include <mutex>


namespace Pylon {
	class CInstantCamera;
}
class BaslerCameraInterface : public CameraInterface
{

public:
	enum BaslerType
	{
		PYLON_GIGE,
		PYLON_1394,
		PYLON_USB,
		PYLON_CLSER
	};
	BaslerCameraInterface(const BaslerType&);
	~BaslerCameraInterface();
	void open_camera(const int&);
	cv::Mat get_cvmat(const int&, bool flag = false);
	bool is_open();
	void openWithSn(const std::string&, const int& = 0);
	std::map<std::string, int> getCameraList();
private:
	BaslerType camera_type;
	std::shared_ptr<Camera_t> camera;
	bool camera_statu;

public:

};
#endif // BASLERCAMERAINTERFACE_H
