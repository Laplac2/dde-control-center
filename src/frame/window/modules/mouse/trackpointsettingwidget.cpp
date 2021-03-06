/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#include "trackpointsettingwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "modules/mouse/mousemodel.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

TrackPointSettingWidget::TrackPointSettingWidget(QWidget *parent) : dcc::ContentWidget(parent)
{
    m_trackPointSettingsGrp = new SettingsGroup;
    //~ contents_path /mouse/TrackPoint
    //~ child_page TrackPoint
    m_trackMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    QStringList trackPointlist;
    trackPointlist << tr("Slow") << "" << "" << "" << "" << "";
    trackPointlist << tr("Fast");
    DCCSlider *pointSlider = m_trackMoveSlider->slider();
    pointSlider->setType(DCCSlider::Vernier);
    pointSlider->setTickPosition(QSlider::TicksBelow);
    pointSlider->setRange(0, 6);
    pointSlider->setTickInterval(1);
    pointSlider->setPageStep(1);
    m_trackMoveSlider->setAnnotations(trackPointlist);
    m_trackPointSettingsGrp->appendItem(m_trackMoveSlider);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setMargin(0);
    m_contentLayout->addWidget(m_trackPointSettingsGrp);
    m_contentLayout->addStretch();

    TranslucentFrame *tFrame = new TranslucentFrame;
    tFrame->setLayout(m_contentLayout);
    layout()->setMargin(0);
    setContent(tFrame);
    connect(m_trackMoveSlider->slider(), &DCCSlider::valueChanged, this, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration);
}

void TrackPointSettingWidget::setModel(dcc::mouse::MouseModel *const model)
{
    m_mouseModel = model;
    connect(m_mouseModel, &MouseModel::redPointMoveSpeedChanged, this, &TrackPointSettingWidget::onRedPointMoveSpeedChanged);
    onRedPointMoveSpeedChanged(m_mouseModel->redPointMoveSpeed());
}

void TrackPointSettingWidget::onRedPointMoveSpeedChanged(int speed)
{
    m_trackMoveSlider->slider()->blockSignals(true);
    m_trackMoveSlider->slider()->setValue(speed);
    m_trackMoveSlider->slider()->blockSignals(false);
}
