/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  begin:     Mit Aug 7 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/
// Copyright (C) 2004-2007 Shintaro Matsuoka <shin@shoegazed.org>

#ifndef TRANSFERPANEL_H
#define TRANSFERPANEL_H

#include "chatwindow.h"
#include "transferpanelitem.h"

class QPushButton;

class K3ListView;
class KMenu;

namespace Konversation
{
    namespace DCC
    {
        class TransferDetailedInfoPanel;

        class TransferPanel : public ChatWindow
        {
            Q_OBJECT

            public:
                class Column
                {
                    public:
                        enum Object
                        {
                            TypeIcon,
                            OfferDate,
                            Status,
                            FileName,
                            PartnerNick,
                            Progress,
                            Position,
                            TimeLeft,
                            CurrentSpeed,
                            SenderAddress,
                            COUNT
                        };
                };

                TransferPanel(QWidget* parent);
                ~TransferPanel();

                K3ListView* getListView();

            protected slots:
                void slotNewTransferAdded( Konversation::DCC::Transfer* transfer );
                void slotTransferStatusChanged();

                void acceptDcc();
                void abortDcc();
                void resendFile();
                void clearDcc();
                void runDcc();
                void openLocation();
                void showFileInfo();
                void selectAll();
                void selectAllCompleted();

                void popupRequested(Q3ListViewItem* item,const QPoint& pos,int col);
                void popupActivated( QAction *);

                void doubleClicked(Q3ListViewItem* _item,const QPoint& _pos,int _col);

                void updateButton();

                void setDetailPanelItem(Q3ListViewItem* item_);

            protected:
                /** Called from ChatWindow adjustFocus */
                virtual void childAdjustFocus();

                void initGUI();

                K3ListView* m_listView;
                KMenu* m_popup;

                TransferDetailedInfoPanel* m_detailPanel;

                QPushButton* m_buttonAccept;
                QPushButton* m_buttonAbort;
                QPushButton* m_buttonClear;
                QPushButton* m_buttonOpen;
                QPushButton* m_buttonOpenLocation;
                QPushButton* m_buttonDetail;
                QAction *m_abort;
                QAction *m_accept;
                QAction *m_clear;
                QAction *m_info;
                QAction *m_open;
                QAction *m_selectAll;
                QAction *m_selectAllCompleted;
                QAction *m_resend;
        };
    }
}

#endif