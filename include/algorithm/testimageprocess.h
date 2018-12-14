#ifndef TESTIMAGEPROCESS_H
#define TESTIMAGEPROCESS_H

#include "include/config/configservice.h"
#include "include/communication/communication.h"
#include "include/ui/mainwindow.h"
#include "include/util/imageutil.h"
#include "include/storage/storageservice.h"
#include "include/hardware/cframeinfo.h"
#include "include/controller/controller.h"
#include "include/algorithm/algorithmwrapper.hpp"


class TestImageAloProcess{
public:
    TestImageAloProcess();

    AlgorithmWrapper *mAlgWrapper;

    void sendFrameToAlgo(int camId, int statId);
    void startTest();
private:
    ConfigService *configService;
    Config_Params getConfigParams();
    int frameIndex;
};

#endif // TESTIMAGEPROCESS_H
