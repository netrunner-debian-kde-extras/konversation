/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006 Dario Abatianni <eisfuchs@tigress.com>
*/

#include "nicksonlineitem.h"

#include <QHeaderView>

NicksOnlineItem::NicksOnlineItem(int type, QTreeWidget* parent, const QString& name, const QString& col2) :
                 QTreeWidgetItem(parent)
{
  setText(0, name);
  setText(1, col2);
  m_type=type;
  m_connectionId = 0;
  setOffline(false);
}

NicksOnlineItem::NicksOnlineItem(int type, QTreeWidgetItem* parent, const QString& name, const QString& col2) :
                 QTreeWidgetItem(parent)
{
  setText(0, name);
  setText(1, col2);
  m_type=type;
  m_connectionId = 0;
  setOffline(false);
}

bool NicksOnlineItem::operator<(const QTreeWidgetItem &item) const
{
  // make sure that offline items are always at the bottom
  QVariant itemState = item.data(0, Qt::UserRole);
  if (itemState.type() == QVariant::Bool)
  {
    bool itemOffline = itemState.toBool();
    // we are online but other item is offline
    if (!this->isOffline() && itemOffline)
      return treeWidget()->header()->sortIndicatorOrder() == Qt::AscendingOrder;
    // we are offline but other item is online
    if (this->isOffline() && !itemOffline)
      return treeWidget()->header()->sortIndicatorOrder() != Qt::AscendingOrder;
  }
  // otherwise compare items case-insensitively
  return text(treeWidget()->sortColumn()).toLower() < item.text(treeWidget()->sortColumn()).toLower();
}

/**
 * Returns the type of the item.
 * @return                  One of the enum NickListViewColumn
 */
int NicksOnlineItem::type() const
{
  return m_type;
}
