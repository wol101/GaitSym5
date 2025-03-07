/*
 *  Preferences.cpp
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 12/02/2017.
 *  Copyright 2017 Bill Sellers. All rights reserved.
 *
 */

#include "Preferences.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QColor>
#include <QFont>
#include <QDebug>
#include <QDomDocument>
#include <QSettings>
#include <QDataStream>

const QString Preferences::applicationName("GaitSym5");
const QString Preferences::organizationName("AnimalSimulationLaboratory");
QSettings Preferences::m_qtSettings(QSettings::IniFormat, QSettings::UserScope, Preferences::getOrganizationName(), Preferences::getApplicationName());
QMap<QString, SettingsItem> Preferences::m_settings;

void Preferences::Write()
{
    qDebug() << "Writing preferences to \"" << fileName() << "\"\n";
    Preferences::setQtValue("XML", ExportData(m_settings));
    Preferences::sync();
}

void Preferences::Read()
{
    qDebug() << "Reading preferences from \"" << fileName() << "\"\n";
    QByteArray xmlData = Preferences::qtValue("XML", QByteArray()).toByteArray();
    QMap<QString, SettingsItem> defaultSettings = ImportDefaults();
    m_settings = ImportData(xmlData);
    if (m_settings.size() == 0) // could use other sorts of error checking here - perhaps a unique code in the settings file that needs to match the subversion
    {
        qDebug() << "Error reading preferences from" << fileName();
        m_settings = defaultSettings;
    }
    // but sometimes the saved settings need to be corrected by the default settings e.g. as a result of a version change
    for (auto defaultIt = defaultSettings.begin(); defaultIt != defaultSettings.end(); ++defaultIt)
    {
        auto it = m_settings.find(defaultIt.key());
        if (it == m_settings.end())
        {
            qDebug() << "Error reading preferences from" << fileName() << "key" << defaultIt.key() << "missing so set to" << defaultIt.value().value;
            m_settings[defaultIt.key()] = defaultIt.value();
        }
        else
        {
            it.value().label = defaultIt.value().label;
            it.value().defaultValue = defaultIt.value().defaultValue;
            it.value().minimumValue = defaultIt.value().minimumValue;
            it.value().maximumValue = defaultIt.value().maximumValue;
            it.value().display = defaultIt.value().display;
            if (it.value().type != defaultIt.value().type)
            {
                qDebug() << "Error reading preferences from" << fileName() << "key" << defaultIt.key() << "type mismatch so set to" << defaultIt.value().value;
                it.value().type = defaultIt.value().type;
                it.value().value = defaultIt.value().value;
            }
        }
    }
}

void Preferences::Export(const QString &filename, const QMap<QString, SettingsItem> &settings)
{
    QByteArray xmlData = ExportData(settings);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Unable to open settings export file: %s", qPrintable(filename));
        return;
    }
    // and now the actual xml doc
    qint64 bytesWritten = file.write(xmlData);
    if (bytesWritten != xmlData.size()) qWarning("Unable to write to settings export file: %s", qPrintable(filename));
    file.close();
}

