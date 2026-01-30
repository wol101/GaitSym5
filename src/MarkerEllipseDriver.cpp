 /*
 *  MarkerEllipseDriver.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 08/01/2017.
 *  Copyright 2017 Bill Sellers. All rights reserved.
 *
 */

#include "MarkerEllipseDriver.h"

#include "Simulation.h"
#include "Marker.h"
#include "GSUtil.h"
#include "Drivable.h"

#include <cmath>
#include <vector>
#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

MarkerEllipseDriver::MarkerEllipseDriver()
{
}

void MarkerEllipseDriver::Initialise(double omega, double sigma, const pgd::Vector4 &xr, const pgd::Vector4 &yr, double phi, Marker *markerEllipseCentre, Marker *markerEllipseRim, DataTarget *phaseControlInput)
{
    m_omega   =   omega;        // rad s-1     intrinsic angular velocity
    m_sigma   =   sigma;        //             gain for the phase correction
    m_xr      =   xr;           // m           x-direction radius
    m_yr      =   yr;           // m           y-direction radius
    m_phi     =   std::fmod(2 * M_PI + std::fmod(phi, 2 * M_PI), 2 * M_PI); // rad initial phase normalised from 0 to 2 pi
    m_markerEllipseCentre = markerEllipseCentre;  // this marker defines the centre position and local coordinate system for the controller

    // and set the derived values
    m_phiDot = m_omega;
    while (true)
    {
        if (m_phi < M_PI_2)
        {
            m_x = m_xr[0] * std::cos(m_phi);
            m_y = m_yr[0] * std::sin(m_phi);
            break;
        }
        if (m_phi < M_PI)
        {
            m_x = m_xr[1] * std::cos(m_phi);
            m_y = m_yr[1] * std::sin(m_phi);
            break;
        }
        if (m_phi < 3 * M_PI_2)
        {
            m_x = m_xr[2] * std::cos(m_phi);
            m_y = m_yr[2] * std::sin(m_phi);
            break;
        }
        m_x = m_xr[3] * std::cos(m_phi);
        m_y = m_yr[3] * std::sin(m_phi);
        break;
    }

    pgd::Quaternion rimLocalQ = m_markerEllipseCentre->quaternion();
    pgd::Vector3 rimWorldP = m_markerEllipseCentre->worldPosition(pgd::Vector3(m_x, m_y, 0));
    m_markerEllipseRim = markerEllipseRim;
    m_markerEllipseRim->setQuaternion(rimLocalQ.n, rimLocalQ.x, rimLocalQ.y, rimLocalQ.z);
    m_markerEllipseRim->setWorldPosition(rimWorldP.x ,rimWorldP.y, rimWorldP.z);
    m_phaseControlInput = phaseControlInput;
}

void MarkerEllipseDriver::sendData()
{
    for (auto &&it : *targetList())
    {
        it.second->receiveData(clamp(std::sqrt(SQUARE(m_x) + SQUARE(m_y))), simulation()->stepCount());
    }
}

