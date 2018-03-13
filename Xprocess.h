#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>



class Xprocess : public QProcess {

	Q_OBJECT

protected:
	quint8 ubID;
	quint8 ubD;
	quint32 ulW;
	quint32 ulH;
	QString sCommandLine;
	QString sID;

protected slots:
	void onError(QProcess::ProcessError iError);
	void onFinished(int iExitCode, QProcess::ExitStatus iExitStatus);
	void onStarted();

public:
	explicit Xprocess(const quint8 ubProcessIndex, QObject *pParent);
	~Xprocess();

	void startVirtualWindow(const quint8 ubDisplay, const quint32 ulWidth,
							const quint32 ulHeight,
							//const QString sWindowManager,
							const QString sCommand);

	inline quint8 processIndex() const { return this->ubID; }

signals:
	void debugMessage(const QString &sMessage) const;
	void done(const quint8 ubIndex) const;
	void error(QProcess::ProcessError iError, const quint8 ubIndex) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("P:" + this->sID + ":" + sMessage); }

}; // Process

#endif // PROCESS_H
