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

#include "powermodel.h"

#include <QDebug>

using namespace dcc;
using namespace dcc::power;

const double EPSINON = 1e-6;

PowerModel::PowerModel(QObject *parent)
    : QObject(parent)
    , m_lidPresent(false)
    , m_sleepOnLidOnPowerClose(false)
    , m_sleepOnLidOnBatteryClose(false)
    , m_screenBlackLock(false)
    , m_sleepLock(false)
    , m_canSleep(true)
    , m_canHibernate(false)
    , m_screenBlackDelayOnPower(0)
    , m_sleepDelayOnPower(0)
    , m_screenBlackDelayOnBattery(0)
    , m_sleepDelayOnBattery(0)
    , m_haveBettary(false)
    , m_batteryLockScreenDelay(0)
    , m_powerLockScreenDelay(0)
    , m_batteryPercentage(0.0)
    , m_bPowerSavingModeAutoWhenQuantifyLow(0)
    , m_bPowerSavingModeAuto(false)
    , m_dPowerSavingModeLowerBrightnessThreshold(0)
    , m_nLinePowerPressPowerBtnAction(0)
    , m_nLinePowerLidClosedAction(0)
    , m_nBatteryPressPowerBtnAction(0)
    , m_nBatteryLidClosedAction(0)
    , m_bLowPowerNotifyEnable(false)
    , m_dLowPowerNotifyThreshold(0)
    , m_dLowPowerAutoSleepThreshold(0)
    , m_isSuspend(false)
    , m_isHibernate(false)
    , m_powerPlan("")
    , m_isHighPerformanceSupported(false)
{
}

void PowerModel::setScreenBlackLock(const bool lock)
{
    if (lock != m_screenBlackLock) {
        m_screenBlackLock = lock;

        Q_EMIT screenBlackLockChanged(lock);
    }
}

void PowerModel::setLidPresent(bool lidPresent)
{
    if (lidPresent != m_lidPresent) {
        m_lidPresent = lidPresent;

        Q_EMIT lidPresentChanged(lidPresent);
    }
}

void PowerModel::setScreenBlackDelayOnPower(const int screenBlackDelayOnPower)
{
    if (screenBlackDelayOnPower != m_screenBlackDelayOnPower) {
        m_screenBlackDelayOnPower = screenBlackDelayOnPower;

        Q_EMIT screenBlackDelayChangedOnPower(screenBlackDelayOnPower);
    }
}

void PowerModel::setSleepDelayOnPower(const int sleepDelayOnPower)
{
    if (sleepDelayOnPower != m_sleepDelayOnPower) {
        m_sleepDelayOnPower = sleepDelayOnPower;

        Q_EMIT sleepDelayChangedOnPower(sleepDelayOnPower);
    }
}

void PowerModel::setScreenBlackDelayOnBattery(const int screenBlackDelayOnBattery)
{
    if (screenBlackDelayOnBattery != m_screenBlackDelayOnBattery) {
        m_screenBlackDelayOnBattery = screenBlackDelayOnBattery;

        Q_EMIT screenBlackDelayChangedOnBattery(screenBlackDelayOnBattery);
    }
}

void PowerModel::setSleepDelayOnBattery(const int sleepDelayOnBattery)
{
    if (sleepDelayOnBattery != m_sleepDelayOnBattery) {
        m_sleepDelayOnBattery = sleepDelayOnBattery;

        Q_EMIT sleepDelayChangedOnBattery(sleepDelayOnBattery);
    }
}

void PowerModel::setSleepOnLidOnPowerClose(bool sleepOnLidClose)
{
    if (sleepOnLidClose != m_sleepOnLidOnPowerClose) {
        m_sleepOnLidOnPowerClose = sleepOnLidClose;

        Q_EMIT sleepOnLidOnPowerCloseChanged(sleepOnLidClose);
    }
}

void PowerModel::setSleepOnLidOnBatteryClose(bool sleepOnLidOnBatteryClose)
{
    if (sleepOnLidOnBatteryClose != m_sleepOnLidOnBatteryClose) {
        m_sleepOnLidOnBatteryClose = sleepOnLidOnBatteryClose;

        Q_EMIT sleepOnLidOnBatteryCloseChanged(sleepOnLidOnBatteryClose);
    }
}

void PowerModel::setBatteryLockScreenDelay(const int value)
{
    if (value != m_batteryLockScreenDelay) {
        m_batteryLockScreenDelay = value;

        Q_EMIT batteryLockScreenDelayChanged(value);
    }
}

void PowerModel::setPowerLockScreenDelay(const int value)
{
    if (value != m_powerLockScreenDelay) {
        m_powerLockScreenDelay = value;

        Q_EMIT powerLockScreenDelayChanged(value);
    }
}

#ifndef DCC_DISABLE_POWERSAVE
void PowerModel::setAutoPowerSaveMode(bool autoPowerSavingMode)
{
    if (m_autoPowerSaveMode == autoPowerSavingMode)
        return;

    m_autoPowerSaveMode = autoPowerSavingMode;

    Q_EMIT autoPowerSavingModeChanged(autoPowerSavingMode);
}

void PowerModel::setPowerSaveMode(bool powerSaveMode)
{
    if (m_powerSaveMode == powerSaveMode)
        return;

    m_powerSaveMode = powerSaveMode;

    Q_EMIT powerSaveModeChanged(powerSaveMode);
}
#endif


void PowerModel::setHaveBettary(bool haveBettary)
{
    if (haveBettary == m_haveBettary)
        return;

    m_haveBettary = haveBettary;

    Q_EMIT haveBettaryChanged(haveBettary);
}