QByteArray Preferences::ExportData(const QMap<QString, SettingsItem> &settings)
{
    QDomDocument doc("GaitSym5Preferences");
    doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    QDomElement root = doc.createElement("PREFERENCES");
    doc.appendChild(root);

    // this bit of code gets the SettingsItems sorted by order
    QStringList keys = settings.keys();
    keys.sort(Qt::CaseInsensitive);

    for (QStringList::const_iterator i = keys.constBegin(); i != keys.constEnd(); i++)
    {
        SettingsItem item = settings[*i];
        QDomElement setting = doc.createElement("SETTING");
        root.appendChild(setting);
        setting.setAttribute("key", item.key);
        setting.setAttribute("type", item.value.typeName());
        setting.setAttribute("display", QString::number(item.display));
        setting.setAttribute("label", item.label);

        QMetaType::Type type = static_cast<QMetaType::Type>(item.value.typeId());
        switch (type)
        {
        case QMetaType::QStringList:
            setting.setAttribute("defaultValue", serialize(item.defaultValue.toStringList()));
            setting.setAttribute("value", serialize(item.value.toStringList()));
            break;
        case QMetaType::QByteArray:
            setting.setAttribute("defaultValue", QString::fromUtf8(item.defaultValue.toByteArray().toBase64(QByteArray::Base64UrlEncoding)));
            setting.setAttribute("value", QString::fromUtf8(item.value.toByteArray().toBase64( QByteArray::Base64UrlEncoding)));
            break;
        case QMetaType::QColor:
            setting.setAttribute("defaultValue", qvariant_cast<QColor>(item.defaultValue).name(QColor::HexArgb));
            setting.setAttribute("value", qvariant_cast<QColor>(item.value).name(QColor::HexArgb));
            break;
        case QMetaType::QFont:
            setting.setAttribute("defaultValue", qvariant_cast<QFont>(item.defaultValue).toString());
            setting.setAttribute("value", qvariant_cast<QFont>(item.value).toString());
            break;
        case QMetaType::QVector2D:
            setting.setAttribute("defaultValue", QString("%1 %2").arg(double(qvariant_cast<QVector2D>(item.defaultValue).x()))
                                 .arg(double(qvariant_cast<QVector2D>(item.defaultValue).y())));
            setting.setAttribute("value", QString("%1 %2").arg(double(qvariant_cast<QVector2D>(item.value).x()))
                                 .arg(double(qvariant_cast<QVector2D>(item.value).y())));
            break;
        case QMetaType::QVector3D:
            setting.setAttribute("defaultValue", QString("%1 %2 %3").arg(double(qvariant_cast<QVector3D>(item.defaultValue).x()))
                                  .arg(double(qvariant_cast<QVector3D>(item.defaultValue).y())).arg(double(qvariant_cast<QVector3D>(item.defaultValue).z())));
            setting.setAttribute("value", QString("%1 %2 %3").arg(double(qvariant_cast<QVector3D>(item.value).x()))
                                 .arg(double(qvariant_cast<QVector3D>(item.value).y())).arg(double(qvariant_cast<QVector3D>(item.value).z())));
            break;
        case QMetaType::Float:
        case QMetaType::Double:
        case QMetaType::Int:
            setting.setAttribute("defaultValue", item.defaultValue.toString());
            setting.setAttribute("value", item.value.toString());
            setting.setAttribute("minimumValue", item.minimumValue.toString());
            setting.setAttribute("maximumValue", item.maximumValue.toString());
            break;
        default:
            setting.setAttribute("defaultValue", item.defaultValue.toString());
            setting.setAttribute("value", item.value.toString());
        }
    }
    return doc.toByteArray(4);
}

QMap<QString, SettingsItem> Preferences::Import(const QString &filename)
{
    QMap<QString, SettingsItem> settings;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Unable to open settings export file: %s", qPrintable(filename));
        return settings;
    }
    QByteArray xmlData = file.readAll();
    settings = ImportData(xmlData);
    if (settings.size() == 0)
    {
        qWarning("Unable to parse settings export file: %s", qPrintable(filename));
        return settings;
    }
    return settings;
}

QMap<QString, SettingsItem> Preferences::ImportData(const QByteArray &xmlData)
{
    QMap<QString, SettingsItem> settings;
    QDomDocument doc("GaitSym5Preferences");
    if (!doc.setContent(xmlData)) { return settings; }
    QDomElement docElem = doc.documentElement();
    settings = ParseQDomElement(docElem);
    return settings;
}

