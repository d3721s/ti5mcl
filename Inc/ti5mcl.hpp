#ifndef TI5MCL_HPP_INCLUDED
#define TI5MCL_HPP_INCLUDED
#include <iostream>
#include <cstdint>
#include <thread>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <chrono>

#include <CanDriver.hpp>
#include <exceptions/CanException.hpp>
#include <exceptions/CanInitException.hpp>
#include <exceptions/InvalidSocketException.hpp>
#include "tlog.h"


#define LOGLEVEL TLOG_DEBUG
#warning "LOGLEVEL-debug"

#ifndef CANINTERFACE
#define CANINTERFACE "can0"
#endif
#ifndef LOGLEVEL
#define LOGLEVEL TLOG_WARN
#endif

constexpr float PI = 3.1416;

namespace ti5mcl
{

using namespace std;
using namespace sockcanpp;
class ti5Motor
{
public:
    typedef enum
    {
        reductionRatio51 = 51,
        reductionRatio81 = 81,
        reductionRatio101 = 101,
        reductionRatio121 = 121,
    } reductionRatio;
    ti5Motor(uint8_t canId,
             reductionRatio reductionRatio)
    {
        static once_flag canDriverInitializedFlag;
        static once_flag tlogInitializedFlag;
        call_once(canDriverInitializedFlag, [this]()
                  mutable
        {
            try
            {
                _canDriver = make_shared<CanDriver>(CANINTERFACE,
                CAN_RAW);
            }
            catch (exceptions::CanInitException& ex)
            {
                cerr << "An error occurred while initialising CanDriver: "
                << ex.what()
                << endl;
                exit(1);
            }
        });
        call_once(tlogInitializedFlag, []
        {
            try
            {
                tlog_init("ti5motor.log", 1048576, 8, 0,
                TLOG_SCREEN | TLOG_SCREEN_COLOR);
            }
            catch (exception& e)
            {
                cerr << "log system init error" << e.what()
                << endl;
                exit(2);
            }
            tlog_setlevel(LOGLEVEL);
        });
        _canId = canId;
        _reductionRatio = reductionRatio;
        _canFrameSend.can_id = canId;
        _name = "ti5Motor" + to_string(canId);
    }
    ~ti5Motor()
    {
    }
public://工程1

public://常用1

    bool power(bool en);//使能控制
    bool reset();//清楚错误,建议使用this->autoMonitor()管理错误！
    bool home();//回原点
    bool halt();//急停
    bool moveAbsolute(float position,
                      float velocity); //绝对运动
    bool moveRelative(float distance,
                      float velocity); //相对运动
    bool moveVelocity(float velocity);//速度运动
    bool moveVelocity(float velocity,float position);//速度运动
    typedef enum
    {
        DIRECTIONPOSITIVE = 0,//正向
        DIRECTIONNEGATIVE = 1,//负向
    } Direction;//方向

    bool moveJog(float velocity); //点动
public://常用2
    bool quickSetMaxSpeed(float
                          maxSpeed);//设置最大速度
    bool quickSetMaxAcceleration(float
                                 maxAcceleration);//设置最大加速度
    bool quickSetMaxPosition(float
                             maxPosition); //设置最大位置
    bool quickSetMinPosition(float
                             minPosition); //设置最小位置
    bool quickSetEnableStatus(bool
                              status); //设置指令使能状态
    bool quickGetMaxSpeed(float*
                          maxSpeed); //获取最大速度
    bool quickGetMaxAcceleration(float*
                                 maxAcceleration); //获取最大加速度
    bool quickGetMaxPosition(float*
                             maxPosition); //获取最大位置
    bool quickGetMinPosition(float*
                             minPosition); //获取最小位置
    bool quickGetEnableStatus(bool*
                              status); //获取使能状态
    bool quickGetMotorTemperature(int16_t*
                                  temperature); //获取电机温度
    bool quickGetDriverTemperature(int16_t*
                                   temperature); //获取驱动器温度
public://常用3
    bool autoMonitor(bool enable);//自动监控
    bool autoCurrentSpeedPosition(bool enable,
                                  uint16_t period); //自动获取当前电流、速度、位置 单位5ms
    float autoCurrent() const;//当前电流
    float autoSpeed() const;//当前速度
    float autoPosition() const;//当前位置

public://扩展1

