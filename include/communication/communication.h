#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QModbusTcpClient>
#include <QModbusServer>

struct Detect_info
{
    int machine_state;
    int mStation;
    int current_detect_result;
    //int result_code;
    QVector<int> errorCodes;

    //for station1
    //int dryer_posX;
    //int dryer_posY;
};

class ConnectListener {
public:
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
};

class TransporterStopListener {
public:
    virtual void onTransporterStopped() = 0;
};

class ModbusService : public QObject
{
    Q_OBJECT

public:
    ModbusService(QString &addr);
    ~ModbusService();
    void onDataWritten(QModbusDataUnit::RegisterType table, int address, int size);
    void onStateChanged(int state);
    void handleDeviceError(QModbusDevice::Error newError);
    void setupDeviceData();
    void start_service();
    void stop_service();
    void setData(int id, QModbusDataUnit::RegisterType table, bool value);
    void setTransporterStopListener(TransporterStopListener *listener);

private:
    QModbusServer *modbusServer;
    TransporterStopListener *transporterStopListener;
};

class ModbusClient : public QObject
{
    Q_OBJECT
public:
    ModbusClient(QString &addr);
    ~ModbusClient();
    void connect_to_server();
    void disconnect_to_server();
    void onStateChanged(int state);
    void writeData(Detect_info &info);
    void readData();
    void readReady();
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest(int length) const;

    void setDetectInfo(Detect_info &info);
    void setConnectListener(ConnectListener *listener);

private:
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;
    ConnectListener *connectListener;

    void detectInfo_to_vector(Detect_info &info, QVector<int> &data);

    //Detect_info detect_info;
public:
    QString TAG;
    //WriteRegisterModel *writeModel;
};

class CommunicationService : public QObject
{
    Q_OBJECT
public:
    CommunicationService();
    ~CommunicationService();
    void start();
    void stop();
    void notifyDisplaySystem(Detect_info &info);
    void notifyPlcSystem(Detect_info &info);
    void setDetectInfo(Detect_info &info);
    void init(QString &plcAddr, QString &displayAddr, QString &plcServerAddr);
    void setConnectListener(ConnectListener *listener);
    void setTransporterStopListener(TransporterStopListener *listener);

private:
    ModbusClient *display_master;
    ModbusClient *plc_master;
    ModbusService *plc_server;

};

#endif // COMMUNICATION_H
