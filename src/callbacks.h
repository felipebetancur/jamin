#include <gtk/gtk.h>


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data);

void
on_quit_button_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_in_trim_scale_value_changed         (GtkRange        *range,
                                        gpointer         user_data);

void
on_window1_show                        (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_EQ_curve_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_EQ_curve_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_bypass_button_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_comp_kn_1_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_comp_kn_2_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_comp_kn_3_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_geq_min_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_geq_max_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_lim_out_trim_scale_value_changed        (GtkRange        *range,
                                        gpointer         user_data);

void
on_pan_scale_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

gboolean
on_comp1_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp1_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_comp2_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp2_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_comp3_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp3_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_low_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_motion_notify_event  (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_motion_notify_event  (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

GtkWidget*
make_meter (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
make_mscale (gchar *widget_name, gchar *string1, gchar *string2,
		gint int1, gint int2);

void
on_autoutton1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autoutton2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autoutton3_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_pre_eq_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_post_eq_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_post_compressor_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_output_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_undo_button_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_lim_lh_scale_value_changed          (GtkRange        *range,
                                        gpointer         user_data);

void
on_release_val_label_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_low2mid_button_press_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_low2mid_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_mid2high_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_mid2high_button_release_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_lim_input_hscale_value_changed      (GtkRange        *range,
                                        gpointer         user_data);

void
on_lim_input_hscale_realize            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_optionmenu1_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_high_meter_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_high_meter_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_high_meter_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
rev_button                             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
fwd_button                             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
play_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
stop_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
stop_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
rewind_button                          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
play_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
stop_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
rewind_transport                       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_autobutton_l_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autobutton_m_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autobutton_h_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
play_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
stop_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
foward_transport                       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
forward_transport                      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_boost_scale_value_changed           (GtkRange        *range,
                                        gpointer         user_data);

gboolean
on_scene1_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_scene2_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_scene3_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_scene4_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_scene5_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_scene6_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_setscene_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clearscene_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_scene1_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scene2_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scene3_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scene4_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scene5_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scene6_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_help_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_show_help                           (GtkWidget       *widget,
                                        GtkWidgetHelpType  help_type,
                                        gpointer         user_data);

gboolean
on_input_eventbox_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_geq_eventbox_enter_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_spectrum_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_crossover_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_comp_curve_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_comp_eventbox_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_stereo_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_gain_meter_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_limiter_eventbox_enter_notify_event (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_boost_eventbox_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_output_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_help_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_eq_options_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_spectrum_options_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_status_label_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_transport_controls_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_bypass_button_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_scenes_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_notebook1_switch_page               (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data);

gboolean
on_window1_key_press_event             (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

gboolean
on_scene1_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene1_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene2_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene2_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene3_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene3_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene4_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene4_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene5_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene5_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene6_name_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_scene6_name_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_undo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_redo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_cc_window_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_hscale_1_l_value_changed            (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale_1_l_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_hscale_1_m_value_changed            (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale_1_m_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_hscale_1_h_value_changed            (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale_1_h_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_frame_l_enter_notify_event          (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_frame_m_enter_notify_event          (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_frame_h_enter_notify_event          (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_help1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_keys1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ports1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_in1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_left1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_right1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_out1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_left2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_right2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ports1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_in1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_left1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_right1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_out1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_left2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_right2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_out_trim_scale_value_changed        (GtkRange        *range,
                                        gpointer         user_data);
