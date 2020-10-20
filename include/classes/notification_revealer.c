/*-----------------------------------------------------------------------------
Wrapper struct to handle notifications.

Members:
    revealer: container used to display a notification
    notify: label with the text of the notification
-----------------------------------------------------------------------------*/
typedef struct
{
    GtkWidget *revealer;
    GtkWidget *notify;
}
notification_revealer_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
notification_revealer_t *notification_revealer_new(void);
void notification_revealer_show(notification_revealer_t *self, char const *message);
gboolean notification_revealer_hide(gpointer data);

/*-----------------------------------------------------------------------------
Initialiser for the `notification_revealer_t' struct. Create a GTK revealer.
Add a notification-style GTK box in it. Add a GTK label in said box. Save the
revealer and the label as struct members.
-----------------------------------------------------------------------------*/
notification_revealer_t *notification_revealer_new(void)
{
    notification_revealer_t *self = malloc(sizeof *self);

    // revealer
    self->revealer = gtk_revealer_new();
    gtk_revealer_set_transition_duration(GTK_REVEALER(self->revealer), NOTIF_FADE_TIME);
    gtk_revealer_set_transition_type(GTK_REVEALER(self->revealer), GTK_REVEALER_TRANSITION_TYPE_CROSSFADE);
    gtk_widget_set_halign(self->revealer, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(self->revealer, GTK_ALIGN_START);

    // box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkStyleContext *style = gtk_widget_get_style_context(box);
    gtk_style_context_add_class(style, "app-notification");
    gtk_container_add(GTK_CONTAINER(self->revealer), box);

    // label
    self->notify = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(box), self->notify);

    return self;
}

/*-----------------------------------------------------------------------------
Set the text of the notification and make it visible.
-----------------------------------------------------------------------------*/
void notification_revealer_show(notification_revealer_t *self, char const *message)
{
    gtk_label_set_text(GTK_LABEL(self->notify), message);
    gtk_revealer_set_reveal_child(GTK_REVEALER(self->revealer), TRUE);
    g_timeout_add(NOTIF_VIEW_TIME, notification_revealer_hide, self->revealer);
}

/*-----------------------------------------------------------------------------
Make the notification invisible.
-----------------------------------------------------------------------------*/
gboolean notification_revealer_hide(gpointer data)
{
    GtkRevealer *revealer = data;
    if(GTK_IS_REVEALER(revealer))
    {
        gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
    }

    return FALSE;
}

