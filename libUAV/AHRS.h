#ifndef AHRS_H
#define AHRS_H
#include "UavObject.h"

namespace uav
{
class AHRS : public UavObject
{
public:
    class Message : public UavMessage
    {
    public:
        Message() : UavMessage(), roll(0.f), pitch(0.f), yaw(0.f)
        {
        }
        Message::optional<float> roll;
        Message::optional<float> pitch;
        Message::optional<float> yaw;
    };

    AHRS();
    virtual ~AHRS();

    void process(AHRS::Message *message);

    void get(float &roll, float &pitch, float &yaw);

private:
    void setRoll(float roll);
    void setPitch(float pitch);
    void setYaw(float yaw);

private:
    float m_roll  = 0;
    float m_pitch = 0;
    float m_yaw   = 0;
};
}
#endif // AHRS_H
