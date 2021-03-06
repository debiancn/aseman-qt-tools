/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "asemanqttools.h"
#include "asemanquickview.h"
#include "asemandesktoptools.h"
#include "asemanqtlogger.h"
#include "asemandevices.h"
#include "asemantools.h"
#include "asemanapplication.h"
#include "asemanhashobject.h"
#include "asemanlistobject.h"
#include "asemancalendarconverter.h"
#include "asemanimagecoloranalizor.h"
#include "asemanmimedata.h"
#include "asemandragobject.h"
#include "asemandownloader.h"
#include "asemanfilesystemmodel.h"
#include "asemanbackhandler.h"
#include "asemanquickobject.h"
#include "asemancountriesmodel.h"
#include "asemanquickitemimagegrabber.h"
#include "asemanfiledownloaderqueueitem.h"
#include "asemanfiledownloaderqueue.h"
#include "asemannotification.h"
#include "asemanautostartmanager.h"
#include "asemanmimeapps.h"
#include "asemanwebpagegrabber.h"
#include "asemantitlebarcolorgrabber.h"
#include "asemantaskbarbutton.h"
#include "asemanmapdownloader.h"
#include "asemandragarea.h"
#ifdef Q_OS_ANDROID
#include "asemanjavalayer.h"
#endif
#ifdef ASEMAN_SENSORS
#include "asemansensors.h"
#endif
#ifdef ASEMAN_MULTIMEDIA
#include "asemanaudiorecorder.h"
#include "asemanaudioencodersettings.h"
#endif
#if defined(Q_OS_LINUX) && defined(QT_DBUS_LIB)
#include "asemankdewallet.h"
#endif

#include <QPointer>
#include <QSharedPointer>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QScreen>
#include <QSet>
#include <QDebug>

class AsemanQuickViewPrivate
{
public:
    QPointer<QObject> root;
    QPointer<QQuickItem> focused_text;

    int layoutDirection;
    bool reverseScroll;

    QQmlEngine *engine;
};

AsemanQuickView::AsemanQuickView(QQmlEngine *engine, QObject *parent ) :
    QObject(parent)
{
    p = new AsemanQuickViewPrivate;
    p->engine = engine;
    p->layoutDirection = Qt::LeftToRight;
    p->reverseScroll = false;
}

AsemanDesktopTools *AsemanQuickView::desktopTools() const
{
    return AsemanQtTools::desktopTools();
}

AsemanDevices *AsemanQuickView::devices() const
{
    return AsemanQtTools::devices();
}

AsemanQtLogger *AsemanQuickView::qtLogger() const
{
    return AsemanQtTools::qtLogger();
}

AsemanTools *AsemanQuickView::tools() const
{
    return AsemanQtTools::tools();
}

#ifdef Q_OS_ANDROID
AsemanJavaLayer *AsemanQuickView::javaLayer() const
{
    return AsemanJavaLayer::instance();
}
#endif

AsemanCalendarConverter *AsemanQuickView::calendar() const
{
    return AsemanQtTools::calendar(p->engine);
}

AsemanBackHandler *AsemanQuickView::backHandler() const
{
    return AsemanQtTools::backHandler(p->engine);
}

void AsemanQuickView::setReverseScroll(bool stt)
{
    if(p->reverseScroll == stt)
        return;

    p->reverseScroll = stt;
    emit reverseScrollChanged();
}

bool AsemanQuickView::reverseScroll() const
{
    return p->reverseScroll;
}

qreal AsemanQuickView::statusBarHeight() const
{
    return AsemanDevices::statusBarHeight();
}

qreal AsemanQuickView::navigationBarHeight() const
{
    return AsemanDevices::navigationBarHeight();
}

void AsemanQuickView::setRoot(QObject *root)
{
    if( p->root == root )
        return;

    p->root = root;
    emit rootChanged();
}

QObject *AsemanQuickView::root() const
{
    if( p->root )
        return p->root;

    return p->root;
}

void AsemanQuickView::setFocusedText(QQuickItem *item)
{
    if( p->focused_text == item )
        return;
    if( p->focused_text )
        disconnect( p->focused_text, SIGNAL(destroyed()), this, SIGNAL(focusedTextChanged()) );

    p->focused_text = item;
    if( item )
    {
        connect( item, SIGNAL(destroyed()), this, SIGNAL(focusedTextChanged()) );
        devices()->showKeyboard();
    }
    else
    {
        devices()->hideKeyboard();
    }

    emit focusedTextChanged();
}

QQuickItem *AsemanQuickView::focusedText() const
{
    return p->focused_text;
}

int AsemanQuickView::layoutDirection() const
{
    return p->layoutDirection;
}

void AsemanQuickView::setLayoutDirection(int l)
{
    if( l == p->layoutDirection )
        return;

    p->layoutDirection = l;
    emit layoutDirectionChanged();
}

qreal AsemanQuickView::flickVelocity() const
{
#ifdef DESKTOP_DEVICE
    return 2500;
#else
    return 25000;
#endif
}

void AsemanQuickView::setOfflineStoragePath(const QString &path)
{
    if(path == offlineStoragePath())
        return;

    p->engine->setOfflineStoragePath(path);
    emit offlineStoragePathChanged();
}

QString AsemanQuickView::offlineStoragePath() const
{
    return p->engine->offlineStoragePath();
}

void AsemanQuickView::registerWindow(QQuickWindow *window)
{
    window->installEventFilter(this);
}

void AsemanQuickView::discardFocusedText()
{
    setFocusedText(0);
}

bool AsemanQuickView::eventFilter(QObject *o, QEvent *e)
{
    QQuickWindow *win = qobject_cast<QQuickWindow*>(o);
    if(win)
    {
        switch( static_cast<int>(e->type()) )
        {
        case QEvent::Close:
            if(o->property("backController").toBool())
            {
                QCloseEvent *ce = static_cast<QCloseEvent*>(e);
                if( o->property("try_close").toBool() || AsemanDevices::isDesktop() )
                    ce->accept();
                else
                {
                    ce->ignore();
                    QMetaObject::invokeMethod(o, "closeRequest");
                }
            }
            break;
        case QEvent::KeyPress:
        {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            switch(ke->key())
            {
            case Qt::Key_Escape:
                QMetaObject::invokeMethod(o, "closeRequest");
                break;
            }
        }
            break;
        }
    }

    return QObject::eventFilter(o,e);
}

AsemanQuickView::~AsemanQuickView()
{
    delete p;
}
