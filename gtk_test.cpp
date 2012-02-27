#include <gtk/gtk.h>

static void hello(GtkWidget *widget, gpointer data) {
  g_print("Hello World\n");
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event,
                             gpointer data) {
  g_print("delete event occured\n");
  return TRUE;
}

static void destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

int main(int argc, char *argv[]) {
  GtkWidget *main_window, *button;
  gtk_init(&argc, &argv);
  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(main_window, "delete_event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
  button = gtk_button_new_with_label("Hello World!");
  g_signal_connect(button, "clicked", G_CALLBACK(hello), NULL);
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), main_window);
  gtk_container_add(GTK_CONTAINER(main_window), button);
  gtk_widget_show(button);
  gtk_widget_show(main_window);
  gtk_main();
  return 0;
}
