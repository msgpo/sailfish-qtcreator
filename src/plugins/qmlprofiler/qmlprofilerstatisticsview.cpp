/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "qmlprofilerstatisticsview.h"
#include "qmlprofilertool.h"

#include <coreplugin/minisplitter.h>
#include <utils/qtcassert.h>
#include <timeline/timelineformattime.h>

#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>
#include <QMenu>
#include <QSortFilterProxyModel>

#include <functional>

namespace QmlProfiler {
namespace Internal {

const int DEFAULT_SORT_COLUMN = MainTimeInPercent;

static void setViewDefaults(Utils::TreeView *view)
{
    view->setFrameStyle(QFrame::NoFrame);
    QHeaderView *header = view->header();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setDefaultSectionSize(100);
    header->setMinimumSectionSize(50);
}

static void getSourceLocation(const QModelIndex &index,
                              std::function<void (const QString &, int, int)> receiver)
{
    const int line = index.data(LineRole).toInt();
    const int column = index.data(ColumnRole).toInt();
    const QString fileName = index.data(FilenameRole).toString();
    if (line != -1 && !fileName.isEmpty())
        receiver(fileName, line, column);
}

QmlProfilerStatisticsView::QmlProfilerStatisticsView(QmlProfilerModelManager *profilerModelManager,
                                                     QWidget *parent)
    : QmlProfilerEventsView(parent)
{
    setObjectName(QLatin1String("QmlProfiler.Statistics.Dock"));
    setWindowTitle(tr("Statistics"));

    QmlProfilerStatisticsModel *model = new QmlProfilerStatisticsModel(profilerModelManager);
    m_mainView.reset(new QmlProfilerStatisticsMainView(model));
    connect(m_mainView.get(), &QmlProfilerStatisticsMainView::gotoSourceLocation,
            this, &QmlProfilerStatisticsView::gotoSourceLocation);
    connect(m_mainView.get(), &QmlProfilerStatisticsMainView::typeSelected,
            this, &QmlProfilerStatisticsView::typeSelected);

    m_calleesView.reset(new QmlProfilerStatisticsRelativesView(
                new QmlProfilerStatisticsRelativesModel(profilerModelManager, model,
                                                        QmlProfilerStatisticsCallees)));
    m_callersView.reset(new QmlProfilerStatisticsRelativesView(
                new QmlProfilerStatisticsRelativesModel(profilerModelManager, model,
                                                        QmlProfilerStatisticsCallers)));
    connect(m_mainView.get(), &QmlProfilerStatisticsMainView::typeSelected,
            m_calleesView.get(), &QmlProfilerStatisticsRelativesView::displayType);
    connect(m_mainView.get(), &QmlProfilerStatisticsMainView::typeSelected,
            m_callersView.get(), &QmlProfilerStatisticsRelativesView::displayType);
    connect(m_calleesView.get(), &QmlProfilerStatisticsRelativesView::typeClicked,
            m_mainView.get(), &QmlProfilerStatisticsMainView::selectType);
    connect(m_callersView.get(), &QmlProfilerStatisticsRelativesView::typeClicked,
            m_mainView.get(), &QmlProfilerStatisticsMainView::selectType);
    connect(m_calleesView.get(), &QmlProfilerStatisticsRelativesView::gotoSourceLocation,
            this, &QmlProfilerStatisticsView::gotoSourceLocation);
    connect(m_callersView.get(), &QmlProfilerStatisticsRelativesView::gotoSourceLocation,
            this, &QmlProfilerStatisticsView::gotoSourceLocation);

    // widget arrangement
    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupLayout->setContentsMargins(0,0,0,0);
    groupLayout->setSpacing(0);

    Core::MiniSplitter *splitterVertical = new Core::MiniSplitter;
    splitterVertical->addWidget(m_mainView.get());
    Core::MiniSplitter *splitterHorizontal = new Core::MiniSplitter;
    splitterHorizontal->addWidget(m_callersView.get());
    splitterHorizontal->addWidget(m_calleesView.get());
    splitterHorizontal->setOrientation(Qt::Horizontal);
    splitterVertical->addWidget(splitterHorizontal);
    splitterVertical->setOrientation(Qt::Vertical);
    splitterVertical->setStretchFactor(0,5);
    splitterVertical->setStretchFactor(1,2);
    groupLayout->addWidget(splitterVertical);
    setLayout(groupLayout);
}

QString QmlProfilerStatisticsView::summary(const QVector<int> &typeIds) const
{
    return m_mainView->summary(typeIds);
}

QStringList QmlProfilerStatisticsView::details(int typeId) const
{
    return m_mainView->details(typeId);
}

void QmlProfilerStatisticsView::contextMenuEvent(QContextMenuEvent *ev)
{
    QMenu menu;
    QAction *copyRowAction = 0;
    QAction *copyTableAction = 0;
    QAction *showExtendedStatsAction = 0;
    QAction *getGlobalStatsAction = 0;

    QPoint position = ev->globalPos();

    QList <QAction *> commonActions = QmlProfilerTool::profilerContextMenuActions();
    foreach (QAction *act, commonActions)
        menu.addAction(act);

    if (mouseOnTable(position)) {
        menu.addSeparator();
        if (m_mainView->selectedModelIndex().isValid())
            copyRowAction = menu.addAction(tr("Copy Row"));
        copyTableAction = menu.addAction(tr("Copy Table"));

        showExtendedStatsAction = menu.addAction(tr("Extended Event Statistics"));
        showExtendedStatsAction->setCheckable(true);
        showExtendedStatsAction->setChecked(m_mainView->showExtendedStatistics());
    }

    menu.addSeparator();
    getGlobalStatsAction = menu.addAction(tr("Show Full Range"));
    if (!m_mainView->isRestrictedToRange())
        getGlobalStatsAction->setEnabled(false);

    QAction *selectedAction = menu.exec(position);

    if (selectedAction) {
        if (selectedAction == copyRowAction)
            m_mainView->copyRowToClipboard();
        if (selectedAction == copyTableAction)
            m_mainView->copyTableToClipboard();
        if (selectedAction == getGlobalStatsAction)
            emit showFullRange();
        if (selectedAction == showExtendedStatsAction)
            m_mainView->setShowExtendedStatistics(m_mainView->showExtendedStatistics());
    }
}

bool QmlProfilerStatisticsView::mouseOnTable(const QPoint &position) const
{
    QPoint tableTopLeft = m_mainView->mapToGlobal(QPoint(0,0));
    QPoint tableBottomRight = m_mainView->mapToGlobal(QPoint(m_mainView->width(),
                                                             m_mainView->height()));
    return position.x() >= tableTopLeft.x() && position.x() <= tableBottomRight.x()
            && position.y() >= tableTopLeft.y() && position.y() <= tableBottomRight.y();
}

void QmlProfilerStatisticsView::selectByTypeId(int typeIndex)
{
    if (m_mainView->selectedTypeId() != typeIndex)
        m_mainView->selectType(typeIndex);
}

void QmlProfilerStatisticsView::onVisibleFeaturesChanged(quint64 features)
{
    m_mainView->restrictToFeatures(features);
}

QmlProfilerStatisticsMainView::QmlProfilerStatisticsMainView(QmlProfilerStatisticsModel *model) :
    m_model(model)
{
    setViewDefaults(this);
    setObjectName(QLatin1String("QmlProfilerEventsTable"));

    auto sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(model);
    sortModel->setSortRole(SortRole);
    sortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortModel->setFilterRole(FilterRole);
    sortModel->setFilterKeyColumn(MainType);
    sortModel->setFilterFixedString("+");

    setModel(sortModel);

    connect(this, &QAbstractItemView::activated, this, &QmlProfilerStatisticsMainView::jumpToItem);

    setSortingEnabled(true);
    sortByColumn(DEFAULT_SORT_COLUMN, Qt::DescendingOrder);

    setShowExtendedStatistics(m_showExtendedStatistics);
    setRootIsDecorated(false);

    resizeColumnToContents(MainLocation);
    resizeColumnToContents(MainType);
}

QmlProfilerStatisticsMainView::~QmlProfilerStatisticsMainView()
{
}

void QmlProfilerStatisticsMainView::setShowExtendedStatistics(bool show)
{
    // Not checking if already set because we don't want the first call to skip
    m_showExtendedStatistics = show;
    if (show) {
        showColumn(MainMedianTime);
        showColumn(MainMaxTime);
        showColumn(MainMinTime);
    } else {
        hideColumn(MainMedianTime);
        hideColumn(MainMaxTime);
        hideColumn(MainMinTime);
    }
}

bool QmlProfilerStatisticsMainView::showExtendedStatistics() const
{
    return m_showExtendedStatistics;
}

void QmlProfilerStatisticsMainView::restrictToFeatures(quint64 features)
{
    m_model->restrictToFeatures(features);
}

bool QmlProfilerStatisticsMainView::isRestrictedToRange() const
{
    return m_model->isRestrictedToRange();
}

QString QmlProfilerStatisticsMainView::summary(const QVector<int> &typeIds) const
{
    return m_model->summary(typeIds);
}

QStringList QmlProfilerStatisticsMainView::details(int typeId) const
{
    return m_model->details(typeId);
}

int QmlProfilerStatisticsMainView::selectedTypeId() const
{
    return model()->data(selectedModelIndex(), TypeIdRole).toInt();
}

void QmlProfilerStatisticsMainView::jumpToItem(const QModelIndex &index)
{
    // show in editor
    getSourceLocation(index, [this](const QString &fileName, int line, int column) {
        emit gotoSourceLocation(fileName, line, column);
    });

    // show in callers/callees subwindow
    emit typeSelected(model()->data(index, TypeIdRole).toInt());
}

void QmlProfilerStatisticsMainView::selectItem(const QModelIndex &index)
{
    // If the same item is already selected, don't reselect it.
    if (index != currentIndex()) {
        setCurrentIndex(index);

        // show in callers/callees subwindow
        emit typeSelected(index.data(TypeIdRole).toInt());
    }
}

void QmlProfilerStatisticsMainView::selectType(int typeIndex)
{
    QAbstractItemModel *itemModel = model();
    for (int i = 0; i < itemModel->rowCount(); i++) {
        QModelIndex index = itemModel->index(i, MainLocation);
        if (itemModel->data(index, TypeIdRole).toInt() == typeIndex) {
            selectItem(index);
            return;
        }
    }
}

QModelIndex QmlProfilerStatisticsMainView::selectedModelIndex() const
{
    QModelIndexList sel = selectedIndexes();
    if (sel.isEmpty())
        return QModelIndex();
    else
        return sel.first();
}

QString QmlProfilerStatisticsMainView::textForItem(const QModelIndex &index) const
{
    QString str;

    // item's data
    const int colCount = model()->columnCount();
    for (int j = 0; j < colCount; ++j) {
        const QModelIndex cellIndex = model()->index(index.row(), j);
        str += cellIndex.data(Qt::DisplayRole).toString();
        if (j < colCount-1) str += QLatin1Char('\t');
    }
    str += QLatin1Char('\n');

    return str;
}

void QmlProfilerStatisticsMainView::copyTableToClipboard() const
{
    QString str;

    const QAbstractItemModel *itemModel = model();

    // headers
    const int columnCount = itemModel->columnCount();
    for (int i = 0; i < columnCount; ++i) {
        str += itemModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        if (i < columnCount - 1)
            str += QLatin1Char('\t');
        else
            str += QLatin1Char('\n');
    }

    // data
    const int rowCount = itemModel->rowCount();
    for (int i = 0; i != rowCount; ++i)
        str += textForItem(itemModel->index(i, 0));

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str, QClipboard::Selection);
    clipboard->setText(str, QClipboard::Clipboard);
}

void QmlProfilerStatisticsMainView::copyRowToClipboard() const
{
    QString str = textForItem(selectedModelIndex());
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str, QClipboard::Selection);
    clipboard->setText(str, QClipboard::Clipboard);
}

