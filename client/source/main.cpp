#include <QApplication>
#include <QLabel>

int main(int argc, char** argv)
{
    QApplication a(argc, argv);

    QLabel label("Hi");
    label.show();


    return a.exec();
}