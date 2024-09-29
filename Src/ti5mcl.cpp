#include "ti5mcl.hpp"
using namespace std;
using namespace sockcanpp;
using namespace sockcanpp::exceptions;
namespace ti5mcl
{
    bool ti5Motor::power(bool en)
    {
    }

    bool ti5Motor::reset()
    {
    }

    bool ti5Motor::home()
    {
        tlog_info << "Homing!" << endl;
        return moveAbsolute(0, PI / 6);
    }

    bool ti5Motor::halt()
    {
        return true;
    }

    bool ti5Motor::moveAbsolute(float position,
                                float velocity)
    {
        tlog_info << "Moving to " << to_string(
                      position) << " with velocity " << to_string(
                      velocity) << endl;
        return writeParameter(setPositionAndVelocityCode,
                              position * static_cast<uint8_t>(_reductionRatio) *
                              32768 / PI, velocity * static_cast<uint8_t>
                              (_reductionRatio) * 50 / PI);
    }

    bool ti5Motor::moveRelative(float distance,
                                float velocity)
    {
        tlog_info << "Moving " << to_string(
                      distance) << " with velocity " << to_string(
                      velocity) << endl;
        if(_autoStatus)
        {
            return writeParameter(setPositionAndVelocityCode,
                                  _autoPositionRaw + distance *
                                  static_cast<uint8_t>(_reductionRatio) * 32768 /
                                  PI, velocity * static_cast<uint8_t>
                                  (_reductionRatio) * 50 / PI);
        }
        readParameter(getPositionCode, &_autoPositionRaw);
        return writeParameter(setPositionAndVelocityCode,
                              _autoPositionRaw + distance *
                              static_cast<uint8_t>(_reductionRatio) * 32768 /
                              PI, velocity * static_cast<uint8_t>
                              (_reductionRatio) * 50 / PI);
    }

    bool ti5Motor::moveVelocity(float velocity,
                                bool positionControlled,float position)
    {
        tlog_info << "Moving with velocity " << to_string(
                      velocity) << endl;
        if(positionControlled)
        {
            if(!_autoStatus)
                autoCurrentSpeedPosition();
            return writeParameter(setVelocityCode,
                                  velocity * static_cast<uint8_t>(_reductionRatio) *
                                  50 / PI);
        }
        else
        {
            return writeParameter(setCurrentCode,
                                  velocity * static_cast<uint8_t>(_reductionRatio) *
                                 50 / PI);
        }
        return true;
    }

    bool ti5Motor::moveJog(Direction direction,
                           float velocity,
                           bool positionControlled)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxSpeed(float maxSpeed)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxAcceleration(
        float maxAcceleration)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxPosition(
        float maxPosition)
    {
        return true;
    }

    bool ti5Motor::quickSetMinPosition(
        float minPosition)
    {
        return true;
    }

    bool ti5Motor::quickGetMaxSpeed(float* maxSpeed)
    {
        return true;
    }

    bool ti5Motor::quickGetMaxAcceleration(
        float* maxAcceleration)
    {
        return true;
    }

    bool ti5Motor::quickGetMaxPosition(
        float* maxPosition)
    {
        return true;
    }

    bool ti5Motor::quickGetMinPosition(
        float* minPosition)
    {
        return true;
    }

