#include "include/hardware/icamera.h"

ICamera::ICamera() {

}

void ICamera::setCameraId(int id) {
   mCameraId = id;
}

void ICamera::setStationId(int id) {
   mStationId = id;
}

int ICamera::getCameraId() {
   return mCameraId;
}

void ICamera::setStatus(int status) {
    mStatus = status;
}

int ICamera::getStatus() {
    return mStatus;
}
