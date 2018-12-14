#include "include/communication/communication.h"
#include "include/util/logger.h"

#include <QUrl>
#include <QStatusBar>
#include <QModbusTcpServer>
#include <iostream>

#define TAG_COMMUNICATION "Communication"
#define TAG_DISPLAY_SERVICE "displayservice"
#define TAG_PLCCLIENT "plcclient"
#define TAG_MODBUS_SERVICE "modbus_server"

#define WRITE_VALUE_COUNT 10
#define READ_VALUE_COUNT 10
#define CONNECT_TIMEPUT 1000
#define RETRY_COUNT 3
#define WRITE_START_ADDR 0
#define READ_START_ADDR 0
#define READ_SERVER_ADDR 1
#define WRITE_SERVER_ADDR 1


CommunicationService::CommunicationService()
{
}

CommunicationService::~CommunicationService()
{
    if (plc_master)
        delete plc_master;

    if (display_master)
        delete display_master;
}

void CommunicationService::init(QString &plcAddr, QString &displayAddr, QString &plcServerAddr)
{
    plc_master = new ModbusClient(plcAddr);
    display_master = new ModbusClient(displayAddr);
    plc_server = new ModbusService(plcServerAddr);

    display_master->TAG = "display_master";
    plc_master->TAG = "plc_master";
}

void CommunicationService::start()
{
    LOGD(TAG_COMMUNICATION, "start");
    plc_server->start_service();

    plc_master->connect_to_server();
    display_master->connect_to_server();
}

void CommunicationService::stop()
{
    LOGD(TAG_COMMUNICATION, "stop");
    display_master->disconnect_to_server();
    plc_master->disconnect_to_server();

    plc_server->stop_service();
}

void CommunicationService::setConnectListener(ConnectListener *listener)
{
    plc_master->setConnectListener(listener);
    //display_master->setConnectListener(listener);
}

void CommunicationService::setTransporterStopListener(TransporterStopListener *listener)
{
    plc_server->setTransporterStopListener(listener);
}

void CommunicationService::notifyDisplaySystem(Detect_info &info)
{
    LOGD(TAG_COMMUNICATION, "notifyDisplaySystem");
    display_master->writeData(info);
}

void CommunicationService::notifyPlcSystem(Detect_info &info)
{
    LOGD(TAG_COMMUNICATION, "notifyPlcSystem");
    plc_master->writeData(info);
}

void CommunicationService::setDetectInfo(Detect_info &info)
{
    LOGD(TAG_COMMUNICATION, "setDetectInfo");
    //parallel to do
    notifyDisplaySystem(info);
    notifyPlcSystem(info);
}


ModbusService::~ModbusService()
{
    if (modbusServer)
        modbusServer->disconnectDevice();
    delete modbusServer;
}

ModbusService::ModbusService(QString &addr):
    modbusServer(nullptr),
    transporterStopListener(nullptr)
{
    modbusServer = new QModbusTcpServer();
    if (!modbusServer) {
        LOGD(TAG_MODBUS_SERVICE, "Could not create Modbus server");
    } else {
        QModbusDataUnitMap reg;
        reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 15 });
        reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
        reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
        reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 10 });

        modbusServer->setMap(reg);

        connect(modbusServer, &QModbusServer::dataWritten,
                this, &ModbusService::onDataWritten);
        connect(modbusServer, &QModbusServer::stateChanged,
                this, &ModbusService::onStateChanged);
        connect(modbusServer, &QModbusServer::errorOccurred,
                this, &ModbusService::handleDeviceError);

        modbusServer->setValue(QModbusServer::ListenOnlyMode, true);
        modbusServer->setValue(QModbusServer::DeviceBusy, 0x0000);

        const QUrl url = QUrl::fromUserInput(addr);
        modbusServer->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusServer->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        modbusServer->setServerAddress(WRITE_SERVER_ADDR);

        setupDeviceData();
    }
}

void ModbusService::setupDeviceData()
{
    if (!modbusServer)
        return;

    const bool checked = true;
    for (int i = 0; i < READ_VALUE_COUNT + 5; ++i)
        modbusServer->setData(QModbusDataUnit::Coils, i, checked);

    for (int i = 0; i < READ_VALUE_COUNT; ++i) {
        modbusServer->setData(QModbusDataUnit::DiscreteInputs, i, checked);
    }

    bool ok;
    for (int i = 0; i < READ_VALUE_COUNT; ++i) {
        modbusServer->setData(QModbusDataUnit::InputRegisters, i, 0);
    }

    for (int i = 0; i < READ_VALUE_COUNT; ++i) {
        modbusServer->setData(QModbusDataUnit::HoldingRegisters, i, 0xab20);
    }
}

