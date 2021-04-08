#ifndef SGCSGCSRTK_H
#define SGCSGCSRTK_H
#include <Optional.h>
#include <geo/Coords3D.h>

namespace sgcs
{
class GCSRTK
{
public:
    enum class Status
    {
        ACTIVE,
        SURVEY,
        UNDEFINED
    };

    GCSRTK();

    bool hasRTK() const;
    void setHasRTK(bool hasRTK);

    geo::Coords3D position() const;
    void setPosition(geo::Coords3D &&position);

    int surveyTime() const;
    void setSurveyTime(int &&surveyTime);

    double surveyAccuracity() const;
    void setSurveyAccuracity(double &&surveyAccuracity);

    Status status() const;
    void setStatus(Status &&status);

private:
    tools::optional_safe<bool> m_hasRTK;

    tools::optional_safe<Status> m_status;
    tools::optional_safe<geo::Coords3D> m_position;
    tools::optional_safe<int> m_surveyTime;
    tools::optional_safe<double> m_surveyAccuracity;
};
}

#endif // GCSRTK_H
