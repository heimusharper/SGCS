#include "GCSRTK.h"

namespace sgcs
{
GCSRTK::GCSRTK()
: m_hasRTK(false), m_status(Status::UNDEFINED), m_position(geo::Coords3D()), m_surveyTime(0), m_surveyAccuracity(0)
{
}

bool GCSRTK::hasRTK() const
{
    return m_hasRTK.get();
}

void GCSRTK::setHasRTK(bool hasRTK)
{
    if (m_hasRTK.get() == hasRTK)
        return;
    m_hasRTK.set(std::move(hasRTK));
}

geo::Coords3D GCSRTK::position() const
{
    return m_position.get();
}

void GCSRTK::setPosition(geo::Coords3D &&position)
{
    if (m_position.get() == position)
        return;
    m_position.set(std::move(position));
}

int GCSRTK::surveyTime() const
{
    return m_surveyTime.get();
}

void GCSRTK::setSurveyTime(int &&surveyTime)
{
    if (m_surveyTime.get() == surveyTime)
        return;
    m_surveyTime.set(std::move(surveyTime));
}

double GCSRTK::surveyAccuracity() const
{
    return m_surveyAccuracity.get();
}

void GCSRTK::setSurveyAccuracity(double &&surveyAccuracity)
{
    if (m_surveyAccuracity.get() == surveyAccuracity)
        return;
    m_surveyAccuracity.set(std::move(surveyAccuracity));
}

GCSRTK::Status GCSRTK::status() const
{
    return m_status.get();
}

void GCSRTK::setStatus(Status &&status)
{
    if (m_status.get() == status)
        return;
    m_status.set(std::move(status));
}

void GCSRTK::addCallback(GCSRTK::RTCMCallback *call)
{
    m_RTCMCallback.push_back(call);
}

void GCSRTK::removeCallback(GCSRTK::RTCMCallback *call)
{
    m_RTCMCallback.remove(call);
}

void GCSRTK::sendRTCM(const tools::CharMap &rtcm)
{
    for (auto x : m_RTCMCallback)
        x->sendRTCM(rtcm);
}
}
