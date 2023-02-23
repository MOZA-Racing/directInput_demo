#pragma once

#include <QMainWindow>
#include <dinput.h>
#include <map>
#include <utility>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initDirectApi();                   // initialization directInput8 api object
    void findR16();                         // find target device， example Moza R16
    void createR16Device();                 // create after find
    bool r16DevicePrepare();                // preparatory work
    void sendNewConstantForceToR16( LONG direction, LONG duration, LONG magnitude);       // create constant force to Control Moza R16

    void beginEffectTest();
    void changeConstantForceMagnitude();
    static BOOL CALLBACK enumDevicesCallback( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef ); //directInput8 api's EnumDevices use it

private:
    //tool function
    static quint16 getVidFromGuid(GUID input);
    static quint16 getPidFromGuid(GUID input);
    LONG getMagnitudeWithDirection();
private:
    Ui::MainWindow *ui;
    LPDIRECTINPUT8 m_pDirectInput = NULL;                                        //API instance
    static std::map<std::pair<quint16, quint16>, DIDEVICEINSTANCE> m_directDevs; //all devices
    LPDIRECTINPUTDEVICE8 m_currentConnectDevice = NULL;                          //device instance
    LPDIRECTINPUTEFFECT m_lpdiEffect = NULL;                                     //effect instance


    //timer
    QTimer m_unitTimer;         // the unit time of change constant force
    QTimer m_durationTimer;     // duration

    //direction
    LONG m_direction = 90;   //degree

    //The time has passed
    LONG m_passedTime = 0;

    LONG m_count = 0;

};
