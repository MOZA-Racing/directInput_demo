#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w. initDirectApi();                   // initialization directInput8 api object
    w. findR16();                         // find target device， example Moza R16
    w. createR16Device();                 // create after find
    if(w. r16DevicePrepare())             // preparatory work
    {
        w. beginEffectTest();             // create constant force and send it to Control Moza R16
    }


    return a.exec();
}
