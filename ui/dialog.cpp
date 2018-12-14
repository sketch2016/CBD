#include "include/ui/dialog.h"
#include "ui_dialog.h"
#include "include/util/logger.h"

#include <iostream>

#define TAG "setting_dialog"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QIntValidator v( 1, 180, this );
    ui->storeTime->setValidator(&v);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setConfigChangeListener(ConfigChangeListener *listener)
{
    configChangeListener = listener;
}

void Dialog::applyConfig(Config_Params &params)
{
    ui->debug->setChecked(params.debug);
    ui->comboBox->setCurrentIndex(params.storeStrategy);
    ui->storeTime->setText(QString::number(params.storeTime));
    ui->plc_addr->setText(params.plcAddr);
    ui->display_addr->setText(params.displayAddr);
    ui->rootpath->setText(params.rootpath);
    ui->plc_server_addr->setText(params.plcServerAddr);
}

void Dialog::on_debug_clicked(bool checked)
{
    LOGD(TAG, "on_checkBox_clicked:checked = %1", checked);
    onDebugChanged(checked);
    configChangeListener->onDebugModeChanged(checked);
}

void Dialog::on_comboBox_currentIndexChanged(int index)
{
    LOGD(TAG, "on_comboBox_currentIndexChanged:index = %1", index);
    configChangeListener->onStoreStrategyChanged(index);
}

void Dialog::on_plc_addr_editingFinished()
{
    LOGD(TAG, "on_plc_addr_editingFinished:ip = %1", ui->plc_addr->text());
    QString newPlcAddr = ui->plc_addr->text();
    configChangeListener->onPlcAddrChanged(newPlcAddr);
}

void Dialog::on_display_addr_editingFinished()
{
    LOGD(TAG, "on_display_addr_editingFinished:ip = %1", ui->display_addr->text());
    QString newDisplayAddr = ui->display_addr->text();
    configChangeListener->onDisplayAddrChanged(newDisplayAddr);
}

void Dialog::on_apply_clicked()
{
    LOGD(TAG, "on_apply_clicked");
    this->close();
    //configChangeListener->onApplyClicked();
}

void Dialog::on_rootpath_editingFinished()
{
    LOGD(TAG, "on_rootpath_editingFinished:rootpath = %1", ui->rootpath->text());
    QString newPath = ui->rootpath->text();
    configChangeListener->onRootpathChanged(newPath);
}

void Dialog::on_plc_server_addr_editingFinished()
{
    LOGD(TAG, "on_plc_server_addr_editingFinished:plc_server_addr = %1", ui->plc_server_addr->text());
    QString newPlcServerAddr = ui->plc_server_addr->text();
    configChangeListener->onPlcServerAddrChanged(newPlcServerAddr);
}

void Dialog::on_storeTime_editingFinished()
{
    LOGD(TAG, "on_storeTime_editingFinished:storeTime = %1", ui->storeTime->text());
    QString newStoreTime = ui->storeTime->text();
    configChangeListener->onStoreTimeChanged(newStoreTime);
}
