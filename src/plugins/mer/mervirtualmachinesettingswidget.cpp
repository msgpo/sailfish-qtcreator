/****************************************************************************
**
** Copyright (C) 2019 Jolla Ltd.
** Copyright (C) 2019 Open Mobile Platform LLC.
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

#include "mervirtualmachinesettingswidget.h"

#include "ui_mervirtualmachinesettingswidget.h"

#include <sfdk/virtualmachine.h>

#include <utils/qtcassert.h>
#include <utils/utilsicons.h>

#include <QFormLayout>

using namespace Sfdk;

namespace Mer {
namespace Internal {

namespace {
const int DEFAULT_MAX_MEMORY_SIZE_MB = 3584;
const int MAX_STORAGE_SIZE_INCREMENT_GB = 10;
const int MIN_MEMORY_SIZE_MB = 512;

} // namespace anonymous

MerVirtualMachineSettingsWidget::MerVirtualMachineSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MerVirtualMachineSettingsWidget)
{
    ui->setupUi(this);
    initGui();
}

MerVirtualMachineSettingsWidget::~MerVirtualMachineSettingsWidget()
{
    delete ui;
}

// hack
void MerVirtualMachineSettingsWidget::setSwapAllowed(bool allowed)
{
    QTC_ASSERT(!allowed, return);
    QTC_ASSERT(!ui->swapLabel->isHidden(), return);

    ui->swapLabel->setVisible(allowed);
    ui->swapSizeInfoLabel->setVisible(allowed);
    ui->swapSizeGbSpinBox->setVisible(allowed);

    // QTBUG-6864
    ui->formLayout->takeRow(ui->swapLabel);
}

void MerVirtualMachineSettingsWidget::setMemorySizeMb(int sizeMb)
{
    if (ui->memorySpinBox->maximum() < sizeMb)
        ui->memorySpinBox->setRange(MIN_MEMORY_SIZE_MB, sizeMb);

    ui->memorySpinBox->setValue(sizeMb);
}

void MerVirtualMachineSettingsWidget::setSwapSizeMb(int sizeMb)
{
    const double sizeGb = sizeMb / 1024.0;
    ui->swapSizeGbSpinBox->setValue(sizeGb);
}

void MerVirtualMachineSettingsWidget::setCpuCount(int count)
{
    ui->cpuCountSpinBox->setValue(count);
}

void MerVirtualMachineSettingsWidget::setStorageSizeMb(int storageSizeMb)
{
    const double storageSizeGb = storageSizeMb / 1024.0;
    // Prohibit adding more than MAX_STORAGE_SIZE_INCREMENT_GB in one step, because there is no way to go back
    // the minimum size is the current size because VBoxManager can't reduce storage size
    ui->storageSizeGbSpinBox->setRange(storageSizeGb, storageSizeGb + MAX_STORAGE_SIZE_INCREMENT_GB);
    ui->storageSizeGbSpinBox->setValue(storageSizeGb);
}

void MerVirtualMachineSettingsWidget::setVmOff(bool vmOff)
{
    ui->memorySpinBox->setEnabled(vmOff);
    ui->swapSizeGbSpinBox->setEnabled(vmOff);
    ui->cpuCountSpinBox->setEnabled(vmOff);
    ui->storageSizeGbSpinBox->setEnabled(vmOff);

    ui->memoryInfoLabel->setVisible(!vmOff);
    ui->swapSizeInfoLabel->setVisible(!vmOff);
    ui->cpuInfoLabel->setVisible(!vmOff);
    ui->storageSizeInfoLabel->setVisible(!vmOff);
}

QFormLayout *MerVirtualMachineSettingsWidget::formLayout() const
{
    return ui->formLayout;
}

void MerVirtualMachineSettingsWidget::initGui()
{
    int maxMemorySizeMb = VirtualMachine::availableMemorySizeMb() > 0
        ? VirtualMachine::availableMemorySizeMb()
        : DEFAULT_MAX_MEMORY_SIZE_MB;
    ui->swapSizeGbSpinBox->setRange(0, 999999); // "unlimited", later limited by storageSizeGb
    ui->memorySpinBox->setRange(MIN_MEMORY_SIZE_MB, maxMemorySizeMb);
    ui->cpuCountSpinBox->setRange(1, VirtualMachine::availableCpuCount());

    connect(ui->memorySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MerVirtualMachineSettingsWidget::memorySizeMbChanged);
    connect(ui->swapSizeGbSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit swapSizeMbChanged(static_cast<int>(value * 1024));
    });
    connect(ui->cpuCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MerVirtualMachineSettingsWidget::cpuCountChanged);
    connect(ui->storageSizeGbSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        QTC_CHECK(ui->swapSizeGbSpinBox->value() <= value);
        ui->swapSizeGbSpinBox->setMaximum(value);

        emit storageSizeMbChnaged(static_cast<int>(value * 1024));
    });

    ui->memoryInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    ui->memoryInfoLabel->setToolTip(
            QLatin1String("<font color=\"red\">")
            + tr("Stop the virtual machine to unlock this field for editing.")
            + QLatin1String("</font>"));

    ui->swapSizeInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    ui->swapSizeInfoLabel->setToolTip(
            QLatin1String("<font color=\"red\">")
            + tr("Stop the virtual machine to unlock this field for editing.")
            + QLatin1String("</font>"));

    ui->cpuInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    ui->cpuInfoLabel->setToolTip(
            QLatin1String("<font color=\"red\">")
            + tr("Stop the virtual machine to unlock this field for editing.")
            + QLatin1String("</font>"));

    ui->storageSizeInfoLabel->setPixmap(Utils::Icons::INFO.pixmap());
    ui->storageSizeInfoLabel->setToolTip(
            QLatin1String("<font color=\"red\">")
            + tr("Stop the virtual machine to unlock this field for editing.")
            + QLatin1String("</font>"));
}

} // Internal
} // Mer
