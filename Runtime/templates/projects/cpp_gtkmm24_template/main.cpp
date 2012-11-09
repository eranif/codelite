#include "mainwindow.h"
#include <gtkmm/main.h>

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    MainWindow mainwindow;
    //Shows the window and returns when it is closed.
    Gtk::Main::run(mainwindow);

    return 0;
}
