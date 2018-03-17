#include "TransparentWindow.h"
#include <QMouseEvent>



TransparentWindow::TransparentWindow(QWidget *pParent) :
	QMainWindow(pParent) {

	// this did not work with an empty window. Only when at least one widget was added
	//this->setWindowFlags(Qt::FramelessWindowHint);// | Qt::WindowStaysOnTopHint);
	//this->setStyleSheet("background:transparent");
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setMouseTracking(true);

	this->pRB = new QRubberBand(QRubberBand::Rectangle, this);

	this->setCursor(Qt::CrossCursor);

} // construct


void TransparentWindow::mouseMoveEvent(QMouseEvent *pEvent) {

	if (!this->bAmDragging) {

		QMainWindow::mouseMoveEvent(pEvent);

		return;

	} // if not dragging

	this->oPointB = pEvent->pos();

	this->pRB->setGeometry(QRect(this->oPointA, this->oPointB).normalized());

} // mouseMoveEvent


void TransparentWindow::mousePressEvent(QMouseEvent *pEvent) {

	this->oPointA = pEvent->pos();

	this->pRB->setGeometry(QRect(this->oPointA, QSize()));
	this->pRB->show();

	QMainWindow::mousePressEvent(pEvent);

} // mousePressEvent


void TransparentWindow::mouseReleaseEvent(QMouseEvent *pEvent) {

	this->oPointB = pEvent->pos();

	this->pRB->hide();

	Q_EMIT this->gotRect(QRect(this->oPointA, this->oPointB).normalized());

	QMainWindow::mouseReleaseEvent(pEvent);

	this->hide();

} // mouseReleaseEvent