    bool customSetMaxCurrent(int32_t
                             maxCurrent);//设置最大电流
    bool customGetMaxCurrent(int32_t*
                             maxCurrent);//获取最大电流
    bool customGetErrorStatus(uint16_t*
                              errorStatus);//获取错误状态
    bool quickGetPositionOffset(int32_t*
                                offset);//获取位置偏移 /*注意单位*/
    bool quickSetPositionOffset(int32_t
                                offset);//设置位置偏移 /*注意单位*/

    bool customGetSpeedProportional(int32_t*
                                    proportional); //获取速度环比例
    bool customGetSpeedIntegral(int32_t*
                                integral); //获取速度积分
    // bool customGetSpeedDerivative(int32_t *derivative); //获取速度微分
    bool customGetPositionProportional(
        int32_t* proportional); //获取位置环比例
    // bool customGetPositionIntegral(int32_t *integral); //获取位置积分
    bool customGetPositionDerivative(int32_t*
                                     derivative); //获取位置微分
    bool customGetVoltage(int32_t*
                          voltage);  //获取母线电压
    bool customRestoreFromFlash();//从Flash恢复参数
    bool customSaveToFlash();//保存参数到Flash
    bool customRestoreFactory();//恢复出厂设置
    // bool customStoreToFactory();//储存参数到出厂

    bool customGetMotorModel(int32_t*
                             model); //获取电机型号
    bool customGetMotorVersion(int32_t*
                               version); //获取电机版本号
    bool customGetDriverVersion(int32_t*
                                version); //获取驱动器软件版本号
    bool customGetCurrentSpeedPosition();
    bool customGetEncoderVoltage(int32_t* voltage);
    bool customGetEncoderStatus(uint16_t* status);


private:
    typedef enum  //set
    {
        setStopModeCode1 = 2, // 停止电机
        setCleanErrorCode = 11, // 清除错误
        setRestoreFromFlashCode = 13,     // 从Flash恢复参数
        setSaveToFlashCode = 14,          // 保存参数到Flash
        setRestorFactoryCode = 15,            // 恢复出厂设置
        //  setStoreToFactoryCode = 31,         // 储存参数到出厂
    } parameterCodeTableSend1;

    typedef enum  // get
    {
        getModeCode = 3,                  // 模式
        getCurrentCode = 4,               // 当前电流
        getTargetCurrentCode = 5,         // 目标电流
        getVelocityCode = 6,              // 速度
        getTargetVelocityCode = 7,        // 目标速度
        getPositionCode = 8,              // 位置
        getTargetPositionCode = 9,        // 目标位置
        getErrorCode = 10,                // 错误
        getSpeedLoopKpCode = 16,          // 速度环P增益
        getSpeedLoopKiCode = 17,          // 速度环I增益
        getPositionLoopKpCode = 18,       // 位置环P增益
        getPositionLoopKdCode = 19,       // 位置环D增益
        getBusVoltageCode = 20,           // 总线电压
        getMaxPositiveAccelerationCode = 22,  // 最大正向加速度
        getMaxNegativeAccelerationCode = 23,  // 最大负向加速度
        getMaxPositiveVelocityCode = 24,      // 最大正向速度
        getMaxNegativeVelocityCode = 25,      // 最大负向速度
        getMaxPositivePositionCode = 26,      // 最大正向位置
        getMaxNegativePositionCode = 27,      // 最大负向位置
        getMotorTemperatureCode = 49,         // 电机温度
        getDriverTemperatureCode = 50,        // 驱动器温度
        getSpeedLoopKdCode = 51,          // 速度环D增益
        getPositionLoopKiCode = 52,       // 位置环I增益
        getMaxPositiveCurrentCode = 53,       // 最大正向电流
        getMaxNegativeCurrentCode = 54,       // 最大负向电流
        getMaxCurrentCode = 55,
        getPositionOffsetCode = 84,       // 位置偏移
        getCurrentLoopKpCode = 97,        // 电流环P增益
        getCurrentLoopKiCode = 98,        // 电流环I增益
        getCurrentLoopKdCode = 99,        // 电流环D增益
        getMotorModelCode = 100,              // 电机型号
        getMotorVersionNumberCode = 101,      // 电机版本号
        getMotorSoftwareVersionNumberCode = 102, // 电机软件版本号
        getEncoderVoltageCode = 120,          // 编码器电压
        getEncoderStatusCode = 121,           // 编码器状态
        getOverVoltageThresholdCode = 138,    // 过压阈值
        getLowVoltageThresholdCode = 140,     // 低压阈值
        getMotorOverTemperatureThresholdCode = 143, // 电机过温阈值
        getDriverOverTemperatureThresholdCode = 147  // 驱动器过温阈值
    } parameterCodeTableSend1Receive1_4;

