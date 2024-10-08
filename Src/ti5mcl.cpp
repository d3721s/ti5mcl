#include "ti5mcl.hpp"
using namespace std;
using namespace sockcanpp;
using namespace sockcanpp::exceptions;
namespace ti5mcl
{
shared_ptr<CanDriver> ti5Motor::_canDriver;

bool ti5Motor::power(bool en)
{
    return true;
}

bool ti5Motor::reset()
{
    tlog_info << "Resetting!" << endl;
    return writeParameter(setCleanErrorCode);
}

bool ti5Motor::home()
{
    tlog_info << "Homing!" << endl;
    return moveAbsolute(0, PI / 6);
}

bool ti5Motor::halt()
{
    tlog_info << "Halting!" << endl;
    return writeParameter(setStopModeCode1);
}

bool ti5Motor::moveAbsolute(float position,
                            float velocity)
{
    tlog_info << "Moving to " << to_string(position) << " with velocity " << to_string(velocity) << endl;
    return writeParameter(setPositionAndVelocityCode,
                          position * static_cast<uint8_t>(_reductionRatio) *
                          32768 / PI,
                          velocity * static_cast<uint8_t>(_reductionRatio) * 50 / PI);
}

bool ti5Motor::moveRelative(float distance,
                            float velocity)
{
    tlog_info << "Moving " << to_string(distance) << " with velocity " << to_string(velocity) << endl;
    if (!_autoStatus)
    {
        if (!readParameter(getPositionCode, &_autoPositionRaw))
            return false;
    }
    return writeParameter(setPositionAndVelocityCode,
                          _autoPositionRaw + distance *
                          static_cast<uint8_t>(_reductionRatio) * 32768 /
                          PI,
                          velocity * static_cast<uint8_t>(_reductionRatio) * 50 / PI);
}

bool ti5Motor::moveVelocity(float velocity, float position)
{
    tlog_info << "Moving with velocity " << to_string(velocity) << endl;
    if (!_autoStatus)
        autoCurrentSpeedPosition(1, 400);
    if (!writeParameter(setVelocityModeCode, velocity * static_cast<uint8_t>(_reductionRatio) * 50 / PI))
        return false;
#warning "timing!"
    while (1)
    {
        if (fabs(_autoPosition - position <= 0.01))
            return true;
    }
}

bool ti5Motor::moveJog(float velocity)
{
    tlog_info << "Joging with velocity " << to_string(velocity) << endl;
    return true;
}

bool ti5Motor::quickSetMaxSpeed(float maxSpeed)
{
    tlog_info << "Setting max speed to " << to_string(maxSpeed) << endl;
    return writeParameter(setMaxPositiveVelocityCode, maxSpeed * static_cast<uint8_t>(_reductionRatio) * 50 / PI) && writeParameter(setMaxNegativeVelocityCode, maxSpeed * static_cast<uint8_t>(_reductionRatio) * 50 / PI);
}

bool ti5Motor::quickSetMaxAcceleration(
    float maxAcceleration)
{
    tlog_info << "Setting max acceleration to " << to_string(maxAcceleration) << endl;
    return writeParameter(setMaxPositiveAccelerationCode, maxAcceleration * static_cast<uint8_t>(_reductionRatio) * 50 / PI) && writeParameter(setMaxNegativeAccelerationCode, maxAcceleration * static_cast<uint8_t>(_reductionRatio) * 50 / PI);
}

bool ti5Motor::quickSetMaxPosition(
    float maxPosition)
{
    tlog_info << "Setting max position to " << to_string(maxPosition) << endl;

    return writeParameter(setMaxPositivePositionCode, maxPosition * static_cast<uint8_t>(_reductionRatio) * 32768 / PI);
}

bool ti5Motor::quickSetMinPosition(
    float minPosition)
{
    tlog_info << "Setting min position to " << to_string(minPosition) << endl;
    return writeParameter(setMaxNegativePositionCode, minPosition * static_cast<uint8_t>(_reductionRatio) * 32768 / PI);
}

bool ti5Motor::quickGetMaxSpeed(float *maxSpeed)
{
    tlog_info << "Getting max speed!" << endl;
    int32_t _maxSpeedRaw;
    if (!readParameter(getMaxPositiveVelocityCode, &_maxSpeedRaw))
        return false;
    *maxSpeed = _maxSpeedRaw * PI / 50 / static_cast<uint8_t>(_reductionRatio);
    return true;
}

bool ti5Motor::quickGetMaxAcceleration(
    float *maxAcceleration)
{
    int32_t _maxAccelerationRaw;
    tlog_info << "Getting max acceleration!" << endl;
    if (readParameter(getMaxPositiveAccelerationCode, &_maxAccelerationRaw) == false)
        return false;
    *maxAcceleration = _maxAccelerationRaw * PI / 50 / static_cast<uint8_t>(_reductionRatio);
#warning "maxAcceleration != maxSpeed"
    return true;
}

bool ti5Motor::quickGetMaxPosition(
    float *maxPosition)
{
    int32_t _maxPositionRaw;
    tlog_info << "Getting max position!" << endl;
    if (!readParameter(getMaxPositivePositionCode, &_maxPositionRaw))
        return false;
    *maxPosition = _maxPositionRaw * PI / static_cast<uint8_t>(_reductionRatio) / 32768;
    return true;
}

bool ti5Motor::quickGetMinPosition(
    float *minPosition)
{
    tlog_info << "Getting min position!" << endl;
    int32_t _minPositionRaw;
    if (!readParameter(getMaxNegativePositionCode, &_minPositionRaw))
        return false;
    *minPosition = _minPositionRaw * PI / static_cast<uint8_t>(_reductionRatio) / 32768;
    return true;
}

bool ti5Motor::quickGetEnableStatus(bool *status)
{
    tlog_info << "Getting enable status!" << endl;
    return true;
}

bool ti5Motor::autoMonitor(bool enable)
{

    return true;
}

bool ti5Motor::autoCurrentSpeedPosition(
    bool enable, uint16_t period)
{
    return true;
}

float ti5Motor::autoCurrent() const
{
    return _autoCurrent;
}

float ti5Motor::autoSpeed() const
{
    return _autoSpeed;
}

float ti5Motor::autoPosition() const
{
    return _autoPosition;
}

bool ti5Motor::writeParameter(
    parameterCodeTableSend1 parameterCode)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 1;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    return true;
}
bool ti5Motor::writeParameter(
    parameterCodeTableSend1_4 parameterCode,
    int32_t value)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 5;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    _canFrameSend.data[1] = static_cast<uint8_t>((
                                value & 0xFF));
    _canFrameSend.data[2] = static_cast<uint8_t>(((
                                value >> 8) &
                            0xFF));
    _canFrameSend.data[3] = static_cast<uint8_t>(((
                                value >> 16) &
                            0xFF));
    _canFrameSend.data[4] = static_cast<uint8_t>(((
                                value >> 24) &
                            0xFF));
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    return true;
}

