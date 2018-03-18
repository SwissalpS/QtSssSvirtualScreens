#include "ListWidgetItem.h"
#include "ui_ListWidgetItem.h"

#include <QComboBox>
#include <QLineEdit>
#include <QProcess>
#include <QTimer>
#include <iostream>

ListWidgetItem::ListWidgetItem(const QString sDisplay, const QString sLabel,
							   QWidget *pParent) :
	QWidget(pParent),
	pUI(new Ui::ListWidgetItem),
	bHaveDetectedResolutions(false),
	sDisplay(sDisplay) {

	this->pUI->setupUi(this);

	this->pUI->label->setText(sLabel);

	QTimer::singleShot(1000, this, SLOT(fetchAvailableResolutions()));

} // construct


ListWidgetItem::~ListWidgetItem() {

	delete this->pUI;

} // dealloc


void ListWidgetItem::changeEvent(QEvent *pEvent) {

	QWidget::changeEvent(pEvent);

	switch (pEvent->type()) {

		case QEvent::LanguageChange:
			pUI->retranslateUi(this);
		break;

		default:
		break;

	} // switch

} // changeEvent


void ListWidgetItem::fetchAvailableResolutions() {

	// get available resolutions
	QStringList aResolutions;
	const QString sCommand = "DISPLAY=" + this->sDisplay + " xrandr;exit 0;\n";
	QString sOut;
	QStringList aLines;

	QProcess *pProcess = new QProcess(this);
	pProcess->start(getenv("SHELL"));
	if (pProcess->waitForStarted()) {
		pProcess->write(sCommand.toUtf8());
		pProcess->waitForFinished();
		if (0 == pProcess->exitCode() + pProcess->exitStatus()) {

			sOut = pProcess->readAll().trimmed();
			aLines = sOut.split('\n');

		} // if ended OK

	} // if started OK
	if (pProcess) delete pProcess;

	// drop first two lines for now
	if (aLines.count()) aLines.removeAt(0);
	if (aLines.count()) aLines.removeAt(0);

	// the rest should be available resolutions
	int iCurrent = 0;
	int iCount = 0;
	QString sLine;
	QStringList aParts;
	for(; iCount < aLines.count(); ++iCount) {

		sLine = aLines.at(iCount);
		aParts = sLine.split(' ', QString::SkipEmptyParts);

		// some resolutions have multiple refresh rates, so this does not work
		//if (2 != aParts.count()) continue;

		// sometimes there are more lines after the last available resolution
		// this is a dirty fix
		if (!aParts.at(0).contains('x')) continue;

		// add to our list
		aResolutions.append(aParts.at(0));

		// get current value
		if (sLine.contains('*')) iCurrent = iCount;

	} // loop

	this->pUI->comboBox->addItems(aResolutions);
	this->pUI->comboBox->setCurrentIndex(iCurrent);
	this->bHaveDetectedResolutions = true;

} // fetchAvailableResolutions


void ListWidgetItem::on_comboBox_currentIndexChanged(int iIndex) {
	Q_UNUSED(iIndex)

	if (!this->bHaveDetectedResolutions) return;

	const QString sCommand = "DISPLAY=" + this->sDisplay + " xrandr -s "
							 + this->pUI->comboBox->currentText() + ";exit 0;\n";

	QProcess *pProcess = new QProcess(this);
	pProcess->start(getenv("SHELL"));
	if (pProcess->waitForStarted()) {
		pProcess->write(sCommand.toUtf8());
		pProcess->waitForFinished();
	} // if started OK
	if (pProcess) delete pProcess;

} // on_comboBox_currentIndexChanged
