/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  This is the class that shows the channel nick list
  begin:     Fre Jun 7 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#include "nicklistview.h"
#include "application.h"
#include "images.h"
#include "linkaddressbook/addressbook.h"

#include <QContextMenuEvent>
#include <QDropEvent>
#include <QToolTip>

#include <KAuthorized>


NickListView::NickListView(QWidget* parent, Channel *chan) :
K3ListView(parent),
    m_whoisAction(0),
    m_versionAction(0),
    m_pingAction(0),
    m_giveOpAction(0),
    m_takeOpAction(0),
    m_giveHalfOpAction(0),
    m_takeHalfOpAction(0),
    m_giveVoiceAction(0),
    m_takeVoiceAction(0),
    m_ignoreAction(0),
    m_unIgnoreAction(0),
    m_kickAction(0),
    m_kickBanAction(0),
    m_banNickAction(0),
    m_banHostAction(0),
    m_banDomainAction(0),
    m_banUserHostAction(0),
    m_banUserDomainAction(0),
    m_kickBanHostAction(0),
    m_kickBanDomainAction(0),
    m_kickBanUserHostAction(0),
    m_kickBanUserDomainAction(0),
    m_addNotifyAction(0),
    m_sendMailAction(0),
    m_AddressbookNewAction(0),
    m_AddressbookChangeAction(0),
    m_AddressbookEditAction(0),
    m_AddressbookDeleteAction(0),
    m_openQueryAction(0),
    m_startDccChatAction(0),
    m_dccSendAction(0)
{
    K3ListView::setSorting(-1);
    setWhatsThis();
    channel=chan;
    popup=new KMenu(this);
    popup->setObjectName("nicklist_context_menu");
    modes=new KMenu(this);
    modes->setObjectName("nicklist_modes_context_submenu");
    kickban=new KMenu(this);
    kickban->setObjectName("nicklist_kick_ban_context_submenu");
    addressbook= new KMenu(this);
    addressbook->setObjectName("nicklist_addressbook_context_submenu");
    setAcceptDrops(true);
    setDropHighlighter(true);
    setDropVisualizer(false);

    m_actionGroup = new QActionGroup(this);
    connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotActionTriggered(QAction*)));

    if (popup)
    {
        m_whoisAction = createAction(popup,i18n("&Whois"),Konversation::Whois);
        m_versionAction = createAction(popup,i18n("&Version"),Konversation::Version);
        m_pingAction = createAction(popup,i18n("&Ping"),Konversation::Ping);

        popup->addSeparator();

        if (modes)
        {
            m_giveOpAction = createAction(modes,i18n("Give Op"),Konversation::GiveOp);
#if KDE_IS_VERSION(4, 2, 85)
            m_giveOpAction->setIcon(KIcon("irc-operator"));
#endif
            m_takeOpAction = createAction(modes,i18n("Take Op"),Konversation::TakeOp);
#if KDE_IS_VERSION(4, 2, 85)
            m_takeOpAction->setIcon(KIcon("irc-remove-operator"));
#endif
            m_giveHalfOpAction = createAction(modes,i18n("Give HalfOp"),Konversation::GiveHalfOp);
            m_takeHalfOpAction = createAction(modes,i18n("Take HalfOp"),Konversation::TakeHalfOp);
            m_giveVoiceAction = createAction(modes,i18n("Give Voice"),Konversation::GiveVoice);
#if KDE_IS_VERSION(4, 2, 85)
            m_giveVoiceAction->setIcon(KIcon("irc-voice"));
#endif
            m_takeVoiceAction = createAction(modes,i18n("Take Voice"),Konversation::TakeVoice);
#if KDE_IS_VERSION(4, 2, 85)
            m_takeVoiceAction->setIcon(KIcon("irc-unvoice"));
#endif
            KAction* modeAction = new KAction(i18n("Modes"), popup);
            popup->addAction(modeAction);
            modeAction->setMenu(modes);
        }

        if (kickban)
        {
            m_kickAction = createAction(kickban,i18n("Kick"),Konversation::Kick);
            m_kickBanAction = createAction(kickban,i18n("Kickban"),Konversation::KickBan);
            m_banNickAction = createAction(kickban,i18n("Ban Nickname"),Konversation::BanNick);
            kickban->addSeparator();
            m_banHostAction = createAction(kickban,i18n("Ban *!*@*.host"),Konversation::BanHost);
            m_banDomainAction = createAction(kickban,i18n("Ban *!*@domain"),Konversation::BanDomain);
            m_banUserHostAction = createAction(kickban,i18n("Ban *!user@*.host"),Konversation::BanUserHost);
            m_banUserDomainAction = createAction(kickban,i18n("Ban *!user@domain"),Konversation::BanUserDomain);
            kickban->addSeparator();
            m_kickBanHostAction = createAction(kickban,i18n("Kickban *!*@*.host"),Konversation::KickBanHost);
            m_kickBanDomainAction = createAction(kickban,i18n("Kickban *!*@domain"),Konversation::KickBanDomain);
            m_kickBanUserHostAction = createAction(kickban,i18n("Kickban *!user@*.host"),Konversation::KickBanUserHost);
            m_kickBanUserDomainAction = createAction(kickban,i18n("Kickban *!user@domain"),Konversation::KickBanUserDomain);
            KAction* kickBanSubAction = new KAction(i18n("Kick / Ban"), popup);
            popup->addAction(kickBanSubAction);
            kickBanSubAction->setMenu(kickban);
        }

        m_ignoreAction = createAction(popup,i18n("Ignore"),Konversation::IgnoreNick);
        m_unIgnoreAction = createAction(popup,i18n("Unignore"),Konversation::UnignoreNick);

        popup->addSeparator();

        m_openQueryAction = createAction(popup,i18n("Open &Query"),Konversation::OpenQuery);
        m_openQueryAction->setWhatsThis(i18n("<qt><p>Start a private chat between you and this person.</p><p><em>Technical note:</em><br />The conversation between you and this person will be sent via the server.  This means that the conversation will be affected by server lag, server stability, and will be terminated when you disconnect from the server.</p></qt>"));
        m_startDccChatAction = createAction(popup,i18n("Open DCC &Chat"),Konversation::StartDccChat);
        m_startDccChatAction->setWhatsThis(i18n("<qt><p>Start a private <em>D</em>irect <em>C</em>lient <em>C</em>onnection chat between you and this person.</p><p><em>Technical note:</em><br />The conversation between you and this person will be sent directly.  This means it is independent from the server - so if the server connection fails, or use disconnect, your DCC Chat will be unaffected.  It also means that no irc server admin can view or spy on this chat.</p></qt>"));
        if (KAuthorized::authorizeKAction("allow_downloading"))
        {
            m_dccSendAction = createAction(popup,i18n("Send &File..."),Konversation::DccSend);
            m_dccSendAction->setIcon(KIcon("arrow-right-double"));
            m_dccSendAction->setWhatsThis(i18n("<qt>Send a file to this person.  If you are having problem sending files, or they are sending slowly, see the Konversation Handbook and DCC preferences page.</qt>"));
        }
        m_sendMailAction = createAction(popup,i18n("&Send Email..."),Konversation::SendEmail);
        m_sendMailAction->setIcon(KIcon("mail-send"));

        popup->addSeparator();

        if (addressbook) {
            KAction* addressbookSubAction = new KAction(i18n("Address Book Associations"), popup);
            popup->addAction(addressbookSubAction);
            addressbookSubAction->setMenu(addressbook);
        }

        m_addNotifyAction = createAction(popup,i18n("Add to Watched Nicks"),Konversation::AddNotify);
    }
    else
    {
        kWarning() << "Could not create popup!" ;
    }

    setShadeSortColumn(false);

    // We have our own tooltips, don't use the default QListView ones
    setShowToolTips(false);

    m_resortTimer = new QTimer(this);
    m_resortTimer->setSingleShot(true);
    connect(m_resortTimer, SIGNAL(timeout()), SLOT(resort()));
}

