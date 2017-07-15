/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <memory>
#include <deque>

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <QtCore/QJsonObject>

#include "jobs/syncjob.h"
#include "joinstate.h"

namespace QMatrixClient
{
    class Event;
    class Connection;
    class User;
    class MemberSorter;

    class TimelineItem
    {
        public:
            // For compatibility with Qt containers, even though we use
            // a std:: container now
            using index_t = int;

            TimelineItem(RoomEvent* e, index_t number) : evt(e), idx(number) { }

            RoomEvent* event() const { return evt.get(); }
            RoomEvent* operator->() const { return event(); } //< Synonym for event()
            index_t index() const { return idx; }

        private:
            std::unique_ptr<RoomEvent> evt;
            index_t idx;
    };
    inline QDebug& operator<<(QDebug& d, const TimelineItem& ti)
    {
        QDebugStateSaver dss(d);
        d.nospace() << "(" << ti.index() << "|" << ti->id() << ")";
        return d;
    }

    class Room: public QObject
    {
            Q_OBJECT
            Q_PROPERTY(const Connection* connection READ connection CONSTANT)
            Q_PROPERTY(QString id READ id CONSTANT)
            Q_PROPERTY(QString name READ name NOTIFY namesChanged)
            Q_PROPERTY(QStringList aliases READ aliases NOTIFY namesChanged)
            Q_PROPERTY(QString canonicalAlias READ canonicalAlias NOTIFY namesChanged)
            Q_PROPERTY(QString displayName READ displayName NOTIFY namesChanged)
            Q_PROPERTY(QString topic READ topic NOTIFY topicChanged)
            Q_PROPERTY(QString readMarkerEventId READ readMarkerEventId WRITE markMessagesAsRead NOTIFY readMarkerMoved)
        public:
            using Timeline = std::deque<TimelineItem>;
            using rev_iter_t = Timeline::const_reverse_iterator;

            Room(Connection* connection, QString id);
            virtual ~Room();

            Connection* connection() const;
            QString id() const;
            QString name() const;
            QStringList aliases() const;
            QString canonicalAlias() const;
            QString displayName() const;
            QString topic() const;
            Q_INVOKABLE JoinState joinState() const;
            Q_INVOKABLE QList<User*> usersTyping() const;
            QList<User*> membersLeft() const;

            Q_INVOKABLE QList<User*> users() const;
            Q_INVOKABLE QStringList memberNames() const;

            /**
             * @brief Produces a disambiguated name for a given user in
             * the context of the room
             */
            Q_INVOKABLE QString roomMembername(User* u) const;
            /**
             * @brief Produces a disambiguated name for a user with this id in
             * the context of the room
             */
            Q_INVOKABLE QString roomMembername(const QString& userId) const;

            void updateData(SyncRoomData&& data );
            Q_INVOKABLE void setJoinState( JoinState state );

            const Timeline& messageEvents() const;
            /**
             * A convenience method returning the read marker to the before-oldest
             * message
             */
            rev_iter_t timelineEdge() const;
            Q_INVOKABLE TimelineItem::index_t minTimelineIndex() const;
            Q_INVOKABLE TimelineItem::index_t maxTimelineIndex() const;
            Q_INVOKABLE bool isValidIndex(TimelineItem::index_t timelineIndex) const;

            rev_iter_t findInTimeline(TimelineItem::index_t index) const;
            rev_iter_t findInTimeline(const QString& evtId) const;

            rev_iter_t readMarker(const User* user) const;
            rev_iter_t readMarker() const;
            QString readMarkerEventId() const;
            /**
             * @brief Mark the event with uptoEventId as read
             *
             * Finds in the timeline and marks as read the event with
             * the specified id; also posts a read receipt to the server either
             * for this message or, if it's from the local user, for
             * the nearest non-local message before. uptoEventId must be non-empty.
             */
            void markMessagesAsRead(QString uptoEventId);

            Q_INVOKABLE bool hasUnreadMessages();

            Q_INVOKABLE int notificationCount() const;
            Q_INVOKABLE void resetNotificationCount();
            Q_INVOKABLE int highlightCount() const;
            Q_INVOKABLE void resetHighlightCount();

            MemberSorter memberSorter() const;

        public slots:
            void postMessage(const QString& type, const QString& plainText);
            void postMessage(const QString& type, const QString& plainText,
                             const QString& richText);
            void getPreviousContent(int limit = 10);

            void leaveRoom() const;
            void userRenamed(User* user, QString oldName);

            /** Mark all messages in the room as read */
            void markAllMessagesAsRead();

        signals:
            void aboutToAddHistoricalMessages(const RoomEvents& events);
            void aboutToAddNewMessages(const RoomEvents& events);
            void addedMessages();

            /**
             * @brief The room name, the canonical alias or other aliases changed
             *
             * Not triggered when displayname changes.
             */
            void namesChanged(Room* room);
            /** @brief The room displayname changed */
            void displaynameChanged(Room* room);
            void topicChanged();
            void userAdded(User* user);
            void userRemoved(User* user);
            void memberRenamed(User* user);
            void joinStateChanged(JoinState oldState, JoinState newState);
            void typingChanged();
            void highlightCountChanged(Room* room);
            void notificationCountChanged(Room* room);
            void lastReadEventChanged(User* user);
            void readMarkerMoved();
            void unreadMessagesChanged(Room* room);

        protected:
            virtual void doAddNewMessageEvents(const RoomEvents& events);
            virtual void doAddHistoricalMessageEvents(const RoomEvents& events);
            virtual void processStateEvents(const RoomEvents& events);
            virtual void processEphemeralEvent(Event* event);

        private:
            class Private;
            Private* d;

            void addNewMessageEvents(RoomEvents events);
            void addHistoricalMessageEvents(RoomEvents events);

            void markMessagesAsRead(rev_iter_t upToMarker);
    };

    class MemberSorter
    {
        public:
            explicit MemberSorter(const Room* r) : room(r) { }

            bool operator()(User* u1, User* u2) const;

            template <typename ContT>
            typename ContT::size_type lowerBoundIndex(const ContT& c,
                                                      typename ContT::value_type v) const
            {
                return  std::lower_bound(c.begin(), c.end(), v, *this) - c.begin();
            }

        private:
            const Room* room;
    };
}  // namespace QMatrixClient
