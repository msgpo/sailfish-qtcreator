/****************************************************************************
**
** Copyright (C) 2012 - 2016 Jolla Ltd.
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

#ifndef VIRTUALBOXMANAGER_H
#define VIRTUALBOXMANAGER_H

#include "merabstractvmmanager.h"

#include <functional>

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>

namespace Utils {
    class Port;
}

namespace Mer {
namespace Internal {

class MerCommandSerializer;
    


class MerVirtualBoxManager : public MerAbstractVMManager
{
    Q_OBJECT
public:
    MerVirtualBoxManager(QObject *parent = nullptr);
    QString type() const override;
    static MerVirtualBoxManager* instance();
    ~MerVirtualBoxManager() override;
    void isVirtualMachineRunning(const QString &vmName, QObject *context,
                                 std::function<void(bool,bool)> slot) override;
    static QStringList fetchRegisteredVirtualMachines();
    static VirtualMachineInfo fetchVirtualMachineInfo(const QString &vmName);
    void startVirtualMachine(const QString &vmName, bool headless) override;
    static void shutVirtualMachine(const QString &vmName);
    static bool updateSharedFolder(const QString &vmName, const QString &mountName, const QString &newFolder);
    static bool updateSdkSshPort(const QString &vmName, quint16 port);
    static bool updateSdkWwwPort(const QString &vmName, quint16 port);
    static bool updateEmulatorSshPort(const QString &vmName, quint16 port);
    static bool updateEmulatorQmlLivePorts(const QString &vmName, const QList<Utils::Port> &ports);
    static void setVideoMode(const QString &vmName, const QSize &size, int depth);
    static QString getExtraData(const QString &vmName, const QString &key);

private:
    static MerVirtualBoxManager *m_instance;
    MerCommandSerializer* m_serializer = nullptr;

public:
    constexpr static const char TYPE[] = "vbox";
};

} // Internal
} // Mer

#endif // VIRTUALBOXMANAGER_H