NickListView::~NickListView()
{
}

bool NickListView::event(QEvent *event)
{
    if(( event->type() == QEvent::ToolTip ) )
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>( event );

        Q3ListViewItem *item = itemAt( helpEvent->pos() );
        if( item )
        {
            Nick *nick = dynamic_cast<Nick*>( item );
            if( nick )
            {
               QString text =  Konversation::removeIrcMarkup(nick->getChannelNick()->tooltip());
               if( !text.isEmpty() )
                       QToolTip::showText( helpEvent->globalPos(), text );
               else
                       QToolTip::hideText();
            }

        }
        else
                QToolTip::hideText();
    }
    return K3ListView::event( event );
}

void NickListView::setWhatsThis()
{
    Images* images = Application::instance()->images();

    if(images->getNickIcon( Images::Normal, false).isNull())
    {
        K3ListView::setWhatsThis(i18n("<qt><p>This shows all the people in the channel.  The nick for each person is shown.<br />Usually an icon is shown showing the status of each person, but you do not seem to have any icon theme installed.  See the Konversation settings - <i>Configure Konversation</i> under the <i>Settings</i> menu.  Then view the page for <i>Themes</i> under <i>Appearance</i>.</p></qt>"));
    }
    else
    {
        K3ListView::setWhatsThis(i18n("<qt><p>This shows all the people in the channel.  The nick for each person is shown, with a picture showing their status.</p>"
            "<table>"

            "<tr><th><img src=\"admin\"/></th><td>This person has administrator privileges.</td></tr>"
            "<tr><th><img src=\"owner\"/></th><td>This person is a channel owner.</td></tr>"
            "<tr><th><img src=\"op\"/></th><td>This person is a channel operator.</td></tr>"
            "<tr><th><img src=\"halfop\"/></th><td>This person is a channel half-operator.</td></tr>"
            "<tr><th><img src=\"voice\"/></th><td>This person has voice, and can therefore talk in a moderated channel.</td></tr>"
            "<tr><th><img src=\"normal\"/></th><td>This person does not have any special privileges.</td></tr>"
            "<tr><th><img src=\"normalaway\"/></th><td>This indicates that this person is currently away.</td></tr>"
            "</table><p>"
            "The meaning of admin, owner and halfop varies between different IRC servers.</p><p>"
            "Hovering over any nick shows their current status"
                    //", as well as any information in the addressbook for this person"
                    ". See the Konversation Handbook for more information."
            "</p></qt>"
            )
            .replace("'admin'", images->getNickIconPath(Images::Admin))
            .replace("'owner'", images->getNickIconPath(Images::Owner))
            .replace("'op'", images->getNickIconPath(Images::Op))
            .replace("'halfop'", images->getNickIconPath(Images::HalfOp))
            .replace("'voice'", images->getNickIconPath(Images::Voice))
            .replace("'normal'", images->getNickIconPath(Images::Normal))
            .replace("'normalaway'", images->getNickIconPath(Images::Normal))
            );
    }

}

