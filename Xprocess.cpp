#include "Xprocess.h"



Xprocess::Xprocess(const quint8 ubProcessIndex, QObject *pParent) :
	QProcess(pParent),
	ubID(ubProcessIndex),
	ubD(0),
	ulW(0),
	ulH(0),
	sCommandLine(""),
	sID(QString::number(ubProcessIndex)) {

} // construct


Xprocess::~Xprocess() {} // dealloc


void Xprocess::onError(QProcess::ProcessError iError) {

	//this->onDebugMessage("error");

	Q_EMIT this->error(iError, this->ubID);

} // onError


void Xprocess::onFinished(int iExitCode, QProcess::ExitStatus iExitStatus) {
	Q_UNUSED(iExitCode)
	Q_UNUSED(iExitStatus)

	//this->onDebugMessage("finished");

	Q_EMIT this->done(//iExitCode, iExitStatus,
					  this->ubID);

} // onFinished


void Xprocess::onStarted() {

	//this->onDebugMessage("started");

	// Xephyr :6 -ac -screen 1000x700 & sleep 1; DISPLAY=:6 awesome
	QString sD = QString::number(this->ubD);
	QString sC = "Xephyr :" + sD + " -ac -screen "
				 + QString::number(this->ulW) + "x"
				 + QString::number(this->ulH) + " & sleep 1; DISPLAY=:"
				 + sD + " " + this->sCommandLine + ";exit 0;\n";

	//this->onDebugMessage(sC);
	this->write(sC.toUtf8());

//	this->waitForBytesWritten();
////	this->write(QString("DISPLAY=:" + sD + " vlc;\n").toUtf8());
//	QProcess *pSub = new QProcess();
//	pSub->start(QString("DISPLAY=:" + sD + " vlc;\n").toUtf8());

} // onStarted


void Xprocess::startVirtualWindow(const quint8 ubDisplay,
								 const quint32 ulWidth,
								 const quint32 ulHeight,
								 //const QString sWindowManager,
								 const QString sCommand) {

	this->sCommandLine = sCommand;
	this->ulH = ulHeight;
	this->ulW = ulWidth;
	this->ubD = ubDisplay;

	this->setProgram(getenv("SHELL"));

	connect(this, SIGNAL(error(QProcess::ProcessError)),
			this, SLOT(onError(QProcess::ProcessError)));

	connect(this, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(onFinished(int,QProcess::ExitStatus)));

	connect(this, SIGNAL(started()),
			this, SLOT(onStarted()));

	this->start();

} // startVirtualWindow


