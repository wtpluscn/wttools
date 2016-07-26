#ifndef TCPSRVDIALOG_H
#define TCPSRVDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QTcpSocket>
#include <QTcpServer>

class QLineEdit;
class QPushButton;

namespace Ui {
class TcpSrvDialog;
}

typedef struct
{
	QLineEdit* leSend;
	QPushButton* btnSend;
}SendGroup_t;
#define MAX_SENDGROUP 3

class TcpSrvDialog : public QDialog
{
    Q_OBJECT

public:
	explicit TcpSrvDialog(QDialog *parent = 0);
	~TcpSrvDialog();
public:
private:
	Ui::TcpSrvDialog *ui;	
	QTcpServer *tcpServer;
	QTcpSocket *tcpSocket;
	QTimer m_timer;
	int	 m_nSendBytes;
	int  m_nRecvBytes;
	bool m_bAutoSend;
	bool m_bConnected;
	bool m_bListened;
	quint32  m_nAutoSendCount;
	int  m_nAutoSendPos;
	SendGroup_t m_sg[MAX_SENDGROUP];
	QStringList m_latestIpList;
public:
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
private:
	void Init();
	void ShowHead();
	void ShowInfo();
	void SendData(QLineEdit* leSend);
	void AutoSend();
public:
	void SaveSetting();
public slots:
	void onClkBtnListen();
	void onClkBtnClear();
	void onClkBtnSend();
	void onClkBtnAutoSend();
	void onClkBtnAbout();
	void onClkBtnHome();
	void onTimer();
private slots:
	void onNewConnection();
	void onDisConnected();
	void onRead();
	void onError(QAbstractSocket::SocketError socketError);
};

#endif // TCPSRVDIALOG_H