void NickListView::refresh()
{
    Q3ListViewItemIterator it(this);

    while (it.current())
    {
        static_cast<Nick*>(it.current())->refresh();
        ++it;
    }

    setWhatsThis();
}

void NickListView::startResortTimer()
{
    if(!m_resortTimer->isActive())
        m_resortTimer->start(3000);
}

void NickListView::resort()
{
    K3ListView::setSorting(m_column, m_ascending);
    sort();
    K3ListView::setSorting(-1);
}

void NickListView::contextMenuEvent(QContextMenuEvent* ce)
{
    ce->accept();

    if (selectedItems().count())
    {
        insertAssociationSubMenu();
        updateActions();
        popup->popup(ce->globalPos());
    }
}

void NickListView::updateActions()
{
    int ignoreCounter = 0;
    int unignoreCounter = 0;
    int notifyCounter = 0;

    int serverGroupId = -1;

    if (channel->getServer()->getServerGroup())
        serverGroupId = channel->getServer()->getServerGroup()->id();

    ChannelNickList nickList=channel->getSelectedChannelNicks();
    ChannelNickList::ConstIterator it;

    for (it = nickList.constBegin(); it != nickList.constEnd(); ++it)
    {
        if (Preferences::isIgnored((*it)->getNickname()))
            ++unignoreCounter;
        else
            ++ignoreCounter;

        if (serverGroupId != -1 && Preferences::isNotify(serverGroupId, (*it)->getNickname()))
            ++notifyCounter;
    }

    if (ignoreCounter)
        m_ignoreAction->setVisible(true);
    else
        m_ignoreAction->setVisible(false);

    if (unignoreCounter)
        m_unIgnoreAction->setVisible(true);
    else
        m_unIgnoreAction->setVisible(false);

    if (notifyCounter || !Preferences::hasNotifyList(serverGroupId))
        m_addNotifyAction->setEnabled(false);
    else
        m_addNotifyAction->setEnabled(true);
}