QmlProfilerStatisticsRelativesView::QmlProfilerStatisticsRelativesView(
        QmlProfilerStatisticsRelativesModel *model) :
    m_model(model)
{
    setViewDefaults(this);
    auto sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(model);
    sortModel->setSortRole(SortRole);
    sortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    setModel(sortModel);
    setRootIsDecorated(false);

    setSortingEnabled(true);
    sortByColumn(DEFAULT_SORT_COLUMN, Qt::DescendingOrder);

    connect(this, &QAbstractItemView::activated,
            this, &QmlProfilerStatisticsRelativesView::jumpToItem);
}

QmlProfilerStatisticsRelativesView::~QmlProfilerStatisticsRelativesView()
{
}

void QmlProfilerStatisticsRelativesView::displayType(int typeIndex)
{
    model()->setData(QModelIndex(), typeIndex, TypeIdRole);
    resizeColumnToContents(RelativeLocation);
}

void QmlProfilerStatisticsRelativesView::jumpToItem(const QModelIndex &index)
{
    // show in editor
    getSourceLocation(index, [this](const QString &fileName, int line, int column) {
        emit gotoSourceLocation(fileName, line, column);
    });

    emit typeClicked(index.data(TypeIdRole).toInt());
}

} // namespace Internal
} // namespace QmlProfiler
