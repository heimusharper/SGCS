#ifndef UAVCALIBRATION_H
#define UAVCALIBRATION_H
#include "UavObject.h"

namespace uav
{
class Calibration : public UavObject<uint16_t>
{
public:
    enum HAS : uint16_t
    {
        HAS_MAGNETOMETER = 0b1 << 0,
    };
    enum class Sides : uint8_t
    {
        DOWN  = 1 << 0,
        UP    = 1 << 1,
        LEFT  = 1 << 2,
        RIGHT = 1 << 3,
        FRONT = 1 << 4,
        BACK  = 1 << 5
    };

    struct Process
    {
        enum State
        {
            WAIT,
            DONE,
            FAILURE,
            ON_PROGRESS
        };

        Process();
        State onCalibration;
        float progress;
        bool hasSides;
        uint8_t sidesReady;
        uint8_t sideActive;
    };

    Calibration();

    class CalibrationControlInterface
    {
    public:
        CalibrationControlInterface()
        {
        }
        virtual bool startMagCal()
        {
            return false;
        }
        virtual bool stopMagCal()
        {
            return false;
        }
    };
    void compas(Calibration::Process &p);
    void setCompas(const Process &compas);

    void addControl(CalibrationControlInterface *i);

    void doStartCompasCalibration();
    void doStopCompasCalibration();

private:
    std::mutex m_compasMutex;
    Process m_compas;

    std::list<CalibrationControlInterface *> m_controls;
};
}
#endif // CALIBRATION_H
