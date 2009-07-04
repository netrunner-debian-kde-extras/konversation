/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006 Dario Abatianni <eisfuchs@tigress.com>
*/

#ifndef NICKSONLINEITEM_H
#define NICKSONLINEITEM_H

#include <K3ListView>


class NicksOnlineItem : public K3ListViewItem
{
    public:
        enum NickListViewColumn
        {
            NetworkRootItem=0,  // TODO: not used yet
            NicknameItem=1,     // TODO: not used yet
            ChannelItem=2,      // TODO: not used yet
            OfflineItem=3       // this item is the "Offline" item
        };

        NicksOnlineItem(int type,
                        Q3ListView* parent,
                        const QString& name,
                        const QString& col2 = QString());

        NicksOnlineItem(int type,
                        Q3ListViewItem* parent,
                        const QString& name,
                        const QString& col2 = QString());

        /**
        * Reimplemented to make sure, "Offline" items always get sorted to the bottom of the list
        * @param i                 Pointer to the QListViewItem to compare with.
        * @param col               The column to compare
        * @param ascending         Specify sorting direction
        * @return                  -1 if this item's value is smaller than i, 0 if they are equal, 1 if it's greater
        */
        virtual int compare(Q3ListViewItem* i,int col,bool ascending) const;

        /**
        * Returns the type of the item.
        * @return                  One of the enum NickListViewColumn
        */
        int type() const;

        /// Set the connection ID this item is associated with to @p id.
        void setConnectionId(int id) { m_connectionId = id; }
        /// Returns the connection ID this item is associated with.
        int connectionId() const { return m_connectionId; }

        /// Set the nick's offline state as @p state
        void setOffline (bool state) { m_offline = state; }
        /// Returns true if the nick is currently offline.
        bool isOffline () const { return m_offline; }

    protected:
        int m_type;
        int m_connectionId;
        bool m_offline;
};

#endif