QMap<QString, SettingsItem> Preferences::ParseQDomElement(const QDomElement &docElem)
{
    QMap<QString, SettingsItem> settings;
 // qDebug() << qPrintable(docElem.tagName()) << "\n";
    if (docElem.tagName() != "PREFERENCES")
    {
        qWarning("Unable to find tag PREFERENCES: %s", qPrintable(docElem.tagName()));
        return settings;
    }

    QFont font;
    SettingsItem item;
    QDomNode n = docElem.firstChild();
    while (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName() == "SETTING")
            {
                item.key = e.attribute("key");
                QString typeName = e.attribute("type");
                item.type = static_cast<QMetaType::Type>(QMetaType::fromName(typeName.toUtf8()).id());
                item.display = toBool(e.attribute("display"));
                item.label = e.attribute("label");
                QStringList stringList;
                switch (item.type)
                {
                case QMetaType::QStringList:
                    deserialize(QString(e.attribute("value")), &stringList);
                    item.value = stringList;
                    deserialize(QString(e.attribute("defaultValue")), &stringList);
                    item.defaultValue = stringList;
                    break;
                case QMetaType::QByteArray:
                    item.value = QByteArray::fromBase64(QString(e.attribute("value")).toUtf8(), QByteArray::Base64UrlEncoding);
                    item.defaultValue = QByteArray::fromBase64(QString(e.attribute("defaultValue")).toUtf8(), QByteArray::Base64UrlEncoding);
                    break;
                case QMetaType::QColor:
                    item.value = QColor(e.attribute("value"));
                    item.defaultValue = QColor(e.attribute("defaultValue"));
                    break;
                case QMetaType::QFont:
                    font.fromString(e.attribute("value"));
                    item.value = QVariant::fromValue(font);
                    font.fromString(e.attribute("defaultValue"));
                    item.defaultValue = QVariant::fromValue(font);
                    break;
                case QMetaType::QVector2D:
                    {
                        QStringList list = e.attribute("value").split(" ", Qt::SkipEmptyParts);
                        QVector2D value;
                        if (list.size() > 0) value.setX(list[0].toFloat());
                        if (list.size() > 1) value.setY(list[1].toFloat());
                        item.value = value;
                        list = e.attribute("defaultValue").split(" ", Qt::SkipEmptyParts);
                        QVector2D defaultValue;
                        if (list.size() > 0) defaultValue.setX(list[0].toFloat());
                        if (list.size() > 1) defaultValue.setY(list[1].toFloat());
                        item.defaultValue = defaultValue;
                    }
                    break;
                case QMetaType::QVector3D:
                    {
                        QStringList list = e.attribute("value").split(" ", Qt::SkipEmptyParts);
                        QVector3D value;
                        if (list.size() > 0) value.setX(list[0].toFloat());
                        if (list.size() > 1) value.setY(list[1].toFloat());
                        if (list.size() > 2) value.setZ(list[2].toFloat());
                        item.value = value;
                        list = e.attribute("defaultValue").split(" ", Qt::SkipEmptyParts);
                        QVector3D defaultValue;
                        if (list.size() > 0) defaultValue.setX(list[0].toFloat());
                        if (list.size() > 1) defaultValue.setY(list[1].toFloat());
                        if (list.size() > 2) defaultValue.setZ(list[2].toFloat());
                        item.defaultValue = defaultValue;
                    }
                    break;
                case QMetaType::Float:
                    item.value = e.attribute("value");
                    item.defaultValue = e.attribute("defaultValue");
                    if (e.hasAttribute("minimumValue")) item.minimumValue = e.attribute("minimumValue");
                    else item.minimumValue = -std::numeric_limits<float>::max();
                    if (e.hasAttribute("maximumValue")) item.maximumValue = e.attribute("maximumValue");
                    else item.maximumValue = std::numeric_limits<float>::max();
                    item.value.convert(QMetaType(item.type));
                    item.defaultValue.convert(QMetaType(item.type));
                    item.minimumValue.convert(QMetaType(item.type));
                    item.maximumValue.convert(QMetaType(item.type));
                    break;
                case QMetaType::Double:
                    item.value = e.attribute("value");
                    item.defaultValue = e.attribute("defaultValue");
                    if (e.hasAttribute("minimumValue")) item.minimumValue = e.attribute("minimumValue");
                    else item.minimumValue = -std::numeric_limits<double>::max();
                    if (e.hasAttribute("maximumValue")) item.maximumValue = e.attribute("maximumValue");
                    else item.maximumValue = std::numeric_limits<double>::max();
                    item.value.convert(QMetaType(item.type));
                    item.defaultValue.convert(QMetaType(item.type));
                    item.minimumValue.convert(QMetaType(item.type));
                    item.maximumValue.convert(QMetaType(item.type));
                    break;
                case QMetaType::Int:
                    item.value = e.attribute("value");
                    item.defaultValue = e.attribute("defaultValue");
                    if (e.hasAttribute("minimumValue")) item.minimumValue = e.attribute("minimumValue");
                    else item.minimumValue = std::numeric_limits<int>::min();
                    if (e.hasAttribute("maximumValue")) item.maximumValue = e.attribute("maximumValue");
                    else item.maximumValue = std::numeric_limits<int>::max();
                    item.value.convert(QMetaType(item.type));
                    item.defaultValue.convert(QMetaType(item.type));
                    item.minimumValue.convert(QMetaType(item.type));
                    item.maximumValue.convert(QMetaType(item.type));
                    break;
                default:
                    item.value = e.attribute("value");
                    item.defaultValue = e.attribute("defaultValue");
                    item.value.convert(QMetaType(item.type));
                    item.defaultValue.convert(QMetaType(item.type));
                }
                settings[item.key] = item;
            }
        }
        n = n.nextSibling();
    }
    return settings;
}

