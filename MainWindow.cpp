#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringListModel>



MainWindow::MainWindow(QWidget *pParent) :
	QMainWindow(pParent),
	pUI(new Ui::MainWindow) {

	pUI->setupUi(this);

	this->checkDependencies();

	// prepare list of processes
	this->apProcesses.clear();

	// deduce first screen number to use
	this->deduceBaseScreenID();

	// adding command to awesome did not work, so hide this
	// running it after in a sub process did not work either
	// maybe if it wasn't the Process that ran it?
	this->pUI->comboBoxWM->setHidden(true);

	// did not work so, hide for now
	this->pUI->checkBoxKill->setHidden(true);

	// init history
	this->initHistory();

} // construct


MainWindow::~MainWindow() {

	delete pUI;

} // dealloc


void MainWindow::changeEvent(QEvent *pEvent) {

	QMainWindow::changeEvent(pEvent);

	switch (pEvent->type()) {

		case QEvent::LanguageChange:
			pUI->retranslateUi(this);
		break;
		default:
		break;

	} // switch type

} // changeEvent


void MainWindow::checkDependencies() {

	// check for Xephyr
	QProcess *pProcess = new QProcess(this);
	pProcess->start("which", QStringList() << "Xephyr");
	pProcess->waitForFinished();
	if (pProcess->exitStatus() + pProcess->exitCode()) {
		// something went wrong
		this->onDebugMessage(tr("KO:Xephyr not found. This application won't work."));
	} else {
		this->onDebugMessage(tr("OK:Xephyr found."));
	} // if found Xephyr or not

} // checkDependencies


void MainWindow::closeEvent(QCloseEvent *pEvent) {

	// kill processes?
	if (this->pUI->checkBoxKill->isChecked()) {

		// does not seem to work this way
		Xprocess *pProcess;
		foreach (pProcess, this->apProcesses) pProcess->kill();

	} // if kill processes

	QMainWindow::closeEvent(pEvent);

} // closeEvent


void MainWindow::deduceBaseScreenID() {

	// scan for a base number to start at
	const QString sTempFile = QStringLiteral("/tmp/QtSssSvirtualScreens.test.txt");
	const QString sCommand = "ps xeww | grep -Po ' DISPLAY=[\\.0-9A-Za-z:]* ' | sort -u > " + sTempFile + ";exit 0;\n";

	QProcess *pProcess = new QProcess(this);
	pProcess->start(getenv("SHELL"));
	pProcess->waitForStarted();
	pProcess->write(sCommand.toUtf8());

	pProcess->waitForFinished();

	QFile oF(sTempFile);
	if (!oF.exists()) {

		this->onDebugMessage("file not here");

		// give it our best shot
		this->ubDisplay = 7;

	} else {

		oF.open(QIODevice::ReadOnly);
		QString sOut = oF.readAll().trimmed();
		oF.close();
		oF.remove();

		//this->onDebugMessage("<" + sOut + ">");

		this->ubDisplay = 1 + sOut.split(10).last().split(':').last().toUInt();

	} // if file exists or not

	// destroy process
	pProcess->close();
	pProcess->deleteLater();

	this->onDebugMessage("starting with DISPLAY=:"
						 + QString::number(this->ubDisplay));

} // deduceBaseScreenID


void MainWindow::initHistory() {

	QFile oFile(this->historyPathFile());
	if (oFile.open(QIODevice::ReadOnly)) {

		this->oJdoc = QJsonDocument::fromJson(oFile.readAll());
		oFile.close();

	} else {

		this->oJdoc = QJsonDocument();
		QJsonObject oJo;
		this->oJdoc.setObject(oJo);

	} // if able to open or not

	this->pCcommand = new QCompleter(this);
	this->pCheight = new QCompleter(this);
	this->pCwidth = new QCompleter(this);
	this->pCwindowManager = new QCompleter(this);

	this->pCcommand->setCaseSensitivity(Qt::CaseInsensitive);
	this->pCheight->setCaseSensitivity(Qt::CaseInsensitive);
	this->pCwidth->setCaseSensitivity(Qt::CaseInsensitive);
	this->pCwindowManager->setCaseSensitivity(Qt::CaseInsensitive);

	this->pCcommand->setFilterMode(Qt::MatchContains);
	this->pCheight->setFilterMode(Qt::MatchContains);
	this->pCwidth->setFilterMode(Qt::MatchContains);
	this->pCwindowManager->setFilterMode(Qt::MatchContains);

	this->updateCompleters();

	this->pUI->comboBoxCommand->setCompleter(this->pCcommand);
	this->pUI->comboBoxWM->setCompleter(this->pCwindowManager);
	this->pUI->lineEditHeight->setCompleter(this->pCheight);
	this->pUI->lineWidth->setCompleter(this->pCwidth);

} // initHistory