void MarkerEllipseDriver::update()
{
    assert(simulation()->stepCount() == lastStepCount() + 1);
    setLastStepCount(simulation()->stepCount());

    if (m_omegaDriver) m_omega = m_omegaDriver->value();
    if (m_sigmaDriver) m_sigma = m_sigmaDriver->value();
    if (m_xrDriver0) m_xr[0] = m_xrDriver0->value();
    if (m_yrDriver0) m_yr[0] = m_yrDriver0->value();
    if (m_xrDriver1) m_xr[1] = m_xrDriver1->value();
    if (m_yrDriver1) m_yr[1] = m_yrDriver1->value();
    if (m_xrDriver2) m_xr[2] = m_xrDriver2->value();
    if (m_yrDriver2) m_yr[2] = m_yrDriver2->value();
    if (m_xrDriver3) m_xr[3] = m_xrDriver3->value();
    if (m_yrDriver3) m_yr[3] = m_yrDriver3->value();

    // main control algorithm
    if (!m_phaseControlInput)
    {
        m_phiDot = m_omega;
    }
    else
    {
        // we need to do something to correct for the phase
        m_valueChangeDirection = detectSignChange(m_phaseControlInput->calculateError(simulation()->simulationTime()));
        if (m_valueChangeDirection != 0)
        {
            m_halfPeriod = simulation()->simulationTime() - m_lastPhaseChangeTime;
            m_lastPhaseChangeTime = simulation()->simulationTime();
            m_phiDot = std::clamp(M_PI / (m_halfPeriod * m_periodMultiplier), 0.0, m_maxPhiDot); // this copes with halfPeriod of zero since divide by zero is +/- infinity
            // but we need to tweak m_phiDot to get the phase relationship eventually
            if (m_valueChangeDirection > 0) m_wantedPhi = std::fmod(2 * M_PI + std::fmod(M_PI_2 + m_phaseOffset, 2 * M_PI), 2 * M_PI);
            else m_wantedPhi = std::fmod(2 * M_PI + std::fmod(3 * M_PI_2 + m_phaseOffset, 2 * M_PI), 2 * M_PI);
            m_delPhi = m_wantedPhi - m_phi;
            if (std::fabs(m_delPhi) > M_PI) m_delPhi = 2 * M_PI + m_phi - m_wantedPhi;
            m_phiDot += m_sigma * -m_delPhi; // this is a P controller. A PID controller might be better (certainly a PD controller)
        }
    }

    // update m_phi depending on m_phi_dot values
    m_phi = std::fmod(2 * M_PI + std::fmod(m_phi + m_phiDot * simulation()->global()->stepSize(), 2 * M_PI), 2 * M_PI); // do fmod twice to get a value from 0 to 2pi

    while (true)
    {
        if (m_phi < M_PI_2)
        {
            m_x = m_xr[0] * std::cos(m_phi);
            m_y = m_yr[0] * std::sin(m_phi);
            break;
        }
        if (m_phi < M_PI)
        {
            m_x = m_xr[1] * std::cos(m_phi);
            m_y = m_yr[1] * std::sin(m_phi);
            break;
        }
        if (m_phi < 3 * M_PI_2)
        {
            m_x = m_xr[2] * std::cos(m_phi);
            m_y = m_yr[2] * std::sin(m_phi);
            break;
        }
        m_x = m_xr[3] * std::cos(m_phi);
        m_y = m_yr[3] * std::sin(m_phi);
        break;
    }

    // get the world position of the MarkerEllipse target
    pgd::Quaternion rimLocalQ = m_markerEllipseCentre->quaternion();
    pgd::Vector3 rimWorldP = m_markerEllipseCentre->worldPosition(pgd::Vector3(m_x, m_y, 0));
    m_markerEllipseRim->setQuaternion(rimLocalQ.n, rimLocalQ.x, rimLocalQ.y, rimLocalQ.z);
    m_markerEllipseRim->setWorldPosition(rimWorldP.x ,rimWorldP.y, rimWorldP.z);

}

int MarkerEllipseDriver::detectSignChange(double value)
{
    double lastValue = m_butterworthFilter.output();
    m_butterworthFilter.addNewSample(value);
    double delta = m_butterworthFilter.output() - lastValue;
//    std::cerr << "delta = " << delta << "\n";
    if (m_phaseStateIncreasing && delta > 0)
    {
        m_phaseStateChangeCount = 0;
        return 0;
    }
    if (!m_phaseStateIncreasing && delta < 0)
    {
        m_phaseStateChangeCount = 0;
        return 0;
    }
    if (m_phaseStateIncreasing && delta < 0)
    {
        m_phaseStateChangeCount++;
        if (m_phaseStateChangeCount > m_phaseStateCountThreshold)
        {
            m_phaseStateIncreasing = false;
            m_phaseStateChangeCount = 0;
            return -1;
        }
        return 0;
    }
    if (!m_phaseStateIncreasing && delta > 0)
    {
        m_phaseStateChangeCount++;
        if (m_phaseStateChangeCount > m_phaseStateCountThreshold)
        {
            m_phaseStateIncreasing = true;
            m_phaseStateChangeCount = 0;
            return +1;
        }
        return 0;
    }
    return 0;
}

