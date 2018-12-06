#ifndef TCPGATEDIALOG_H
#define TCPGATEDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QTcpSocket>
#include <QTcpServer>

class QLineEdit;
class QPushButton;
class QGroupBox;
class QLCDNumber;
class QSpinBox;
class QComboBox;

namespace Ui {
class TcpGateDialog;
}


typedef struct
{
	QTcpServer* tcpServer;
	QTcpSocket* tcpSocket;	
	int			nSendBytes;
	int			nRecvBytes;
	bool		bConnected;
	bool		bStarted;
	QStringList latestIpList;
	
	//ui
	QGroupBox*  group;
	QComboBox*	cbIp;
	QSpinBox*	spinPort;
	QLCDNumber* lnSend;
	QLCDNumber* lnRecv;
	QLineEdit*	leStatus;
	QPushButton* btnStart;
}NetData_t;



class TcpGateDialog : public QDialog
{
    Q_OBJECT

public:
	explicit TcpGateDialog(QDialog *parent = 0);
	~TcpGateDialog();
public:
private:
	NetData_t  m_ndListen;
	NetData_t  m_ndConnect;
	QTimer	   m_timer;
	
	
	Ui::TcpGateDialog *ui;	
public:
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
private:
	void InitListen();
	void InitConnect();
	NetData_t* GetNetData(QTcpSocket* tcpSocket);
	void disconnected(QTcpSocket* tcpSocket);
	
public:
	void SaveSetting();
	void Log2Main(QString strLog);
public slots:
	void onClkBtnListen();
	void onClkBtnConnect();
	void onClkBtnClear();
	void onClkBtnAbout();
	void onClkBtnHome();
	void onTimer();
private slots:
	void onNewConnection();
	void onConnected();
	void onDisConnected();
	void onRead();
	void onError(QAbstractSocket::SocketError socketError);
};

#endif // TCPGATEDIALOG_H
