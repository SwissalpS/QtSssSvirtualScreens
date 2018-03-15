#ifndef TRANSPARENTWINDOW_H
#define TRANSPARENTWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QRubberBand>


class TransparentWindow : public QMainWindow {

	Q_OBJECT

protected:
	bool bAmDragging;
	QPoint oPointA;
	QPoint oPointB;
	QRubberBand *pRB;

protected slots:
	void mouseMoveEvent(QMouseEvent *pEvent);
	void mousePressEvent(QMouseEvent *pEvent);
	void mouseReleaseEvent(QMouseEvent *pEvent);

public:
	explicit TransparentWindow(QWidget *pParent = nullptr);

signals:
	void gotRect(QRect oRect) const;

public slots:

}; // TransparentWindow

#endif // TRANSPARENTWINDOW_H
