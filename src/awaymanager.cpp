/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
  Copyright (C) 2008 Eike Hein <hein@kde.org>
*/

#include "awaymanager.h"
#include "application.h"
#include "mainwindow.h"
#include "connectionmanager.h"
#include "server.h"
#include "preferences.h"
#include <config-konversation.h>

#include <QVariant>
#include <QTimer>
#include <QDBusInterface>
#include <QDBusReply>

#include <KSelectAction>
#include <KActionCollection>
#include <KToggleAction>

#ifdef Q_WS_X11

#if defined(HAVE_X11) && defined(HAVE_XUTIL)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#define HasXHeaders
#endif

#ifdef HAVE_XSCREENSAVER
#define HasScreenSaver
#include <X11/extensions/scrnsaver.h>
#include <QX11Info>
#endif

// Don't use XIdle for now, it's experimental.
#undef HAVE_XIDLE
#undef HasXidle

#include <fixx11h.h>
#endif


struct AwayManagerPrivate
{
    int mouseX;
    int mouseY;
    unsigned int mouseMask;
#ifdef HasXHeaders
    Window root;
    Screen* screen;
    Time xIdleTime;
#endif
    bool useXidle;
    bool useMit;
};

AwayManager::AwayManager(QObject* parent) : QObject(parent)
{
    int dummy = 0;
    dummy = dummy;

    d = new AwayManagerPrivate;

    d->mouseX = d->mouseY = 0;
    d->mouseMask = 0;
    d->useXidle = false;
    d->useMit = false;

    m_connectionManager = static_cast<Application*>(kapp)->getConnectionManager();

#ifdef HasXHeaders
    Display* display = QX11Info::display();
    d->root = DefaultRootWindow(display);
    d->screen = ScreenOfDisplay(display, DefaultScreen (display));

    d->xIdleTime = 0;
#endif

#ifdef HasXidle
    d->useXidle = XidleQueryExtension(QX11Info::display(), &dummy, &dummy);
#endif

#ifdef HasScreenSaver
    if (!d->useXidle)
        d->useMit = XScreenSaverQueryExtension(QX11Info::display(), &dummy, &dummy);
#endif

    m_activityTimer = new QTimer(this);
    m_activityTimer->setObjectName("AwayTimer");
    connect(m_activityTimer, SIGNAL(timeout()), this, SLOT(checkActivity()));

    resetIdle();
}

AwayManager::~AwayManager()
{
    delete d;
}

void AwayManager::resetIdle()
{
    m_idleTime.start();
}

void AwayManager::identitiesChanged()
{
    QList<int> newIdentityList;

    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
    {
        IdentityPtr identity = server->getIdentity();

        if (identity && identity->getAutomaticAway() && server->isConnected())
            newIdentityList.append(identity->id());
    }

    m_identitiesOnAutoAway = newIdentityList;

    toggleTimer();
}

void AwayManager::identityOnline(int identityId)
{
    IdentityPtr identity = Preferences::identityById(identityId);

    if (identity && identity->getAutomaticAway() &&
        !m_identitiesOnAutoAway.contains(identityId))
    {
        m_identitiesOnAutoAway.append(identityId);

        toggleTimer();
    }
}

void AwayManager::identityOffline(int identityId)
{
    if (m_identitiesOnAutoAway.removeOne(identityId))
    {
        toggleTimer();
    }
}

void AwayManager::toggleTimer()
{
    if (m_identitiesOnAutoAway.count() > 0)
    {
        if (!m_activityTimer->isActive())
            m_activityTimer->start(Preferences::self()->autoAwayPollInterval() * 1000);
    }
    else if (m_activityTimer->isActive())
        m_activityTimer->stop();
}

void AwayManager::checkActivity()
{
    // Allow the event loop to be called, to avoid deadlock.
    static bool rentrencyProtection = false;
    if (rentrencyProtection) return;

    rentrencyProtection = true;
    QDBusInterface screenSaver("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver");
    QDBusReply<bool> isBlanked = screenSaver.call("GetActive");
    rentrencyProtection = false;

    if (!isBlanked.isValid() || !isBlanked.value())
         implementIdleAutoAway(Xactivity());
}

