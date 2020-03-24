#include <gtk/gtk.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

int request_passphrase(void)
{
	// create window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Enter Passphrase");

	// create label
	GtkWidget *label = gtk_label_new("Water is wet!");
	gtk_container_add(GTK_CONTAINER(window), label);
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_main();
}

///////////////////////////////////////////////////////////////////////////////

int main(int const argc, char const *argv[])
{
	// must be called before any other GTK functions
	// normally, arguments are pointers to `argc' and `argv'
	gtk_init(0, NULL);

	// if user fails to log in, quit
	if(request_passphrase() == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	return 0;
}