void NickListView::insertAssociationSubMenu()
{

    bool existingAssociation = false;
    bool noAssociation = false;
    bool emailAddress = false;

    addressbook->clear();

    ChannelNickList nickList=channel->getSelectedChannelNicks();
    for(ChannelNickList::ConstIterator it=nickList.constBegin();it!=nickList.constEnd();++it)
    {
        KABC::Addressee addr = (*it)->getNickInfo()->getAddressee();
        if(addr.isEmpty())
        {
            noAssociation=true;
            if(existingAssociation && emailAddress) break;
        }
        else
        {
            if(!emailAddress && !addr.preferredEmail().isEmpty())
                emailAddress = true;
            existingAssociation=true;
            if(noAssociation && emailAddress) break;
        }
    }

    if(!noAssociation && existingAssociation)
    {
        m_AddressbookEditAction = createAction(addressbook,i18n("Edit Contact..."),Konversation::AddressbookEdit);
        m_AddressbookEditAction->setIcon(KIcon("document-edit"));
        addressbook->addSeparator();
    }

    if(noAssociation && existingAssociation)
        m_AddressbookChangeAction = createAction(addressbook,i18n("Choose/Change Associations..."),Konversation::AddressbookChange);
    else if(noAssociation)
        m_AddressbookChangeAction = createAction(addressbook,i18n("Choose Contact..."),Konversation::AddressbookChange);
    else
        m_AddressbookChangeAction = createAction(addressbook,i18n("Change Association..."),Konversation::AddressbookChange);

    if(noAssociation && !existingAssociation)
        m_AddressbookNewAction = createAction(addressbook,i18n("Create New Contact..."),Konversation::AddressbookNew);

    if(existingAssociation)
    {
        m_AddressbookDeleteAction = createAction(addressbook,i18n("Delete Association"),Konversation::AddressbookDelete);
        m_AddressbookDeleteAction->setIcon(KIcon("edit-delete"));
    }

    if(!emailAddress)
        m_sendMailAction->setEnabled(false);
    else
        m_sendMailAction->setEnabled(true);

}

void NickListView::setSorting(int column, bool ascending)
{
    m_column = column;
    m_ascending = ascending;
}

bool NickListView::acceptDrag (QDropEvent* event) const
{
    if (KUrl::List::canDecode(event->mimeData()))
    {
        const KUrl::List uris = KUrl::List::fromMimeData(event->mimeData());
        if (!uris.isEmpty())
        {
            const KUrl first = uris.first();

            if ((first.protocol() == QLatin1String("irc")) || channel->getNickList().containsNick(first.url()))
                return false;
        }

        return true;
    }
    else
        return false;
}

Q_DECLARE_METATYPE(Konversation::PopupIDs)

KAction* NickListView::createAction(QMenu* menu, const QString& text, Konversation::PopupIDs id)
{
    KAction* action = new KAction(text, menu);
    menu->addAction(action);
    action->setData(QVariant::fromValue(id));
    m_actionGroup->addAction(action);
    return action;
}

void NickListView::slotActionTriggered(QAction* action)
{
    popupCommand(action->data().value<Konversation::PopupIDs>());
}

#include "nicklistview.moc"