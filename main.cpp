#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <string>
#include "Object.hpp"
#define PI 3.1415926535897932384626433832795
static cairo_surface_t *surface = NULL;
using namespace std;

double sx, sy;
vector<Object> displayfile;
GtkBuilder  *gtkBuilder;
GtkWidget *drawing_area;
GtkWidget *window_widget;
GtkWidget *dialog_pnt, *dialog_line;
GtkWidget *entryName_dgpnt, *entryX_dgpnt, *entryY_dgpnt;
GtkWidget *entryName_dgline, *entryX1_dgline, *entryY1_dgline, *entryX2_dgline, *entryY2_dgline;
GtkWidget *dialog_plg, *entryName_dgplg, *entryX_dgplg, *entryY_dgplg;

GtkTreeModel *displayfile_model;
GtkTreeSelection *treeSelection;
GtkListStore *listStore;
GtkTreeIter iter;

GtkListStore *listStore_dgplg;
GtkTreeModel *dgplg_model;
GtkTreeSelection *treeplgSelection;


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
extern "C" G_MODULE_EXPORT void btn_add_coord_plg_clicked(){
  listStore_dgplg = GTK_LIST_STORE( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "liststore_addcoordplg"));
  GtkTreeIter iterP;
  double x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX_dgplg));
  double y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY_dgplg));

  gtk_list_store_append(listStore_dgplg, &iterP);
  gtk_list_store_set(listStore_dgplg, &iterP, 0, x, 1, y, -1);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(entryX_dgplg), 0.0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(entryY_dgplg), 0.0);


}

extern "C" G_MODULE_EXPORT void btn_moveto_right_clicked(){
    sx += -50;
    clear_surface();
    gtk_widget_queue_draw (window_widget);
}

extern "C" G_MODULE_EXPORT void btn_moveto_down_clicked(){ }

extern "C" G_MODULE_EXPORT void btn_moveto_left_clicked(){ }

extern "C" G_MODULE_EXPORT void btn_moveto_up_clicked(){ }

extern "C" G_MODULE_EXPORT void btn_zoom_out_clicked(){ }

extern "C" G_MODULE_EXPORT void btn_zoom_in_clicked(){ }


extern "C" G_MODULE_EXPORT void btn_line_clicked(){
   double x1, y1, x2, y2;
   string name;
   Line *l1;
   gint result = gtk_dialog_run (GTK_DIALOG (dialog_line));
   switch (result)
     {
       case GTK_RESPONSE_OK:
          //getting dialog values
          name = gtk_entry_get_text(GTK_ENTRY(entryName_dgline));
          x1 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX1_dgline));
          y1 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY1_dgline));
          x2 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX2_dgline));
          y2 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY2_dgline));
          // --------------------

          //CREATE OBJECT/model e add object to the displayfile
          l1 = new Line(name);
          l1->addCoordinate(x1,y1);
          l1->addCoordinate(x2,y2);

          displayfile.push_back(*l1);
          // -------------------------

          //show in displayfile interface
          gtk_list_store_append(listStore, &iter);
          gtk_list_store_set(listStore, &iter, 0, name.c_str(), 1, "Reta", -1);
          // -------------------------------

          // draw in the drawing_area
          cairo_t *cr;
          cr = cairo_create (surface);
          cairo_move_to(cr, x1, y1);
          cairo_line_to(cr, x2, y2);
          cairo_stroke(cr);
          gtk_widget_queue_draw (window_widget);
          // -----------------------------
          break;

       default:
          break;
     }
     gtk_widget_hide(dialog_line);
}

extern "C" G_MODULE_EXPORT void btn_pnt_clicked(){
   double x, y;
   string name;
   Point *point;
   gint result = gtk_dialog_run (GTK_DIALOG (dialog_pnt));
   switch (result)
   {
       case GTK_RESPONSE_OK:
          //getting dialog values
          name = gtk_entry_get_text(GTK_ENTRY(entryName_dgpnt));
          x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryX_dgpnt));
          y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryY_dgpnt));
          // ---------------------

          //CREATE OBJECT/model e add object to the displayfile
          point = new Point(name);
          point->addCoordinate(x,y);

          displayfile.push_back(*point);
          // ---------------------------

          //show in displayfile interface
          gtk_list_store_append(listStore, &iter);
          gtk_list_store_set(listStore, &iter, 0, name.c_str(), 1, "Ponto", -1);
          // --------------------

          //draw in the drawing_area
          cairo_t *cr;
          cr = cairo_create (surface);
          cairo_arc(cr, x, y, 1.0, 0.0, (2*PI) );
          cairo_stroke(cr);
          gtk_widget_queue_draw (window_widget);
          // ------------------------
          break;
       default:
          break;
   }
   gtk_widget_hide(dialog_pnt);
}