    typedef enum  //get
    {
        getCurrentSpeedPositionCode = 65, // 循环同步位置
    } parameterCodeTableSend1Receive2_2_4;

    typedef enum   // set
    {
        setStopMotorCode5 = 2, // 停止电机
        setCurrentModeCode = 28, // 电流模式
        setVelocityModeCode = 29, // 速度模式
        setPositionModeCode = 30, // 位置模式
        setMaxPositiveCurrentCode = 32, // 最大正向电流
        setMaxNegativeCurrentCode = 33, // 最大负向电流
        setMaxPositiveAccelerationCode = 34, // 最大正向加速度
        setMaxNegativeAccelerationCode = 35, // 最大负向加速度
        setMaxPositiveVelocityCode = 36, // 最大正向速度
        setMaxNegativeVelocityCode = 37, // 最大负向速度
        setMaxPositivePositionCode = 38, // 最大正向位置
        setMaxNegativePositionCode = 39, // 最大负向位置
        setSpeedLoopKpCode = 41, // 速度环比例增益
        setSpeedLoopKiCode = 42, // 速度环积分增益
        setPositionLoopKpCode = 43, // 位置环比例增益
        setPositionLoopKiCode = 44, // 位置环积分增益
        setPositionLoopKdCode = 45, // 位置环微分增益
        setCanIdCode = 46, // CAN ID
        setBaudRateCode = 63, // 波特率
        // setEncoderZeroCode = 80, // 设置编码器零点
        setPositionOffsetCode = 83, // 位置偏移
        // setDualEncoderCode = 85, // 双编码器
        setCurrentCirclerPositionLimitCode = 85,   //设置电流环位置限制
        setOverVoltageThresholdCode = 135,  // 过压阈值
        setLowVoltageThresholdCode = 137, // 低压阈值
        setMaxCoilVoltageCode = 141,
        setMaxDriverVoltageCode = 145,
    } parameterCodeTableSend1_4;

    typedef enum
    {
        setTiming = 89,
    } parameterCodeTableSend1_2_2;


    typedef enum  // gsetet
    {
        setCurrentCode = 66, // 实际电流
        setVelocityCode = 67, // 实际速度
        setPositionCode = 68, // 实际位置
    } parameterCodeTableSend1_4Receive2_2_4;

    typedef enum  // set
    {
        setInterpolationPositionModeCode = 86,
    } parameterCodeTableSend1_4_1;

    typedef enum  // set
    {
        setPositionAndVelocityCode = 88,
    } parameterCodeTableSend1_4_2;

    typedef enum  // set
    {
#warning "TODO:8 bytes order"
    } parameterCodeTableSend8;

    bool writeParameter(parameterCodeTableSend1
                        parameterCode);
    bool writeParameter(parameterCodeTableSend1_4
                        parameterCode, int32_t value);
    bool writeParameter(parameterCodeTableSend1_2_2
                        parameterCode, uint16_t value1,
                        uint16_t value2);
    bool writeParameter(parameterCodeTableSend1_4_1
                        parameterCode, int32_t value1,
                        uint8_t value2);
    bool writeParameter(parameterCodeTableSend1_4_2
                        parameterCode, int32_t value1,
                        uint16_t value2);
    //bool writeParameter(parameterCodeTableSend8 parameterCode,uint64_t value);
#warning "TODO:8 bytes order"
    bool readParameter(
        parameterCodeTableSend1Receive1_4 parameterCode,
        int32_t* value);
    bool readParameter(
        parameterCodeTableSend1Receive2_2_4 parameterCode,
        int16_t* value1, int16_t* value2,
        int32_t* value3);
    bool writeAndReadParameter(
        parameterCodeTableSend1_4Receive2_2_4
        parameterCode,
        int32_t value, int16_t* value1, int16_t* value2,
        int32_t* value3);

    static shared_ptr<CanDriver> _canDriver;
    uint8_t _canId;
    reductionRatio _reductionRatio;
    can_frame _canFrameSend;
    can_frame _canFrameReceive;
    bool _status = true;
    bool _autoStatus = false;
    volatile int32_t _autoCurrent;
    volatile float _autoSpeed;
    volatile float _autoPosition;
    int32_t _autoSpeedRaw;
    int32_t _autoPositionRaw;
    string _name;
    mutex canMutex;
};

}
#endif // TI5MCL_HPP_INCLUDED
