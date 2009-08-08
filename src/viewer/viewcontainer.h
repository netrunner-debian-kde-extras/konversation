/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006-2008 Eike Hein <hein@kde.org>
*/

#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include "mainwindow.h"
#include "common.h"
#include "server.h"

#include <QList>
#include <QObject>
#include <QPointer>


class QSplitter;

class KTabWidget;
class KActionCollection;
class KVBox;

class MainWindow;
class ViewTree;
class ChatWindow;
class Server;
class Images;
class UrlCatcher;
class NicksOnline;
class QueueTuner;

namespace Konversation
{
    class InsertCharDialog;
    class ServerGroupSettings;

    namespace DCC
    {
        class TransferPanel;
    }
}

class ViewContainer : public QObject
{
    Q_OBJECT

    public:
        explicit ViewContainer(MainWindow* window);
        ~ViewContainer();

        QSplitter* getWidget() { return m_viewTreeSplitter; }
        MainWindow* getWindow() { return m_window; }
        KActionCollection* actionCollection() { return m_window->actionCollection(); }

        QPointer<ChatWindow> getFrontView() { return m_frontView; }
        Server* getFrontServer() { return m_frontServer; }

        void prepareShutdown();

        QString currentViewTitle();
        QString currentViewURL(bool passNetwork);

        void appendToFrontmost(const QString& type,const QString& message,ChatWindow* serverView,
                               bool parseURL = true);

        void showQueueTuner(bool);

        int getViewIndex(QWidget* widget);

    public slots:
        void updateAppearance();
        void saveSplitterSizes();
        void setViewTreeShown(bool show);
        void syncTabBarToTree();

        void updateViews(const Konversation::ServerGroupSettingsPtr serverGroup = Konversation::ServerGroupSettingsPtr());
        void updateViewIcons();
        void setViewNotification(ChatWindow* widget, const Konversation::TabNotifyType& type);
        void unsetViewNotification(ChatWindow* view);
        void toggleViewNotifications();
        void toggleAutoJoin();

        void switchView(int newIndex);
        void showView(ChatWindow* view);

        void goToView(int page);
        void showNextView();
        void showPreviousView();
        void moveViewLeft();
        void moveViewRight();

        void closeView(QWidget* view);
        void closeView(ChatWindow* view);
        void closeViewMiddleClick(QWidget* view);
        void closeCurrentView();
        void renameKonsole();
        void cleanupAfterClose(ChatWindow* view);

        void changeViewCharset(int index);
        void updateViewEncoding(ChatWindow* view);

        void showViewContextMenu(QWidget* tab, const QPoint& pos);

        void clearView();
        void clearAllViews();

        void findText();
        void findNextText();
        void findPrevText();

        void insertCharacter();
        void insertChar(const QChar& chr);
        void insertIRCColor();

        void clearViewLines();
        void insertRememberLine();
        void cancelRememberLine();
        void insertMarkerLine();
        void insertRememberLines(Server* server);

        void openLogFile();
        void openLogFile(const QString& caption, const QString& file);

        void addKonsolePanel();

        void addUrlCatcher();
        void closeUrlCatcher();

        void toggleDccPanel();
        void addDccPanel();
        void closeDccPanel();
        void deleteDccPanel();
        Konversation::DCC::TransferPanel* getDccPanel();

        void addDccChat(const QString& myNick,const QString& nick,const QStringList& arguments,bool listen);

        StatusPanel* addStatusView(Server* server);
        RawLog* addRawLog(Server* server);
        void disconnectFrontServer();
        void reconnectFrontServer();
        void showJoinChannelDialog();
        void connectionStateChanged(Server* server, Konversation::ConnectionState state);
        void channelJoined(Channel* channel);

        Channel* addChannel(Server* server, const QString& name);
        void rejoinChannel();
        void openChannelSettings();
        void toggleChannelNicklists();

        Query* addQuery(Server* server,const NickInfoPtr & name, bool weinitiated=true);
        void updateQueryChrome(ChatWindow* view, const QString& name);
        void closeQueries();

        ChannelListPanel* addChannelListPanel(Server* server);
        void openChannelList(const QString& filter = QString(), bool getList = false);

        void openNicksOnlinePanel();
        void closeNicksOnlinePanel();

        void showNextActiveView();

    signals:
        void viewChanged(ChatWindow* view);
        void removeView(ChatWindow* view);
        void setWindowCaption(const QString& caption);
        void updateChannelAppearance();
        void contextMenuClosed();
        void resetStatusBar();
        void setStatusBarTempText(const QString& text);
        void clearStatusBarTempText();
        void setStatusBarInfoLabel(const QString& text);
        void clearStatusBarInfoLabel();
        void setStatusBarLagLabelShown(bool shown);
        void updateStatusBarLagLabel(Server* server, int msec);
        void resetStatusBarLagLabel();
        void setStatusBarLagLabelTooLongLag(Server* server, int msec);
        void updateStatusBarSSLLabel(Server* server);
        void removeStatusBarSSLLabel();
        void autoJoinToggled(const Konversation::ServerGroupSettingsPtr);

        void frontServerChanging(Server *);

    private:
        void setupTabWidget();
        void setupViewTree();
        void removeViewTree();
        void updateTabWidgetAppearance();

        void addView(ChatWindow* view, const QString& label, bool weinitiated=true);

        void updateViewActions(int index);
        void updateSwitchViewAction();
        void updateFrontView();

        void setFrontServer(Server *);

        void initializeSplitterSizes();
        bool m_saveSplitterSizesLock;

        MainWindow* m_window;

        QSplitter* m_viewTreeSplitter;
        KTabWidget* m_tabWidget;
        ViewTree* m_viewTree;
        KVBox *m_vbox;
        QueueTuner *m_queueTuner;

        Images* images;

        QPointer<Server> m_frontServer;
        QPointer<Server> m_contextServer;
        QPointer<ChatWindow> m_frontView;
        QPointer<ChatWindow> m_searchView;

        UrlCatcher* m_urlCatcherPanel;
        NicksOnline* m_nicksOnlinePanel;

        Konversation::DCC::TransferPanel* m_dccPanel;
        bool m_dccPanelOpen;

        Konversation::InsertCharDialog* m_insertCharDialog;

        int m_popupViewIndex;
        int m_queryViewCount;

        QList<ChatWindow*> m_activeViewOrderList;
};

#endif