void MainWindow::on_buttonAddScreen_clicked() {

	Xprocess *pProcess = new Xprocess(this->apProcesses.count(), this);
	this->apProcesses.append(pProcess);

	connect(pProcess, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(pProcess, SIGNAL(done(quint8)),
			this, SLOT(onProcessDone(quint8)));

	connect(pProcess, SIGNAL(error(QProcess::ProcessError,quint8)),
			this, SLOT(onProcessError(QProcess::ProcessError,quint8)));

	QString sWidth = this->pUI->lineWidth->text();
	QString sHeight = this->pUI->lineEditHeight->text();
	if (sWidth.contains("x")) {
		QStringList aList = sWidth.split("x");
		sWidth = aList.first();
		sHeight = aList.last();
	} // if got both dimensions in width

	this->pUI->listWidgetScreens->addItem(
				QString::number(pProcess->processIndex())
				+ " " + sWidth + "x" + sHeight
				+ " " + QString::number(this->ubDisplay));

	const QString sWM = (0 == this->pUI->comboBoxWM->currentIndex())
				  ? "" : this->pUI->comboBoxWM->currentText();
	QString sC = sWM;
	const QString sCommand = (this->pUI->comboBoxCommand->currentText().isEmpty())
							 ? "" : this->pUI->comboBoxCommand->currentText();
	sC += (sCommand.isEmpty()) ? "" : " " + sCommand;

	pProcess->startVirtualWindow(this->ubDisplay++, sWidth.toUInt(),
								 sHeight.toUInt(), sC);

	bool bChanged = false;
	QJsonObject oJo = this->oJdoc.object();
	QJsonArray oJa;
	int iValue;

	iValue = sWidth.toInt();
	if (0 < iValue) {
		oJa	= oJo.value("width").toArray();
		if (!oJa.contains(iValue)) {
			oJa.append(iValue);
			oJo.insert("width", oJa);
			bChanged = true;
		} // if new width
	} // if got something

	iValue = sHeight.toInt();
	if (0 < iValue) {
		oJa = oJo.value("height").toArray();
		if (!oJa.contains(iValue)) {
			oJa.append(iValue);
			oJo.insert("height", oJa);
			bChanged = true;
		} // if new height
	} // if got something


	if (!sWM.isEmpty()) {

		oJa = oJo.value("windowManager").toArray();
		if (!oJa.contains(sWM)) {
			oJa.append(sWM);
			oJo.insert("windowManager", oJa);
			bChanged = true;
		} // if new

	} // if not empty

	if (!sCommand.isEmpty()) {

		oJa = oJo.value("command").toArray();
		if (!oJa.contains(sCommand)) {
			oJa.append(sCommand);
			oJo.insert("command", oJa);
			bChanged = true;
		} // if new

	} // if not empty

	if (bChanged) {

		this->oJdoc.setObject(oJo);

		this->saveHistory();

		this->updateCompleters();

	} // if changed

} // on_buttonAddScreen_clicked


void MainWindow::onProcessDone(const quint8 ubIndex) {

	this->pUI->listWidgetScreens->item(ubIndex)->setHidden(true);

} // onProcessDone


void MainWindow::onProcessError(const QProcess::ProcessError iError,
								const quint8 ubIndex) {

	this->onDebugMessage("KO:" + QString::number(ubIndex)
						 + ":" + QString::number(iError) );

} // onProcessError


void MainWindow::saveHistory() {

	QByteArray oData = this->oJdoc.toJson();

	QFile oFile(this->historyPathFile());
	if (oFile.open(QIODevice::WriteOnly)) {

		oFile.write(oData);
		oFile.close();

	} else {

		this->onDebugMessage(tr("KO:Unable to write history file."));

	} // if can open for writing or not

} // saveHistory


void MainWindow::updateCompleters() {

	QJsonArray oJa;
	QJsonValue oJv;
	QStringList oList;

	oList.clear();
	oJa = this->oJdoc.object().value("command").toArray();
	foreach (oJv, oJa) oList << oJv.toString();
	this->pCcommand->setModel(new QStringListModel(oList));

	oList.clear();
	oJa = this->oJdoc.object().value("height").toArray();
	foreach (oJv, oJa) oList << oJv.toString();
	this->pCheight->setModel(new QStringListModel(oList));

	oList.clear();
	oJa = this->oJdoc.object().value("width").toArray();
	foreach (oJv, oJa) oList << oJv.toString();
	if (oList.isEmpty()) {
		oList << "1366x768" << "1920x1080" << "1440x900" << "1600x900"
			  << "1280x800" << "1024x768" << "1280x1024" << "1536x864"
			  << "1680x1050" << "1280x720" << "1360x768" << "2560x1440"
			  << "1920x1200" << "1280x768" << "1024x600" << "1152x864"
			  << "800x600" << "2560x1080" << "3440x1440" << "3840x2160";

		QString sValue;
		foreach (sValue, oList) oJa.append(sValue);
		QJsonObject oJo = this->oJdoc.object();
		oJo.insert("width", oJa);
		this->oJdoc.setObject(oJo);
		this->saveHistory();

	} // if none set yet
	this->pCwidth->setModel(new QStringListModel(oList));

	oList.clear();
	oJa = this->oJdoc.object().value("windowManager").toArray();
	foreach (oJv, oJa) oList << oJv.toString();
	this->pCwindowManager->setModel(new QStringListModel(oList));

} // updateCompleters
