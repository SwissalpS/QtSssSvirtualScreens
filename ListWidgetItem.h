#ifndef LISTWIDGETITEM_H
#define LISTWIDGETITEM_H

#include <QWidget>



namespace Ui {



class ListWidgetItem;



}


class QObject;
class ListWidgetItem : public QWidget {

	Q_OBJECT

private:
	Ui::ListWidgetItem *pUI;

private slots:
	void on_comboBox_currentIndexChanged(int iIndex);

protected:
	bool bHaveDetectedResolutions;
	QString sDisplay;
	void changeEvent(QEvent *pEvent);

protected slots:
	void fetchAvailableResolutions();

public:
	explicit ListWidgetItem(const QString sDisplay, const QString sLabel,
							QWidget *pParent = 0);
	~ListWidgetItem();

}; // ListWidgetItem

#endif // LISTWIDGETITEM_H
