#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <string>
#include "Object.h"
#include "ViewPort.hpp" //apenas para teste
#define ANGLE 20

static cairo_surface_t *surface = NULL;
using namespace std;
const double PI = 3.1415926535897932384626433832795;
double sx, sy;
ViewPort *vp;
vector<Object> displayfile;
GtkBuilder  *gtkBuilder;
GtkWidget *drawing_area;
GtkWidget *window_widget;
GtkWidget *dialog_pnt, *dialog_line, *dialog_rotate, *dialog_scale, *dialog_translate;
GtkWidget *entryName_dgpnt, *entryX_dgpnt, *entryY_dgpnt;
GtkWidget *entryName_dgline, *entryX1_dgline, *entryY1_dgline, *entryX2_dgline, *entryY2_dgline;
GtkWidget *dialog_plg, *entryName_dgplg, *entryX_dgplg, *entryY_dgplg;
GtkWidget *entryDX_dgtrans, *entryDY_dgtrans;
//SCALE WIDGETS ---------
GtkWidget *entrySX_dgscale, *entrySY_dgscale;
//-----------------------
//ROTATE WIDGETS ---------
GtkWidget *entryPX_dgrotate, *entryPY_dgrotate, *entryAngle_dgrotate, *objCenter_dgrotate, *worldCenter_dgrotate, *pntCenter_dgrotate;
//------------------------

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

Object::RotationType getRotationTypeByRotateDialog(){
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(objCenter_dgrotate)))
      return Object::RotationType::CENTER;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(worldCenter_dgrotate)))
      return Object::RotationType::ORIGIN;
    else
      return Object::RotationType::POINT;
}

bool getSelectedObjectName(string &object_name){
    if(!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeSelection), NULL, NULL)) {
        return false;
    }
    char *name;
    GtkTreeIter iterr;
    gtk_tree_selection_get_selected(GTK_TREE_SELECTION(treeSelection), NULL, &iterr);
    gtk_tree_model_get(displayfile_model, &iterr, 0, &name, -1);
    object_name = name;
    delete name;
    return true;
}

Object* getObjectByName(string object_name){
    string c_name;
    Object* object = nullptr;
    for(auto &obj : displayfile) {
        c_name = obj.getName();
        if(!c_name.compare(object_name)) {
            object = &obj;
        }
    }
    return object;
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
    vp->move(10.0, 0.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_moveto_down_clicked(){
    vp->move(0.0, -10.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_moveto_left_clicked(){
    vp->move(-10.0, 0.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_moveto_up_clicked(){
    vp->move(0.0, 10.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_zoom_out_clicked(){
    vp->zoom(-10.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_zoom_in_clicked(){
    vp->zoom(10.0);
    clear_surface();
    cairo_t *cr;
    cr = cairo_create (surface);
    vp->drawObjects(displayfile, cr);
    gtk_widget_queue_draw(window_widget);
}


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
          vp->drawLine(l1->getCoords(), cr);
          gtk_widget_queue_draw(window_widget);
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
          vp->drawPoint(point->getCoords(), cr);

          gtk_widget_queue_draw(window_widget);
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
            xI = x;
            yI = y;
            poly->addCoordinate(x,y);
            valid = gtk_tree_model_iter_next(dgplg_model, &iterP);

            while(valid)
            {
                gtk_tree_model_get(dgplg_model, &iterP, 0, &x, 1, &y, -1);


                poly->addCoordinate(x,y);
                valid = gtk_tree_model_iter_next(dgplg_model, &iterP);
            }

            // draw in the drawing_area
            cairo_t *cr;
            cr = cairo_create (surface);
            vp->drawPolygon(poly->getCoords(), cr);
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

extern "C" G_MODULE_EXPORT void btn_translate_obj_clicked(){
    string name_selected_obj;
    bool hasObjectSelected = getSelectedObjectName(name_selected_obj);

    //ONLY CALLS THE DIALOG IF AN OBJECT IS SELECTED IN THE DISPLAYFILE
    if(hasObjectSelected) {
        double dx, dy;
        gint result = gtk_dialog_run (GTK_DIALOG (dialog_translate));
        switch(result)
        {
            case GTK_RESPONSE_OK:
                  dx = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryDX_dgtrans));
                  dy = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryDY_dgtrans));
              break;
            default:
              break;
        }
        gtk_widget_hide(dialog_translate);

        cairo_t *cr;
        cr = cairo_create (surface);
        Object *selected_obj = getObjectByName(name_selected_obj);
        clear_surface();
        selected_obj->translate(Coordinate(dx,dy));
        vp->drawObjects(displayfile, cr);
        gtk_widget_queue_draw(window_widget);
    }
}

extern "C" G_MODULE_EXPORT void btn_scale_obj_clicked(){
    string name_selected_obj;
    bool hasObjectSelected = getSelectedObjectName(name_selected_obj);

    //ONLY CALLS THE DIALOG IF AN OBJECT IS SELECTED IN THE DISPLAYFILE
    if(hasObjectSelected) {
        double sx, sy;
        gint result = gtk_dialog_run (GTK_DIALOG (dialog_scale));
        switch(result)
        {
            case GTK_RESPONSE_OK:
                  sx = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entrySX_dgscale));
                  sy = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entrySY_dgscale));
              break;
            default:
              break;
        }
        gtk_widget_hide(dialog_scale);

        cairo_t *cr;
        cr = cairo_create(surface);
        Object *selected_obj = getObjectByName(name_selected_obj);
        clear_surface();
        selected_obj->scale(Coordinate(sx,sy));
        vp->drawObjects(displayfile, cr);
        gtk_widget_queue_draw(window_widget);

    }
}

extern "C" G_MODULE_EXPORT void btn_rotate_obj_clicked(){
    string selected_obj_name;
    bool hasObjectSelected = getSelectedObjectName(selected_obj_name);
    if(hasObjectSelected) {
        double angle, px, py;
        Object::RotationType r_type;
        gint result = gtk_dialog_run (GTK_DIALOG (dialog_rotate));
        switch(result)
        {
            case GTK_RESPONSE_OK:
                angle = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryAngle_dgrotate));
                px = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryPX_dgrotate));
                py = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entryPY_dgrotate));
                r_type = getRotationTypeByRotateDialog();
                break;
            default:
                break;
        }
        gtk_widget_hide(dialog_rotate);

        cairo_t *cr;
        cr = cairo_create (surface);
        Object *selected_obj = getObjectByName(selected_obj_name);
        clear_surface();
        selected_obj->rotate(angle, r_type, Coordinate(px, py));
        vp->drawObjects(displayfile, cr);
        gtk_widget_queue_draw(window_widget);
    }
}

