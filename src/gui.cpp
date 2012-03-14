#include "../include/gui.hpp"

static void on_open_image(Gtk::Image *image);
static void on_fit_image(Gtk::Image *image);

//GUI constructor - creates main window and its widgets
GUI::GUI(int argc, char *argv[]) : kit(argc, argv) {
  //creating main window
  main_window = new Gtk::Window(Gtk::WINDOW_TOPLEVEL);
  main_window->set_title("Image Viewer");

  //creating widgets
  image = new Gtk::Image();
  menu = new Gtk::MenuBar();
  library_button = new Gtk::Button("Back to library");
  open_button = new Gtk::Button("Open Image...");
  fit_button = new Gtk::Button("Fit");
  left_button = new Gtk::Button("Left");
  right_button = new Gtk::Button("Right");
  filename_label = new Gtk::Label("place for filename");
  basic_label = new Gtk::Label("Basic editing");
  colors_label = new Gtk::Label("Colors modification");
  effects_label = new Gtk::Label("Other effects");
  notebook = new Gtk::Notebook();

  //editing widgets
  notebook->append_page(*basic_label, "Basic");
  notebook->append_page(*colors_label, "Colors");
  notebook->append_page(*effects_label, "Effects");

  //organising widgets
  bottom_box = new Gtk::Box();
  bottom_box->set_hexpand(true);
  bottom_box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  bottom_box->set_spacing(2);
  bottom_box->pack_start(*left_button, false, false);
  bottom_box->pack_start(*right_button, false, false);
  bottom_box->pack_start(*filename_label, true, true);
  bottom_box->pack_start(*fit_button, false, false);
  bottom_box->pack_start(*open_button, false, false);

  right_box = new Gtk::Box();
  right_box->set_hexpand(true);
  right_box->set_vexpand(true);
  right_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  right_box->set_spacing(4);
  right_box->pack_start(*image, true, true);
  right_box->pack_start(*bottom_box, false, false);

  left_box = new Gtk::Box();
  left_box->set_vexpand(true);
  left_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
  left_box->set_spacing(4);
  left_box->pack_start(*library_button, false, false);
  left_box->pack_start(*notebook, true, true);

  grid = new Gtk::Grid();
  //grid->set_vexpand(true);
  //grid->set_hexpand(true);              //?????
  //grid->add(*menu);
  grid->attach(*menu, 0, 0, 2, 1);
  grid->attach(*left_box, 0, 2, 1, 1);
  grid->attach(*right_box, 1, 2, 1, 1);

  main_window->add(*grid);
}

//function connects signals and shows main window
void GUI::createMainWindow() {
  //connecting button clicked signal to function
  open_button->signal_clicked().connect(sigc::bind<Gtk::Image*>(sigc::ptr_fun(&on_open_image), image));
  fit_button->signal_clicked().connect(sigc::bind<Gtk::Image*>(sigc::ptr_fun(&on_fit_image), image));

  //showing widgets
  main_window->show_all_children();
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

static void on_fit_image(Gtk::Image *image) {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf = image->get_pixbuf();
  if(pixbuf) {
    Gdk::Rectangle rectangle = image->get_allocation();
    pixbuf->scale_simple(rectangle.get_width(), rectangle.get_height(), Gdk::INTERP_BILINEAR);
    image->set(pixbuf);
  }
}