/**
 * @brief MarkerEllipseDriver::dumpToString
 * @return string containing the data for this time point
 *
 * This function returns useful data to the user about values contained in this object during the simulation
 *
 * Column Headings:
 *
 * - time
 *   - the simulation time
 * - omega
 *   - the initial angular velocity
 * - sigma
 *   - the phase matching gain
 * - phaseOffset
 *   - the goal phase offset to the target
 * - xr
 *   - the current X radius
 * - yr
 *   - the current Y radius
 * - X
 *   - the current X value (marker local coordinates)
 * - Y
 *   - the current Y value (marker local coordinates)
 * - phi
 *   - the current angle
 * - phi_dot
 *   - the current angular velocity
 * - wantedPhi
 *   - the phiWanted to get the desired phaseOffset
 * - delPhi
 *   - the current change of phi
 * - lastPhaseChangeTime
 *   - the time when the phase was last checked
 * - halfPeriod
 *   - the half period of the driving signal
 * - valueChangeDirection
 *   - the direction that dribing signal is changing (+1, -1 or 0)
 */

std::string MarkerEllipseDriver::dumpToString()
{
    std::string s;
    if (firstDump())
    {
        setFirstDump(false);
        s += dumpHelper({"time", "omega"s, "sigma"s, "phaseOffset"s, "xr"s, "yr"s, "X"s, "Y"s, "phi"s, "phi_dot"s, "wantedPhi"s, "delPhi"s, "lastPhaseChangeTime"s, "halfPeriod"s, "valueChangeDirection"s});
    }
    double xr, yr;
    while (true)
    {
        if (m_phi < M_PI_2)
        {
            xr = m_xr[0];
            yr = m_yr[0];
            break;
        }
        if (m_phi < M_PI)
        {
            xr = m_xr[1];
            yr = m_yr[1];
            break;
        }
        if (m_phi < 3 * M_PI_2)
        {
            xr = m_xr[2];
            yr = m_yr[2];
            break;
        }
        xr = m_xr[3];
        yr = m_yr[3];
        break;
    }

    s += dumpHelper({simulation()->simulationTime(), m_omega, m_sigma, m_phaseOffset, xr, yr, m_x, m_y, m_phi, m_phiDot, m_wantedPhi, m_delPhi, m_lastPhaseChangeTime, m_halfPeriod, double(m_valueChangeDirection)});
    return s;
}

double MarkerEllipseDriver::omega() const
{
    return m_omega;
}

double MarkerEllipseDriver::sigma() const
{
    return m_sigma;
}

double MarkerEllipseDriver::phi() const
{
    return m_phi;
}

double MarkerEllipseDriver::x() const
{
    return m_x;
}

double MarkerEllipseDriver::y() const
{
    return m_y;
}

pgd::Vector4 MarkerEllipseDriver::xr() const
{
    return m_xr;
}

pgd::Vector4 MarkerEllipseDriver::yr() const
{
    return m_yr;
}

double MarkerEllipseDriver::phi_dot() const
{
    return m_phiDot;
}


/**
 * @brief MarkerEllipseDriver::createFromAttributes
 * @return nullptr on success and a pointer to lastError() on failure
 *
 * This function initialises the data in the object based on the contents
 * of an xml_node node. It uses information from the simulation as required
 * to satisfy dependencies
 *
 * Attributes in addition to standard DRIVER:
 *
 * - Type="MarkerEllipse"
 * - Omega="double"
 *   - The initial angular velocity
 * - Sigma="double"
 *   - The phase matching gain
 * - xr="list of doubles"
 *   - The X radius for each quadrant (repeated if only 1 value given)
 * - yr="list of doubles"
 *   - The Y radius for each quadrant (repeated if only 1 value given)
 * - Phi="double"
 *   - The initial phase angle
 * - CentreMarkerID
 *   - ID of the marker that identifies the rotaion centre and axis (rotates around the Z axis, and the X & Y axes are the ones defined for the driver)
 * - RimMarkerID
 *   - ID of the marker that gets moved in the ellipse
 * - PhaseControlInputID
 *   - ID of the data target used to control the phase of the marker's movement
 * - LowPassFrequency
 *   - Low pass filter applied to the phase control signal
 * - PhaseOffset
 *   - Phase offset from the control signal
 * - MaxPhiDot
 *   - Maximum allowable rotational velocity
 * - PeriodMultiplier
 *   - Attempt to multiply the rotational period of the driver signal by this value
 *
 * Optional Attributes
 *
 * - OmegaDriverID
 *   - ID of driver that can change the value of omega
 * - SigmaDriverID
 *   - ID of driver that can change the value of omega
 * - xrDriver0ID
 *   - ID of driver that can change the value of xr in quadrant 0
 * - yrDriver0ID
 *   - ID of driver that can change the value of yr in quadrant 0
 * - xrDriver1ID
 *   - ID of driver that can change the value of xr in quadrant 1
 * - yrDriver1ID
 *   - ID of driver that can change the value of yr in quadrant 1
 * - xrDriver2ID
 *   - ID of driver that can change the value of xr in quadrant 2
 * - yrDriver2ID
 *   - ID of driver that can change the value of yr in quadrant 2
 * - xrDriver3ID
 *   - ID of driver that can change the value of xr in quadrant 3
 * - yrDriver3ID
 *   - ID of driver that can change the value of yr in quadrant 3
 *
 */

