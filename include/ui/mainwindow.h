#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "include/ui/dialog.h"
#include "include/ui/imagebroswer.h"
#include "include/config/configservice.h"
#include "include/communication/communication.h"

#include <opencv/cv.h>
#include <QMainWindow>
#include <QLabel>

class ServiceManager;

namespace Ui {
class MainWindow;
}

class UiActionListener {
public:
    virtual void onStartClicked() = 0;
    virtual void onStopClicked() = 0;
    virtual void onDefectPicsClicked() = 0;
    virtual void onBrandSelectorIndexChange() = 0;
    virtual void onTransporterStopped() = 0;
};

class MainWindow : public QMainWindow, public ConnectListener
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showImages(QVector<QImage*> &imgs);
    void showImage(int id, QPixmap& pixmap);
    void setListener(UiActionListener *listener);
    void addConfigChangeListener(ConfigChangeListener *listener);
    void applyConfig(Config_Params &params);
    void initDetectData(Detect_Data &data);
    void updateDetectData(Detect_Data &data);

    //ConnectListener
    void onConnected();
    void onDisconnected();

public slots:
    void onDebugModeChanged(bool debug);

private:
    void initAction();
    void adjustImage(QLabel *label, QPixmap &pixmap);
    void timeout();

private slots:
    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_defect_pics_clicked();

    void on_brand_selector_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    int total_detected_count;
    int unqualified_count;
    int detect_time;
    QString rootpath;
    bool isStation3;

    Dialog *setting_dialog;
    ImageBroswer *imagebroswer;
    //QVector<QImage> *images;
    UiActionListener *uiActionListener;

    QTimer *timer;
};

#endif // MAINWINDOW_H