extern "C" G_MODULE_EXPORT void btn_plg_clicked(){
    GtkTreeIter iterP;
    gboolean valid;
    string name;
    Polygon *poly;
    gint result = gtk_dialog_run (GTK_DIALOG (dialog_plg));
    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "treeview_addcoordplg" ) );
    dgplg_model = gtk_tree_view_get_model(tree);
    treeplgSelection = gtk_tree_view_get_selection(tree);
    switch (result)
    {
        case GTK_RESPONSE_OK:
            gdouble x,y;
            gdouble xI, yI;
            name = gtk_entry_get_text(GTK_ENTRY(entryName_dgplg));
            valid = gtk_tree_model_get_iter_first(dgplg_model, &iterP);
            poly = new Polygon(name);

            gtk_tree_model_get(dgplg_model, &iterP, 0, &x, 1, &y, -1);
            cairo_t *cr;
            cr = cairo_create (surface);
            cairo_move_to(cr, x, y);
            std::cout << x << ".\n";
            std::cout << y << ".\n";
            xI = x;
            yI = y;
            poly->addCoordinate(x,y);
            valid = gtk_tree_model_iter_next(dgplg_model, &iterP);

            while(valid)
            {
                gtk_tree_model_get(dgplg_model, &iterP, 0, &x, 1, &y, -1);
                cairo_line_to(cr, x, y);
                std::cout << x << ".\n";
                std::cout << y << ".\n";
                poly->addCoordinate(x,y);
                valid = gtk_tree_model_iter_next(dgplg_model, &iterP);
            }

            // draw in the drawing_area
            cairo_line_to(cr, xI, yI);
            cairo_stroke(cr);
            gtk_widget_queue_draw (window_widget);
            // =====================

            gtk_list_store_clear(listStore_dgplg);
            displayfile.push_back(*poly);

            //show in displayfile interface
            gtk_list_store_append(listStore, &iter);
            gtk_list_store_set(listStore, &iter, 0, name.c_str(), 1, "Poligono", -1);
            // ===========================================

            break;
        default:
            gtk_list_store_clear(listStore_dgplg);
            break;
    }
    gtk_widget_hide(dialog_plg);
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "window.glade", NULL);

    //TODO isolar widgets
    window_widget = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "main_window") );
    drawing_area = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "drawing_area") );


    //DIALOGS WIDGETS ----
    dialog_pnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_pnt") );
    dialog_line = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_line") );
    dialog_plg = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_plg") );
    // --------------------

    //POINT WIDGETS ----
    entryName_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "name_dgpnt") );
    entryX_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx_dgpnt") );
    entryY_dgpnt = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy_dgpnt") );
    // -----------------

    //LINE WIDGETS ----
    entryName_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "name_dgline") );
    entryX1_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx1_dgline") );
    entryY1_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy1_dgline") );
    entryX2_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx2_dgline") );
    entryY2_dgline = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy2_dgline") );
    // ---------------

    //POLYGON WIDGETS ----
    entryName_dgplg = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "name_dgplg") );
    entryX_dgplg = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryx_dgplg") );
    entryY_dgplg = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryy_dgplg") );

    // -------------------


    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "treeview_displayf" ) );
    displayfile_model = gtk_tree_view_get_model(tree);
    treeSelection = gtk_tree_view_get_selection(tree);

    listStore = GTK_LIST_STORE( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "liststore_displayf"));





    g_signal_connect (drawing_area, "draw", G_CALLBACK (redraw), NULL);
    g_signal_connect (drawing_area,"configure-event", G_CALLBACK (create_surface), NULL);

    gtk_builder_connect_signals(gtkBuilder, NULL);
    gtk_widget_show_all(window_widget);

    gtk_main ();
    return 0;
}
