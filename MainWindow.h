#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QCompleter>
#include <QDir>
#include <QJsonDocument>
#include <QMainWindow>

#include "Xprocess.h"
#include "TransparentWindow.h"



namespace Ui {



class MainWindow;



}



class MainWindow : public QMainWindow {

	Q_OBJECT

private:
	Ui::MainWindow *pUI;

private slots:
	void on_buttonAddScreen_clicked();
	void on_buttonMouseRect_clicked();

protected:
	quint8 ubDisplay;
	QCompleter *pCcommand;
	QCompleter *pCheight;
	QCompleter *pCwidth;
	QCompleter *pCwindowManager;
	TransparentWindow *pTW;
	QJsonDocument oJdoc;
	QList<Xprocess *> apProcesses;

	void changeEvent(QEvent *pEvent);
	void checkDependencies();
	void closeEvent(QCloseEvent *pEvent);
	void deduceBaseScreenID();
	void initHistory();
	void updateCompleters();
	void saveHistory();

protected slots:
	void onProcessDone(const quint8 ubIndex);
	void onProcessError(const QProcess::ProcessError iError,
						const quint8 ubIndex);
	void onGotRect(const QRect oRect);

public:
	explicit MainWindow(QWidget *pParent = 0);
	~MainWindow();

	inline QString historyPathFile() const {
		return QDir::homePath() + "/.QtSssSvirtualScreens.json"; }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		std::cout << "M:" << sMessage.toStdString() << std::endl;
		Q_EMIT this->debugMessage("M:" + sMessage); }

}; // MainWindow

#endif // MAINWINDOW_H