void ModbusService::setData(int id, QModbusDataUnit::RegisterType table, bool value)
{
    if (!modbusServer)
        return;

    if (!modbusServer->setData(table, id, value))
    {
        //statusBar()->showMessage(tr("Could not set data: ") + modbusDevice->errorString(), 5000);
    }

}

void ModbusService::start_service()
{
    bool intendToConnect = (modbusServer->state() == QModbusDevice::UnconnectedState);
    if (intendToConnect) {
        if (!modbusServer->connectDevice()) {
            LOGD(TAG_MODBUS_SERVICE, "Connect failed:%1", modbusServer->errorString());
        } else {
            LOGD(TAG_MODBUS_SERVICE, "ModbusService:Connected");
        }
    }
}

void ModbusService::stop_service()
{
    bool intendToStop = (modbusServer->state() == QModbusDevice::ConnectedState);
    if (intendToStop) {
        modbusServer->disconnectDevice();
    }
}

void ModbusService::onDataWritten(QModbusDataUnit::RegisterType table, int address, int size)
{
    //to do
    QString out_str = QString("table:%1 address:[%2] size:%3")
                .arg(table)
                .arg(address)
                .arg(size);
    LOGD(TAG_MODBUS_SERVICE, "onDataWritten--%1", out_str);

    for (int i = 0; i < size; ++i) {
        quint16 value;
        switch (table) {
        case QModbusDataUnit::Coils:
            modbusServer->data(QModbusDataUnit::Coils, address + i, &value);
            break;
        case QModbusDataUnit::HoldingRegisters:
            modbusServer->data(QModbusDataUnit::HoldingRegisters, address + i, &value);
            break;
        default:
            break;
        }
        LOGD(TAG_MODBUS_SERVICE, "onDataWritten--value=%1", value);
        if (WRITE_START_ADDR == address && value == 1) {
            if (transporterStopListener != nullptr) {
                transporterStopListener->onTransporterStopped();
                break;
            }
        }
    }
}

void ModbusService::setTransporterStopListener(TransporterStopListener *listener)
{
    transporterStopListener = listener;
}

void ModbusService::onStateChanged(int state)
{
    LOGD(TAG_MODBUS_SERVICE, "onStateChanged state = %1", state);
    bool connected = (state != QModbusDevice::UnconnectedState);

    if (state == QModbusDevice::UnconnectedState) {
        LOGD(TAG_MODBUS_SERVICE, "onStateChanged Unconnected");
    } else if (state == QModbusDevice::ConnectedState) {
        LOGD(TAG_MODBUS_SERVICE, "onStateChanged Connected");
    }
}

void ModbusService::handleDeviceError(QModbusDevice::Error newError)
{
    if (newError == QModbusDevice::NoError || !modbusServer)
        return;

    LOGD(TAG_MODBUS_SERVICE, "handleDeviceError newError = %1", newError);
}

ModbusClient::ModbusClient(QString &addr):
    lastRequest(nullptr)
    , modbusDevice(nullptr)
    , connectListener(nullptr)
{
    modbusDevice = new QModbusTcpClient(this);
    const QUrl url = QUrl::fromUserInput(addr);
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
    modbusDevice->setTimeout(CONNECT_TIMEPUT);
    modbusDevice->setNumberOfRetries(RETRY_COUNT);

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        LOGD(TAG, "error:%1", modbusDevice->errorString());
    });
    connect(modbusDevice, &QModbusClient::stateChanged,
            this, &ModbusClient::onStateChanged);

}

ModbusClient::~ModbusClient()
{
    if (modbusDevice->state() == QModbusDevice::ConnectedState)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
}

void ModbusClient::onStateChanged(int state)
{
    LOGD(TAG, "onStateChanged state = %1", state);
    bool connected = (state != QModbusDevice::UnconnectedState);

    if (state == QModbusDevice::UnconnectedState)
    {
        QString addr = this->modbusDevice->connectionParameter(QModbusDevice::NetworkAddressParameter).toString()
                + ":" + this->modbusDevice->connectionParameter(QModbusDevice::NetworkPortParameter).toString();
        LOGD(TAG, "onStateChanged Unconnected:addr=%1", addr);
        if (connectListener != nullptr) {
            connectListener->onDisconnected();
        }
    }
    else if (state == QModbusDevice::ConnectedState)
    {
        LOGD(TAG, "onStateChanged Connected");
        if (connectListener != nullptr) {
            connectListener->onConnected();
        }
        Detect_info info = {
            1,1,0,{1,2,3,4}
        };
        writeData(info);
    }
}

