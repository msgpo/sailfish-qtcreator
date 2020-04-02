/****************************************************************************
**
** Copyright (C) 2012-2017 Jolla Ltd.
** Copyright (C) 2020 Open Mobile Platform LLC.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Digia.
**
****************************************************************************/

#ifndef MERHARDWAREDEVICEWIDGET_H
#define MERHARDWAREDEVICEWIDGET_H

#include <projectexplorer/devicesupport/idevicewidget.h>

namespace Mer {
namespace Internal {

namespace Ui {
class MerHardwareDeviceWidget;
}

class MerHardwareDeviceWidget : public ProjectExplorer::IDeviceWidget
{
    Q_OBJECT
public:
    explicit MerHardwareDeviceWidget(const ProjectExplorer::IDevice::Ptr &deviceConfig,
                                          QWidget *parent = 0);
    ~MerHardwareDeviceWidget() override;

private slots:
    void authenticationTypeChanged();
    void hostNameEditingFinished();
    void sshPortEditingFinished();
    void timeoutEditingFinished();
    void userNameEditingFinished();
    void keyFileEditingFinished();
    void handleFreePortsChanged();
    void handleQmlLivePortsChanged();
    void setPrivateKey(const QString &path);
    void createNewKey();

private:
    void updateDeviceFromUi() override;
    void updatePortsWarningLabel();
    void updateQmlLivePortsWarningLabel();
    void initGui();

    Ui::MerHardwareDeviceWidget *m_ui;
};

} // Internal
} // Mer

#endif // MERDEVICECONFIGURATIONWIDGET_H
