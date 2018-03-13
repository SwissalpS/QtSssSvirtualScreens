#include "MainWindow.h"
#include <QApplication>

int main(int iArgumentCount, char *aArguments[]) {

	QApplication oApp(iArgumentCount, aArguments);

	MainWindow oMainWindow;
	oMainWindow.show();

	return oApp.exec();

} // main
