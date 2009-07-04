/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2007 Shintaro Matsuoka <shin@shoegazed.org>
*/

#include "transferdetailedinfopanel.h"
#include "transfer.h"
#include "transferrecv.h"
#include "transfermanager.h"
#include "transferpanelitem.h"
#include "application.h"

#include "connectionmanager.h"
#include "server.h"

#include <QLabel>
#include <QTimer>

#include <KProgressDialog>
#include <KRun>

namespace Konversation
{
    namespace DCC
    {
        TransferDetailedInfoPanel::TransferDetailedInfoPanel( QWidget* parent )
            : QWidget( parent )
        {
            setupUi( this );

            m_autoViewUpdateTimer = new QTimer( this );

            connect( m_urlreqLocation, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
            connect( Application::instance()->getDccTransferManager(), SIGNAL( fileURLChanged( Konversation::DCC::TransferRecv* ) ),
                     this, SLOT( updateView() ) );  // it's a little rough..

            //only enable when needed
            m_urlreqLocation->lineEdit()->setReadOnly( true );
            m_urlreqLocation->button()->setEnabled( false );
            m_urlreqLocation->setMode(KFile::File | KFile::LocalOnly);
        }

        TransferDetailedInfoPanel::~TransferDetailedInfoPanel()
        {
        }

        void TransferDetailedInfoPanel::setItem( TransferPanelItem* item )
        {
            m_autoViewUpdateTimer->stop();

            // disconnect all slots once
            disconnect( this );
            // we can't do disconnect( m_item->transfer(), 0, this, 0 ) here
            // because m_item can have been deleted already.

            // set up the auto view-update timer
            connect( m_autoViewUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateChangeableView() ) );

            m_item = item;

            // If the file is already being transferred, the timer must be started here,
            // otherwise the information will not be updated every 0.5sec
            if (m_item->transfer()->getStatus() == Transfer::Transferring)
                m_autoViewUpdateTimer->start( 500 );
            connect( m_item->transfer(), SIGNAL( statusChanged( Konversation::DCC::Transfer*, int, int ) ), this, SLOT( slotTransferStatusChanged( Konversation::DCC::Transfer*, int, int ) ) );

            updateView();
        }

        void TransferDetailedInfoPanel::updateView()
        {
            Transfer* transfer = m_item->transfer();

            // Type:
            QString type( transfer->getType() == Transfer::Send ? i18n( "DCC Send" ) : i18n( "DCC Receive" ) );
            if ( transfer->isReverse() )
                type += i18n( " (Reverse DCC)" );
            m_labelDccType->setText( type );

            // Filename:
            m_labelFilename->setText( transfer->getFileName() );

            // Location:
            m_urlreqLocation->setUrl( transfer->getFileURL().prettyUrl() );
            //m_urlreqLocation->lineEdit()->setFocusPolicy( transfer->getStatus() == Transfer::Queued ? Qt::StrongFocus : ClickFocus );
            m_urlreqLocation->lineEdit()->setReadOnly( transfer->getStatus() != Transfer::Queued );
            m_urlreqLocation->lineEdit()->setFrame( transfer->getStatus() == Transfer::Queued );
            m_urlreqLocation->button()->setEnabled( transfer->getStatus() == Transfer::Queued );

            // Partner:
            QString partnerInfoServerName;
            if ( transfer->getConnectionId() == -1 )
                partnerInfoServerName = i18n( "Unknown server" );
            else if ( Application::instance()->getConnectionManager()->getServerByConnectionId( transfer->getConnectionId() ) )
                partnerInfoServerName = Application::instance()->getConnectionManager()->getServerByConnectionId( transfer->getConnectionId() )->getServerName();
            else
                partnerInfoServerName = i18n( "Unknown server" );
            QString partnerInfo( i18n( "%1 on %2",
                transfer->getPartnerNick().isEmpty() ? "?" : transfer->getPartnerNick(),
                partnerInfoServerName ) );
            if ( !transfer->getPartnerIp().isEmpty() )
                partnerInfo += i18n( ", %1 (port %2)", transfer->getPartnerIp(), QString::number( transfer->getPartnerPort() ) );
            m_labelPartner->setText( partnerInfo );

            // Self:
            if ( transfer->getOwnIp().isEmpty() )
                m_labelSelf->clear();
            else
                m_labelSelf->setText( i18n( "%1 (port %2)", transfer->getOwnIp(), QString::number( transfer->getOwnPort() ) ) );

            // File Size:
            m_labelFileSize->setText( KGlobal::locale()->formatNumber( transfer->getFileSize(), 0 ) );

            // Resumed:
            if ( transfer->isResumed() )
                m_labelIsResumed->setText( i18n( "Yes, %1", KGlobal::locale()->formatNumber( transfer->getTransferStartPosition(), 0 ) ) );
            else
                m_labelIsResumed->setText( i18n( "No" ) );

            // Offered at:
            m_labelTimeOffered->setText( transfer->getTimeOffer().toString( "hh:mm:ss" ) );

            // Started at:
            if ( !transfer->getTimeTransferStarted().isNull() )
                m_labelTimeStarted->setText( transfer->getTimeTransferStarted().toString( "hh:mm:ss" ) );
            else
                m_labelTimeStarted->clear();

            // Finished at:
            if ( !transfer->getTimeTransferFinished().isNull() )
            {
                m_labelTimeFinished->setText( transfer->getTimeTransferFinished().toString( "hh:mm:ss" ) );
            }
            else
            {
                m_labelTimeFinished->clear();
            }

            updateChangeableView();
        }

