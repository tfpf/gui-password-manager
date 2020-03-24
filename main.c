#include <gtk/gtk.h>

int main(int const argc, char const *argv[])
{
	GtkWidget *window;
	// gtk_init(&argc, &argv);
	gtk_init(0, NULL);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_show(window);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);  
	gtk_main();

	return 0;
}