void ModbusClient::setConnectListener(ConnectListener *listener)
{
    this->connectListener = listener;
}

void ModbusClient::connect_to_server()
{
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        if (!modbusDevice->connectDevice()) {
            LOGD(TAG, "Connect failed:", modbusDevice->errorString());
        } else {
            LOGD(TAG, "Connected");
        }
    }
}

void ModbusClient::disconnect_to_server()
{
    if (modbusDevice->state() == QModbusDevice::ConnectedState)
        modbusDevice->disconnectDevice();
}

void ModbusClient::writeData(Detect_info &info)
{
    if (!modbusDevice || modbusDevice->state() != QModbusDevice::ConnectedState)
        return;

    QVector<int> data;
    detectInfo_to_vector(info, data);
    QModbusDataUnit writeUnit = writeRequest(data.length());
    QModbusDataUnit::RegisterType table = writeUnit.registerType();
    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        if (table == QModbusDataUnit::HoldingRegisters)
            writeUnit.setValue(i, data.at(i));
    }
    QString error_codes;
    for (int i=0; i<info.errorCodes.length();i++) {
        error_codes.append("_").append(QString::number(info.errorCodes.at(i)));
    }

    QString send_data = QString("writeData: state=%1, station=%2, result=%3, error_codes's length=%5 error_codes=%4")
                .arg(data.at(0))
                .arg(data.at(1))
                .arg(data.at(2))
                .arg(error_codes)
                .arg(data.at(3));
    LOGD(TAG, send_data);

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, WRITE_SERVER_ADDR)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]()  {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    LOGD(TAG, "Write response error: %1 (Mobus exception: 0x%2)", reply->errorString(), reply->rawResult().exceptionCode());
                } else if (reply->error() != QModbusDevice::NoError) {
                    LOGD(TAG, "Write response error: %1 (code: 0x%2)", reply->errorString(), reply->error());
                }
                reply->deleteLater();
            });
            LOGD(TAG, "reply has not finished");
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
            LOGD(TAG, "reply has finished");
        }
    } else {
        LOGD(TAG, "Write error:: %1", modbusDevice->errorString());
    }
}

QModbusDataUnit ModbusClient::writeRequest(int length) const
{
    const auto table = QModbusDataUnit::RegisterType::HoldingRegisters;
    //static_cast<QModbusDataUnit::RegisterType> (ui->writeTable->currentData().toInt());

    int startAddress = WRITE_START_ADDR;
    Q_ASSERT(startAddress >= 0 && startAddress < length);

    int numberOfEntries = length;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

void ModbusClient::readData()
{
    LOGD(TAG, "readData");
    if (auto *reply = modbusDevice->sendReadRequest(readRequest(), READ_SERVER_ADDR)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModbusClient::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        LOGD(TAG, "readData error:: %1", modbusDevice->errorString());
    }
}

void ModbusClient::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            //ui->readValue->addItem(entry);
            LOGD(TAG, "read entry: %1", entry);
            //int transfer_speed ;
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        LOGD(TAG, "read response error: %1 (Mobus exception: 0x%2)", reply->errorString(), reply->rawResult().exceptionCode());
    } else {
        LOGD(TAG, "read response error: %1 (code: 0x%2)", reply->errorString(), reply->error());
    }

    reply->deleteLater();
}

QModbusDataUnit ModbusClient::readRequest() const
{
    const auto table = QModbusDataUnit::RegisterType::HoldingRegisters;
    //static_cast<QModbusDataUnit::RegisterType> (ui->writeTable->currentData().toInt());

    int startAddress = READ_START_ADDR;
    Q_ASSERT(startAddress >= 0 && startAddress < READ_VALUE_COUNT);

    // do not go beyond 10 entries
    int numberOfEntries = READ_VALUE_COUNT;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

void ModbusClient::setDetectInfo(Detect_info &info)
{
    //this->detect_info = info;
}

void ModbusClient::detectInfo_to_vector(Detect_info &info, QVector<int> &data)
{
    data.append(info.machine_state);
    data.append(info.mStation);
    data.append(info.current_detect_result);
    data.append(info.errorCodes.length());
    data.append(info.errorCodes);
}