        void TransferDetailedInfoPanel::updateChangeableView()
        {
            Transfer* transfer = m_item->transfer();

            // Status:
            if ( transfer->getStatus() == Transfer::Transferring )
            {
                m_labelStatus->setText( m_item->getStatusText() + " ( " + m_item->getCurrentSpeedPrettyText() + " )" );
            }
            else
            {
                m_labelStatus->setText( transfer->getStatusDetail().isEmpty() ? m_item->getStatusText() : m_item->getStatusText() + " (" + transfer->getStatusDetail() + ')' );
            }

            // Progress:
            m_progress->setValue( transfer->getProgress() );

            // Current Position:
            m_labelCurrentPosition->setText( KGlobal::locale()->formatNumber( transfer->getTransferringPosition(), 0 ) );

            // Current Speed:
            m_labelCurrentSpeed->setText( m_item->getCurrentSpeedPrettyText() );

            // Average Speed:
            m_labelAverageSpeed->setText( m_item->getAverageSpeedPrettyText() );

            // Transferring Time:
            if ( transfer->getTimeTransferStarted().isNull() )
            {
                m_labelTransferringTime->clear();
            }
            else
            {
                int transferringTime;

                // The transfer is still in progress
                if ( transfer->getTimeTransferFinished().isNull() )
                    transferringTime = transfer->getTimeTransferStarted().secsTo( QDateTime::currentDateTime() );
                // The transfer has finished
                else
                    transferringTime = transfer->getTimeTransferStarted().secsTo( transfer->getTimeTransferFinished() );

                if ( transferringTime >= 1 )
                    m_labelTransferringTime->setText( TransferPanelItem::secToHMS( transferringTime ) );
                else
                    m_labelTransferringTime->setText( i18n( "< 1sec" ) );
            }

            // Estimated Time Left:
            m_labelTimeLeft->setText( m_item->getTimeLeftPrettyText() );
        }

        void TransferDetailedInfoPanel::slotTransferStatusChanged( Transfer* /* transfer */, int newStatus, int oldStatus )
        {
            updateView();
            if ( newStatus == Transfer::Transferring )
            {
                // start auto view-update timer
                m_autoViewUpdateTimer->start( 500 );
            }
            else if ( oldStatus == Transfer::Transferring )
            {
                // stop auto view-update timer
                m_autoViewUpdateTimer->stop();
            }
        }

        void TransferDetailedInfoPanel::slotLocationChanged( const QString& url )
        {
            if ( m_item &&  m_item->transfer() && m_item->transfer()->getType() == Transfer::Receive )
            {
                TransferRecv* transfer = static_cast< TransferRecv* >( m_item->transfer() );
                transfer->setFileURL( KUrl( url ) );
                updateView();
            }
        }

    }
}

#include "transferdetailedinfopanel.moc"