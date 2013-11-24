/*
   DrawPile - a collaborative drawing program.

   Copyright (C) 2013 Calle Laakkonen

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/
#ifndef DP_NET_CLIENT_H
#define DP_NET_CLIENT_H

#include <QObject>

#include "core/point.h"
#include "../shared/net/message.h"

namespace dpcore {
	class Point;
}

namespace drawingboard {
	class ToolContext;
}

namespace protocol {
	class SnapshotMode;
	class Chat;
	class UserJoin;
	class UserAttr;
	class UserLeave;
}

namespace net {
	
class Server;
class LoopbackServer;
class LoginHandler;
class UserListModel;

/**
 * The client for accessing the drawing server.
 */
class Client : public QObject {
Q_OBJECT
public:
	Client(QObject *parent=0);
	~Client();

	/**
	 * @brief Connect to a remote server
	 * @param loginhandler the login handler to use
	 */
	void connectToServer(LoginHandler *loginhandler);

	/**
	 * @brief Disconnect from the remote server
	 */
	void disconnectFromServer();

	/**
	 * @brief Get the local user's user/context ID
	 * @return user ID
	 */
	int myId() const { return _my_id; }

	/**
	 * @brief Is the client connected to a local server?
	 *
	 * A local server is one that is running on this computer
	 * and thus has minimum latency.
	 * @return true if server is local
	 */
	bool isLocalServer() const;

	/**
	 * @brief Is the client connected by network?
	 * @return true if a network connection is open
	 */
	bool isConnected() const { return !_isloopback; }

	/**
	 * @brief Is the user connected and logged in?
	 * @return true if there is an active network connection and login process has completed
	 */
	bool isLoggedIn() const;

	/**
	 * @brief Is the currently logged in user a session operator?
	 *
	 * This is always true in local mode.
	 * @return true
	 */
	bool isOperator() const { return _isloopback || _isOp; }

	/**
	 * @brief Get the user list
	 * @return user list model
	 */
	UserListModel *userlist() const { return _userlist; }

	//! Reinitialize after clearing out the old board
	void init();

	// Layer changing
	void sendCanvasResize(const QSize &newsize);
	void sendNewLayer(int id, const QColor &fill, const QString &title);
	void sendLayerAttribs(int id, float opacity, const QString &title);
	void sendLayerReorder(const QList<uint8_t> &ids);
	void sendDeleteLayer(int id, bool merge);

	// Drawing
	void sendToolChange(const drawingboard::ToolContext &ctx);
	void sendStroke(const dpcore::Point &point);
	void sendStroke(const dpcore::PointVector &points);
	void sendPenup();
	void sendImage(int layer, int x, int y, const QImage &image, bool blend);

	// Annotations
	void sendAnnotationCreate(int id, const QRect &rect);
	void sendAnnotationReshape(int id, const QRect &rect);
	void sendAnnotationEdit(int id, const QColor &bg, const QString &text);
	void sendAnnotationDelete(int id);

	// Snapshot	
	void sendLocalInit(const QList<protocol::MessagePtr> commands);

	// Operator commands
	void sendLockUser(int userid, bool lock);
	void sendKickUser(int userid);

public slots:
	void sendSnapshot(const QList<protocol::MessagePtr> commands);
	void sendChat(const QString &message);

signals:
	void drawingCommandReceived(protocol::MessagePtr msg);
	void chatMessageReceived(const QString &user, const QString &message);
	void needSnapshot();

	void serverConnected(const QString &address);
	void serverLoggedin(bool join);
	void serverDisconnected(const QString &message);

	//! Emitted when current user's operator priviles are granted or revoked
	void opPrivilegeChange(bool op);

private slots:
	void handleMessage(protocol::MessagePtr msg);
	void handleConnect(int userid, bool join);
	void handleDisconnect(const QString &message);

private:
	void handleSnapshotRequest(const protocol::SnapshotMode &msg);
	void handleChatMessage(const protocol::Chat &msg);
	void handleUserJoin(const protocol::UserJoin &msg);
	void handleUserAttr(const protocol::UserAttr &msg);
	void handleUserLeave(const protocol::UserLeave &msg);

	Server *_server;
	LoopbackServer *_loopback;
	
	int _my_id;
	bool _isloopback;
	bool _isOp;
	UserListModel *_userlist;
};

}

#endif