extern "C" G_MODULE_EXPORT void btn_rotate_right_clicked(){
  cairo_t *cr;
  cr = cairo_create (surface);
  vp->rotate(-ANGLE);
  clear_surface();
  vp->drawObjects(displayfile, cr);

  gtk_widget_queue_draw(window_widget);
}

extern "C" G_MODULE_EXPORT void btn_rotate_left_clicked(){
  cairo_t *cr;
  cr = cairo_create (surface);
  vp->rotate(-ANGLE);
  clear_surface();
  vp->drawObjects(displayfile, cr);

  gtk_widget_queue_draw(window_widget);
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
    dialog_translate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_translate") );
    dialog_scale = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_scale") );
    dialog_rotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "dialog_rotate") );
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

    //TRANSLATE WIDGETS ----
    entryDX_dgtrans = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrydx_dgtrans") );
    entryDY_dgtrans = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrydy_dgtrans") );
    // ---------------------
    //SCALE WIDGETS ----
    entrySX_dgscale = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrysx_dgscale") );
    entrySY_dgscale = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrysy_dgscale") );
    // -----------------
    //ROTATE WIDGETS ----
    entryPX_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrypx_dgrotate") );
    entryPY_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entrypy_dgrotate") );
    entryAngle_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "entryangle_dgrotate"));
    objCenter_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "rb_rotate_obj") );
    worldCenter_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "rb_rotate_world") );
    pntCenter_dgrotate = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "rb_rotate_point") );
    // ------------------



    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "treeview_displayf" ) );
    displayfile_model = gtk_tree_view_get_model(tree);
    treeSelection = gtk_tree_view_get_selection(tree);

    listStore = GTK_LIST_STORE( gtk_builder_get_object( GTK_BUILDER(gtkBuilder), "liststore_displayf"));

    GtkRequisition min;
    gtk_widget_get_preferred_size(drawing_area, &min, NULL);

    vp = new ViewPort(min.width, min.height);


    g_signal_connect (drawing_area, "draw", G_CALLBACK (redraw), NULL);
    g_signal_connect (drawing_area,"configure-event", G_CALLBACK (create_surface), NULL);

    gtk_builder_connect_signals(gtkBuilder, NULL);
    gtk_widget_show_all(window_widget);

    gtk_main ();
    return 0;
}
