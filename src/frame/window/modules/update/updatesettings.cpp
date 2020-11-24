/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "updatesettings.h"
#include "widgets/settingsgroup.h"
#include "modules/update/updatemodel.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/smalllabel.h"
#include "widgets/titlelabel.h"
#include "widgets/switchwidget.h"
#include "widgets/nextpagewidget.h"
#include "dsysinfo.h"
#include "window/utils.h"

#include <DTipLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(nullptr)
    , m_autoCheckUpdate(new SwitchWidget(this))
    , m_autoCheckSystemUpdate(new SwitchWidget(tr("Check for System Updates"), this))
    , m_autoCheckAppUpdate(new SwitchWidget(tr("Check for Application Updates"), this))
    , m_autoCheckSecureUpdate(new SwitchWidget(tr("Check for Secure Updates"), this))
    , m_updateNotify(new SwitchWidget(tr("Updates Notification"), this))
    , m_autoDownloadUpdate(new SwitchWidget(tr("Download Updates"), this))
    , m_autoDownloadUpdateTips(new DTipLabel(tr("Switch it on to automatically download the updates in wireless or wired network"), this))
    , m_autoCleanCache(new SwitchWidget(this))
{
    initUi();
    initConnection();
    setModel(model);
}

void UpdateSettings::initUi()
{
    setTitle(tr("Update Settings"));

    TranslucentFrame *contentWidget = new TranslucentFrame(this); // 添加一层半透明框架
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    QLabel *autoUpdateSettingsLabel = new QLabel(tr("Automatic Updating Settings"), this);
    DFontSizeManager::instance()->bind(autoUpdateSettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    autoUpdateSettingsLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
    contentLayout->addWidget(autoUpdateSettingsLabel);

    //~ contents_path /update/Update Settings
    m_autoCheckUpdate->setTitle(tr("Check for Updates"));

    SettingsGroup *updateSettingsGrp = new SettingsGroup;
    updateSettingsGrp->appendItem(m_autoCheckUpdate);
    updateSettingsGrp->appendItem(m_autoCheckSystemUpdate);
    updateSettingsGrp->appendItem(m_autoCheckAppUpdate);
    updateSettingsGrp->appendItem(m_autoCheckSecureUpdate);
    updateSettingsGrp->appendItem(m_updateNotify);
    updateSettingsGrp->appendItem(m_autoDownloadUpdate);
    updateSettingsGrp->setSpacing(List_Interval);
    contentLayout->addWidget(updateSettingsGrp);

    m_autoDownloadUpdateTips->setWordWrap(true);
    m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
    m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoDownloadUpdateTips);

#if 0 // 定时、闲时下载功能需添加时再显示
    //自动下载更新控件初始化
    m_timerDownload = new SwitchWidget("定时下载可用更新", this);
    m_timerDownloadLbl = new DTipLabel("当前设置时间为：", this);
    m_timerDownloadLbl->setWordWrap(true);
    m_timerDownloadLbl->setAlignment(Qt::AlignLeft);
    m_timerDownloadLbl->setContentsMargins(10, 0, 10, 0);
    m_setTimerLbl = new QLabel(QString("<a style='color: blue; text-decoration: none;'; href=' '>%1</a>").arg("更改"));

    //闲时下载更新控件初始化
    m_freeTimeDownload = new SwitchWidget("闲时下载更新", this);
    m_freeTimeDownloadLbl = new DTipLabel("当前使用时间段：", this);
    m_freeTimeDownloadLbl->setWordWrap(true);
    m_freeTimeDownloadLbl->setAlignment(Qt::AlignLeft);
    m_freeTimeDownloadLbl->setContentsMargins(10, 0, 10, 0);
    m_setFreeTimeLbl = new QLabel(QString("<a style='color: blue; text-decoration: none;'; href=' '>%1</a>").arg("更改"));

    auto setDownloadTimeCtrlLayout = [ & ](SwitchWidget *setSwitch, DTipLabel *timeInfoLbl, QLabel *changeLbl) {
        setSwitch->addBackground();
        contentLayout->addWidget(setSwitch);
        QHBoxLayout *downloadLblLayout = new QHBoxLayout;
        downloadLblLayout->addWidget(timeInfoLbl);
        downloadLblLayout->addWidget(changeLbl);
        contentLayout->addLayout(downloadLblLayout);
    };

    //定时下载更新布局
    setDownloadTimeCtrlLayout(m_timerDownload, m_timerDownloadLbl, m_setTimerLbl);
    //闲时下载更新布局
    setDownloadTimeCtrlLayout(m_freeTimeDownload, m_freeTimeDownloadLbl, m_setFreeTimeLbl);
#endif

    //~ contents_path /update/Update Settings
    m_autoCleanCache->setTitle(tr("Clear Package Cache"));
    m_autoCleanCache->addBackground();
    contentLayout->addWidget(m_autoCleanCache);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        //~ contents_path /update/Update Settings
        m_sourceCheck = new SwitchWidget(tr("System Repository Detection"), this);
        m_sourceCheck->addBackground();
        contentLayout->addWidget(m_sourceCheck);
        DTipLabel *sourceCheckTips = new DTipLabel(tr("Show a notification if system update repository has been modified"), this);
        sourceCheckTips->setWordWrap(true);
        sourceCheckTips->setAlignment(Qt::AlignLeft);
        sourceCheckTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(sourceCheckTips);
    }
