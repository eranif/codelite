#include "mainwindow.h"
#include <gtkmm/application.h>

int main (int argc, char *argv[])
{
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  MainWindow mainwindow;

  //Shows the window and returns when it is closed.
  return app->run(mainwindow);
}