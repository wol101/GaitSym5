/*
 *  Global.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/11/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#include "Global.h"
#include "GSUtil.h"

#include "pystring.h"

#include <string>

using namespace std::string_literals;
namespace GaitSym {

Global::Global()
{
    m_springConstant = m_erp / (m_cfm * m_stepSize);
    m_dampingConstant = (1.0 - m_erp) / m_cfm;
}

double Global::springConstant() const
{
    return m_springConstant;
}

void Global::setSpringConstant(double SpringConstant)
{
    m_springConstant = SpringConstant;
}

double Global::dampingConstant() const
{
    return m_dampingConstant;
}

void Global::setDampingConstant(double DampingConstant)
{
    m_dampingConstant = DampingConstant;
}

std::vector<std::string> *Global::meshSearchPath()
{
    return &m_meshSearchPath;
}

const std::vector<std::string> *Global::constMeshSearchPath() const
{
    return &m_meshSearchPath;
}

void Global::meshSearchPathAddToFront(const std::string &meshSearchPath)
{
    if (m_meshSearchPath[0] != meshSearchPath)
    {
        meshSearchPathRemove(meshSearchPath);
        m_meshSearchPath.insert(m_meshSearchPath.begin(), meshSearchPath); // because there is no push_front in a vector
    }
}

void Global::meshSearchPathAddToBack(const std::string &meshSearchPath)
{
    if (m_meshSearchPath[m_meshSearchPath.size() - 1] != meshSearchPath)
    {
        meshSearchPathRemove(meshSearchPath);
        m_meshSearchPath.push_back(meshSearchPath);
    }
}

bool Global::meshSearchPathRemove(const std::string &meshSearchPath)
{
    bool altered = false;
    for (auto it = m_meshSearchPath.begin(); it != m_meshSearchPath.end();)
    {
        if ((*it) == meshSearchPath)
        {
            it = m_meshSearchPath.erase(it);
            altered = true;
        }
        else
        {
            it++;
        }
    }
    return altered;
}

double Global::linearDamping() const
{
    return m_linearDamping;
}

void Global::setLinearDamping(double LinearDamping)
{
    m_linearDamping = LinearDamping;
}

double Global::angularDamping() const
{
    return m_angularDamping;
}

void Global::setAngularDamping(double AngularDamping)
{
    m_angularDamping = AngularDamping;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *Global::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    std::string buf2;
    double m_DoubleList[3];
    size_t i;

    if (findAttribute("PhysicsEngine", &buf) == nullptr) return lastErrorPtr();
    for (i = 0; i < physicsEngineTypeCount; i++)
    {
        if (strcmp(buf.c_str(), physicsEngineTypeStrings(i)) == 0)
        {
            m_physicsEngine = PhysicsEngine(i);
            break;
        }
    }
    if (i >= physicsEngineTypeCount)
    {
        setLastError("Error GLOBAL: Unrecognised PhysicsEngine=\""s + buf + "\""s);
        return lastErrorPtr();
    }

    // gravity
    if (findAttribute("GravityVector", &buf) == nullptr) return lastErrorPtr();
    GSUtil::toDouble(buf, 3, m_DoubleList);
    m_gravity.Set(m_DoubleList);

    // set the simulation integration step size
    if (findAttribute("IntegrationStepSize", &buf) == nullptr) return lastErrorPtr();
    m_stepSize = GSUtil::toDouble(buf);
    if (m_stepSize <= 0.0) { setLastError("Error: GLOBAL IntegrationStepSize must be > 0"s); return lastErrorPtr(); }

    while (true)
    {
        if (m_physicsEngine == ODE)
        {
            // can specify ERP & CFM; SpringConstant & DampingConstant; SpringConstant & ERP; SpringConstant & CFM; DampingConstant & ERP; DampingConstant & CFM
            if (findAttribute("ERP", &buf) && findAttribute("CFM", &buf2))
            {
                m_erp = GSUtil::toDouble(buf);
                m_cfm = GSUtil::toDouble(buf2);
                if (m_erp <= 0.0) { setLastError("Error: GLOBAL ERP must be > 0"s); return lastErrorPtr(); }
                if (m_cfm <= 0.0) { setLastError("Error: GLOBAL CFM must be > 0"s); return lastErrorPtr(); }
                m_springConstant = m_erp / (m_cfm * m_stepSize);
                m_dampingConstant = (1.0 - m_erp) / m_cfm;
            }
            else if (findAttribute("ERP", &buf) && findAttribute("SpringConstant", &buf2))
            {
                m_erp = GSUtil::toDouble(buf);
                m_springConstant = GSUtil::toDouble(buf2);
                if (m_erp <= 0.0) { setLastError("Error: GLOBAL ERP must be > 0"s); return lastErrorPtr(); }
                if (m_springConstant <= 0.0) { setLastError("Error: GLOBAL SpringConstant must be > 0"s); return lastErrorPtr(); }
                m_dampingConstant = m_stepSize * (m_springConstant / m_erp - m_springConstant);
                m_cfm = 1.0/(m_stepSize * m_springConstant + m_dampingConstant);
            }
            else if (findAttribute("ERP", &buf) && findAttribute("DampingConstant", &buf2))
            {
                m_erp = GSUtil::toDouble(buf);
                m_dampingConstant = GSUtil::toDouble(buf2);
                if (m_erp <= 0.0) { setLastError("Error: GLOBAL ERP must be > 0"s); return lastErrorPtr(); }
                if (m_dampingConstant <= 0.0) { setLastError("Error: GLOBAL DampingConstant must be > 0"s); return lastErrorPtr(); }
                m_springConstant = m_dampingConstant / (m_stepSize / m_erp - m_stepSize);
                m_cfm = 1.0/(m_stepSize * m_springConstant + m_dampingConstant);
            }
            else if (findAttribute("CFM", &buf) && findAttribute("DampingConstant", &buf2))
            {
                m_cfm = GSUtil::toDouble(buf);
                m_dampingConstant = GSUtil::toDouble(buf2);
                if (m_cfm <= 0.0) { setLastError("Error: GLOBAL CFM must be > 0"s); return lastErrorPtr(); }
                if (m_dampingConstant <= 0.0) { setLastError("Error: GLOBAL DampingConstant must be > 0"s); return lastErrorPtr(); }
                m_springConstant = (1.0 / m_cfm - m_dampingConstant) / m_stepSize;
                m_erp = m_stepSize * m_springConstant/(m_stepSize * m_springConstant + m_dampingConstant);
            }
            else if (findAttribute("CFM", &buf) && findAttribute("SpringConstant", &buf2))
            {
                m_cfm = GSUtil::toDouble(buf);
                m_springConstant = GSUtil::toDouble(buf2);
                if (m_cfm <= 0.0) { setLastError("Error: GLOBAL CFM must be > 0"s); return lastErrorPtr(); }
                if (m_springConstant <= 0.0) { setLastError("Error: GLOBAL SpringConstant must be > 0"s); return lastErrorPtr(); }
                m_dampingConstant = 1.0 / m_cfm - m_stepSize * m_springConstant;
                m_erp = m_stepSize * m_springConstant/(m_stepSize * m_springConstant + m_dampingConstant);
            }
            else if (findAttribute("DampingConstant", &buf) && findAttribute("SpringConstant", &buf2))
            {
                m_dampingConstant = GSUtil::toDouble(buf);
                m_springConstant = GSUtil::toDouble(buf2);
                m_cfm = 1.0/(m_stepSize * m_springConstant + m_dampingConstant);
                m_erp = m_stepSize * m_springConstant/(m_stepSize * m_springConstant + m_dampingConstant);
            }
            else
            {
                setLastError("Error: GLOBAL needs one of these pairs ERP & CFM; SpringConstant & DampingConstant; SpringConstant & ERP; SpringConstant & CFM; DampingConstant & ERP; DampingConstant & CFM"s);
                return lastErrorPtr();
            }

            if (findAttribute("ContactMaxCorrectingVel", &buf) == nullptr) return lastErrorPtr();
            m_contactMaxCorrectingVel = GSUtil::toDouble(buf);
            if (m_contactMaxCorrectingVel < 0.0) { setLastError("Error: GLOBAL ContactMaxCorrectingVel must be >= 0"s); return lastErrorPtr(); }

            if (findAttribute("ContactSurfaceLayer", &buf) == nullptr) return lastErrorPtr();
            m_contactSurfaceLayer = GSUtil::toDouble(buf);
            if (m_contactSurfaceLayer < 0.0) { setLastError("Error: GLOBAL ContactSurfaceLayer must be >= 0"s); return lastErrorPtr(); }

            // get the stepper required
            // WorldStep, accurate but slow
            // QuickStep, faster but less accurate
            findAttribute("StepType", &buf);
            for (i = 0; i < stepTypeCount; i++)
            {
                if (strcmp(buf.c_str(), stepTypeStrings(i)) == 0)
                {
                    m_stepType = StepType(i);
                    break;
                }
            }
            if (i >= stepTypeCount)
            {
                setLastError("GLOBAL: Unrecognised StepType=\""s + buf + "\""s);
                return lastErrorPtr();
            }
            break;
        }

        if (m_physicsEngine == PhysX)
        {
            if (findAttribute("DefaultLength", &buf) == nullptr) return lastErrorPtr();
            m_defaultLength = GSUtil::toDouble(buf);
            if (findAttribute("DefaultSpeed", &buf) == nullptr) return lastErrorPtr();
            m_defaultSpeed = GSUtil::toDouble(buf);
            break;
        }

        if (m_physicsEngine == MuJoCo)
        {
            break;
        }

        setLastError("GLOBAL: Unimplemented PhysicsEngine"s);
        return lastErrorPtr();
    }

    // allow internal collisions
    if (findAttribute("AllowInternalCollisions", &buf) == nullptr) return lastErrorPtr();
    m_allowInternalCollisions = GSUtil::toBool(buf);

    // allow collisions for objects connected by a joint
    if (findAttribute("AllowConnectedCollisions", &buf) == nullptr) return lastErrorPtr();
    m_allowConnectedCollisions = GSUtil::toBool(buf);

    if (findAttribute("LinearDamping"s, &buf)) this->setLinearDamping(GSUtil::toDouble(buf));
    if (findAttribute("AngularDamping"s, &buf)) this->setAngularDamping(GSUtil::toDouble(buf));

    // now some run parameters

    if (findAttribute("BMR", &buf) == nullptr) return lastErrorPtr();
    m_bmr = GSUtil::toDouble(buf);

    if (findAttribute("TimeLimit", &buf) == nullptr) return lastErrorPtr();
    m_timeLimit = GSUtil::toDouble(buf);
    if (findAttribute("MechanicalEnergyLimit", &buf) == nullptr) return lastErrorPtr();
    m_mechanicalEnergyLimit = GSUtil::toDouble(buf);
    if (findAttribute("MetabolicEnergyLimit", &buf) == nullptr) return lastErrorPtr();
    m_metabolicEnergyLimit = GSUtil::toDouble(buf);
    if (findAttribute("FitnessType", &buf) == nullptr) return lastErrorPtr();
    for (i = 0; i < fitnessTypeCount; i++)
    {
        if (strcmp(buf.c_str(), fitnessTypeStrings(i)) == 0)
        {
            m_fitnessType = FitnessType(i);
            break;
        }
    }
    if (i >= fitnessTypeCount)
    {
        setLastError("Error GLOBAL: Unrecognised FitnessType=\""s + buf + "\""s);
        return lastErrorPtr();
    }

    if (findAttribute("PermittedNumericalErrors", &buf)) m_permittedNumericalErrors = GSUtil::toInt(buf);
    if (findAttribute("NumericalErrorsScore", &buf)) m_numericalErrorsScore = GSUtil::toDouble(buf);

    m_meshSearchPath.clear();
    findAttribute("MeshSearchPath", &buf);
    std::vector<std::string> encodedMeshSearchPath;
    if (buf.size())
    {
        pystring::split(buf, encodedMeshSearchPath, ":"s);
        for (i = 0; i < encodedMeshSearchPath.size(); i++) m_meshSearchPath.push_back(percentDecode(encodedMeshSearchPath[i]));
    }

    return nullptr;
}

// this function copies the data in the object to an xml_node node that it creates internally.
// doc is used to allocate the memory so deletion should be automatic
void Global::saveToAttributes()
{
    this->setTag("GLOBAL"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

void Global::appendToAttributes()
{
    NamedObject::appendToAttributes();
    std::string buf;

    setAttribute("AllowConnectedCollisions", *GSUtil::toString(m_allowConnectedCollisions, &buf));
    setAttribute("AllowInternalCollisions", *GSUtil::toString(m_allowInternalCollisions, &buf));
    setAttribute("BMR", *GSUtil::toString(m_bmr, &buf));
    setAttribute("CFM", *GSUtil::toString(m_cfm, &buf));
    setAttribute("ContactMaxCorrectingVel", *GSUtil::toString(m_contactMaxCorrectingVel, &buf));
    setAttribute("ContactSurfaceLayer", *GSUtil::toString(m_contactSurfaceLayer, &buf));
    setAttribute("ERP", *GSUtil::toString(m_erp, &buf));
    setAttribute("FitnessType", fitnessTypeStrings(m_fitnessType));
    setAttribute("LinearDamping", *GSUtil::toString(m_linearDamping, &buf));
    setAttribute("AngularDamping", *GSUtil::toString(m_angularDamping, &buf));
    setAttribute("GravityVector", *GSUtil::toString(m_gravity, &buf));
    setAttribute("IntegrationStepSize", *GSUtil::toString(m_stepSize, &buf));
    setAttribute("MechanicalEnergyLimit", *GSUtil::toString(m_mechanicalEnergyLimit, &buf));
    setAttribute("MetabolicEnergyLimit", *GSUtil::toString(m_metabolicEnergyLimit, &buf));
    setAttribute("PhysicsEngine", physicsEngineTypeStrings(m_physicsEngine));
    setAttribute("StepType", stepTypeStrings(m_stepType));
    setAttribute("TimeLimit", *GSUtil::toString(m_timeLimit, &buf));
    setAttribute("NumericalErrorsScore", *GSUtil::toString(m_numericalErrorsScore, &buf));
    setAttribute("PermittedNumericalErrors", *GSUtil::toString(m_permittedNumericalErrors, &buf));

    std::vector<std::string> encodedMeshSearchPath;
    for (size_t i = 0; i < m_meshSearchPath.size(); i++) encodedMeshSearchPath.push_back(percentEncode(m_meshSearchPath[i], "%:"s));
    setAttribute("MeshSearchPath", pystring::join(":"s, encodedMeshSearchPath));
}

std::string Global::percentEncode(const std::string &input, const std::string &encodeList)
{
    // this routine encodes the characters in encodeList

//    UTF-8 cheat sheet
//    Binary    Hex          Comments
//    0xxxxxxx  0x00..0x7F   Only byte of a 1-byte character encoding
//    10xxxxxx  0x80..0xBF   Continuation bytes (1-3 continuation bytes)
//    110xxxxx  0xC0..0xDF   First byte of a 2-byte character encoding
//    1110xxxx  0xE0..0xEF   First byte of a 3-byte character encoding
//    11110xxx  0xF0..0xF4   First byte of a 4-byte character encoding

    // what this means is that no UTF-8 character looks like a 1-byte colon or 1-byte percent
    std::string output;
    static const std::string digits = "0123456789ABCDEF"s;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (encodeList.find(input[i]) == std::string::npos)
        {
            output.push_back(input[i]);
            continue;
        }
        output.push_back('%');
        uint8_t quotient = uint8_t(input[i]) / uint8_t(16);
        uint8_t remainder = uint8_t(input[i]) % uint8_t(16);
        output.push_back(digits[quotient]);
        output.push_back(digits[remainder]);
    }
    return output;
}

std::string Global::percentDecode(const std::string &input)
{
    // this routine decodes everything that is percent encoded in the string (%XX)
    // if the encoding is poorly formed it assumes that no encoding is present

//    UTF-8 cheat sheet
//    Binary    Hex          Comments
//    0xxxxxxx  0x00..0x7F   Only byte of a 1-byte character encoding
//    10xxxxxx  0x80..0xBF   Continuation bytes (1-3 continuation bytes)
//    110xxxxx  0xC0..0xDF   First byte of a 2-byte character encoding
//    1110xxxx  0xE0..0xEF   First byte of a 3-byte character encoding
//    11110xxx  0xF0..0xF4   First byte of a 4-byte character encoding

    // what this means is that no UTF-8 character looks like a 1-byte percent
    std::string output;
    static const std::map<char, uint8_t> characterMap =
    {
        {'0', 0}, {'1', 1},
        {'2', 2}, {'3', 3},
        {'4', 4}, {'5', 5},
        {'6', 6}, {'7', 7},
        {'8', 8}, {'9', 9},
        {'A', 10}, {'B', 11},
        {'C', 12}, {'D', 13},
        {'E', 14}, {'F', 15},
        {'a', 10}, {'b', 11},
        {'c', 12}, {'d', 13},
        {'e', 14}, {'f', 15}
    };
    for (size_t i = 0; i < input.size(); i++)
    {
        if (input[i] != '%')
        {
            output.push_back(input[i]);
            continue;
        }
        if (i >= input.size() - 2) { output.push_back(input[i]); continue; }
        auto it1 = characterMap.find(input[i + 1]);
        auto it2 = characterMap.find(input[i + 2]);
        if (it1 == characterMap.end() || it2 == characterMap.end()) { output.push_back(input[i]); continue; }
        uint8_t decodedChar = it1->second * uint8_t(16) + it2->second;
        output.push_back(char(decodedChar));
        i += 2;
    }
    return output;
}

int Global::permittedNumericalErrors() const
{
    return m_permittedNumericalErrors;
}

void Global::setPermittedNumericalErrors(int PermittedNumericalErrors)
{
    m_permittedNumericalErrors = PermittedNumericalErrors;
}

double Global::numericalErrorsScore() const
{
    return m_numericalErrorsScore;
}

void Global::setNumericalErrorsScore(double NumericalErrorsScore)
{
    m_numericalErrorsScore = NumericalErrorsScore;
}

double Global::defaultLength() const
{
    return m_defaultLength;
}

void Global::setDefaultLength(double newDefaultLength)
{
    m_defaultLength = newDefaultLength;
}

double Global::defaultSpeed() const
{
    return m_defaultSpeed;
}

void Global::setDefaultSpeed(double newDefaultSpeed)
{
    m_defaultSpeed = newDefaultSpeed;
}

Global::PhysicsEngine Global::physicsEngine() const
{
    return m_physicsEngine;
}

void Global::setPhysicsEngine(PhysicsEngine newPhysicsEngine)
{
    m_physicsEngine = newPhysicsEngine;
}

Global::FitnessType Global::fitnessType() const
{
    return m_fitnessType;
}

void Global::setFitnessType(FitnessType fitnessType)
{
    m_fitnessType = fitnessType;
}

Global::StepType Global::stepType() const
{
    return m_stepType;
}

void Global::setStepType(Global::StepType stepType)
{
    m_stepType = stepType;
}

bool Global::allowConnectedCollisions() const
{
    return m_allowConnectedCollisions;
}

void Global::setAllowConnectedCollisions(bool allowConnectedCollisions)
{
    m_allowConnectedCollisions = allowConnectedCollisions;
}

bool Global::allowInternalCollisions() const
{
    return m_allowInternalCollisions;
}

void Global::setAllowInternalCollisions(bool allowInternalCollisions)
{
    m_allowInternalCollisions = allowInternalCollisions;
}

pgd::Vector3 Global::gravity() const
{
    return m_gravity;
}

void Global::setGravity(const pgd::Vector3 &gravity)
{
    m_gravity = gravity;
}

void Global::setGravity(double gravityX, double gravityY, double gravityZ)
{
    m_gravity.Set(gravityX, gravityY, gravityZ);
}

double Global::BMR() const
{
    return m_bmr;
}

void Global::setBMR(double BMR)
{
    m_bmr = BMR;
}

double Global::CFM() const
{
    return m_cfm;
}

void Global::setCFM(double CFM)
{
    m_cfm = CFM;
}

double Global::contactMaxCorrectingVel() const
{
    return m_contactMaxCorrectingVel;
}

void Global::setContactMaxCorrectingVel(double ContactMaxCorrectingVel)
{
    m_contactMaxCorrectingVel = ContactMaxCorrectingVel;
}

double Global::contactSurfaceLayer() const
{
    return m_contactSurfaceLayer;
}

void Global::setContactSurfaceLayer(double ContactSurfaceLayer)
{
    m_contactSurfaceLayer = ContactSurfaceLayer;
}

double Global::ERP() const
{
    return m_erp;
}

void Global::setERP(double ERP)
{
    m_erp = ERP;
}

double Global::mechanicalEnergyLimit() const
{
    return m_mechanicalEnergyLimit;
}

void Global::setMechanicalEnergyLimit(double MechanicalEnergyLimit)
{
    m_mechanicalEnergyLimit = MechanicalEnergyLimit;
}

double Global::metabolicEnergyLimit() const
{
    return m_metabolicEnergyLimit;
}

void Global::setMetabolicEnergyLimit(double MetabolicEnergyLimit)
{
    m_metabolicEnergyLimit = MetabolicEnergyLimit;
}

double Global::stepSize() const
{
    return m_stepSize;
}

void Global::setStepSize(double StepSize)
{
    m_stepSize = StepSize;
}

double Global::timeLimit() const
{
    return m_timeLimit;
}

void Global::setTimeLimit(double TimeLimit)
{
    m_timeLimit = TimeLimit;
}



} // namespace GaitSym


