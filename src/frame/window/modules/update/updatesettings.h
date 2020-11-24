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
#pragma once

#include "interface/moduleinterface.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class NextPageWidget;
class TipsLabel;
}

namespace update {
class UpdateModel;
}
}

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
DWIDGET_END_NAMESPACE

const int TipLeftInterver = 10;

namespace DCC_NAMESPACE {
namespace update {

class UpdateSettings : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(dcc::update::UpdateModel *model, QWidget *parent = 0);

    void setModel(dcc::update::UpdateModel *model);

Q_SIGNALS:
    void requestSetAutoCheckUpdates(const bool autocheckUpdate);
    void requestSetAutoCheckSystemUpdate(const bool &autoUpdate);
    void requestSetAutoCheckAppUpdate(const bool &autoUpdate);
    void requestSetAutoCheckSecureUpdate(const bool &autoUpdate);
    void requestSetUpdateNotify(bool notify);
    void requestSetAutoDownloadUpdate(const bool &autoUpdate);
    void requestSetAutoCleanCache(const bool autoClean);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void requestSetSourceCheck(const bool check);
#endif
    void requestEnableSmartMirror(bool enable);
    void requestShowMirrorsView();

private:
    void initUi();
    void initConnection();

private:
    dcc::update::UpdateModel *m_model;

    dcc::widgets::SwitchWidget *m_autoCheckUpdate;       // 检查更新
    dcc::widgets::SwitchWidget *m_autoCheckSystemUpdate; // 检查系统更新
    dcc::widgets::SwitchWidget *m_autoCheckAppUpdate;    // 检查应用商店的应用更新
    dcc::widgets::SwitchWidget *m_autoCheckSecureUpdate; // 检查安全更新
    dcc::widgets::SwitchWidget *m_updateNotify;          // 更新提醒
    dcc::widgets::SwitchWidget *m_autoDownloadUpdate;    // 下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoDownloadUpdateTips;

    dcc::widgets::SwitchWidget *m_timerDownload; // 定时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_timerDownloadLbl;
    QLabel *m_setTimerLbl;

    dcc::widgets::SwitchWidget *m_freeTimeDownload; // 闲时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_freeTimeDownloadLbl;
    QLabel *m_setFreeTimeLbl;

    dcc::widgets::SwitchWidget *m_autoCleanCache; // 清除更新包缓存

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    dcc::widgets::SwitchWidget *m_sourceCheck;
#endif
    dcc::widgets::SwitchWidget *m_smartMirrorBtn;
    dcc::widgets::NextPageWidget *m_updateMirrors;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
