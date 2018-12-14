#include "include/ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "include/util/logger.h"
#include "include/defines.h"

#include <QDesktopServices>
#include <QTimer>

#define TAG "mainwindow"
#define STATION3 false
#define STATION3_CAMERAID_STARTINDEX 3

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Tusvision-CBD");
    setFixedSize(this->width(),this->height());
    //showFullScreen();
    //this->setWindowState(Qt::WindowMaximized);

    total_detected_count = 0;
    unqualified_count = 0;
    detect_time = 1;

    ui->start_stop->setTitle("STOPPED");

    onDebugModeChanged(false);
    initAction();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete setting_dialog;
    delete imagebroswer;
}

void MainWindow::setListener(UiActionListener *listener)
{
    uiActionListener = listener;
}

void MainWindow::addConfigChangeListener(ConfigChangeListener *listener)
{
    setting_dialog->setConfigChangeListener(listener);
}

void MainWindow::applyConfig(Config_Params &params)
{
    this->onDebugModeChanged(params.debug);
    rootpath = params.rootpath;
    isStation3 = (params.station_info.station_id == 3);

    QPixmap image(":/img/resource/logo_deep.jpg");
    QLabel *label;
    if (!isStation3) {
        ui->station_3->setVisible(false);
        label = ui->icon;
        adjustImage(label, image);
    } else {
        ui->station_12->setVisible(false);
        QRegularExpression regexp(QStringLiteral("icon(?<ID>\\d+)"));
        const QList<QLabel *> station3_icons = findChildren<QLabel *>(regexp);
        for (QLabel *station3_icon : station3_icons) {
            label = station3_icon;
            adjustImage(label, image);
        }

    }

    setting_dialog->applyConfig(params);
    imagebroswer->applyRootpath(params.rootpath);
}

void MainWindow::initDetectData(Detect_Data &data)
{
    total_detected_count = data.total;
    unqualified_count = data.unqualified;

    ui->total_count->setText(QString::number(total_detected_count, 10));
    ui->defect_count->setText(QString::number(unqualified_count, 10));
}

void MainWindow::updateDetectData(Detect_Data &data)
{
    total_detected_count += data.total;
    unqualified_count += data.unqualified;

    ui->total_count->setText(QString::number(total_detected_count, 10));
    ui->defect_count->setText(QString::number(unqualified_count, 10));

    bool pass = (data.unqualified == 0);
    QPalette palette;
    palette.setColor(QPalette::WindowText,pass ? (Qt::green) : (Qt::red));
    ui->current_result->setPalette(palette);
    ui->current_result->setText(pass ? "PASS" : "FAIL");
}

void MainWindow::initAction()
{
    setting_dialog = new Dialog(this);
    imagebroswer = new ImageBroswer();

    ui->actionstart->setEnabled(true);
    ui->actionstop->setEnabled(false);
    ui->actionexit->setEnabled(true);

    connect(ui->actionstart, &QAction::triggered, this, &MainWindow::on_btn_start_clicked);
    connect(ui->actionstop, &QAction::triggered, this, &MainWindow::on_btn_stop_clicked);
    connect(ui->actionexit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionoptions, &QAction::triggered, setting_dialog, &Dialog::show);

    connect(setting_dialog, &Dialog::onDebugChanged, this, &MainWindow::onDebugModeChanged);

}

void MainWindow::onDebugModeChanged(bool debug)
{
    LOGD(TAG, "onDebugSwitched:debug = %1", debug);
    if (!debug)
    {
        ui->debug_info->hide();
    } else
    {
        ui->debug_info->show();
    }
}

void MainWindow::on_btn_start_clicked()
{
    LOGD(TAG,"on_btn_start_clicked");
    uiActionListener->onStartClicked();

    if (timer == nullptr) {
        timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, &QTimer::timeout, this, &MainWindow::timeout);
        timer->start();
    }

}

void MainWindow::timeout() {
    LOGD(TAG,"timeout");
    uiActionListener->onStartClicked();
}

void MainWindow::onConnected()
{
    LOGD(TAG,"onConnected");
    ui->actionstart->setEnabled(false);
    ui->actionstop->setEnabled(true);
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);
    ui->start_stop->setTitle("RUNNING");

    statusBar()->showMessage("onConnected");
}