bool ti5Motor::writeParameter(
    parameterCodeTableSend1_2_2 parameterCode,
    uint16_t value1, uint16_t value2)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 5;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    _canFrameSend.data[1] = static_cast<uint8_t>((
                                value1 & 0xFF));
    _canFrameSend.data[2] = static_cast<uint8_t>(((
                                value1 >> 8) &
                            0xFF));
    _canFrameSend.data[3] = static_cast<uint8_t>((
                                value2 & 0xFF));
    _canFrameSend.data[4] = static_cast<uint8_t>(((
                                value2 >> 8) &
                            0xFF));
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    return true;
}

bool ti5Motor::writeParameter(
    parameterCodeTableSend1_4_1 parameterCode,
    int32_t value1, uint8_t value2)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 6;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    _canFrameSend.data[1] = static_cast<uint8_t>((
                                value1 & 0xFF));
    _canFrameSend.data[2] = static_cast<uint8_t>(((
                                value1 >> 8) &
                            0xFF));
    _canFrameSend.data[3] = static_cast<uint8_t>(((
                                value1 >> 16) &
                            0xFF));
    _canFrameSend.data[4] = static_cast<uint8_t>(((
                                value1 >> 24) &
                            0xFF));
    _canFrameSend.data[5] = value2;
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    return true;
}

bool ti5Motor::writeParameter(
    parameterCodeTableSend1_4_2 parameterCode,
    int32_t value1, uint16_t value2)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 7;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    _canFrameSend.data[1] = static_cast<uint8_t>((
                                value1 & 0xFF));
    _canFrameSend.data[2] = static_cast<uint8_t>(((
                                value1 >> 8) &
                            0xFF));
    _canFrameSend.data[3] = static_cast<uint8_t>(((
                                value1 >> 16) &
                            0xFF));
    _canFrameSend.data[4] = static_cast<uint8_t>(((
                                value1 >> 24) &
                            0xFF));
    _canFrameSend.data[5] = static_cast<uint8_t>((
                                value2 & 0xFF));
    _canFrameSend.data[6] = static_cast<uint8_t>(((
                                value2 >> 8) &
                            0xFF));
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);

    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    return true;
}

bool ti5Motor::readParameter(
    parameterCodeTableSend1Receive1_4 parameterCode,
    int32_t *value)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 1;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);

    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    if (!_canDriver->waitForMessages())
    {
        tlog_error << "Receive Timeout! " << endl;
        return false;
    }
    _canFrameReceive = _canDriver->readMessage().getRawFrame();
    if (((_canFrameReceive.can_id) !=
            _canFrameSend.can_id) ||
            (_canFrameReceive.can_dlc != 5) ||
            (_canFrameReceive.data[0] != parameterCode))
    {
        tlog_error << "Receive Wrong Message! " << endl;
        stringstream ssReceived;
        for (auto i : _canFrameReceive.data)
        {
            ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
        }
        tlog_debug << "Received message!" << ssReceived.str() << endl;
        return false;
    }
    stringstream ssReceived;
    for (auto i : _canFrameReceive.data)
    {
        ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Received message!" << ssReceived.str() << endl;
    *value = static_cast<int32_t>(_canFrameReceive.data[1]) |
             (static_cast<int32_t>(_canFrameReceive.data[2]) << 8) |
             (static_cast<int32_t>(_canFrameReceive.data[3]) << 16) |
             (static_cast<int32_t>(_canFrameReceive.data[4]) << 24);
    return true;
}

