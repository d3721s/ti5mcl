#include "ti5mcl.hpp"
using namespace sockcanpp;
using namespace sockcanpp::exceptions;

namespace ti5mcl
{


    void ti5Motor::power(bool en)
    {
    }

    void ti5Motor::reset()
    {
    }

    bool ti5Motor::home()
    {
        return true;
    }

    bool ti5Motor::halt()
    {
        return true;
    }

    bool ti5Motor::moveAbsolute(long position, long velocity)
    {
        return true;
    }

    bool ti5Motor::moveRelative(long distance, long velocity)
    {
        return true;
    }

    bool ti5Motor::moveVelocity(long velocity, bool positionControlled)
    {
        return true;
    }

    bool ti5Motor::moveJog(Direction direction, long velocity,
                           bool positionControlled)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxSpeed(float maxSpeed)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxAcceleration(float maxAcceleration)
    {
        return true;
    }

    bool ti5Motor::quickSetMaxPosition(float maxPosition)
    {
        return true;
    }

    bool ti5Motor::quickSetMinPosition(float minPosition)
    {
        return true;
    }

    bool ti5Motor::quickSetEnableStatus(bool status)
    {
        return true;
    }

    bool ti5Motor::quickSetZero()
    {
        return true;
    }

    bool ti5Motor::quickClearError()
    {
        return true;
    }

    bool ti5Motor::quickGetMaxSpeed(float *maxSpeed)
    {
        return true;
    }

    bool ti5Motor::quickGetMaxAcceleration(float *maxAcceleration)
    {
        return true;
    }

    bool ti5Motor::quickGetMaxPosition(float *maxPosition)
    {
        return true;
    }

    bool ti5Motor::quickGetMinPosition(float *minPosition)
    {
        return true;
    }

    bool ti5Motor::quickGetEnableStatus(bool *status)
    {
        return true;
    }

    bool ti5Motor::quickGetErrorCode(uint16_t *errorCode)
    {
        return true;
    }

    bool ti5Motor::autoMonitor(bool enable)
    {
        return true;
    }

    bool ti5Motor::autoCurrentSpeedPosition(bool enable, uint16_t period)
    {
        return true;
    }

    float ti5Motor::autoCurrent() const
    {
        return 0.0f;
    }

    float ti5Motor::autoSpeed() const
    {
        return 0.0f;
    }

    float ti5Motor::autoPosition() const
    {
        return 0.0f;
    }

    bool ti5Motor::writeParameter(parameterCodeTableSend1 parameterCode)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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
    bool ti5Motor::writeParameter(parameterCodeTableSend1_4 parameterCode,
                                  int32_t value)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 5;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((value & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((value >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((value >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((value >> 24) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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

    bool ti5Motor::writeParameter(parameterCodeTableSend1_2_2 parameterCode,
                                  uint16_t value1, uint16_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 5;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>((value2 & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((value2 >> 8) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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

    bool ti5Motor::writeParameter(parameterCodeTableSend1_4_1 parameterCode,
                                  int32_t value1, uint8_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 6;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((value1 >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((value1 >> 24) & 0xFF));
        _canFrameSend.data[5] = value2;
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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

    bool ti5Motor::writeParameter(parameterCodeTableSend1_4_2 parameterCode,
                                  int32_t value1, uint16_t value2)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 7;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((value1 & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((value1 >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((value1 >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((value1 >> 24) & 0xFF));
        _canFrameSend.data[5] = static_cast<uint8_t>((value2 & 0xFF));
        _canFrameSend.data[6] = static_cast<uint8_t>(((value2 >> 8) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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

    bool ti5Motor::readParameter(parameterCodeTableSend1Receive1_4 parameterCode,
                                 int32_t *value)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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
        auto _canFrameReceive = _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.getCanId()) != _canFrameSend.can_id) ||
                (_canFrameReceive.getFrameData().size() != 5) ||
                (_canFrameReceive.getFrameData()[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        *value = (_canFrameReceive.getFrameData()[1]) |
                 (_canFrameReceive.getFrameData()[2] << 8) | (_canFrameReceive.getFrameData()[3]
                         << 16) | (_canFrameReceive.getFrameData()[4] << 24);
        return true;
    }

    bool ti5Motor::readParameter(parameterCodeTableSend1Receive2_2_4 parameterCode,
                                 int16_t *value1, int16_t *value2, int32_t *value3)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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
        auto _canFrameReceive = _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.getCanId()) != _canFrameSend.can_id) ||
                (_canFrameReceive.getFrameData().size() != 5) ||
                (_canFrameReceive.getFrameData()[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        return true;
    }

    bool ti5Motor::writeAndReadParameter(parameterCodeTableSend1_4Receive2_2_4
                                         parameterCode, int32_t value, int16_t *value1, int16_t *value2, int32_t *value3)
    {
        _canFrameSend.can_id = this->_canId;
        _canFrameSend.can_dlc = 1;
        _canFrameSend.data[0] = static_cast<uint8_t>(parameterCode);
        _canFrameSend.data[1] = static_cast<uint8_t>((value & 0xFF));
        _canFrameSend.data[2] = static_cast<uint8_t>(((value >> 8) & 0xFF));
        _canFrameSend.data[3] = static_cast<uint8_t>(((value >> 16) & 0xFF));
        _canFrameSend.data[4] = static_cast<uint8_t>(((value >> 24) & 0xFF));
        lock_guard<mutex> lock(canMutex);
        try
        {
            _canDriver->sendMessage(CanMessage(_canFrameSend));
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
        auto _canFrameReceive = _canDriver->readMessage(); // read the message
        if(((_canFrameReceive.getCanId()) != _canFrameSend.can_id) ||
                (_canFrameReceive.getFrameData().size() != 5) ||
                (_canFrameReceive.getFrameData()[0] != parameterCode))
        {
            tlog_error << "Receive Wrong Message! " << endl;
            return false;
        }
        return true;
    }

} // namespace ti5mcl
