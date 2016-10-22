#include "parserurl.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParserUrl w;
    w.show();

    return a.exec();
}
