#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <iostream>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&m_unitTimer, &QTimer::timeout, [this]{
        changeConstantForceMagnitude();
    });
    connect(&m_durationTimer, &QTimer::timeout, [this]{
        m_unitTimer.stop();
        m_durationTimer.stop();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initDirectApi()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if(FAILED(DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&m_pDirectInput, NULL)))
    {
        std::cout<<"DirectInput8 Create false"<< E_FAIL<<"\n";
    }
    else{
        std::cout<< "DirectInput8 Create complete"<<"\n";
    }
}

void MainWindow::findR16()
{
    m_directDevs.clear();
    if(!m_pDirectInput){
        std::cout << "DirectInput8 instantce is NULL"<<"\n";
        return;
    }

    auto result = m_pDirectInput->EnumDevices(
                DI8DEVCLASS_ALL,//DI8DEVTYPE_JOYSTICK,
                &MainWindow::enumDevicesCallback,
                this,//非NULL，null为不调用
                DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY
                );

    if(result != DI_OK){
        std::cout <<" ffb-device enum false"<<result<<"\n";
    }
    else{
        std::cout <<"ffb-device enum complete"<<"\n";
    }

}

/**
 * @brief MainWindow::createR16Device
 * create after find
 */
void MainWindow::createR16Device()
{
    if(!m_pDirectInput){
        std::cout<<"directinput8 instance is null, can not create device"<<"\n";
        return ;
    }
    //MOZA R16's VID&PID
    auto vid = 0x346e;
    auto pid = 0x0;
    auto devs = m_directDevs.find(std::pair(vid, pid));

    //find R16 in all devices
    if(devs == m_directDevs.end()){
        std::cout<<"can not find vid:"<<vid<<"   pid:"<<pid<<"\n";
        return;
    }
    else{
        std::cout<< "find R16 device"<<"\n";
        auto deviceInstance = m_directDevs[std::pair(vid, pid)];
        //create device
        if(FAILED( m_pDirectInput -> CreateDevice(
                       deviceInstance.guidInstance,
                       &m_currentConnectDevice,
                       NULL ))){
            std::cout<<"device create false"<<"\n";
            return;
        }else{
            std::cout<<"device create complete"<<"\n";
        }
    }
}

/**
 * @brief MainWindow::r16DevicePrepare
 * gain control
 */
bool MainWindow::r16DevicePrepare()
{
    if(!m_currentConnectDevice){
        std::cout<<"device acquire LPDIRECTINPUTDEVICE8 is null"<<"\n";
        return false;
    }

    if(FAILED(m_currentConnectDevice-> SetDataFormat(&c_dfDIJoystick2)))
    {
        std::cout<<"can not set data's format"<<"\n";
        m_currentConnectDevice->Release();
        m_currentConnectDevice = NULL;
        return false;
    }
    else{
        std::cout<<"set data's format complete"<<"\n";
    }

    auto hwnd = (HWND)(this->winId()); //Qt: get app window’s handle
    if(FAILED(m_currentConnectDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND|DISCL_EXCLUSIVE)))//线程不在前台
    {
        std::cout<<"can not Set Cooperative Level"<<"\n";
        m_currentConnectDevice->Release();
        m_currentConnectDevice = NULL;
        return false;
    }
    else{
        std::cout<<"Set Cooperative Level complete"<<"\n";
    }

    if(FAILED(m_currentConnectDevice->Acquire()))
    {
        std::cout<<"can not gain control of the device"<<"\n";
        return false;
    }
    else
    {
        std::cout<<"Gain control of the device complete"<<"\n";
    }
    return true;
}

/**
 * @brief MainWindow::sendNewConstantForceToR16
 * @param direction: ±90°
 * @param duration: 1s
 * @param Magnitude: max*10%
 */