QMap<QString, SettingsItem> Preferences::ImportDefaults()
{
    QMap<QString, SettingsItem> settings = Import(":/preferences/default_values.xml");
    return settings;
}

bool Preferences::toBool(const QString &string)
{
    if (string.trimmed().compare("true", Qt::CaseInsensitive) == 0 || string.toInt() != 0) return true;
    return false;
}

const SettingsItem Preferences::settingsItem(const QString &key)
{
    return m_settings.value(key);
}

const QVariant Preferences::valueQVariant(const QString &key)
{
    QVariant v;
    if (m_settings.contains(key)) v = m_settings.value(key).value;
    else insert(key, v);
    return v;
}

const QString Preferences::valueQString(const QString &key)
{
    QString v;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toString();
    else insert(key, v);
    return v;
}

const QColor Preferences::valueQColor(const QString &key)
{
    QColor v;
    if (m_settings.contains(key)) v = qvariant_cast<QColor>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QFont Preferences::valueQFont(const QString &key)
{
    QFont v;
    if (m_settings.contains(key)) v = qvariant_cast<QFont>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QByteArray Preferences::valueQByteArray(const QString &key)
{
    QByteArray v;
    if (m_settings.contains(key)) v = qvariant_cast<QByteArray>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QStringList Preferences::valueQStringList(const QString &key)
{
    QStringList v;
    if (m_settings.contains(key)) v = qvariant_cast<QStringList>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QVector2D Preferences::valueQVector2D(const QString &key)
{
    QVector2D v;
    if (m_settings.contains(key)) v = qvariant_cast<QVector2D>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QVector3D Preferences::valueQVector3D(const QString &key)
{
    QVector3D v;
    if (m_settings.contains(key)) v = qvariant_cast<QVector3D>(m_settings.value(key).value);
    else insert(key, v);
    return v;
}

const QVariant Preferences::valueQVariant(const QString &key, const QVariant &defaultValue)
{
    QVariant v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value;
    else insert(key, v, defaultValue);
    return v;
}

const QString Preferences::valueQString(const QString &key, const QString &defaultValue)
{
    QString v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toString();
    else insert(key, v, defaultValue);
    return v;
}

const QColor Preferences::valueQColor(const QString &key, const QColor &defaultValue)
{
    QColor v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QColor>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

const QFont Preferences::valueQFont(const QString &key, const QFont &defaultValue)
{
    QFont v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QFont>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

const QByteArray Preferences::valueQByteArray(const QString &key, const QByteArray &defaultValue)
{
    QByteArray v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QByteArray>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

const QStringList Preferences::valueQStringList(const QString &key, const QStringList &defaultValue)
{
    QStringList v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QStringList>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

const QVector2D Preferences::valueQVector2D(const QString &key, const QVector2D &defaultValue)
{
    QVector2D v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QVector2D>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

const QVector3D Preferences::valueQVector3D(const QString &key, const QVector3D &defaultValue)
{
    QVector3D v = defaultValue;
    if (m_settings.contains(key)) v = qvariant_cast<QVector3D>(m_settings.value(key).value);
    else insert(key, v, defaultValue);
    return v;
}

double Preferences::valueDouble(const QString &key)
{
    double v = 0.0;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toDouble();
    else insert(key, v);
    return v;
}

float Preferences::valueFloat(const QString &key)
{
    float v = 0.0f;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toFloat();
    else insert(key, v);
    return v;
}

int Preferences::valueInt(const QString &key)
{
    int v = 0;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toInt();
    else insert(key, v);
    return v;
}

bool Preferences::valueBool(const QString &key)
{
    bool v = false;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toBool();
    else insert(key, v);
    return v;
}

double Preferences::valueDouble(const QString &key, double defaultValue)
{
    double v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toDouble();
    else insert(key, v, defaultValue);
    return v;
}

float Preferences::valueFloat(const QString &key, float defaultValue)
{
    float v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toFloat();
    else insert(key, v, defaultValue);
    return v;
}

int Preferences::valueInt(const QString &key, int defaultValue)
{
    int v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toInt();
    else insert(key, v, defaultValue);
    return v;
}

bool Preferences::valueBool(const QString &key, bool defaultValue)
{
    bool v = defaultValue;
    if (m_settings.contains(key)) v = m_settings.value(key).value.toBool();
    else insert(key, v, defaultValue);
    return v;
}


void Preferences::insert(const SettingsItem &item)
{
    m_settings.insert(item.key, item);
}

bool Preferences::contains(const QString &key)
{
    return m_settings.contains(key);
}

QStringList Preferences::keys()
{
    return m_settings.keys();
}

void Preferences::insert(const QString &key, const QVariant &value)
{
    SettingsItem item;
    if (m_settings.contains(key))
    {
        item = m_settings.value(key);
        item.value = value;
        m_settings.insert(key, item);
    }
    else
    {
        auto typeID = value.typeId();
        if (static_cast<QMetaType::Type>(typeID) != QMetaType::QByteArray) // QByteArray preferences are always things like the window geometry so I would not expect them
            qDebug("Preferences::value %s \"%s\" missing", value.typeName(), qUtf8Printable(key));
        item.key = key;
        item.display = false;
        item.label = key;
        item.value = value;
        item.type = static_cast<QMetaType::Type>(typeID);
        item.defaultValue = QVariant(typeID);
        switch (item.type)
        {
        case QMetaType::Double:
            item.minimumValue = -std::numeric_limits<double>::max();
            item.maximumValue = std::numeric_limits<double>::max();
            break;
        case QMetaType::Float:
            item.minimumValue = -std::numeric_limits<float>::max();
            item.maximumValue = std::numeric_limits<float>::max();
            break;
        case QMetaType::Int:
            item.minimumValue = std::numeric_limits<int>::min();
            item.maximumValue = std::numeric_limits<int>::max();
            break;
        default:
            item.minimumValue = QString();
            item.maximumValue = QString();
        }
        m_settings.insert(key, item);
    }
}

void Preferences::insert(const QString &key, const QVariant &value, const QVariant &defaultValue)
{
    SettingsItem item;
    if (m_settings.contains(key))
    {
        item = m_settings.value(key);
        item.value = value;
        item.defaultValue = defaultValue;
        m_settings.insert(key, item);
    }
    else
    {
        auto typeID = value.typeId();
        item.key = key;
        item.display = false;
        item.label = key;
        item.value = value;
        item.type = static_cast<QMetaType::Type>(typeID);
        item.defaultValue = defaultValue;
        switch (item.type)
        {
        case QMetaType::Double:
            item.minimumValue = -std::numeric_limits<double>::max();
            item.maximumValue = std::numeric_limits<double>::max();
            break;
        case QMetaType::Float:
            item.minimumValue = -std::numeric_limits<float>::max();
            item.maximumValue = std::numeric_limits<float>::max();
            break;
        case QMetaType::Int:
            item.minimumValue = INT_MIN;
            item.maximumValue = INT_MAX;
            break;
        default:
            item.minimumValue = QString();
            item.maximumValue = QString();
        }
        m_settings.insert(key, item);
    }
}

void Preferences::setQtValue(const QString &key, const QVariant &value)
{
    QString newKey1 = QString("%1_key_%2").arg(applicationName, key);
    QString newKey2 = QString("%1_type_%2").arg(applicationName, key);
    m_qtSettings.setValue(newKey1, value);
    m_qtSettings.setValue(newKey2, value.typeName()); // we have to do this because some settings formats lose the explicit type
}

QVariant Preferences::qtValue(const QString &key, const QVariant &defaultValue)
{
    QVariant variant = defaultValue;
    QString newKey1 = QString("%1_key_%2").arg(applicationName, key);
    QString newKey2 = QString("%1_type_%2").arg(applicationName, key);
    if (m_qtSettings.contains(newKey1))
    {
        variant = m_qtSettings.value(newKey1);
        if (m_qtSettings.contains(newKey2))
        {
            QString typeName = m_qtSettings.value(newKey2).toString();
            variant.convert(QMetaType::fromName(typeName.toUtf8())); // convert to the type stored in the settings
        }
        else
        {
            m_qtSettings.setValue(newKey2, defaultValue.typeName());
        }
    }
    else     // if the setting does not exist, create it and set it to the default value
    {
        setQtValue(key, defaultValue);
    }
    return variant;
}

void Preferences::clear()
{
    m_qtSettings.clear();
}

void Preferences::sync()
{
    m_qtSettings.sync();
}

QStringList Preferences::allKeys()
{
    QStringList keys = m_qtSettings.allKeys();
    QStringList newKeys;
    newKeys.reserve(keys.size());
    QString prefix = QString("%1_key_").arg(applicationName);
    for (int i = 0; i < keys.size(); i++)
    {
        if (keys.at(i).startsWith(prefix) == false) continue;
        QString newKey = keys.at(i).mid(prefix.size());
        newKeys.append(newKey);
    }
    return newKeys;
}

QString Preferences::fileName()
{
    return m_qtSettings.fileName();
}

const QString &Preferences::getApplicationName()
{
    return applicationName;
}

const QString &Preferences::getOrganizationName()
{
    return organizationName;
}

QString Preferences::serialize(const QStringList &data)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << data;
    return QString::fromUtf8(byteArray.toBase64( QByteArray::Base64UrlEncoding));
}

void Preferences::deserialize(const QString &qString, QStringList *data)
{
    QByteArray bytes = QByteArray::fromBase64(qString.toUtf8(), QByteArray::Base64UrlEncoding);
    QDataStream in(&bytes, QIODevice::ReadOnly);
    in >> *data;
}

QMap<QString, SettingsItem> Preferences::settings()
{
    return m_settings;
}

void Preferences::setSettings(const QMap<QString, SettingsItem> &newSettings)
{
    m_settings = newSettings;
}
