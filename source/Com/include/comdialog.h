#ifndef COMDIALOG_H
#define COMDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QKeyEvent>
#include "qextserialport.h"

class QLineEdit;
class QPushButton;

namespace Ui {
class ComDialog;
}

typedef struct
{
	QLineEdit* leSend;
	QPushButton* btnSend;
}SendGroup_t;
#define MAX_SENDGROUP 3

class ComDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ComDialog(QDialog *parent = 0);
	~ComDialog();
public:
private:
	Ui::ComDialog *ui;	
	QextSerialPort* serialPort;
	QTimer m_timer;
	int	 m_nSendBytes;
	int  m_nRecvBytes;
	bool m_bAutoSend;
	quint32  m_nAutoSendCount;
	int  m_nAutoSendPos;
	SendGroup_t m_sg[MAX_SENDGROUP];
public:
	virtual void closeEvent(QCloseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
private:
	void Init();
	void ShowHead();
	void ShowInfo();
	void onOpen();
	void onClose();
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
};

#endif // COMDIALOG_H
