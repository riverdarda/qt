/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <q3toolbar.h>
#include <q3mainwindow.h>
#include <qaction.h>
#include <qapplication.h>
#include <QToolButton>
#include <q3action.h>
#include <qmenu.h>

//TESTED_CLASS=
//TESTED_FILES=

#if defined Q_CC_MSVC && _MSC_VER <= 1200
#define NOFINDCHILDRENMETHOD
#endif

class tst_Q3ToolBar : public QObject
{
    Q_OBJECT

public:
    tst_Q3ToolBar();
    virtual ~tst_Q3ToolBar();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void toggled();
    void actionGroupPopup();

    // task-specific tests below me
    void task182657();

private:
    Q3ToolBar* testWidget;
};

tst_Q3ToolBar::tst_Q3ToolBar()
{
}

tst_Q3ToolBar::~tst_Q3ToolBar()
{

}

void tst_Q3ToolBar::initTestCase()
{
    testWidget = new Q3ToolBar(0, "testWidget");
    testWidget->show();
    qApp->setMainWidget(testWidget);

    QTest::qWait(100);
}

void tst_Q3ToolBar::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3ToolBar::init()
{
}

void tst_Q3ToolBar::cleanup()
{
}

void tst_Q3ToolBar::toggled()
{
    // When clicking on a toggled action it should emit a signal
    QAction *action = new QAction( this, "action" );
    action->setToggleAction( true );
    action->addTo(testWidget);
    testWidget->show();
    QSignalSpy spy(action, SIGNAL(toggled(bool)));
#ifndef NOFINDCHILDRENMETHOD
    QList<QToolButton *> list = testWidget->findChildren<QToolButton *>();
#else
    QList<QToolButton *> list = qFindChildren<QToolButton *>(testWidget, QString());

#endif
    for (int i = 0; i < list.size(); ++i)
        QTest::mouseClick(list.at(i), Qt::LeftButton);
    QCOMPARE(spy.count(), 1);

    // Also try the othe case (a toggled action will emit the toolbuttons toggled)
    QSignalSpy spy2(list.at(1), SIGNAL(toggled(bool)));
    action->setChecked(!action->isChecked());
    QCOMPARE(spy2.count(), 1);

}

class MenuEventFilter : public QObject
{
public:
    MenuEventFilter(QObject *parent = 0) : QObject(parent), menuShown(false) {}
    bool wasMenuShown() const { return menuShown; }
    void setMenuShown(bool b) { menuShown = b; }
protected:
    bool eventFilter(QObject *o, QEvent *e)
    {
        if (e->type() == QEvent::Show) {
            menuShown = true;
            QTimer::singleShot(0, o, SLOT(hide()));
        }
        return false;
    }
private:
    bool menuShown;
};

void tst_Q3ToolBar::actionGroupPopup()
{
    Q3ActionGroup* ag = new Q3ActionGroup(testWidget);
    ag->setText("Group");
    ag->setUsesDropDown(true);
    ag->setExclusive(false);
    Q3Action *a = new Q3Action(QIcon(), "ActionA", QKeySequence(), ag);
    a->setToggleAction(true);
    Q3Action *b = new Q3Action(QIcon(), "ActionB", QKeySequence(), ag);
    b->setToggleAction(true);
    ag->addTo(testWidget);
    QTest::qWait(100);
#ifndef NOFINDCHILDRENMETHOD
    QList<QToolButton *> list = testWidget->findChildren<QToolButton *>();
#else
    QList<QToolButton *> list = qFindChildren<QToolButton *>(testWidget, QString());
#endif
    QToolButton *tb = 0;
    for (int i=0;i<list.size();i++) {
        if (list.at(i)->menu()) {
            tb = list.at(i);
            break;
        }
    }
    MenuEventFilter mef;
    tb->menu()->installEventFilter(&mef);
    QTest::mouseClick(tb, Qt::LeftButton, 0, QPoint(5,5));
    QVERIFY(!mef.wasMenuShown());
    QTest::mouseClick(tb, Qt::LeftButton, 0, QPoint(tb->rect().right() - 5, tb->rect().bottom() - 5));
    QVERIFY(mef.wasMenuShown());
}

class Q3MainWindow_task182657 : public Q3MainWindow
{
    Q3ToolBar *toolbar;

public:
    Q3MainWindow_task182657(QWidget *parent = 0)
        : Q3MainWindow(parent)
    {
	toolbar = new Q3ToolBar(this);
    }

    void rebuild()
    {
	toolbar->clear();
 	new QToolButton(toolbar, "b");
 	new QToolButton(toolbar, "a");
    }
};

void tst_Q3ToolBar::task182657()
{
    Q3MainWindow_task182657 *window = new Q3MainWindow_task182657;
    window->show();
    qApp->processEvents();
    window->rebuild();
    qApp->processEvents();
    window->rebuild();
    qApp->processEvents();
}

QTEST_MAIN(tst_Q3ToolBar)
#include "tst_q3toolbar.moc"