#endif

    if (SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        //~ contents_path /update/Update Settings
        SwitchWidget *m_smartMirrorBtn = new SwitchWidget(tr("Smart Mirror Switch"), this);
        m_smartMirrorBtn->addBackground();
        contentLayout->addWidget(m_smartMirrorBtn);

        //~ contents_path /update/Update Settings
        DTipLabel *smartTips = new DTipLabel(tr("Switch it on to connect to the quickest mirror site automatically"), this);
        smartTips->setWordWrap(true);
        smartTips->setAlignment(Qt::AlignLeft);
        smartTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(smartTips);

        NextPageWidget *m_updateMirrors = new NextPageWidget(nullptr, false);
        //~ contents_path /update/Update Settings/Mirror List
        m_updateMirrors->setTitle(tr("Mirror List"));
        m_updateMirrors->setRightTxtWordWrap(true);
        m_updateMirrors->addBackground();
        contentLayout->addWidget(m_updateMirrors);
    }

    contentLayout->setAlignment(Qt::AlignTop);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(0, 10, 0, 5);

    setContent(contentWidget);
}

void UpdateSettings::initConnection()
{
    connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckUpdates);
    connect(m_autoCheckSystemUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckSystemUpdate);
    connect(m_autoCheckAppUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckAppUpdate);
    connect(m_autoCheckSecureUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckSecureUpdate);
    connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetUpdateNotify);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoDownloadUpdate);
    //connect(m_setTimerLbl, &QLabel::linkActivated,);
    //connect(m_setFreeTimeLbl, &QLabel::linkActivated,);
    connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCleanCache);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        qDebug() << "connect m_sourceCheck";
        connect(m_sourceCheck, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetSourceCheck);
    }
#endif

    if (SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
        connect(m_smartMirrorBtn, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestEnableSmartMirror);
    }
}

void UpdateSettings::setModel(UpdateModel *model)
{
    m_model = model;

    if (SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        auto setDefaultMirror = [this](const MirrorInfo & mirror) {
            m_updateMirrors->setValue(mirror.m_name);
        };

        if (!model->mirrorInfos().isEmpty()) {
            setDefaultMirror(model->defaultMirror());
        }

        connect(model, &UpdateModel::defaultMirrorChanged, this, setDefaultMirror);
        connect(model, &UpdateModel::smartMirrorSwitchChanged, m_smartMirrorBtn, &SwitchWidget::setChecked);
        m_smartMirrorBtn->setChecked(m_model->smartMirrorSwitch());

        auto setMirrorListVisible = [ = ](bool visible) {
            m_updateMirrors->setVisible(!visible);
        };

        connect(model, &UpdateModel::smartMirrorSwitchChanged, this, setMirrorListVisible);
        setMirrorListVisible(model->smartMirrorSwitch());
    }

    m_autoDownloadUpdate->setChecked(model->autoDownloadUpdates());

    connect(model, &UpdateModel::autoDownloadUpdatesChanged, m_autoDownloadUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::updateNotifyChanged, m_updateNotify, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::updateNotifyChanged, m_autoDownloadUpdate, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::updateNotifyChanged, m_autoDownloadUpdateTips, &DTipLabel::setVisible);

    m_autoDownloadUpdate->setVisible(model->updateNotify());
    m_autoCheckUpdate->setChecked(model->autoCheckUpdates());
    m_updateNotify->setChecked(model->updateNotify());
    m_autoCleanCache->setChecked(m_model->autoCleanCache());
    m_autoDownloadUpdateTips->setVisible(model->updateNotify());

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal" && DSysInfo::DeepinDesktop != DSysInfo::deepinType()) {
        connect(model, &UpdateModel::sourceCheckChanged, m_sourceCheck, &SwitchWidget::setChecked);
        m_sourceCheck->setChecked(model->sourceCheck());
    }
#endif
}