bool ti5Motor::readParameter(
    parameterCodeTableSend1Receive2_2_4 parameterCode,
    int16_t *value1, int16_t *value2, int32_t *value3)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 1;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    if (!_canDriver->waitForMessages())
    {
        tlog_error << "Receive Timeout! " << endl;
        return false;
    }
    _canFrameReceive = _canDriver->readMessage().getRawFrame();
    if (((_canFrameReceive.can_id) !=
            _canFrameSend.can_id) ||
            (_canFrameReceive.can_dlc != 5) ||
            (_canFrameReceive.data[0] != parameterCode))
    {
        tlog_error << "Receive Wrong Message! " << endl;
        stringstream ssReceived;
        for (auto i : _canFrameReceive.data)
        {
            ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
        }
        tlog_debug << "Received message!" << ssReceived.str() << endl;
        return false;
    }
    stringstream ssReceived;
    for (auto i : _canFrameReceive.data)
    {
        ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Received message!" << ssReceived.str() << endl;
    *value1 = static_cast<int16_t>(_canFrameReceive.data[0]) |
              (static_cast<int16_t>(_canFrameReceive.data[1]) << 8);

    *value2 = static_cast<int16_t>(_canFrameReceive.data[2]) |
              (static_cast<int16_t>(_canFrameReceive.data[3]) << 8);

    *value3 = static_cast<int32_t>(_canFrameReceive.data[4]) |
              (static_cast<int32_t>(_canFrameReceive.data[5]) << 8) |
              (static_cast<int32_t>(_canFrameReceive.data[6]) << 16) |
              (static_cast<int32_t>(_canFrameReceive.data[7]) << 24);
    return true;
}

bool ti5Motor::writeAndReadParameter(
    parameterCodeTableSend1_4Receive2_2_4
    parameterCode,
    int32_t value, int16_t *value1,
    int16_t *value2, int32_t *value3)
{
    _canFrameSend.can_id = this->_canId;
    _canFrameSend.can_dlc = 5;
    _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
    _canFrameSend.data[1] = static_cast<uint8_t>((
                                value & 0xFF));
    _canFrameSend.data[2] = static_cast<uint8_t>(((
                                value >> 8) &
                            0xFF));
    _canFrameSend.data[3] = static_cast<uint8_t>(((
                                value >> 16) &
                            0xFF));
    _canFrameSend.data[4] = static_cast<uint8_t>(((
                                value >> 24) &
                            0xFF));
    stringstream ssSend;
    for (auto i : _canFrameSend.data)
    {
        ssSend << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Sent message!" << ssSend.str() << endl;
    lock_guard<mutex> lock(canMutex);
    try
    {
        _canDriver->sendMessage(CanMessage(
                                    _canFrameSend));
    }
    catch (CanException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    catch (InvalidSocketException &ex)
    {
        tlog_error << "Failed to send test message! " << ex.what() << endl;
        return false;
    }
    if (!_canDriver->waitForMessages())
    {
        tlog_error << "Receive Timeout! " << endl;
        return false;
    }
    _canFrameReceive = _canDriver->readMessage().getRawFrame();

    if (((_canFrameReceive.can_id) !=
            _canFrameSend.can_id) ||
            (_canFrameReceive.can_dlc != 5) ||
            (_canFrameReceive.data[0] != parameterCode))
    {
        tlog_error << "Receive Wrong Message! " << endl;
        stringstream ssReceived;
        for (auto i : _canFrameReceive.data)
        {
            ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
        }
        tlog_debug << "Received message!" << ssReceived.str() << endl;
        return false;
    }
    stringstream ssReceived;
    for (auto i : _canFrameReceive.data)
    {
        ssReceived << setw(2) << setfill('0') << hex << static_cast<int>(i) << " ";
    }
    tlog_debug << "Received message!" << ssReceived.str() << endl;
    *value1 = (_canFrameReceive.data[0]) |
              (_canFrameReceive.data[1] << 8);
    *value2 = (_canFrameReceive.data[2]) |
              (_canFrameReceive.data[3] << 8);
    *value3 = (_canFrameReceive.data[4]) |
              (_canFrameReceive.data[5] << 8) |
              (_canFrameReceive.data[6] << 16) |
              (_canFrameReceive.data[7] << 24);
    return true;
}

} // namespace ti5mcl