bool AwayManager::Xactivity()
{
    bool activity = false;

#ifdef HasXHeaders
    Display* display = QX11Info::display();
    Window dummyW;
    int dummyC;
    unsigned int mask;
    int rootX;
    int rootY;

    if (!XQueryPointer (display, d->root, &(d->root), &dummyW, &rootX, &rootY,
            &dummyC, &dummyC, &mask))
    {
        // Figure out which screen the pointer has moved to.
        for (int i = 0; i < ScreenCount(display); i++)
        {
            if (d->root == RootWindow(display, i))
            {
                d->screen = ScreenOfDisplay (display, i);

                break;
            }
        }
    }

    Time xIdleTime = 0;

    #ifdef HasXidle
    if (d->useXidle)
        XGetIdleTime(display, &xIdleTime);
    else
    #endif
    {
    #ifdef HasScreenSaver
        if (d->useMit)
        {
            static XScreenSaverInfo* mitInfo = 0;
            if (!mitInfo) mitInfo = XScreenSaverAllocInfo();
            XScreenSaverQueryInfo (display, d->root, mitInfo);
            xIdleTime = mitInfo->idle;
        }
    #endif
    }

    if (rootX != d->mouseX || rootY != d->mouseY || mask != d->mouseMask
        || ((d->useXidle || d->useMit) && xIdleTime < d->xIdleTime + 2000))
    {
        // Set by setManagedIdentitiesAway() to skip X-based activity checking for one
        // round, to avoid jumping on residual mouse activity after manual screensaver
        // activation.
        if (d->mouseX != -1) activity = true;

        d->mouseX = rootX;
        d->mouseY = rootY;
        d->mouseMask = mask;
        d->xIdleTime = xIdleTime;
    }
#endif

    return activity;
}

void AwayManager::implementIdleAutoAway(bool activity)
{
    if (activity)
    {
        resetIdle();

        const QList<Server*> serverList = m_connectionManager->getServerList();

        foreach (Server* server, serverList)
        {
            IdentityPtr identity = server->getIdentity();

            if (m_identitiesOnAutoAway.contains(identity->id()) && identity->getAutomaticUnaway()
                && server->isConnected() && server->isAway())
            {
                server->requestUnaway();
            }
        }
    }
    else
    {
        long int idleTime = m_idleTime.elapsed() / 1000;

        QList<int> identitiesIdleTimeExceeded;
        QList<int>::ConstIterator it;

        for (it = m_identitiesOnAutoAway.constBegin(); it != m_identitiesOnAutoAway.constEnd(); ++it)
        {
            if (idleTime >= Preferences::identityById((*it))->getAwayInactivity() * 60)
                identitiesIdleTimeExceeded.append((*it));
        }

        const QList<Server*> serverList = m_connectionManager->getServerList();

        foreach (Server* server, serverList)
        {
            int identityId = server->getIdentity()->id();

            if (identitiesIdleTimeExceeded.contains(identityId) && server->isConnected() && !server->isAway())
                server->requestAway();
        }
    }
}

void AwayManager::setManagedIdentitiesAway()
{
    // Used to skip X-based activity checking for one round, to avoid jumping
    // on residual mouse activity after manual screensaver activation.
    d->mouseX = -1;

    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
    {
        if (m_identitiesOnAutoAway.contains(server->getIdentity()->id()) && server->isConnected() && !server->isAway())
            server->requestAway();
    }
}

void AwayManager::setManagedIdentitiesUnaway()
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
    {
        IdentityPtr identity = server->getIdentity();

        if (m_identitiesOnAutoAway.contains(identity->id()) && identity->getAutomaticUnaway()
            && server->isConnected() && server->isAway())
        {
            server->requestUnaway();
        }
    }
}

void AwayManager::requestAllAway(const QString& reason)
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected()) server->requestAway(reason);
}

void AwayManager::requestAllUnaway()
{
    const QList<Server*> serverList = m_connectionManager->getServerList();

    foreach (Server* server, serverList)
        if (server->isConnected() && server->isAway()) server->requestUnaway();
}

void AwayManager::toggleGlobalAway(bool away)
{
    if (away)
        requestAllAway();
    else
        requestAllUnaway();
}

void AwayManager::updateGlobalAwayAction(bool away)
{
    // FIXME: For now, our only triggers for resetting the idle time
    // are mouse movement and the screensaver getting disabled. This
    // means that typing '/unaway' or '/back' does not reset the idle
    // time and won't prevent AwayManager from setting a connection
    // away again shortly after when its identity's maximum auto-away
    // idle time, counted from the last mouse movement or screensaver
    // deactivation rather than the actual last user activity (the key
    // presses), has been exceeded. We work around this here by reset-
    // ting the idle time whenever any connection changes its state to
    // unaway in response to the server until we find a better solu-
    // tion (i.e. a reliable way to let keyboard activity in the sys-
    // tem reset the idle time).
    if(!away) resetIdle();

    Application* konvApp = static_cast<Application*>(kapp);
    KToggleAction* awayAction = qobject_cast<KToggleAction*>(konvApp->getMainWindow()->actionCollection()->action("toggle_away"));

    if (!awayAction) return;

    if (away)
    {
        const QList<Server*> serverList = m_connectionManager->getServerList();
        int awayCount = 0;

        foreach (Server* server, serverList)
        {
            if (server->isAway())
                awayCount++;
        }

        if (awayCount == serverList.count())
        {
            awayAction->setChecked(true);
            awayAction->setIcon(KIcon("im-user-away"));
        }
    }
    else
    {
        awayAction->setChecked(false);
        awayAction->setIcon(KIcon("im-user"));
    }
}

#include "awaymanager.moc"