std::string *MarkerEllipseDriver::createFromAttributes()
{
    if (Driver::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    double omega, sigma, phi;
    std::vector<double> xr, yr;
    if (findAttribute("Omega"s, &buf) == nullptr) return lastErrorPtr();
    omega = GSUtil::toDouble(buf);
    if (findAttribute("Sigma"s, &buf) == nullptr) return lastErrorPtr();
    sigma = GSUtil::toDouble(buf);
    if (findAttribute("xr"s, &buf) == nullptr) return lastErrorPtr();
    GSUtil::toDouble(buf, &xr);
    if (findAttribute("yr"s, &buf) == nullptr) return lastErrorPtr();
    GSUtil::toDouble(buf, &yr);
    if (findAttribute("Phi"s, &buf) == nullptr) return lastErrorPtr();
    phi = GSUtil::toDouble(buf);

    if (findAttribute("CentreMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    Marker *markerEllipseCentre = simulation()->getMarker(buf);
    if (!markerEllipseCentre)
    {
        setLastError("MarkerEllipseDriver ID=\""s + name() + "\" CentreMarkerID marker not found \""s + buf + "\"");
        return lastErrorPtr();
    }
    if (findAttribute("RimMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    Marker *markerEllipseRim = simulation()->getMarker(buf);
    if (!markerEllipseRim)
    {
        setLastError("MarkerEllipseDriver ID=\""s + name() + "\" RimMarkerID marker not found \""s + buf + "\"");
        return lastErrorPtr();
    }
    if (markerEllipseCentre->body() != markerEllipseRim->body())
    {
        setLastError("MarkerEllipseDriver ID=\""s + name() + "\" RimMarkerID marker and CentreMarkerID must have the same BODY\"");
        return lastErrorPtr();
    }
    if (findAttribute("PhaseControlInputID"s, &buf) == nullptr) return lastErrorPtr();
    DataTarget *phaseControlInput = simulation()->getDataTarget(buf);
    if (!phaseControlInput)
    {
        setLastError("PhaseControlInputID ID=\""s + name() + "\" PhaseControlInputID data target not found \""s + buf + "\"");
        return lastErrorPtr();
    }
    pgd::Vector4 xrV, yrV;
    if (xr.size() == 1) xrV.set(xr[0], xr[0], xr[0], xr[0]);
    else for (size_t i = 0; i < xr.size(); i++) { xrV[i] = xr[i]; }
    if (yr.size() == 1) yrV.set(yr[0], yr[0], yr[0], yr[0]);
    else for (size_t i = 0; i < yr.size(); i++) { yrV[i] = yr[i]; }
    Initialise(omega, sigma, xrV, yrV, phi, markerEllipseCentre, markerEllipseRim, phaseControlInput);

    if (findAttribute("LowPassFrequency"s, &buf) == nullptr) return lastErrorPtr();
    m_butterworthFilter.calculateCoefficients(GSUtil::toDouble(buf), 1.0 / simulation()->global()->stepSize());
    if (findAttribute("PhaseOffset"s, &buf) == nullptr) return lastErrorPtr();
    m_phaseOffset = GSUtil::toDouble(buf);
    if (findAttribute("MaxPhiDot"s, &buf) == nullptr) return lastErrorPtr();
    m_maxPhiDot = GSUtil::toDouble(buf);
    if (findAttribute("PeriodMultiplier"s, &buf) == nullptr) return lastErrorPtr();
    m_periodMultiplier = GSUtil::toDouble(buf);

    if (findAttribute("OmegaDriverID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" OmegaDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_omegaDriver = driver;
    }
    if (findAttribute("SigmaDriverID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" SigmaDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_sigmaDriver = driver;
    }
    if (findAttribute("xrDriver0ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" ADriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_xrDriver0 = driver;
    }
    if (findAttribute("yrDriver0ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" AprimeDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_yrDriver0 = driver;
    }
    if (findAttribute("xrDriver1ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" ADriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_xrDriver1 = driver;
    }
    if (findAttribute("yrDriver1ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" AprimeDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_yrDriver1 = driver;
    }
    if (findAttribute("xrDrive2rID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" ADriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_xrDriver2 = driver;
    }
    if (findAttribute("yrDriver2ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" AprimeDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_yrDriver2 = driver;
    }
    if (findAttribute("xrDriver3ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" ADriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_xrDriver3 = driver;
    }
    if (findAttribute("yrDriver3ID"s, &buf))
    {
        auto driver = simulation()->getDriver(buf);
        if (!driver) { setLastError("Driver ID=\""s + name() +"\" AprimeDriverID=\""s + buf + "\" not found"s); return lastErrorPtr(); }
        m_yrDriver3 = driver;
    }


    std::vector<NamedObject *> upstreamObjects;
    upstreamObjects.push_back(m_markerEllipseCentre);
    upstreamObjects.push_back(m_markerEllipseRim);
    upstreamObjects.push_back(m_phaseControlInput);
    if (m_omegaDriver) upstreamObjects.push_back(m_omegaDriver);
    if (m_sigmaDriver) upstreamObjects.push_back(m_sigmaDriver);
    if (m_xrDriver0) upstreamObjects.push_back(m_xrDriver0);
    if (m_yrDriver0) upstreamObjects.push_back(m_yrDriver0);
    if (m_xrDriver1) upstreamObjects.push_back(m_xrDriver1);
    if (m_yrDriver1) upstreamObjects.push_back(m_yrDriver1);
    if (m_xrDriver2) upstreamObjects.push_back(m_xrDriver2);
    if (m_yrDriver2) upstreamObjects.push_back(m_yrDriver2);
    if (m_xrDriver3) upstreamObjects.push_back(m_xrDriver3);
    if (m_yrDriver3) upstreamObjects.push_back(m_yrDriver3);
    setUpstreamObjects(std::move(upstreamObjects));
    return nullptr;
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void MarkerEllipseDriver::appendToAttributes()
{
    Driver::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "MarkerEllipse"s);
    setAttribute("Omega"s, *GSUtil::toString(m_omega, &buf));
    setAttribute("Sigma"s, *GSUtil::toString(m_sigma, &buf));
    setAttribute("xr"s, *GSUtil::toString(m_xr.data(), 4, &buf));
    setAttribute("yr"s, *GSUtil::toString(m_yr.data(), 4, &buf));
    setAttribute("Phi"s, *GSUtil::toString(m_phi, &buf));
    setAttribute("CentreMarkerID"s, m_markerEllipseCentre->name());
    setAttribute("RimMarkerID"s, m_markerEllipseRim->name());
    setAttribute("PhaseControlInputID"s, m_phaseControlInput->name());
    setAttribute("LowPassFrequency"s, *GSUtil::toString(m_butterworthFilter.cutoffFrequency(), &buf));
    setAttribute("PhaseOffset"s, *GSUtil::toString(m_phaseOffset, &buf));
    setAttribute("MaxPhiDot"s, *GSUtil::toString(m_maxPhiDot, &buf));
    setAttribute("PeriodMultiplier"s, *GSUtil::toString(m_periodMultiplier, &buf));
    if (m_omegaDriver) setAttribute("OmegaDriverID"s, m_omegaDriver->name());
    if (m_sigmaDriver) setAttribute("SigmaDriverID"s, m_sigmaDriver->name());
    if (m_xrDriver0) setAttribute("xrDriver0ID"s, m_xrDriver0->name());
    if (m_yrDriver0) setAttribute("yrDriver0ID"s, m_yrDriver0->name());
    if (m_xrDriver1) setAttribute("xrDriver1ID"s, m_xrDriver1->name());
    if (m_yrDriver1) setAttribute("yrDriver1ID"s, m_yrDriver1->name());
    if (m_xrDriver2) setAttribute("xrDriver2ID"s, m_xrDriver2->name());
    if (m_yrDriver2) setAttribute("yrDriver2ID"s, m_yrDriver2->name());
    if (m_xrDriver3) setAttribute("xrDriver3ID"s, m_xrDriver3->name());
    if (m_yrDriver3) setAttribute("yrDriver3ID"s, m_yrDriver3->name());
}
} // namespace GaitSym


