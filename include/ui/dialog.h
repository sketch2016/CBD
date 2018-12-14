#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "include/defines.h"

namespace Ui {
class Dialog;
}

class ConfigChangeListener {
public:
    virtual void onDebugModeChanged(bool debug) = 0;
    virtual void onStoreStrategyChanged(int newStrategy) = 0;
    virtual void onPlcAddrChanged(QString &newPlcAddr) = 0;
    virtual void onDisplayAddrChanged(QString &newDisplayAddr) = 0;
    virtual void onRootpathChanged(QString &newPath) = 0;
    virtual void onPlcServerAddrChanged(QString &newPlcServerAddr) = 0;
    virtual void onStoreTimeChanged(QString &newStoreTime) = 0;
    //virtual void onApplyClicked() = 0;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void setConfigChangeListener(ConfigChangeListener *listener);
    void applyConfig(Config_Params &params);

signals:
    void onDebugChanged(bool debug);

private slots:

    void on_debug_clicked(bool checked);

    void on_comboBox_currentIndexChanged(int index);

    void on_plc_addr_editingFinished();

    void on_display_addr_editingFinished();

    void on_apply_clicked();

    void on_rootpath_editingFinished();

    void on_plc_server_addr_editingFinished();

    void on_storeTime_editingFinished();

private:
    Ui::Dialog *ui;
    ConfigChangeListener *configChangeListener;
};

#endif // DIALOG_H
