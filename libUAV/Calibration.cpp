#include "Calibration.h"

namespace uav
{
Calibration::Calibration()
{
}

void Calibration::compas(Process &p)
{
    std::lock_guard grd(m_compasMutex);
    p = m_compas;
}

void Calibration::setCompas(const Process &compas)
{
    std::lock_guard grd(m_compasMutex);
    m_compas = compas;
}

void Calibration::addControl(Calibration::CalibrationControlInterface *i)
{
    m_controls.push_back(i);
}

void Calibration::doStartCompasCalibration()
{
    for (auto i : m_controls)
        i->startMagCal();
}

void Calibration::doStopCompasCalibration()
{
    for (auto i : m_controls)
        i->stopMagCal();
}

Calibration::Process::Process() : onCalibration(State::WAIT), progress(0), hasSides(false), sidesReady(0), sideActive(0)
{
}

}