void MainWindow::on_btn_stop_clicked()
{
    LOGD(TAG,"on_btn_stop_clicked");
    uiActionListener->onStopClicked();
}

void MainWindow::onDisconnected()
{
    LOGD(TAG,"onDisconnected");
    ui->actionstart->setEnabled(true);
    ui->actionstop->setEnabled(false);
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
    ui->start_stop->setTitle("STOPPED");

    statusBar()->showMessage("onDisconnected");
}

void MainWindow::on_defect_pics_clicked()
{
    LOGD(TAG,"on_defect_pics_clicked");
    uiActionListener->onDefectPicsClicked();
    //QDesktopServices::openUrl(QUrl(rootpath, QUrl::TolerantMode));
    imagebroswer->startShow();
}

void MainWindow::on_brand_selector_currentIndexChanged(int index)
{
    LOGD(TAG,"on_brand_selector_currentIndexChanged:index=%1", index);
    uiActionListener->onBrandSelectorIndexChange();
}

void MainWindow::showImages(QVector<QImage*> &imgs)
{
    LOGD(TAG,"setImages:size=%1", imgs.length());
    //images = imgs;

    if (!isStation3) {
        QImage *img = imgs.at(0);
        img->scaled(ui->icon->width(),ui->icon->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->icon->setPixmap(QPixmap::fromImage(*img));
    } else {
        QRegularExpression regexp(QStringLiteral("icon(?<ID>\\d+)"));
        const QList<QLabel *> station3_icons = findChildren<QLabel *>(regexp);

        int length = qMin(station3_icons.length(), imgs.length());
        for (int i=0; i<length; i++) {
            QLabel *station3_icon = station3_icons.at(i);
            QImage *image = imgs.at(i);
            image->scaled(station3_icon->width(),station3_icon->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            station3_icon->setPixmap(QPixmap::fromImage(*image));
        }

    }

}

void MainWindow::adjustImage(QLabel *label, QPixmap &pixmap) {

    LOGD(TAG, "label width%1 label height%2", label->width(), label->height());

    float scale_w = (float)label->width() / pixmap.width();  //宽的比例
    float scale_h = (float)label->height() / pixmap.height();  //高的比例

    float new_width, new_height;  //新的宽和高
    if ((label->width() >= pixmap.width()) && (label->height() >= pixmap.height()))  //图片的原始宽和高均小于显示控件的宽和高
    {
        new_width = pixmap.width();
        new_height = pixmap.height();
    }
    else if (scale_w > scale_h)
    {
        new_width = pixmap.width() * scale_h;
        new_height = pixmap.height() * scale_h;
    }
    else if (scale_w <= scale_h)
    {
        new_width = pixmap.width() * scale_w;
        new_height = pixmap.height() * scale_w;
    }

    LOGD(TAG,"adjustImage:new_width=%1 new_height=%2", new_width, new_height);
    pixmap = pixmap.scaled(new_width,new_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    label->setPixmap(pixmap);
}

void MainWindow::showImage(int cameraId, QPixmap &pixmap)
{
    QLabel *label;

    if (!isStation3) {
        //img = img.scaled(ui->icon->width(),ui->icon->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        //ui->icon->setPixmap(QPixmap::fromImage(img));
        //pixmap = pixmap.scaled(ui->icon->size(), Qt::KeepAspectRatio);
        //ui->icon->setScaledContents(true);
        //ui->icon->setPixmap(pixmap);
        label = ui->icon;
    } else {
        QRegularExpression regexp(QStringLiteral("icon(?<ID>\\d+)"));
        QString label_id = QString("icon");
        QLabel *station3_icon = this->findChild<QLabel *>(label_id.append(STATION3_CAMERAID_STARTINDEX - 1));
        //pixmap = pixmap.scaled(station3_icon->size(), Qt::KeepAspectRatio);
        //station3_icon->setScaledContents(true);
        //station3_icon->setPixmap(pixmap);
        label = station3_icon;
    }

    adjustImage(label, pixmap);
}