void MainWindow::sendNewConstantForceToR16( LONG direction, LONG duration, LONG magnitude)
{

    if(!m_currentConnectDevice){
        std::cout<<"no device is connected, can not send any message"<<"\n";
        return;
    }

    DWORD dwAxes[2] = {DIJOFS_X, DIJOFS_Y};
    LONG lDirection[2] = {direction*DI_DEGREES, 0};

    DIENVELOPE diEnvelope;
    DIEFFECT diEffect;
    DICONSTANTFORCE diPeriodic;
    diPeriodic.lMagnitude = magnitude;

    diEnvelope.dwSize = sizeof(DIENVELOPE);
    diEnvelope.dwAttackLevel = 0;
    diEnvelope.dwAttackTime = 0;
    diEnvelope.dwFadeLevel = 0;
    diEnvelope.dwFadeTime = 0;

    //基本参数
    diEffect.dwSize = sizeof(DIEFFECT);
    diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration = (DWORD)(duration);

    diEffect.dwSamplePeriod = 0;
    diEffect.dwGain = DI_FFNOMINALMAX;
    diEffect.dwTriggerButton = DIEB_NOTRIGGER;;
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes = 2;
    diEffect.rgdwAxes = dwAxes;
    diEffect.rglDirection = &lDirection[0];
    diEffect.lpEnvelope = &diEnvelope;
    diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
    diEffect.lpvTypeSpecificParams = &diPeriodic;


    HRESULT hr = m_currentConnectDevice->CreateEffect(
                GUID_ConstantForce,
                &diEffect,
                &m_lpdiEffect,
                NULL);

    if(FAILED(hr)){
        std::cout<<"create constantForce effect false"<<"\n";
    }
    else{
        std::cout<<"create effect complete"<<"\n";
    }

    if(!m_lpdiEffect){
        std::cout<<"startEffect LPDIRECTINPUTEFFECT is null"<<"\n";
        return;
    }

    m_lpdiEffect->Start(1, 0);
}

/**
 * @brief MainWindow::beginEffectTest
 * target: create a new constant force(effect) to send to R16 wheelbase:
 * effect-period: 1s( 0.5s:positive direction + 0.5s: negative direction)
 * effect-duration: 2.5 period
 * effect-max torque: 10%
 */
void MainWindow::beginEffectTest()
{
    LONG magnitude = DI_FFNOMINALMAX*10/100;
    sendNewConstantForceToR16(m_direction, 2.5*DI_SECONDS, magnitude);
        m_unitTimer.start(10);
        m_durationTimer.start(2500);
}

/**
 * @brief MainWindow::changeConstantForceMagnitude
 * change effect'direction parameters
 * make constant force effect likes shine effect
 */
void MainWindow::changeConstantForceMagnitude()
{
    if(!m_lpdiEffect){
        std::cout<<"can not change effect, because no effect instance";
        return;
    }
    m_count += 10;

    auto magnitude = getMagnitudeWithDirection();
//    qDebug()<<"magnitude:"<<magnitude;
    DIEFFECT diEffect;
    diEffect.dwSize = sizeof(DIEFFECT);
    diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.cAxes = 2;
    DICONSTANTFORCE diPeriodic;
    diPeriodic.lMagnitude = magnitude;
    diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
    diEffect.lpvTypeSpecificParams = &diPeriodic;

    HRESULT hr = m_lpdiEffect->SetParameters(&diEffect, DIEP_TYPESPECIFICPARAMS);
    if(FAILED(hr)){
//        qDebug()<<QString::number( hr, 16);
        std::cout<<"update parameter false"<<hr;
    }
    else{
        std::cout<<"update parameter complete";
    }
}

/**
 * @brief MainWindow::enumDevicesCallback
 * @param lpddi //system input this param
 * @param pvRef //system input this param
 * @return user set the result
 */
BOOL MainWindow::enumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{

    quint16 vid;
    quint16 pid;

    vid = getVidFromGuid(lpddi->guidProduct);
    pid = getPidFromGuid(lpddi->guidProduct);

    //enum and save all device as map; key: vid&pid, value:device instance
    m_directDevs.try_emplace(std::pair(vid, pid), *lpddi);
    return DIENUM_CONTINUE;
}

/**
 * @brief MainWindow::getVidFromGuid
 * @param input:device's guid
 * @return device's vid
 */
quint16 MainWindow::getVidFromGuid(GUID input)
{
    return input.Data1;
}

/**
 * @brief MainWindow::getPidFromGuid
 * @param input:device's guid
 * @return device's pid
 */
quint16 MainWindow::getPidFromGuid(GUID input)
{
    quint16 output = input.Data1 >> 16;
    return output;
}

/**
 * @brief MainWindow::getMagnitudeWithDirection
 * The goal is to make a constant force look like a shine force
 * for example:Set the maximum/minimum limit, set the duration, every 1ms call this function,
 *  according to the sinusoidal change of the force magnitude and direction
 */
LONG MainWindow::getMagnitudeWithDirection()
{
    auto ffbSineValue = sin(m_passedTime / 1000.0 * 2 * 3.1415926);
    if(ffbSineValue<0){
        ffbSineValue = -ffbSineValue;
    }
//    qDebug()<<"passedTime:"<<m_passedTime<<"   ffbSineValue"<<ffbSineValue;
    LONG magnitude = DI_FFNOMINALMAX*10/100;

    LONG ffb = (int16_t)(ffbSineValue * (magnitude));

    m_passedTime += 10;

    if (m_passedTime >= 1000)
    {
        m_passedTime = 0;
    }

    return ffb;
}
std::map<std::pair<quint16, quint16>, DIDEVICEINSTANCE> MainWindow:: m_directDevs;

