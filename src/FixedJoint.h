/*
 *  FixedJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 20/09/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#ifndef FixedJoint_h
#define FixedJoint_h

#include "Joint.h"
#include "SmartEnum.h"
#include "PGDMath.h"

#include <memory>

namespace GaitSym
{

class ButterworthFilter;
class MovingAverage;
class Filter;

class FixedJoint: public Joint
{
    public:

    FixedJoint();

    SMART_ENUM(StressCalculationType, stressCalculationTypeStrings, stressCalculationTypeCount, none, beam, spring);
//    enum StressCalculationType { none = 0, beam, spring };
    SMART_ENUM(LowPassType, lowPassTypeStrings, lowPassTypeCount, NoLowPass, MovingAverageLowPass, Butterworth2ndOrderLowPass);
//    enum LowPassType { NoLowPass = 0, MovingAverageLowPass, Butterworth2ndOrderLowPass };

    virtual void lateInitialisation();

    void setCrossSection(const std::vector<unsigned char> &stiffness, size_t nx, size_t ny, double dx, double dy);
    void setStressOrigin(double x, double y, double z);
    void setStressOrientation(double q0, double q1, double q2, double q3);

    void setStressCalculationType(StressCalculationType type) { m_stressCalculationType = type; }
    StressCalculationType stressCalculationType() { return m_stressCalculationType; }

    pgd::Vector3 stressOrigin() { return m_stressOrigin; }
    pgd::Quaternion stressOrientation() { return m_stressOrientation; }

    double maxStress() { return m_maxStress; }
    double minStress() { return m_minStress; }

    void setStressLimit(double stressLimit) { m_stressLimit = stressLimit; }
    bool checkStressAbort();

    void setLowPassType(LowPassType lowPassType) { m_lowPassType = lowPassType; }
    LowPassType lowPassType() { return m_lowPassType; }
    void setWindow(size_t window);
    void setCutoffFrequency(double cutoffFrequency);
    double lowPassMinStress() { return m_lowPassMinStress; }
    double lowPassMaxStress() { return m_lowPassMaxStress; }

    const std::vector<double> &stress() { return m_stress; }

    virtual void update();
    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    double maxStress() const;

    double width() const;

    double height() const;

    double xOrigin() const;

    double yOrigin() const;

    size_t nx() const;

    size_t ny() const;

    const std::vector<unsigned char> &stiffness() const;

    double lowRange() const;
    void setLowRange(double lowRange);

    double highRange() const;
    void setHighRange(double highRange);

    void calculatePixmap();
    bool calculatePixmapNeeded();
    const std::vector<unsigned char> &pixMap() const;

    bool lateFix() const;
    void setLateFix(bool lateFix);

private:

    void calculateStress();
    static std::vector<unsigned char> asciiToBitMap(const std::string &buffer, size_t width, size_t height, char setChar, bool reverseY);

    bool m_lateFix = false;

    // these are used for the stress/strain calculations
    std::vector<unsigned char> m_stiffness;
    std::vector<double> m_stress;
    std::vector<double> m_xDistances;
    std::vector<double> m_yDistances;
    size_t m_nx = 0;
    size_t m_ny = 0;
    size_t m_nActivePixels = 0;
    double m_dx = 0;
    double m_dy = 0;
    double m_Ix = 0;
    double m_Iy = 0;
    double m_Ixy = 0;
    double m_area = 0;
    double m_xOrigin = 0;
    double m_yOrigin = 0;
    double m_minStress = 0;
    double m_maxStress = 0;
    double m_width = 0;
    double m_height = 0;
    StressCalculationType m_stressCalculationType = StressCalculationType::none;

    pgd::Vector3 m_stressOrigin;
    pgd::Quaternion m_stressOrientation;
    pgd::Vector3 m_torqueStressCoords;
    pgd::Vector3 m_forceStressCoords;
    pgd::Vector3 m_torqueAxis;
    double m_torqueScalar = 0;

    double m_stressLimit = -1;
    std::vector<std::unique_ptr<Filter>> m_filteredStress;
    double m_lowPassMinStress = 0;
    double m_lowPassMaxStress = 0;
    LowPassType m_lowPassType = LowPassType::NoLowPass;
    double m_cutoffFrequency = 0;
    size_t m_window = 0;

    std::vector<pgd::Vector3> m_vectorList;

    std::vector<unsigned char> m_colourMap;
    std::vector<unsigned char> m_pixMap;
    double m_lastDisplayTime = -1;
    double m_lowRange = 0;
    double m_highRange = 0;
};

}

#endif

