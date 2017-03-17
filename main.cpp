#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <string>
#define PI 3.1415926535897932384626433832795
static cairo_surface_t *surface = NULL;
GtkWidget *drawing_area;
GtkWidget *window_widget;
GtkWidget *dialog_pnt, *dialog_line;
GtkWidget *entryName_dgpnt, *entryX_dgpnt, *entryY_dgpnt;
GtkWidget *entryX1_dgline, *entryY1_dgline, *entryX2_dgline, *entryY2_dgline;

GtkTreeModel *displayfile_model;
GtkTreeSelection *treeSelection;
GtkListStore *listStore;


/*Clear the surface, removing the scribbles*/
static void clear_surface (){
  cairo_t *cr;

  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);

  cairo_destroy (cr);
}

/*Creates the surface*/
static gboolean create_surface (GtkWidget *widget, GdkEventConfigure *event, gpointer data){
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                       CAIRO_CONTENT_COLOR,
                                       gtk_widget_get_allocated_width (widget),
                                       gtk_widget_get_allocated_height (widget));
  clear_surface ();
  return TRUE;
}

/* Redraw the screen from the surface */
static gboolean redraw (GtkWidget *widget, cairo_t   *cr,  gpointer   data){
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}

//Callbacks
extern "C" G_MODULE_EXPORT void btn_line_clicked(){
   double x1, y1, x2, y2;
   gint result = gtk_dialog_run (GTK_DIALOG (dialog_line));
   switch (result)
     {
       case GTK_RESPONSE_OK:
          x1 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX1_dgline));
          y1 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY1_dgline));
          x2 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX2_dgline));
          y2 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY2_dgline));
          cairo_t *cr;
          cr = cairo_create (surface);
          cairo_move_to(cr, x1, y1);
          cairo_line_to(cr, x2, y2);
          cairo_stroke(cr);
          gtk_widget_queue_draw (window_widget);
          break;
       default:
          break;
     }
     gtk_widget_hide(dialog_line);
}

extern "C" G_MODULE_EXPORT void btn_pnt_clicked(){
   double x, y;
   std::string name;
   gint result = gtk_dialog_run (GTK_DIALOG (dialog_pnt));
   switch (result)
   {
       case GTK_RESPONSE_OK:
          name = gtk_entry_get_text(GTK_ENTRY(entryName_dgpnt));
          x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX_dgpnt));
          y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY_dgpnt));

          //PARTE QUE MOSTRA NO DISPLAYFILE



          cairo_t *cr;
          cr = cairo_create (surface);

          cairo_arc(cr, x, y, 1.0, 0.0, (2*PI) );
          cairo_stroke(cr);
          gtk_widget_queue_draw (window_widget);
          break;
       default:
          break;
   }
   gtk_widget_hide(dialog_pnt);
}

extern "C" G_MODULE_EXPORT void btn_plg_clicked(){
    cairo_t *cr;
    cr = cairo_create (surface);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, 200, 100);
    cairo_line_to(cr, 400, 400);
    cairo_line_to(cr, 0, 0);
    cairo_stroke(cr);
    gtk_widget_queue_draw (window_widget);
}


int main(int argc, char *argv[]) {
    GtkBuilder  *gtkBuilder;
    gtk_init(&argc, &argv);

    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "window.glade", NULL);

    //TODO isolar widgets
    window_widget = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "main_window") );
    drawing_area = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "drawing_area") );

    dialog_pnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_pnt") );
    dialog_line = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_line") );

    entryName_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "name_dgpnt") );
    entryX_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx_dgpnt") );
    entryY_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy_dgpnt") );

    entryX1_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx1_dgline") );
    entryY1_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy1_dgline") );
    entryX2_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx2_dgline") );
    entryY2_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy2_dgline") );

    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "treeview_displayf" ) );
    displayfile_model = gtk_tree_view_get_model(tree);
    treeSelection = gtk_tree_view_get_selection(tree);

    listStore = GTK_LIST_STORE( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "liststore_displayf"));

    GtkTreeIter iter;





    g_signal_connect (drawing_area, "draw", G_CALLBACK (redraw), NULL);
    g_signal_connect (drawing_area,"configure-event", G_CALLBACK (create_surface), NULL);

    gtk_builder_connect_signals(gtkBuilder, NULL);
    gtk_widget_show_all(window_widget);

    gtk_main ();
    return 0;
}
