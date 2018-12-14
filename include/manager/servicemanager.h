#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "include/config/configservice.h"
#include "include/communication/communication.h"
#include "include/ui/mainwindow.h"
#include "include/util/imageutil.h"
#include "include/storage/storageservice.h"
#include "include/hardware/cframeinfo.h"
#include "include/controller/controller.h"

class ServiceManager : public UiActionListener, public EventListener, public TransporterStopListener
{

public:

    ServiceManager();
    ~ServiceManager();
    void exec();
    void cancel();
    void onFrames(QVector<CFrameInfo>& frames);
    Config_Params getConfigParams();

    //UiActionListener
    void onStartClicked();
    void onStopClicked();
    void onDefectPicsClicked();
    void onBrandSelectorIndexChange();

    //Eventlistener
    void onFramesFromControl(int stationid, QMap<int, CFrameInfo *> framelist);
    void onFrameFromControl(int stationid, int cameraid, CFrameInfo *frame);
    void onAnalysisResult(AnalysisResult &result);

    //TransporterStopListener
    void onTransporterStopped();

private:
    ConfigService *configService;
    CommunicationService *communicationService;
    MainWindow *mainWindow;
    StorageService *storageService;
    ControllerService *mControllerService;

    ImageUtil imageUtil;
    //Detect_info detect_info;
    int station;
};

#endif // SERVICEMANAGER_H
