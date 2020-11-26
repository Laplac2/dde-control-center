/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displaymodel.h"

using namespace dcc;
using namespace dcc::display;

const QString DisplayModel::DDE_Display_Config = "_dde_display_config_private";

const double DoubleZero = 0.000001;

bool contains(const QList<Resolution> &container, const Resolution &item)
{
    for (auto r : container)
        if (r.width() == item.width() && r.height() == item.height())
            return true;

    return false;
}

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
    , m_screenHeight(0)
    , m_screenWidth(0)
    , m_uiScale(1)
    , m_minimumBrightnessScale(0.0)
    , m_redshiftIsValid(false)
    , m_allowEnableMultiScaleRatio(false)
    , m_isMerged(false)
{

}

double DisplayModel::monitorScale(Monitor *moni)
{
    qDebug() << "ui scale : "<< m_uiScale << "\tmonitor scale:" << moni->scale();
    return moni->scale() < 1.0 ? m_uiScale : moni->scale();
}

const QList<Resolution> DisplayModel::monitorsSameModeList() const
{
    Q_ASSERT(m_monitors.size() > 1);

    QList<Resolution> resultList = m_monitors.first()->modeList();
    for (int i(1); i != m_monitors.size(); ++i) {
        const QList<Resolution> originList = m_monitors[i]->modeList();
        QList<Resolution> filteredList;

        for (auto r : resultList)
            if (contains(originList, r))
                filteredList.append(r);
        resultList = filteredList;
    }

    return resultList;
}

Monitor *DisplayModel::primaryMonitor() const
{
    for (auto mon : m_monitors)
        if (mon->name() == m_primary)
            return mon;

    return nullptr;
}

bool DisplayModel::monitorsIsIntersect() const
{
    if (m_monitors.size() < 2)
        return false;

    // only support 2 screens
    Q_ASSERT(m_monitors.size() == 2);

    return m_monitors.first()->rect().intersects(m_monitors.last()->rect());
}

void DisplayModel::setScreenHeight(const int h)
{
    if (m_screenHeight != h) {
        m_screenHeight = h;
        Q_EMIT screenHeightChanged(m_screenHeight);
    }
}

void DisplayModel::setScreenWidth(const int w)
{
    if (m_screenWidth != w) {
        m_screenWidth = w;
        Q_EMIT screenWidthChanged(m_screenWidth);
    }
}

void DisplayModel::setDisplayMode(const int mode)
{
    if (m_mode != mode && mode >= 0 && mode < 5) {
        m_mode = mode;
        Q_EMIT displayModeChanged(m_mode);
    }
}

void DisplayModel::setUIScale(const double scale)
{
    if (fabs(m_uiScale - scale) > DoubleZero) {
        m_uiScale = scale;
        Q_EMIT uiScaleChanged(m_uiScale);
    }
}

void DisplayModel::setMinimumBrightnessScale(const double scale)
{
    if (fabs(m_minimumBrightnessScale - scale) > DoubleZero) {
        m_minimumBrightnessScale = scale;
        Q_EMIT minimumBrightnessScaleChanged(m_minimumBrightnessScale);
    }
}

void DisplayModel::setPrimary(const QString &primary)
{
    if (m_primary != primary) {
        m_primary = primary;
        Q_EMIT primaryScreenChanged(m_primary);
    }
}

void DisplayModel::setCurrentConfig(const QString &config)
{
    if (m_currentConfig != config) {
        m_currentConfig = config;
        Q_EMIT currentConfigChanged(m_currentConfig);
    }
}

void DisplayModel::setConfigList(const QStringList &configList)
{
    if (m_configList != configList) {
        m_configList = configList;
        Q_EMIT configListChanged(m_configList);
    }
}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    Q_EMIT monitorListChanged();
}

void DisplayModel::monitorRemoved(Monitor *mon)
{
    m_monitors.removeOne(mon);

    Q_EMIT monitorListChanged();
}

void DisplayModel::setAutoLightAdjustIsValid(bool ala)
{
    if (m_AutoLightAdjustIsValid == ala)
        return;
    m_AutoLightAdjustIsValid = ala;
    Q_EMIT autoLightAdjustVaildChanged(ala);
}

void DisplayModel::setLastConfig(const std::pair<int, QString> &lastConfig)
{
    m_lastConfig = lastConfig;
}

void DisplayModel::setBrightnessMap(const BrightnessMap &brightnessMap)
{
    if (brightnessMap == m_brightnessMap) return;

    m_brightnessMap = brightnessMap;
}

void DisplayModel::setTouchscreenList(const TouchscreenInfoList &touchscreenList)
{
    qDebug() << "---> setTouchscreenList()" << touchscreenList.size();
    if (touchscreenList == m_touchscreenList) return;

    m_touchscreenList = touchscreenList;

    Q_EMIT touchscreenListChanged();
}

void DisplayModel::setTouchMap(const TouchscreenMap &touchMap)
{
    if (touchMap == m_touchMap) return;

    m_touchMap = touchMap;

    Q_EMIT touchscreenMapChanged();
}

void DisplayModel::setAutoLightAdjust(bool ala)
{
    if (ala == m_isAutoLightAdjust) return;

    m_isAutoLightAdjust = ala;

    Q_EMIT autoLightAdjustSettingChanged(m_isAutoLightAdjust);
}

bool DisplayModel::redshiftIsValid() const
{
    return m_redshiftIsValid;
}

void DisplayModel::setAdjustCCTmode(int mode)
{
    if (m_adjustCCTMode == mode)
        return;

    m_adjustCCTMode = mode;

    Q_EMIT adjustCCTmodeChanged(mode);
}

void DisplayModel::setColorTemperature(int value)
{
    if (m_colorTemperature == value)
        return;

    m_colorTemperature = value;

    Q_EMIT colorTemperatureChanged(value);
}

void DisplayModel::setRedshiftIsValid(bool redshiftIsValid)
{
    if (m_redshiftIsValid == redshiftIsValid)
        return;

    m_redshiftIsValid = redshiftIsValid;

    Q_EMIT colorTemperatureChanged(redshiftIsValid);
}

void DisplayModel::setAllowEnableMultiScaleRatio(bool allowEnableMultiScaleRatio)
{
    if (m_allowEnableMultiScaleRatio == allowEnableMultiScaleRatio) return;

    m_allowEnableMultiScaleRatio = allowEnableMultiScaleRatio;
}

void DisplayModel::setIsMerge(bool isMerge)
{
    if (m_isMerged == isMerge) return;

    m_isMerged = isMerge;
    Q_EMIT isMergeChange(m_isMerged);
}

void DisplayModel::setMouseLeftHand(bool isLeft)
{
    if(isLeft == m_mouseLeftHand) return;

    m_mouseLeftHand = isLeft;
    Q_EMIT mouseLeftHandChanged(isLeft);
}
void DisplayModel::setRefreshRateEnable(bool isEnable)
{
    m_RefreshRateEnable = isEnable;
}
void DisplayModel::setmaxBacklightBrightness(const uint value)
{
    if(m_maxBacklightBrightness != value && value < 100){
        m_maxBacklightBrightness = value;
        Q_EMIT maxBacklightBrightnessChanged(value);
    }
}
