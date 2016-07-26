#ifndef TCPCLIDIALOG_H
#define TCPCLIDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QTcpSocket>

class QLineEdit;
class QPushButton;

namespace Ui {
class TcpCliDialog;
}

typedef struct
{
	QLineEdit* leSend;
	QPushButton* btnSend;
}SendGroup_t;
#define MAX_SENDGROUP 3

class TcpCliDialog : public QDialog
{
    Q_OBJECT

public:
	explicit TcpCliDialog(QDialog *parent = 0);
	~TcpCliDialog();
public:
private:
	Ui::TcpCliDialog *ui;	
	QTcpSocket *tcpSocket;
	QTimer m_timer;
	int	 m_nSendBytes;
	int  m_nRecvBytes;
	bool m_bAutoSend;
	bool m_bConnected;
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
	void onClkBtnOpen();
	void onClkBtnClear();
	void onClkBtnSend();
	void onClkBtnAutoSend();
	void onClkBtnAbout();
	void onClkBtnHome();
	void onTimer();
private slots:
	void onConnected();
	void onDisConnected();
	void onRead();
	void onError(QAbstractSocket::SocketError socketError);
};

#endif // TCPCLIDIALOG_H