    bool ti5Motor::quickGetEnableStatus(bool* status)
    {
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
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        return true;
    }
    bool ti5Motor::writeParameter(
        parameterCodeTableSend1_4 parameterCode,
        int32_t value)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 5;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((
                                    value & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((
                                    value >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((
                                    value >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((
                                    value >> 24) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        return true;
    }

    bool ti5Motor::writeParameter(
        parameterCodeTableSend1_2_2 parameterCode,
        uint16_t value1, uint16_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 5;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((
                                    value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((
                                    value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>((
                                    value2 & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((
                                    value2 >> 8) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        return true;
    }

    bool ti5Motor::writeParameter(
        parameterCodeTableSend1_4_1 parameterCode,
        int32_t value1, uint8_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 6;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((
                                    value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((
                                    value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((
                                    value1 >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((
                                    value1 >> 24) & 0xFF));
        _canFrameSend.data[5] = value2;
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        return true;
    }

    bool ti5Motor::writeParameter(
        parameterCodeTableSend1_4_2 parameterCode,
        int32_t value1, uint16_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 7;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((
                                    value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((
                                    value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((
                                    value1 >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((
                                    value1 >> 24) & 0xFF));
        _canFrameSend.data[5] = static_cast<uint8_t>((
                                    value2 & 0xFF));
        _canFrameSend.data[6] = static_cast<uint8_t>(((
                                    value2 >> 8) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        return true;
    }

    bool ti5Motor::readParameter(
        parameterCodeTableSend1Receive1_4 parameterCode,
        int32_t* value)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        if (!_canDriver->waitForMessages())
        {
            tlog_error << "Receive Timeout! " << endl;
            return false;
        }
        CanMessage{_canFrameReceive} =
            _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.can_id) !=
                _canFrameSend.can_id) ||
                (_canFrameReceive.can_dlc != 5) ||
                (_canFrameReceive.data[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        string _canFrameReceiveToString;
        for (auto i : _canFrameReceive.data)
        {
            _canFrameReceiveToString += format("{:02x} ", i);
        }
        tlog_debug << "Received message!" <<
                   _canFrameReceiveToString << endl;
        *value = (_canFrameReceive.data[1]) |
                 (_canFrameReceive.data[2] << 8) |
                 (_canFrameReceive.data[3]
                  << 16) | (_canFrameReceive.data[4] << 24);
        return true;
    }

    bool ti5Motor::readParameter(
        parameterCodeTableSend1Receive2_2_4 parameterCode,
        int16_t* value1, int16_t* value2, int32_t* value3)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        if (!_canDriver->waitForMessages())
        {
            tlog_error << "Receive Timeout! " << endl;
            return false;
        }
        CanMessage{_canFrameReceive} =
            _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.can_id) !=
                _canFrameSend.can_id) ||
                (_canFrameReceive.can_dlc != 5) ||
                (_canFrameReceive.data[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        string _canFrameReceiveToString;
        for (auto i : _canFrameReceive.data)
        {
            _canFrameReceiveToString += format("{:02x} ", i);
        }
        tlog_debug << "Received message!" <<
                   _canFrameReceiveToString << endl;
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

    bool ti5Motor::writeAndReadParameter(
        parameterCodeTableSend1_4Receive2_2_4
        parameterCode, int32_t value, int16_t* value1,
        int16_t* value2, int32_t* value3)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 5;
        _canFrameSend.data[0] = static_cast<uint8_t>
                                (parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((
                                    value & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((
                                    value >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((
                                    value >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((
                                    value >> 24) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(
                                        _canFrameSend));
        }
        catch (CanException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        catch (InvalidSocketException& ex)
        {
            tlog_error << "Failed to send test message! " <<
                       ex.what() << endl;
            return false;
        }
        string _canFrameSendToString;
        for (auto i : _canFrameSend.data)
        {
            _canFrameSendToString += format("{:02x} ", i);
        }
        tlog_debug << "Sent message!" <<
                   _canFrameSendToString << endl;
        if (!_canDriver->waitForMessages())
        {
            tlog_error << "Receive Timeout! " << endl;
            return false;
        }
        CanMessage{_canFrameReceive} =
            _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.can_id) !=
                _canFrameSend.can_id) ||
                (_canFrameReceive.can_dlc != 5) ||
                (_canFrameReceive.data[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        string _canFrameReceiveToString;
        for (auto i : _canFrameReceive.data)
        {
            _canFrameReceiveToString += format("{:02x} ", i);
        }
        tlog_debug << "Received message!" <<
                   _canFrameReceiveToString << endl;
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
