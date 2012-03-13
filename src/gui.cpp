#include "../include/gui.hpp"

static void on_open_image(Gtk::Image *image);

GUI::GUI(int argc, char *argv[]) : kit(argc, argv) {
  main_window = new Gtk::Window(Gtk::WINDOW_TOPLEVEL);
  main_window->set_title("Image Viewer");

  box = new Gtk::Box();
  box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  box->set_spacing(5);

  image = new Gtk::Image();
  box->pack_start(*image, true, true);

  button = new Gtk::Button("Open Image...");
  box->pack_start(*button, false, false);

  main_window->add(*box);
}

void GUI::createMainWindow() {
  //connecting button clicked signal to function
  button->signal_clicked().connect(sigc::bind<Gtk::Image*>(sigc::ptr_fun(&on_open_image), image));

  //showing widgets
  main_window->show_all();
  main_window->maximize();
  if(main_window) kit.run(*main_window);
}

static void on_open_image(Gtk::Image *image) {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
  Gdk::Rectangle rectangle;

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
      //image->set(dialog.get_filename());
      //pixbuf = image->get_pixbuf();
      pixbuf = Gdk::Pixbuf::create_from_file(dialog.get_filename());
      rectangle = image->get_allocation();
      //std::cout << rectangle.get_width() << " " << rectangle.get_height() << std::endl;
      //pixbuf->scale_simple(rectangle.get_width(), rectangle.get_height(), Gdk::INTERP_BILINEAR);
      pixbuf->scale_simple(500, 500, Gdk::INTERP_BILINEAR);
      image->set(pixbuf);
      break;
    default:
      break;
  }

  //chowamy okno
  dialog.hide();
}