void PowerModel::setBatteryPercentage(double batteryPercentage)
{
    if (!getDoubleCompare(batteryPercentage, m_batteryPercentage))
        return;

    m_batteryPercentage = batteryPercentage;

    Q_EMIT batteryPercentageChanged(batteryPercentage);
}

bool PowerModel::getDoubleCompare(const double value1, const double value2)
{
    return ((value1 - value2 >= -EPSINON) && (value1 - value2 <= EPSINON));
}

void PowerModel::setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode)
{
    if (bLowBatteryAutoIntoSaveEnergyMode != m_bPowerSavingModeAutoWhenQuantifyLow) {
        m_bPowerSavingModeAutoWhenQuantifyLow = bLowBatteryAutoIntoSaveEnergyMode;

        Q_EMIT powerSavingModeAutoWhenQuantifyLowChanged(bLowBatteryAutoIntoSaveEnergyMode);
    }
}

void PowerModel::setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode)
{
    if (bAutoIntoSaveEnergyMode != m_bPowerSavingModeAuto) {
        m_bPowerSavingModeAuto = bAutoIntoSaveEnergyMode;

        Q_EMIT powerSavingModeAutoChanged(bAutoIntoSaveEnergyMode);
    }
}

void PowerModel::setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold)
{
    if (dPowerSavingModeLowerBrightnessThreshold != m_dPowerSavingModeLowerBrightnessThreshold) {
        m_dPowerSavingModeLowerBrightnessThreshold = dPowerSavingModeLowerBrightnessThreshold;

        Q_EMIT powerSavingModeLowerBrightnessThresholdChanged(dPowerSavingModeLowerBrightnessThreshold);
    }
}

void PowerModel::setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction)
{
    if (nLinePowerPressPowerBtnAction != m_nLinePowerPressPowerBtnAction) {
        m_nLinePowerPressPowerBtnAction = nLinePowerPressPowerBtnAction;

        Q_EMIT linePowerPressPowerBtnActionChanged(nLinePowerPressPowerBtnAction);
    }
}

void PowerModel::setLinePowerLidClosedAction(int nLinePowerLidClosedAction)
{
    if (nLinePowerLidClosedAction != m_nLinePowerLidClosedAction) {
        m_nLinePowerLidClosedAction = nLinePowerLidClosedAction;

        Q_EMIT linePowerLidClosedActionChanged(nLinePowerLidClosedAction);
    }
}

void PowerModel::setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction)
{
    if (nBatteryPressPowerBtnAction != m_nBatteryPressPowerBtnAction) {
        m_nBatteryPressPowerBtnAction = nBatteryPressPowerBtnAction;

        Q_EMIT batteryPressPowerBtnActionChanged(nBatteryPressPowerBtnAction);
    }
}

void PowerModel::setBatteryLidClosedAction(int nBatteryLidClosedAction)
{
    if (nBatteryLidClosedAction != m_nBatteryLidClosedAction) {
        m_nBatteryLidClosedAction = nBatteryLidClosedAction;

        Q_EMIT batteryLidClosedActionChanged(nBatteryLidClosedAction);
    }
}

void PowerModel::setLowPowerNotifyEnable(bool bLowPowerNotifyEnable)
{
    if (bLowPowerNotifyEnable != m_bLowPowerNotifyEnable) {
        m_bLowPowerNotifyEnable = bLowPowerNotifyEnable;

        Q_EMIT lowPowerNotifyEnableChanged(bLowPowerNotifyEnable);
    }
}

void PowerModel::setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold)
{
    if (dLowPowerNotifyThreshold != m_dLowPowerNotifyThreshold) {
        m_dLowPowerNotifyThreshold = dLowPowerNotifyThreshold;

        Q_EMIT lowPowerNotifyThresholdChanged(dLowPowerNotifyThreshold);
    }
}

void PowerModel::setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold)
{
    if (dLowPowerAutoSleepThreshold != m_dLowPowerAutoSleepThreshold) {
        m_dLowPowerAutoSleepThreshold = dLowPowerAutoSleepThreshold;

        Q_EMIT lowPowerAutoSleepThresholdChanged(dLowPowerAutoSleepThreshold);
    }
}

void PowerModel::setSleepLock(bool sleepLock)
{
    if (sleepLock != m_sleepLock) {
        m_sleepLock = sleepLock;

        Q_EMIT sleepLockChanged(sleepLock);
    }
}

void PowerModel::setCanSleep(bool canSleep)
{
    if (canSleep != m_canSleep) {
        m_canSleep = canSleep;

        Q_EMIT canSleepChanged(canSleep);
    }
}

void PowerModel::setCanHibernate(bool value)
{
    if (m_canHibernate != value) {
        m_canHibernate = value;
    }
}

void PowerModel::setPowerPlan(const QString &powerPlan)
{
    if (m_powerPlan != powerPlan) {
        m_powerPlan = powerPlan;

        Q_EMIT powerPlanChanged(m_powerPlan);
    }
}

void PowerModel::setHighPerformanceSupported(bool isHighSupport)
{
    if (m_isHighPerformanceSupported != isHighSupport) {
        m_isHighPerformanceSupported = isHighSupport;

        Q_EMIT highPerformaceChanged(isHighSupport);
    }
}

void PowerModel::setSuspend(bool suspend)
{
    if (suspend == m_isSuspend)
        return;
    m_isSuspend = suspend;
    Q_EMIT suspendChanged(suspend);
}

void PowerModel::setHibernate(bool hibernate)
{
    if (m_isHibernate != hibernate) {
        m_isHibernate = hibernate;
    }
}

void PowerModel::setmaxBacklightBrightness(const uint value)
{
    if (m_maxBacklightBrightness != value && value < 100) {
        m_maxBacklightBrightness = value;
        Q_EMIT maxBacklightBrightnessChanged(value);
    }
}
