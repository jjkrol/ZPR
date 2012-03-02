//Kodze sobie prosta przegladarke zdjec z tutoriala dla pocwiczenia.
//Usunalem gtk_test.cpp, bo doczytalem ze korzystalem z 'wersji' GTK
//przeznaczonej dla C, zamiast tej dla C++. Przerobilem SConstruct
//zeby dzialalo z tym plikiem, kompiluje sie teraz tak samo jak
//gtk_test.cpp: poleceniem scons.

//Mozecie sobie skompilowac, juz cos ten programik robi. :-)
//Na razie jest bardzo slabo, bo obrazek sie nawet nie skaluje,
//ale to bedzie nastepna rzecz, jaka ogarne. Pozniej sprobuje zrobic
//przeskakiwanie lewo/prawo. Do kompilacji trzeba miec gtkmm
//w wersji 3.0 na kompie. Kod jest znowu brzydki, ale to nadal testy.
//mc

#include <gtkmm.h>
   
static void on_open_image(Gtk::Image *image) {
  //tworzymy okno wyboru pliku
  Gtk::FileChooserDialog dialog("Open image", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  //filtrowanie obrazow
  Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
  filter->add_pixbuf_formats();
  dialog.add_filter(filter);

  //wylapujemy wcisniecie OK
  switch(dialog.run()) {
    case Gtk::RESPONSE_ACCEPT:
      image->set(dialog.get_filename());
      break;
    default:
      break;
  }

  //chowamy okno
  dialog.hide();
}

int main(int argc, char *argv[]) {
  Gtk::Main kit(argc, argv);

  //tworzenie okna
  Gtk::Window *main_window = new Gtk::Window(Gtk::WINDOW_TOPLEVEL);
  main_window->set_title("Image Viewer");

  Gtk::Box *box = new Gtk::Box();               //podzial okna
  box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  box->set_spacing(5);

  Gtk::Image *image = new Gtk::Image();         //widzet z obrazkiem
  box->pack_start(*image, true, true);

  Gtk::Button *button = new Gtk::Button("Open Image...");      //przycisk
  box->pack_start(*button, false, false);

  main_window->add(*box);

  //sygnal wysylany po wcisnieciu przycisku
  button->signal_clicked().connect(sigc::bind<Gtk::Image*>(sigc::ptr_fun(&on_open_image), image));

  //pokazujemy widzety i okno
  main_window->show_all();
  if(main_window) kit.run(*main_window);

  return 0;
}
