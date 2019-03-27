/****************************************************************************
**
** Copyright (C) 2019 Jolla Ltd.
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

#ifndef MERDOCKERMANAGER_H
#define MERDOCKERMANAGER_H

#include <QObject>

#include "merabstractvmmanager.h"

namespace Mer {
namespace Internal {

class MerCommandSerializer;

class MerDockerManager : public MerAbstractVMManager
{
    Q_OBJECT
public:
    explicit MerDockerManager(QObject *parent = nullptr);
    QString type() const override;

    QStringList fetchImages();
    static VirtualMachineInfo fetchVirtualMachineInfo(const QString &imageName);
    void isVirtualMachineRunning(const QString &vmName, QObject *context,
                                 std::function<void(bool,bool)> slot) override;
    void startVirtualMachine(const QString &vmName, bool headless) override;
signals:

public slots:

private:
    static bool isContainerRunningFromList(const QString& containerList,
                                           const QString& container);
    MerCommandSerializer* m_serializer = nullptr;
public:
    constexpr static const char TYPE[] = "docker";
};
}
}
#endif // MERDOCKERMANAGER_H
