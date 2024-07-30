
#include <Arduino.h>
#include "gui.h"
#include "lvgl.h"
#include "debug_log.h"

#define TABVIEW_BAR_WIDTH	80
#define TABVIEW_TAB_WIDTH	(DISPLAY_HOR_RES-TABVIEW_BAR_WIDTH)

#define TAB_AMP_COL_W (TABVIEW_TAB_WIDTH/6)

#define TAB_TOP_ROW_H		50
#define TAB_MAIN_AREA_H		(DISPLAY_VER_RES-TAB_TOP_ROW_H)
#define TAB_SLIDER_WIDTH 	10
#define TAB_SLIDER_HEIGHT	(DISPLAY_VER_RES-TAB_TOP_ROW_H-15)

#define TOP_BTN_H			(TAB_TOP_ROW_H - 10)
#define TOP_BTN_W			(100)

#define BTN_MAIN_W			(90)

#define BTN_SETUP_W			(BTN_MAIN_W-10)

LV_FONT_DECLARE(font_eg12)
LV_FONT_DECLARE(font_eg14)
LV_FONT_DECLARE(font_eg32)

extern lv_disp_t* lcd;

lv_obj_t* tabview;

// --- MAIN ---
lv_obj_t* tabMain;
lv_obj_t* btn_main_doubler;
lv_obj_t* btn_main_comp;
lv_obj_t* btn_main_wah;
lv_obj_t* btn_main_boost;
lv_obj_t* btn_main_oct;
lv_obj_t* btn_main_delay;
lv_obj_t* btn_main_delay_freeze;
lv_obj_t* btn_main_reverb;
lv_obj_t* btn_main_reverb_freeze;
lv_obj_t* btn_main_load;
lv_obj_t* btn_main_save;
lv_obj_t* btn_main_setup;
lv_obj_t* btn_main_wet;
lv_obj_t* btn_main_bypass;
lv_obj_t* btn_main_presetUp;
lv_obj_t* btn_main_presetDown;
lv_obj_t* btn_main_dry;
lv_obj_t* label_presetNo;
lv_obj_t* label_presetName;
lv_obj_t* dd_main_ampModel;
lv_obj_t* dd_main_cabModel;

lv_obj_t* label_main_inType;
lv_obj_t* label_main_pot1Assign;
lv_obj_t* label_main_pot2Assign;
lv_obj_t* label_main_pot3Assign;
lv_obj_t* label_main_expAssign;

lv_obj_t* win_setup;
lv_obj_t* btn_setup_save;
lv_obj_t* btn_setup_close;
lv_obj_t* rl_inType;
lv_obj_t* rl_pot1Assign;
lv_obj_t* rl_pot2Assign;
lv_obj_t* rl_pot3Assign;
lv_obj_t* rl_ExpAssign;
lv_obj_t* rl_ft1Assign;
lv_obj_t* rl_ft2Assign;

static const char*  potAssigns = "Gate\nB-Drive\nB-Bottom\nGain\nTreble\nFxMix\nVol\nPan\nHiCut";
// corresponds to the MIDI CC assignements in the TGX4 Teensy project
static const char*  potAssignTable = 
"OFF\n"
"CompPreG\nCompThres\nCompRatio\nCompAtt\nCompRel\nCompVol\n"
"WahFreq\nWahMix\nBoostDrv\nBoostBtm\nBoostPres\nBoostMix\nBoostLevel\n"
"Gate\nAmpGain\nAmpBass\nAmpMid\nAmpTreble\nAmpFxMix\n"
"DlyTime\nDlyInert\nDlyRepeat\nDlyRate\nDlyDepth\nDlyHiCut\nDlyLoCut\nDlyTreble\nDlyBass\n"
"RvPlTime\nRvPlDiff\nRvPlMix\nRvPlLoCut\nRvPlHiCut\nRvPlBass\nRvPlTreble\nRvPlBleed\nRvPlPitch\nRvPlPitchMix\nRvPlShimmer\nRvPlShimPitch\n"
"RvScTime\nRvScMix\nRvScTreble\n"
"RvSpTime\nRvSpMix\nRvSpLoCut\nRvSpHiCut\n"
"MstVol\nDlyRvMix\nMstPan\nMstHiCut";

static const char*  ftAssignTable = 
"OFF\n"
"Comp\nWah\nBoost\nOct\nDelay\nDelayFr\nDelayTap\nReverb\nRevbFr\nPresetUp\nPresetDwn";

lv_obj_t* kbd;
lv_obj_t* kbd_txtArea;
lv_obj_t* win_presetSaveDialog;
lv_obj_t* btn_presetSave_close;
lv_obj_t* btn_presetSave_save;
static const char * kb_map[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
								"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",  "\n",
								"A", "S", "D", "F", "G", "H", "J", "K", "L", "\n",
								"Z", "X", "C", "V",
								 "B", "N", "M", "_", "\n",
								LV_SYMBOL_CLOSE, " ",  " ", " ", LV_SYMBOL_OK, NULL
								};
/*Set the relative width of the buttons and other controls*/
static const lv_buttonmatrix_ctrl_t kb_ctrl[] = {   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6,
													4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
													4, 4, 4, 4, 4, 4, 4, 4, 4,
													4, 4, 4, 4, 4, 4, 4, 4,
													3, LV_BUTTONMATRIX_CTRL_HIDDEN | 1, 6, LV_BUTTONMATRIX_CTRL_HIDDEN | 1, 2
												};

// --- POP UP BOX ---
lv_obj_t* popup_box;
lv_obj_t* popup_box_msg;

lv_obj_t* label_main_footer;

lv_obj_t* sliders[PRESET_PARAM_LAST];
lv_obj_t* sliderLabels[PRESET_PARAM_LAST];


// --- COMPRESSOR ---
lv_obj_t* tabComp;
lv_obj_t* btn_comp;


// --- PREAMP ---
lv_obj_t* tabPreamp;

lv_obj_t* btn_wah;
lv_obj_t* dd_wahModel;
lv_obj_t* btn_boost;
lv_obj_t* btn_octave;

static const char*  WAHModels = "G1\n"
								"G2\n"
								"G3\n"
								"G4\n"
								"VOCAL\n"
								"EXTREME\n"
								"CUSTOM\n"
								"BASS";

// --- AMP ---
lv_obj_t* tabAmp;

static const char*  ampModels = "OFF-AMP\n"
								"Clean 1\n"
								"Clean 2\n"
								"Clean 3\n"
								"Clean 4\n"
								"Crunch 1\n"
								"Crunch 2\n"
								"Lead 1\n"
								"Lead 2";
static const char*  cabModels = "OFF-CAB\n"
								"Guitar 1\n"
								"Guitar 2\n"
								"Guitar 3\n"
								"Guitar 4\n"
								"Guitar 5\n"
								"Guitar 6\n"
								"Guitar 7\n"
								"Bass 1\n"
								"Bass 2\n"
								"Bass 3";
static const char*  inTypes = 	"LR Stereo\n"
								"L Mono\n"
								"R Mono";

lv_obj_t* dd_ampModel;
lv_obj_t* dd_cabModel;
lv_obj_t* dd_inType;
lv_obj_t* btn_doubler;

// --- DELAY ---
lv_obj_t* tabDelay;
lv_obj_t* btn_delay;
lv_obj_t* btn_delay_freeze;

// --- REVERB ---
lv_obj_t* tabReverb;
lv_obj_t* dd_reverb_type;
lv_obj_t* btn_reverb;
lv_obj_t* btn_reverbPLSC_freeze;


lv_obj_t* tabview_reverb;
lv_obj_t* tab_reverbPL;
lv_obj_t* tab_reverbSP;
lv_obj_t* tab_reverbSC;

static const char*  reverbTypes = 	"Plate\n"
									"SC\n"
									"Spring";

static const char* labelTxt_onOff = "ON-OFF";
static const char* labelTxt_ON = "ON";
static const char* labelTxt_OFF = "OFF";
static const char* labelTxtInput = "Input:";
static const char* labelTxtPot1 = "Pot 1:";
static const char* labelTxtPot2 = "Pot 2:";
static const char* labelTxtPot3 = "Pot 3:";
static const char* labelTxtExpP = "ExpP:";

lv_theme_t* theme_main;

lv_style_t style_slider;
lv_style_t style_tab;
lv_style_t style_dd;
lv_style_t style_main_tab;
lv_style_t style_sub_container;

lv_obj_t* gui_make_btn(lv_obj_t* parent, const char* name, int32_t w, int32_t h, bool toggle, lv_event_cb_t cb);
lv_obj_t* gui_make_slider(lv_obj_t* parent, lv_obj_t **slider, lv_obj_t **label_val, const char* name, uint32_t w, uint32_t h, lv_event_cb_t cb);
static void slider_update_valueLabel(lv_obj_t* slider, int32_t value);
lv_obj_t* gui_make_roller(lv_obj_t* parent, lv_obj_t **roller, const char* options, const char* name, uint32_t w, uint32_t h, lv_event_cb_t cb);

bool userData_updateOnly = true;

typedef enum
{
	CHANGE_BTN_LABEL_NONE,
	CHANGE_BTN_LABEL_SRC,
	CHANGE_BTN_LABEL_DST,
	CHANGE_BTN_LABEL_BOTH
}btn_label_change_t;
void set_linked_btn(lv_obj_t* srcBtn, lv_obj_t* destBtn, const char* labelOn, const char* labelOFF, btn_label_change_t labelChange);

void main_tab_create(lv_obj_t* parent);
void comp_tab_create(lv_obj_t* parent);
void preamp_tab_create(lv_obj_t* parent);
void amp_tab_create(lv_obj_t* parent);
void delay_tab_create(lv_obj_t* parent);
void reverb_tab_create(lv_obj_t* parent);

// --- CALLBACKS ---
//static void slider_event_cb(lv_event_t* e);
static void slider_comp_event_cb(lv_event_t* e);
static void slider_preamp_event_cb(lv_event_t* e);
static void slider_amp_event_cb(lv_event_t* e);
static void slider_delay_event_cb(lv_event_t* e);
static void slider_reverb_event_cb(lv_event_t* e);

static void btn_main_event_cb(lv_event_t* e); 	// buttons on the main page
static void btn_comp_event_cb(lv_event_t* e); 
static void btn_preamp_event_cb(lv_event_t* e); 
static void btn_amp_event_cb(lv_event_t* e); 
static void btn_delay_event_cb(lv_event_t* e); 
static void btn_reverb_event_cb(lv_event_t* e);
static void dd_reverbSelect_event_cb(lv_event_t* e);
//static void dd_inType_event_cb(lv_event_t* e);
static void dd_ampSelect_event_cb(lv_event_t* e);
static void dd_cabSelect_event_cb(lv_event_t* e);
static void dd_main_event_cb(lv_event_t* e);
static void dd_wahSelect_cb(lv_event_t* e);
static void rl_setup_event_cb(lv_event_t* e);
static void presetSave_event_cb(lv_event_t* e);
static void timer_closePopup_cb(lv_timer_t * timer);

static void memory_monitor();

// --- TIMING ---
static uint32_t popupTimestamp;
static uint32_t popupTime = 0;

// ---------------------------------------------------------------------------------
void gui_init()
{
	theme_main = lv_theme_default_init(lcd, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_ORANGE),   /*Primary and secondary palette*/
                                        false,    /*Light or dark mode*/
										&font_eg14); 
	lv_display_set_theme(lcd, theme_main);

	tabview = lv_tabview_create(lv_scr_act());
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
	lv_tabview_set_tab_bar_size(tabview, TABVIEW_BAR_WIDTH);

	tabMain = lv_tabview_add_tab(tabview, "MAIN");
	tabComp = lv_tabview_add_tab(tabview, "COMP");
	tabPreamp = lv_tabview_add_tab(tabview, "PREAMP");
	tabAmp = lv_tabview_add_tab(tabview, "AMP");
	tabDelay = lv_tabview_add_tab(tabview, "DELAY");
	tabReverb = lv_tabview_add_tab(tabview, "REVERB");
 
	lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
	// --- STYLES ---
	lv_style_init(&style_main_tab);
	lv_style_set_pad_ver(&style_main_tab, 3);
	lv_style_set_pad_hor(&style_main_tab, 0);	

	lv_style_init(&style_slider);
	lv_style_set_pad_all(&style_slider, 0);
	lv_style_set_pad_column(&style_slider, 3);
	lv_style_set_pad_row(&style_slider, 3);
	lv_style_set_border_width(&style_slider, 0);
	lv_style_set_bg_opa(&style_slider, LV_OPA_0);
	lv_style_set_text_align(&style_slider, LV_TEXT_ALIGN_CENTER);

	lv_style_init(&style_tab);
	lv_style_set_pad_all(&style_tab, 0);

	lv_style_init(&style_dd);
	lv_style_set_pad_top(&style_dd, TOP_BTN_H/4);
	lv_style_set_pad_hor(&style_dd, 5);	
	lv_style_set_text_align(&style_dd, LV_TEXT_ALIGN_AUTO);

	lv_style_init(&style_sub_container);
	lv_style_set_pad_all(&style_sub_container, 0);
	lv_style_set_pad_row(&style_sub_container, 3);
	lv_style_set_border_width(&style_sub_container, 0);
	lv_style_set_bg_opa(&style_sub_container, LV_OPA_TRANSP);
	lv_style_set_text_align(&style_sub_container, LV_TEXT_ALIGN_CENTER);

	main_tab_create(tabMain);
	comp_tab_create(tabComp);
	preamp_tab_create(tabPreamp);
	amp_tab_create(tabAmp);
	delay_tab_create(tabDelay);
	reverb_tab_create(tabReverb);

	// create hidden popup
	popup_box = lv_msgbox_create(lv_scr_act());
	lv_obj_add_flag(popup_box, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_center(popup_box);
	
	popup_box_msg = lv_msgbox_add_text(popup_box, "");
	lv_obj_t* obj = lv_msgbox_get_content(popup_box);
	lv_obj_set_style_pad_all(obj, 5, 0);
	lv_obj_set_style_text_font(obj, &font_eg32, 0);
	lv_obj_set_style_text_align(popup_box_msg, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_border_width(popup_box, 3, 0);
	lv_obj_set_width(popup_box, LV_PCT(70));

	lv_tabview_set_active(tabview, 0, LV_ANIM_OFF);
}

void gui_process()
{
	uint32_t tNow;
	lv_timer_periodic_handler();
	if (!lv_obj_has_flag(popup_box, LV_OBJ_FLAG_HIDDEN))
	{
		tNow = millis();
		if( tNow - popupTimestamp > popupTime)
		{
			lv_obj_add_flag(popup_box, LV_OBJ_FLAG_HIDDEN);
		}
	}
}

#define TAB_MAIN_PAD	(3)
#define DELTAX 3
#define DELTAY 3
#define TAB_MAIN_BTNS_TOP_H		95

void main_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* btn,* cont;

	lv_obj_add_style(parent, &style_main_tab, 0);
	static int32_t column_dsc[] = {TABVIEW_TAB_WIDTH/4 - TAB_MAIN_PAD, TABVIEW_TAB_WIDTH/4 - TAB_MAIN_PAD, TABVIEW_TAB_WIDTH/4 - TAB_MAIN_PAD , TABVIEW_TAB_WIDTH/4 - TAB_MAIN_PAD , LV_GRID_TEMPLATE_LAST};
	static int32_t row_dsc[] = 
	{
		TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD, 	// row 0
		TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD, 	// row 1
		TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD,  // row 2
		TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD,  // row 3
		TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD,  // row 4
		TAB_MAIN_AREA_H - 5*(TAB_MAIN_AREA_H/6 - TAB_MAIN_PAD)+25,  // row 5
		LV_GRID_TEMPLATE_LAST}; 

	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(parent, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(parent, row_dsc, 0);
	lv_obj_set_style_pad_column(parent, 3, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 3, LV_PART_MAIN);
	// row 0
	btn_main_comp = gui_make_btn(parent, "COMP", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_comp, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);

	btn_main_wah = gui_make_btn(parent, "WAH", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_wah, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

	btn_main_boost = gui_make_btn(parent, "BOOST", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_boost, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);	

	btn_main_oct = gui_make_btn(parent, "OCTAVE", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_oct, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

	// row 1
	lv_obj_update_layout(btn_main_comp);
	uint8_t btn_h = lv_obj_get_height(btn_main_comp);
	dd_main_ampModel = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_main_ampModel, ampModels);
	lv_obj_add_style(dd_main_ampModel, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_dropdown_set_dir(dd_main_ampModel, LV_DIR_BOTTOM);
	lv_obj_set_size(dd_main_ampModel, BTN_MAIN_W*2, btn_h);
	lv_obj_set_style_pad_top(dd_main_ampModel, 8, LV_PART_MAIN);
	lv_obj_set_grid_cell(dd_main_ampModel, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
	lv_obj_add_event_cb(dd_main_ampModel, dd_main_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

	dd_main_cabModel = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_main_cabModel, cabModels);
	lv_dropdown_set_dir(dd_main_cabModel, LV_DIR_BOTTOM);
	lv_obj_add_style(dd_main_cabModel, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_size(dd_main_cabModel, BTN_MAIN_W*2, btn_h);
	lv_obj_set_style_pad_top(dd_main_cabModel, 8, LV_PART_MAIN);
	lv_obj_set_grid_cell(dd_main_cabModel, LV_GRID_ALIGN_CENTER, 2, 2, LV_GRID_ALIGN_CENTER, 1, 1);
	lv_obj_add_event_cb(dd_main_cabModel, dd_main_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

	//row 2
	btn_main_delay = gui_make_btn(parent, "DELAY", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_delay, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

	btn_main_reverb = gui_make_btn(parent, "REVERB", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_reverb, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

	btn_main_doubler = gui_make_btn(parent, "DOUBLER", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_doubler, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);	

	btn_main_dry = gui_make_btn(parent, "DRY", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_dry, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);	

	// row 3
	btn_main_delay_freeze = gui_make_btn(parent, "FREEZE", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_delay_freeze, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

	btn_main_reverb_freeze = gui_make_btn(parent, "FREEZE", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_reverb_freeze, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

	btn_main_setup = gui_make_btn(parent, "SETUP", BTN_MAIN_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_setup, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

	btn_main_bypass = gui_make_btn(parent, "BYPASS", BTN_MAIN_W, LV_SIZE_CONTENT, true, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_bypass, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);	

	// row 4
	btn_main_presetDown = gui_make_btn(parent, "", BTN_MAIN_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_set_style_bg_image_src(btn_main_presetDown, LV_SYMBOL_MINUS, 0);
	lv_obj_set_grid_cell(btn_main_presetDown, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

	btn_main_presetUp = gui_make_btn(parent, "", BTN_MAIN_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_set_style_bg_image_src(btn_main_presetUp, LV_SYMBOL_PLUS, 0);
	lv_obj_set_grid_cell(btn_main_presetUp, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

	btn_main_load = gui_make_btn(parent, "PR.LOAD", BTN_MAIN_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_load, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);	

	btn_main_save = gui_make_btn(parent, "PR.SAVE", BTN_MAIN_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_set_grid_cell(btn_main_save, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);	

	// row 5
	label_presetNo = lv_label_create(parent);
	lv_label_set_text_fmt(label_presetNo, "%d", 0);
	lv_obj_set_grid_cell(label_presetNo, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
	lv_obj_set_style_text_font(label_presetNo, &font_eg32, 0);

	label_presetName = lv_label_create(parent);
	lv_label_set_text_fmt(label_presetName, "%s", "");
	lv_obj_set_style_text_font(label_presetName, &font_eg32, 0);
	lv_obj_set_style_pad_left(label_presetName, 8, 0);
	lv_obj_set_grid_cell(label_presetName, LV_GRID_ALIGN_START, 1, 3, LV_GRID_ALIGN_CENTER, 5, 1);

	// bottom status line
	label_main_footer = lv_label_create(parent);
	lv_label_set_text(label_main_footer, "TGX4 V2.0 HEXEFX.COM ");
	lv_obj_set_style_text_font(label_main_footer, &font_eg14, 0);
	lv_obj_set_style_text_color(label_main_footer, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
	lv_obj_set_grid_cell(label_main_footer, LV_GRID_ALIGN_START, 0, 3, LV_GRID_ALIGN_END, 5, 1);
	lv_obj_set_style_pad_left(label_main_footer, 8, 0);

	// ------------------ SETUP WINDOW -----------------------------
	win_setup = lv_win_create(parent);
	lv_obj_add_flag(win_setup, LV_OBJ_FLAG_HIDDEN);
	lv_win_add_title(win_setup, "SETUP");
	lv_obj_add_flag(win_setup, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_set_size(win_setup, LV_PCT(98), LV_PCT(98));
	lv_obj_set_style_border_width(win_setup, 1, 0);
	lv_obj_set_style_border_color(win_setup, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
	lv_obj_t* win_header = lv_win_get_header(win_setup);
	lv_obj_set_height(win_header, 30);
	lv_obj_center(win_setup);

	lv_obj_t* win_cont = lv_win_get_content(win_setup);
	lv_obj_set_flex_flow(win_cont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(win_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
	lv_obj_set_style_pad_column(win_cont, 7, LV_PART_MAIN);
	lv_obj_set_style_pad_row(win_cont, 5, LV_PART_MAIN);
	lv_obj_set_style_pad_all(win_cont, 3, 0);

	cont = gui_make_roller(win_cont, &rl_pot1Assign, potAssignTable, "Pot1 Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	cont = gui_make_roller(win_cont, &rl_pot2Assign, potAssignTable, "Pot2 Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	cont = gui_make_roller(win_cont, &rl_pot3Assign, potAssignTable, "Pot3 Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	cont = gui_make_roller(win_cont, &rl_ExpAssign, potAssignTable, "Exp Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	cont = gui_make_roller(win_cont, &rl_inType, inTypes, "Input Select", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	lv_roller_set_options(rl_inType, inTypes, LV_ROLLER_MODE_INFINITE);
	cont = gui_make_roller(win_cont, &rl_ft1Assign, ftAssignTable, "FT1 Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);
	cont = gui_make_roller(win_cont, &rl_ft2Assign, ftAssignTable, "FT2 Assign", BTN_SETUP_W, LV_SIZE_CONTENT, rl_setup_event_cb);

	btn_setup_close = gui_make_btn(win_cont, "CANCEL", BTN_SETUP_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_add_flag(btn_setup_close, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_setup_close, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
	
	btn_setup_save = gui_make_btn(win_cont, "SAVE", BTN_SETUP_W, LV_SIZE_CONTENT, false, btn_main_event_cb);
	lv_obj_add_flag(btn_setup_save, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align_to(btn_setup_save, btn_setup_close, LV_ALIGN_OUT_TOP_MID, 0, -7);
	lv_obj_set_style_bg_color(btn_setup_save, lv_palette_main(LV_PALETTE_RED), 0);

	// --- PRESET SAVE WINDOW ---
	win_presetSaveDialog = lv_win_create(parent);
	lv_obj_add_flag(win_presetSaveDialog, LV_OBJ_FLAG_HIDDEN);
	lv_win_add_title(win_presetSaveDialog, "PRESET SAVE");
	lv_obj_add_flag(win_presetSaveDialog, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_set_size(win_presetSaveDialog, LV_PCT(98), LV_PCT(98));
	lv_obj_set_style_border_width(win_presetSaveDialog, 1, 0);
	lv_obj_set_style_border_color(win_presetSaveDialog, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
	win_header = lv_win_get_header(win_presetSaveDialog);
	lv_obj_set_height(win_header, 30);
	lv_obj_center(win_presetSaveDialog);

	win_cont = lv_win_get_content(win_presetSaveDialog);
	lv_obj_set_style_pad_all(win_cont, 3, 0);

	kbd_txtArea = lv_textarea_create(win_cont);
	lv_textarea_set_one_line(kbd_txtArea, true);
	lv_obj_set_size(kbd_txtArea, LV_PCT(96), 40);
    lv_obj_align(kbd_txtArea, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(kbd_txtArea, &font_eg14, 0);
	lv_textarea_set_max_length(kbd_txtArea, 14); // limit the preset name length to 14 chars
	
    lv_textarea_set_placeholder_text(kbd_txtArea, "Preset Name");

	kbd = lv_keyboard_create(win_cont);
	lv_keyboard_set_textarea(kbd, kbd_txtArea);
	lv_keyboard_set_map(kbd, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(kbd, LV_KEYBOARD_MODE_USER_1);
	lv_obj_set_size(kbd, LV_PCT(96), LV_PCT(75));

	lv_obj_add_event_cb(kbd_txtArea, presetSave_event_cb, LV_EVENT_ALL, kbd);

}


// ----------------------------- COMP TAB ----------------------------------------------------
void comp_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* cont;
	lv_obj_add_style(parent, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
	static int32_t column_dsc[] = {TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, LV_GRID_TEMPLATE_LAST};
	static int32_t row_dsc[] = {TAB_TOP_ROW_H, DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_style_grid_column_dsc_array(parent, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(parent, row_dsc, 0);
	lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);

	btn_comp = gui_make_btn(parent, "OFF", TOP_BTN_W, TOP_BTN_H, true, btn_comp_event_cb);
	lv_obj_add_flag(btn_comp, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_comp, LV_ALIGN_TOP_LEFT, 10, 10);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_PREGAIN], &sliderLabels[PRESET_PARAM_COMP_PREGAIN], 
			"Pre\nGain", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_THRES], &sliderLabels[PRESET_PARAM_COMP_THRES],
					  "\nThres.", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_RATIO], &sliderLabels[PRESET_PARAM_COMP_RATIO],
					  "\nRatio", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);					  
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_ATTACK], &sliderLabels[PRESET_PARAM_COMP_ATTACK],
					  "\nAttack",TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_RELEASE], &sliderLabels[PRESET_PARAM_COMP_RELEASE],
					  "\nRelease",TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_COMP_POSTGAIN], &sliderLabels[PRESET_PARAM_COMP_POSTGAIN],
					  "Post\nGain", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_comp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 5, 1, LV_GRID_ALIGN_START, 1, 1);
}


// -------------------------- PREAMP TAB (WAH + BOOST + OCT UP) -----------------------------------------------
void preamp_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* cont;
	lv_obj_add_style(parent, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
	static int32_t column_dsc[] = {400/7, 400/7, 400/7, 400/7, 400/7, 400/7, 400/7, LV_GRID_TEMPLATE_LAST};
	static int32_t row_dsc[] = {TAB_TOP_ROW_H, DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_style_grid_column_dsc_array(parent, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(parent, row_dsc, 0);
	lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);

	btn_wah = gui_make_btn(parent, "WAH", BTN_MAIN_W, TOP_BTN_H, true, btn_preamp_event_cb);
	lv_obj_add_flag(btn_wah, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_wah, LV_ALIGN_TOP_LEFT, 7, 10);	

	dd_wahModel = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_wahModel, WAHModels);
	lv_dropdown_set_dir(dd_wahModel, LV_DIR_BOTTOM);
	lv_obj_add_style(dd_wahModel, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(dd_wahModel, dd_wahSelect_cb, LV_EVENT_VALUE_CHANGED, NULL);

	lv_obj_set_size(dd_wahModel, BTN_MAIN_W, TOP_BTN_H);
	lv_obj_add_flag(dd_wahModel, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align_to(dd_wahModel, btn_wah, LV_ALIGN_OUT_RIGHT_MID, 7, 0);

	btn_boost = gui_make_btn(parent, "BOOST", BTN_MAIN_W, TOP_BTN_H, true, btn_preamp_event_cb);
	lv_obj_add_flag(btn_boost, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align_to(btn_boost, dd_wahModel, LV_ALIGN_OUT_RIGHT_MID, 7, 0);

	btn_octave = gui_make_btn(parent, "OCTAVE", BTN_MAIN_W, TOP_BTN_H, true, btn_preamp_event_cb);
	lv_obj_add_flag(btn_octave, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_octave, LV_ALIGN_TOP_RIGHT, -10, 10);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_WAH_FREQ], &sliderLabels[PRESET_PARAM_WAH_FREQ],
					  " Wah ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_WAH_MIX], &sliderLabels[PRESET_PARAM_WAH_MIX],
					  "WahMix", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_BOOST_DRIVE], &sliderLabels[PRESET_PARAM_BOOST_DRIVE],
					  " Drive ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_BOOST_BOTTOM], &sliderLabels[PRESET_PARAM_BOOST_BOTTOM],
					  "  Btm  ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_BOOST_PRESENCE], &sliderLabels[PRESET_PARAM_BOOST_PRESENCE],
					  " Pres ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_BOOST_MIX], &sliderLabels[PRESET_PARAM_BOOST_MIX],
					  "  Mix  ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 5, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_BOOST_LEVEL], &sliderLabels[PRESET_PARAM_BOOST_LEVEL],
					  " Level ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_preamp_event_cb);	
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 6, 1, LV_GRID_ALIGN_START, 1, 1);					  				  
}
// ------------------------------------ AMP TAB ---------------------------------------------
void amp_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* cont;
	lv_obj_add_style(parent, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
	static int32_t column_dsc[] = {	TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, TAB_AMP_COL_W, 40, LV_GRID_TEMPLATE_LAST}; 
	static int32_t row_dsc[] = {TAB_TOP_ROW_H, DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_style_grid_column_dsc_array(parent, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(parent, row_dsc, 0);
	lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);

	dd_ampModel = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_ampModel, ampModels);
	lv_obj_add_style(dd_ampModel, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_dropdown_set_dir(dd_ampModel, LV_DIR_BOTTOM);
	lv_obj_add_event_cb(dd_ampModel, dd_ampSelect_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_add_flag(dd_ampModel, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(dd_ampModel, LV_ALIGN_TOP_LEFT, 10, 10);	
	lv_obj_set_height(dd_ampModel, TOP_BTN_H);

	dd_cabModel = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_cabModel, cabModels);
	lv_obj_add_style(dd_cabModel, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_dropdown_set_dir(dd_cabModel, LV_DIR_BOTTOM);
	lv_obj_add_event_cb(dd_cabModel, dd_cabSelect_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_add_flag(dd_cabModel, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(dd_cabModel, LV_ALIGN_TOP_MID, 15, 10);

	btn_doubler = gui_make_btn(parent, "DOUBLER", TOP_BTN_W, TOP_BTN_H, true, btn_amp_event_cb);
	lv_obj_add_flag(btn_doubler, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_doubler, LV_ALIGN_TOP_RIGHT, -10, 10);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_GATE], &sliderLabels[PRESET_PARAM_AMP_GATE],
					  "Gate", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_GAIN], &sliderLabels[PRESET_PARAM_AMP_GAIN],
					  "Gain", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_BASS],  &sliderLabels[PRESET_PARAM_AMP_BASS],
					  "Bass", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 1, 1);	
	
	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_MID], &sliderLabels[PRESET_PARAM_AMP_MID],
					  " Mid ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_TREBLE], &sliderLabels[PRESET_PARAM_AMP_TREBLE],
					  "Treble", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_AMP_FXMIX], &sliderLabels[PRESET_PARAM_AMP_FXMIX],
					  "FxMix", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 5, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_REVERB_MIX], &sliderLabels[PRESET_PARAM_DELAY_REVERB_MIX],
					  "Dly/Rvb", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 6, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_MASTER_VOLUME], &sliderLabels[PRESET_PARAM_MASTER_VOLUME],
					  " Vol ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 7, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_MASTER_PAN], &sliderLabels[PRESET_PARAM_MASTER_PAN],
					  " Pan ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 8, 1, LV_GRID_ALIGN_START, 1, 1);	

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_MASTER_LOWCUT], &sliderLabels[PRESET_PARAM_MASTER_LOWCUT],
					  " Bcut ", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_amp_event_cb);
	lv_obj_set_style_pad_right(cont, 10, 0);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 9, 1, LV_GRID_ALIGN_START, 1, 1);	
	
}
// ---------------------------------------DELAY TAB ------------------------------------------
void delay_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* cont;
	lv_obj_add_style(parent, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
	static int32_t column_dsc[] = {	TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, 20, LV_GRID_TEMPLATE_LAST}; 
	static int32_t row_dsc[] = {TAB_TOP_ROW_H, DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST}; 
    lv_obj_set_style_grid_column_dsc_array(parent, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(parent, row_dsc, 0);
	lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);

	btn_delay = gui_make_btn(parent, "OFF", TOP_BTN_W, TOP_BTN_H, true, btn_delay_event_cb);
	lv_obj_add_flag(btn_delay, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_delay, LV_ALIGN_TOP_LEFT, 10, 10);

	btn_delay_freeze = gui_make_btn(parent, "FREEZE", TOP_BTN_W, TOP_BTN_H, true, btn_delay_event_cb);
	lv_obj_add_flag(btn_delay_freeze, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_delay_freeze, LV_ALIGN_TOP_MID, 0, 10);
	
	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_TIME], &sliderLabels[PRESET_PARAM_DELAY_TIME],
					  "Time", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_INERTIA], &sliderLabels[PRESET_PARAM_DELAY_INERTIA],
					  "Inertia", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_REPEATS],&sliderLabels[PRESET_PARAM_DELAY_REPEATS],
					  "Repeats", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_MODRATE], &sliderLabels[PRESET_PARAM_DELAY_MODRATE],
					  "ModRate", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_MODDEPTH], &sliderLabels[PRESET_PARAM_DELAY_MODDEPTH],
					  "ModDepth", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_HICUT], &sliderLabels[PRESET_PARAM_DELAY_HICUT],
					  "HiCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 5, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_LOWCUT], &sliderLabels[PRESET_PARAM_DELAY_LOWCUT],
					  "LowCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 6, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_TREBLE],  &sliderLabels[PRESET_PARAM_DELAY_TREBLE],
					  "Treble", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 7, 1, LV_GRID_ALIGN_START, 1, 1);

	cont = gui_make_slider(parent, &sliders[PRESET_PARAM_DELAY_BASS], &sliderLabels[PRESET_PARAM_DELAY_BASS],
					  "Bass", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_delay_event_cb);
	lv_obj_set_style_pad_right(cont, 10, 0);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 8, 1, LV_GRID_ALIGN_START, 1, 1);
}
// --------------------------------------- REVERB TAB -----------------------------------------------------
void reverb_tab_create(lv_obj_t* parent)
{
	lv_obj_t* lbl,* cont;
	lv_obj_add_style(parent, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_layout(parent, LV_LAYOUT_GRID);
	static int32_t col_dsc_subPL[] = {	TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 
									TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, 20, LV_GRID_TEMPLATE_LAST}; 
	
	static int32_t col_dsc_subSP[] = {	400/3, 400/3, 400/3, LV_GRID_TEMPLATE_LAST}; 
	static int32_t col_dsc_subSC[] = {	400/2, 400/2, LV_GRID_TEMPLATE_LAST};	
	static int32_t row_dsc_sub[] = {DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST};
	static int32_t row_dsc_main[] = {TAB_TOP_ROW_H, DISPLAY_VER_RES-TAB_TOP_ROW_H, LV_GRID_TEMPLATE_LAST}; 
	static int32_t col_dsc_main[] = {TAB_AMP_COL_W, TAB_AMP_COL_W,TAB_AMP_COL_W, TAB_AMP_COL_W, TAB_AMP_COL_W, (480 - TAB_AMP_COL_W*5), LV_GRID_TEMPLATE_LAST};	

	lv_obj_set_style_grid_column_dsc_array(parent, col_dsc_main, 0);
	lv_obj_set_style_grid_row_dsc_array(parent, row_dsc_main, 0);
	lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);

	btn_reverb = gui_make_btn(parent, "OFF", TOP_BTN_W, TOP_BTN_H, true, btn_reverb_event_cb);
	lv_obj_add_flag(btn_reverb, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_reverb, LV_ALIGN_TOP_LEFT, 10, 10);

	btn_reverbPLSC_freeze = gui_make_btn(parent, "FREEZE", TOP_BTN_W, TOP_BTN_H, true, btn_reverb_event_cb);
	lv_obj_add_flag(btn_reverbPLSC_freeze, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(btn_reverbPLSC_freeze, LV_ALIGN_TOP_MID, -15, 10);

	lv_obj_update_layout(btn_reverb);
	uint8_t btn_h = lv_obj_get_height(btn_reverb);

	dd_reverb_type = lv_dropdown_create(parent);
	lv_dropdown_set_options_static(dd_reverb_type, reverbTypes);
	lv_obj_add_style(dd_reverb_type, &style_dd, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_dropdown_set_dir(dd_reverb_type, LV_DIR_BOTTOM);
	lv_obj_set_size(dd_reverb_type, TOP_BTN_W*1.3, btn_h);
	lv_obj_add_event_cb(dd_reverb_type, dd_reverbSelect_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_add_flag(dd_reverb_type, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(dd_reverb_type, LV_ALIGN_TOP_RIGHT, -10, 10);

    tabview_reverb = lv_tabview_create(parent);
    lv_tabview_set_tab_bar_position(tabview_reverb, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview_reverb, 0);
	lv_obj_set_size(tabview_reverb, LV_PCT(100), TAB_SLIDER_HEIGHT+15);
	lv_obj_set_grid_cell(tabview_reverb, LV_GRID_ALIGN_START, 0, 6, LV_GRID_ALIGN_START, 1, 1);
	lv_obj_clear_flag(lv_tabview_get_content(tabview_reverb), LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_opa(tabview_reverb, LV_OPA_TRANSP, 0);

	tab_reverbPL = lv_tabview_add_tab(tabview_reverb, "");
	lv_obj_set_layout(tab_reverbPL, LV_LAYOUT_GRID);
	lv_obj_set_style_grid_column_dsc_array(tab_reverbPL, col_dsc_subPL, 0);
	lv_obj_set_style_grid_row_dsc_array(tab_reverbPL, row_dsc_sub, 0);
	lv_obj_set_style_pad_column(tab_reverbPL, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(tab_reverbPL, 0, LV_PART_MAIN);
	lv_obj_add_style(tab_reverbPL, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);

	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_SIZE], &sliderLabels[PRESET_PARAM_REVERBPL_SIZE],
					  "Time", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_DIFF], &sliderLabels[PRESET_PARAM_REVERBPL_DIFF],
					  "Diff", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_TREBLECUT],  &sliderLabels[PRESET_PARAM_REVERBPL_TREBLECUT],
					  "HiCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_BASSCUT], &sliderLabels[PRESET_PARAM_REVERBPL_BASSCUT],
					  "LowCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_TREBLE], &sliderLabels[PRESET_PARAM_REVERBPL_TREBLE],
					  "Treble", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_BASS], &sliderLabels[PRESET_PARAM_REVERBPL_BASS],
					  "Bass", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 5, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_BLEEDIN], &sliderLabels[PRESET_PARAM_REVERBPL_BLEEDIN],
					  "BleedIn", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 6, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_PITCH], &sliderLabels[PRESET_PARAM_REVERBPL_PITCH],
					  "Pitch", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 7, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_PITCHMIX], &sliderLabels[PRESET_PARAM_REVERBPL_PITCHMIX],
					  "PitchMix", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 8, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_SHIMMER], &sliderLabels[PRESET_PARAM_REVERBPL_SHIMMER],
					  "Shimmer", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 9, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbPL, &sliders[PRESET_PARAM_REVERBPL_PITCHSHIMM], &sliderLabels[PRESET_PARAM_REVERBPL_PITCHSHIMM],
					  "PitchSh", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_style_pad_right(cont, 10, 0);					  
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 10, 1, LV_GRID_ALIGN_START, 0, 1);

	tab_reverbSC = lv_tabview_add_tab(tabview_reverb, "");
	lv_obj_set_layout(tab_reverbSC, LV_LAYOUT_GRID);
	lv_obj_set_style_grid_column_dsc_array(tab_reverbSC, col_dsc_subSC, 0);
	lv_obj_set_style_grid_row_dsc_array(tab_reverbSC, row_dsc_sub, 0);
	lv_obj_set_style_pad_column(tab_reverbSC, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(tab_reverbSC, 0, LV_PART_MAIN);
	lv_obj_add_style(tab_reverbSC, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);

	cont = gui_make_slider(tab_reverbSC, &sliders[PRESET_PARAM_REVERBSC_TIME], &sliderLabels[PRESET_PARAM_REVERBSC_TIME],
					  "Time", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbSC, &sliders[PRESET_PARAM_REVERBSC_LOWPASSF], &sliderLabels[PRESET_PARAM_REVERBSC_LOWPASSF],
					  "HiCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 0, 1);

	tab_reverbSP = lv_tabview_add_tab(tabview_reverb, "");
	lv_obj_set_layout(tab_reverbSP, LV_LAYOUT_GRID);
	lv_obj_set_style_grid_column_dsc_array(tab_reverbSP, col_dsc_subSP, 0);
	lv_obj_set_style_grid_row_dsc_array(tab_reverbSP, row_dsc_sub, 0);
	lv_obj_set_style_pad_column(tab_reverbSP, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_row(tab_reverbSP, 0, LV_PART_MAIN);
	lv_obj_add_style(tab_reverbSP, &style_tab, LV_PART_MAIN | LV_STATE_DEFAULT);

	cont = gui_make_slider(tab_reverbSP, &sliders[PRESET_PARAM_REVERBSP_TIME], &sliderLabels[PRESET_PARAM_REVERBSP_TIME],
					  "Time", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbSP, &sliders[PRESET_PARAM_REVERBSP_TREBLECUT], &sliderLabels[PRESET_PARAM_REVERBSP_TREBLECUT],
					  "HiCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 0, 1);
	cont = gui_make_slider(tab_reverbSP, &sliders[PRESET_PARAM_REVERBSP_BASSCUT], &sliderLabels[PRESET_PARAM_REVERBSP_BASSCUT],
					  "LowCut", TAB_SLIDER_WIDTH, TAB_SLIDER_HEIGHT, slider_reverb_event_cb);
	lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 0, 1);
}
// ---------------------------------------------------------------------------------
void gui_show_popup(const char* msg, uint32_t timeout_ms)
{
	lv_label_set_text(popup_box_msg, msg);
	lv_obj_remove_flag(popup_box, LV_OBJ_FLAG_HIDDEN);
	lv_obj_move_foreground(popup_box);
	popupTimestamp = millis();
	popupTime = timeout_ms;
}
// ---------------------------------------------------------------------------------
lv_obj_t* gui_make_slider(lv_obj_t* parent, lv_obj_t**slider, lv_obj_t **label_val, const char* name, uint32_t w, uint32_t h, lv_event_cb_t cb)
{
	lv_obj_t* lbl,*slider_cont;
	slider_cont = lv_obj_create(parent);
	lv_obj_add_style(slider_cont, &style_slider, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_size(slider_cont, LV_SIZE_CONTENT, h);
	lv_obj_set_flex_flow(slider_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(slider_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lbl = lv_label_create(slider_cont);
	lv_label_set_text_static(lbl, name);
	*label_val = lv_label_create(slider_cont);
	*slider = lv_slider_create(slider_cont);
	lv_obj_set_size(*slider, w, LV_PCT(70));
	lv_obj_set_style_pad_top(*slider, 5, 0);
	lv_label_set_text(*label_val, "0.0");
	if (cb)  lv_obj_add_event_cb(*slider, cb, LV_EVENT_VALUE_CHANGED, NULL);

	lv_obj_set_style_radius(*slider, 3, LV_PART_KNOB);
	lv_obj_set_style_pad_hor(*slider, 8, LV_PART_KNOB);
	lv_obj_set_ext_click_area(*slider, 8);
	return slider_cont;
}
// ---------------------------------------------------------------------------------
lv_obj_t* gui_make_roller(lv_obj_t* parent, lv_obj_t**roller, const char* options, const char* name, uint32_t w, uint32_t h, lv_event_cb_t cb)
{
	lv_obj_t* lbl,*cont;

	cont = lv_obj_create(parent);
	lv_obj_add_style(cont, &style_sub_container, 0);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_pad_hor(cont, 5, 0);
	lbl = lv_label_create(cont);
	lv_label_set_text_static(lbl, name);
	lv_obj_set_style_pad_all(lbl, 0, 0);
	*roller = lv_roller_create(cont);
	lv_roller_set_options(*roller , options, LV_ROLLER_MODE_INFINITE);
	lv_obj_set_size(*roller , w, h);	
	lv_obj_set_style_text_font(*roller , &font_eg12, 0);
	lv_roller_set_visible_row_count(*roller , 3);
	lv_obj_add_event_cb(*roller , cb, LV_EVENT_VALUE_CHANGED, NULL);	

	return cont;
}
// ---------------------------------------------------------------------------------
lv_obj_t* gui_make_btn(lv_obj_t* parent, const char* name, int32_t w, int32_t h, bool toggle, lv_event_cb_t cb)
{
	lv_obj_t* lbl,* btn;
	btn = lv_btn_create(parent);
	if (toggle) lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(btn, w, h);
	if (name)
	{
		lbl = lv_label_create(btn);
		lv_label_set_text_static(lbl, name);
		lv_obj_center(lbl);
	}
	if (cb) lv_obj_add_event_cb(btn, cb, LV_EVENT_ALL, NULL);

	return btn; 
}
// ---------------------------------------------------------------------------------

void set_linked_btn(lv_obj_t* srcBtn, lv_obj_t* destBtn, const char* labelOn, const char* labelOff, btn_label_change_t labelChange)
{
	lv_state_t state = lv_obj_get_state(srcBtn);
	lv_obj_t* lbl;
	const char* newLabel = "";
	if (state & LV_STATE_CHECKED)	
	{
		newLabel = labelOn;
		lv_obj_set_state(destBtn, LV_STATE_CHECKED, true);	
	}
	else
	{
		newLabel = labelOff;
		lv_obj_set_state(destBtn, LV_STATE_CHECKED, false);
	}	
	switch(labelChange)
	{
		case CHANGE_BTN_LABEL_NONE: break;
		case CHANGE_BTN_LABEL_SRC:
			lbl = lv_obj_get_child(srcBtn, 0); // get label pointer
			lv_label_set_text_static(lbl, newLabel);
			break;
		case CHANGE_BTN_LABEL_DST:
			lbl = lv_obj_get_child(destBtn, 0);
			lv_label_set_text_static(lbl, newLabel);
			break;
		case CHANGE_BTN_LABEL_BOTH:
			lbl = lv_obj_get_child(srcBtn, 0); // get label pointer
			lv_label_set_text_static(lbl, newLabel);
			lbl = lv_obj_get_child(destBtn, 0);
			lv_label_set_text_static(lbl, newLabel);
			break;
		default: break;
	}
}
// ---------------------------------------------------------------------------------
static void btn_comp_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);

	if ( code == LV_EVENT_VALUE_CHANGED)
	{
		set_linked_btn(obj, btn_main_comp, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_SRC);
		teensy.setFlag(PRESET_FLAG_COMP_EN, state & LV_STATE_CHECKED ? 1 : 0);
	}
}
// ---------------------------------------------------------------------------------
static void btn_preamp_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);
	if ( code == LV_EVENT_VALUE_CHANGED)
	{
		if (obj == btn_boost) 		
		{
			set_linked_btn(obj, btn_main_boost, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_BOOST_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
		else if (obj == btn_octave)	
		{
			set_linked_btn(obj, btn_main_oct, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_OCTAVE_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
		else if (obj == btn_wah)	
		{
			set_linked_btn(obj, btn_main_wah, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_WAH_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}		
	}
}
// ---------------------------------------------------------------------------------
static void btn_amp_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);
	if (code == LV_EVENT_VALUE_CHANGED)
	{
		if (obj == btn_doubler)	set_linked_btn(obj, btn_main_doubler, NULL, NULL, CHANGE_BTN_LABEL_NONE);
		teensy.setFlag(PRESET_FLAG_DOUBLER_EN, state & LV_STATE_CHECKED ? 1 : 0);
	}
}
// ---------------------------------------------------------------------------------
static void btn_delay_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);
	if (code == LV_EVENT_VALUE_CHANGED)
	{
		if (obj == btn_delay) 				
		{
			set_linked_btn(obj, btn_main_delay, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_SRC);
			teensy.setFlag(PRESET_FLAG_DELAY_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
		else if (obj == btn_delay_freeze)	
		{
			set_linked_btn(obj, btn_main_delay_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_DELAY_FREEZE_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
	}
}
// ---------------------------------------------------------------------------------
static void btn_reverb_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);
	if (code == LV_EVENT_VALUE_CHANGED)
	{
		if (obj == btn_reverb) 					
		{
			set_linked_btn(obj, btn_main_reverb, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_SRC);
			teensy.setFlag(PRESET_FLAG_REVB_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
		else if (obj == btn_reverbPLSC_freeze)	
		{
			set_linked_btn(obj, btn_main_reverb_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_REVB_FREEZE_EN, state & LV_STATE_CHECKED ? 1 : 0);
		}
	}
}
// ---------------------------------------------------------------------------------
static void btn_main_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	lv_state_t state = lv_obj_get_state(obj);
	lv_obj_t* lbl;
	uint8_t pressed = state & LV_STATE_CHECKED;
	// Toggle buttons
	if ( code == LV_EVENT_VALUE_CHANGED)
	{
		if (obj == btn_main_comp)
		{
			set_linked_btn(obj, btn_comp, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			teensy.setFlag(PRESET_FLAG_COMP_EN, pressed);
		}
		else if (obj == btn_main_wah)
		{
			set_linked_btn(obj, btn_wah, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_WAH_EN, pressed);
		}		
		else if (obj == btn_main_boost)
		{
			set_linked_btn(obj, btn_boost, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_BOOST_EN, pressed);
		}	
		else if (obj == btn_main_oct)
		{
			set_linked_btn(obj, btn_octave, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_OCTAVE_EN, pressed);
		}
		else if (obj == btn_main_doubler)
		{
			set_linked_btn(obj, btn_doubler, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_DOUBLER_EN, pressed);
		}
		else if (obj == btn_main_delay)
		{
			set_linked_btn(obj, btn_delay, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			teensy.setFlag(PRESET_FLAG_DELAY_EN, pressed);
		}
		else if (obj == btn_main_delay_freeze)
		{
			set_linked_btn(obj, btn_delay_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_DELAY_FREEZE_EN, pressed);
		}
		else if (obj == btn_main_reverb)
		{
			set_linked_btn(obj, btn_reverb, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			teensy.setFlag(PRESET_FLAG_REVB_EN, pressed);
		}
		else if (obj == btn_main_reverb_freeze)
		{
			set_linked_btn(obj, btn_reverbPLSC_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);
			teensy.setFlag(PRESET_FLAG_REVB_FREEZE_EN, pressed);
		}
		else if (obj == btn_main_bypass)
		{
			teensy.setFlag(PRESET_FLAG_BYPASS, pressed);
		}		
		else if (obj == btn_main_dry)
		{
			teensy.setFlag(PRESET_FLAG_DRYSIG, pressed);
		}			
	}
	if ( code == LV_EVENT_CLICKED)
	{
		if (obj == btn_main_presetUp || obj == btn_main_presetDown)		// --- Preset Up/Down ---
		{	
			obj == btn_main_presetUp ? teensy.presetUp() : teensy.presetDown();
		}
		else if (obj == btn_setup_close)
		{
			lv_obj_add_flag(win_setup, LV_OBJ_FLAG_HIDDEN);
			uint8_t val = teensy.getFlag(PRESET_FLAG_POT1_ASSIGN);
			lv_roller_set_selected(rl_pot1Assign, val, LV_ANIM_OFF);
			val = teensy.getFlag(PRESET_FLAG_POT2_ASSIGN);
			lv_roller_set_selected(rl_pot2Assign, val, LV_ANIM_OFF);
			val = teensy.getFlag(PRESET_FLAG_POT3_ASSIGN);
			lv_roller_set_selected(rl_pot3Assign, val, LV_ANIM_OFF);
			val = teensy.getFlag(PRESET_FLAG_EXP_ASSIGN);
			lv_roller_set_selected(rl_ExpAssign, val, LV_ANIM_OFF);
			val = teensy.getFlag(PRESET_FLAG_INSELECT);
			lv_roller_set_selected(rl_inType, val, LV_ANIM_OFF);
			val = teensy.getFlag(PRESET_FLAG_FT1_ASSIGN);
			lv_roller_set_selected(rl_ft1Assign, val, LV_ANIM_OFF);			
			val = teensy.getFlag(PRESET_FLAG_FT2_ASSIGN);
			lv_roller_set_selected(rl_ft2Assign, val, LV_ANIM_OFF);	
		}
		else if (obj == btn_setup_save)
		{
			uint8_t val = lv_roller_get_selected(rl_pot1Assign);
			teensy.setFlag(PRESET_FLAG_POT1_ASSIGN, val);
			val = lv_roller_get_selected(rl_pot2Assign);
			teensy.setFlag(PRESET_FLAG_POT2_ASSIGN, val);
			val = lv_roller_get_selected(rl_pot3Assign);
			teensy.setFlag(PRESET_FLAG_POT3_ASSIGN, val);
			val = lv_roller_get_selected(rl_ExpAssign);
			teensy.setFlag(PRESET_FLAG_EXP_ASSIGN, val);
			val = lv_roller_get_selected(rl_inType);
			teensy.setFlag(PRESET_FLAG_INSELECT, val);
			val = lv_roller_get_selected(rl_ft1Assign);
			teensy.setFlag(PRESET_FLAG_FT1_ASSIGN, val);
			val = lv_roller_get_selected(rl_ft2Assign);
			teensy.setFlag(PRESET_FLAG_FT2_ASSIGN, val);	
			lv_obj_add_flag(win_setup, LV_OBJ_FLAG_HIDDEN);
			gui_show_popup("SETUP SAVED", 1000);

		}		
		else if (obj == btn_main_setup)
		{
			lv_obj_remove_flag(win_setup, LV_OBJ_FLAG_HIDDEN);
		}
		else if (obj == btn_main_save)
		{
			lv_obj_t* win_header = lv_win_get_header(win_presetSaveDialog);
			lv_obj_t* lbl = lv_obj_get_child_by_type(win_header, 0, &lv_label_class);
			lv_label_set_text_fmt(lbl, "PRESET %d SAVE", teensy.getFlag(PRESET_FLAG_IDX)+1);
			lv_textarea_set_placeholder_text(kbd_txtArea, teensy.getPresetName());
			lv_obj_remove_flag(win_presetSaveDialog, LV_OBJ_FLAG_HIDDEN);
		}
		else if (obj == btn_main_load)
		{
			teensy.write(GUI_CMD_PRESET_RELOAD, NULL, 0);
		}
	}
}	
// ---------------------------------------------------------------------------------

static void slider_update_valueLabel(lv_obj_t* slider, int32_t value)
{
	lv_obj_t* label = lv_obj_get_child(lv_obj_get_parent(slider), 1);
	if (label == NULL) return;
	float val = (float)value / 100.0f;
	char buf[6];
	snprintf(buf, sizeof(buf), " %1.2f ", val);
	lv_label_set_text(label, buf);
}

static void slider_comp_event_cb(lv_event_t* e)
{
	lv_obj_t* slider = (lv_obj_t* )lv_event_get_target(e);
	int32_t value = lv_slider_get_value(slider);
	slider_update_valueLabel(slider, value);
	if 		(slider == sliders[PRESET_PARAM_COMP_PREGAIN]) teensy.setParam(PRESET_PARAM_COMP_PREGAIN, value);
	else if (slider == sliders[PRESET_PARAM_COMP_THRES]) teensy.setParam(PRESET_PARAM_COMP_THRES, value);
	else if (slider == sliders[PRESET_PARAM_COMP_RATIO]) teensy.setParam(PRESET_PARAM_COMP_RATIO, value);
	else if (slider == sliders[PRESET_PARAM_COMP_ATTACK]) teensy.setParam(PRESET_PARAM_COMP_ATTACK, value);
	else if (slider == sliders[PRESET_PARAM_COMP_RELEASE]) teensy.setParam(PRESET_PARAM_COMP_RELEASE, value);
	else if (slider == sliders[PRESET_PARAM_COMP_POSTGAIN]) teensy.setParam(PRESET_PARAM_COMP_POSTGAIN, value);
}

static void slider_preamp_event_cb(lv_event_t* e)
{
	lv_obj_t* slider = (lv_obj_t* )lv_event_get_target(e);
	int32_t value = lv_slider_get_value(slider);
	slider_update_valueLabel(slider, value);
	if 		(slider == sliders[PRESET_PARAM_WAH_FREQ]) teensy.setParam(PRESET_PARAM_WAH_FREQ, value);
	else if (slider == sliders[PRESET_PARAM_WAH_MIX]) teensy.setParam(PRESET_PARAM_WAH_MIX, value);
	else if (slider == sliders[PRESET_PARAM_BOOST_DRIVE]) teensy.setParam(PRESET_PARAM_BOOST_DRIVE, value);
	else if (slider == sliders[PRESET_PARAM_BOOST_BOTTOM]) teensy.setParam(PRESET_PARAM_BOOST_BOTTOM, value);
	else if (slider == sliders[PRESET_PARAM_BOOST_PRESENCE]) teensy.setParam(PRESET_PARAM_BOOST_PRESENCE, value);
	else if (slider == sliders[PRESET_PARAM_BOOST_MIX]) teensy.setParam(PRESET_PARAM_BOOST_MIX, value);
	else if (slider == sliders[PRESET_PARAM_BOOST_LEVEL]) teensy.setParam(PRESET_PARAM_BOOST_LEVEL, value);
}
static void slider_amp_event_cb(lv_event_t* e)
{
	lv_obj_t* slider = (lv_obj_t* )lv_event_get_target(e);
	int32_t value = lv_slider_get_value(slider);
	slider_update_valueLabel(slider, value);
	if 		(slider == sliders[PRESET_PARAM_AMP_GATE]) teensy.setParam(PRESET_PARAM_AMP_GATE, value);
	else if (slider == sliders[PRESET_PARAM_AMP_GAIN]) teensy.setParam(PRESET_PARAM_AMP_GAIN, value);
	else if (slider == sliders[PRESET_PARAM_AMP_BASS]) teensy.setParam(PRESET_PARAM_AMP_BASS, value);
	else if (slider == sliders[PRESET_PARAM_AMP_MID]) teensy.setParam(PRESET_PARAM_AMP_MID, value);
	else if (slider == sliders[PRESET_PARAM_AMP_TREBLE]) teensy.setParam(PRESET_PARAM_AMP_TREBLE, value);
	else if (slider == sliders[PRESET_PARAM_AMP_FXMIX]) teensy.setParam(PRESET_PARAM_AMP_FXMIX, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_REVERB_MIX]) teensy.setParam(PRESET_PARAM_DELAY_REVERB_MIX, value);
	else if (slider == sliders[PRESET_PARAM_MASTER_VOLUME]) teensy.setParam(PRESET_PARAM_MASTER_VOLUME, value);
	else if (slider == sliders[PRESET_PARAM_MASTER_PAN]) teensy.setParam(PRESET_PARAM_MASTER_PAN, value);
	else if (slider == sliders[PRESET_PARAM_MASTER_LOWCUT]) teensy.setParam(PRESET_PARAM_MASTER_LOWCUT, value);
}
static void slider_delay_event_cb(lv_event_t* e)
{
	lv_obj_t* slider = (lv_obj_t* )lv_event_get_target(e);
	int32_t value = lv_slider_get_value(slider);
	slider_update_valueLabel(slider, value);
	if 		(slider == sliders[PRESET_PARAM_DELAY_TIME]) teensy.setParam(PRESET_PARAM_DELAY_TIME, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_INERTIA]) teensy.setParam(PRESET_PARAM_DELAY_INERTIA, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_REPEATS]) teensy.setParam(PRESET_PARAM_DELAY_REPEATS, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_MODRATE]) teensy.setParam(PRESET_PARAM_DELAY_MODRATE, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_MODDEPTH]) teensy.setParam(PRESET_PARAM_DELAY_MODDEPTH, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_HICUT]) teensy.setParam(PRESET_PARAM_DELAY_HICUT, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_LOWCUT]) teensy.setParam(PRESET_PARAM_DELAY_LOWCUT, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_TREBLE]) teensy.setParam(PRESET_PARAM_DELAY_TREBLE, value);
	else if (slider == sliders[PRESET_PARAM_DELAY_BASS]) teensy.setParam(PRESET_PARAM_DELAY_BASS, value);
}
static void slider_reverb_event_cb(lv_event_t* e)
{
	lv_obj_t* slider = (lv_obj_t* )lv_event_get_target(e);
	int32_t value = lv_slider_get_value(slider);
	slider_update_valueLabel(slider, value);
	if 		(slider == sliders[PRESET_PARAM_REVERBPL_SIZE]) teensy.setParam(PRESET_PARAM_REVERBPL_SIZE, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_DIFF]) teensy.setParam(PRESET_PARAM_REVERBPL_DIFF, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_TREBLECUT]) teensy.setParam(PRESET_PARAM_REVERBPL_TREBLECUT, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_BASSCUT]) teensy.setParam(PRESET_PARAM_REVERBPL_BASSCUT, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_TREBLE]) teensy.setParam(PRESET_PARAM_REVERBPL_TREBLE, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_BASS]) teensy.setParam(PRESET_PARAM_REVERBPL_BASS, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_BLEEDIN]) teensy.setParam(PRESET_PARAM_REVERBPL_BLEEDIN, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_PITCH]) teensy.setParam(PRESET_PARAM_REVERBPL_PITCH, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_PITCHMIX]) teensy.setParam(PRESET_PARAM_REVERBPL_PITCHMIX, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_SHIMMER]) teensy.setParam(PRESET_PARAM_REVERBPL_SHIMMER, value);
	else if (slider == sliders[PRESET_PARAM_REVERBPL_PITCHSHIMM]) teensy.setParam(PRESET_PARAM_REVERBPL_PITCHSHIMM, value);
	else if (slider == sliders[PRESET_PARAM_REVERBSC_TIME]) teensy.setParam(PRESET_PARAM_REVERBSC_TIME, value);
	else if (slider == sliders[PRESET_PARAM_REVERBSC_LOWPASSF]) teensy.setParam(PRESET_PARAM_REVERBSC_LOWPASSF, value);
	else if (slider == sliders[PRESET_PARAM_REVERBSP_TIME]) teensy.setParam(PRESET_PARAM_REVERBSP_TIME, value);
	else if (slider == sliders[PRESET_PARAM_REVERBSP_TREBLECUT]) teensy.setParam(PRESET_PARAM_REVERBSP_TREBLECUT, value);
	else if (slider == sliders[PRESET_PARAM_REVERBSP_BASSCUT]) teensy.setParam(PRESET_PARAM_REVERBSP_BASSCUT, value);
}
// ---------------------------------------------------------------------------------
static void dd_wahSelect_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	uint32_t sel = lv_dropdown_get_selected(obj);
	teensy.setFlag(PRESET_FLAG_WAH_MODEL, sel);	
}

static void dd_reverbSelect_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);

	uint32_t sel = lv_dropdown_get_selected(obj);
	lv_tabview_set_active(tabview_reverb, sel, LV_ANIM_OFF);
	if (sel == REVERB_SPRING)
	{
		lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_CHECKED, false);
		lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_CHECKED, false);	
		lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_DISABLED, true);
		lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_DISABLED, true);
	}	
	else
	{
		lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_DISABLED, false);
		lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_DISABLED, false);
	}
	teensy.setFlag(PRESET_FLAG_REVBTYPE, sel);
}
// ---------------------------------------------------------------------------------
static void rl_setup_event_cb(lv_event_t* e)
//static void dd_inType_event_cb(lv_event_t* e)
{
	// flags are set in the CANCEL or SAVE button cb
}
// ---------------------------------------------------------------------------------
static void dd_ampSelect_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	uint32_t sel = lv_dropdown_get_selected(obj);
	lv_dropdown_set_selected(dd_main_ampModel, sel);
	teensy.setFlag(PRESET_FLAG_AMPMODEL_NO, sel);
}
// ---------------------------------------------------------------------------------
static void dd_cabSelect_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	uint32_t sel = lv_dropdown_get_selected(obj);
	teensy.setFlag(PRESET_FLAG_CABIR_NO, sel);
	lv_dropdown_set_selected(dd_main_cabModel, sel);
	
	// Cab IRs are counted: 0-n = IR+, last element = OFF
	if (sel == 0) sel = lv_dropdown_get_option_count(dd_main_cabModel)-1;
	else sel--;
	teensy.setFlag(PRESET_FLAG_CABIR_NO, sel);
}
// ---------------------------------------------------------------------------------
static void dd_main_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = (lv_obj_t *)lv_event_get_target(e);
	uint32_t sel = lv_dropdown_get_selected(obj);
	if (obj == dd_main_ampModel)
	{
		lv_dropdown_set_selected(dd_ampModel, sel);
		// trigger the AMP tab callback to update the param flag
		lv_obj_send_event(dd_ampModel, LV_EVENT_VALUE_CHANGED, NULL);
		// teensy flag is set in the tab button callback
	}
	else if (obj == dd_main_cabModel)
	{
		lv_dropdown_set_selected(dd_cabModel, sel);
		lv_obj_send_event(dd_cabModel, LV_EVENT_VALUE_CHANGED, NULL);
		// teensy flag is set in the tab button callback
	}
}
// ---------------------------------------------------------------------------------
static void presetSave_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * ta = (lv_obj_t *)lv_event_get_target(e);
	
	if(code == LV_EVENT_READY) 
	{
		static uint8_t bf[PRESET_NAME_MAXCHARS+1];
		memset(bf, 0, sizeof(bf));
		const char* txtPtr = lv_textarea_get_text(ta);
		if (*txtPtr == '\0') txtPtr = lv_textarea_get_placeholder_text(ta);
		memcpy(bf, (char*)txtPtr, sizeof(bf));

		DBG_printf("Saving preset, name: %s\r\n",txtPtr );
		lv_textarea_set_placeholder_text(ta, txtPtr);
		lv_label_set_text(label_presetName, txtPtr);
		lv_obj_add_flag(win_presetSaveDialog, LV_OBJ_FLAG_HIDDEN);
		teensy.write(GUI_CMD_SAVE_PRESET, bf, sizeof(bf));
		gui_show_popup("PRESET SAVED", 1000);
	}
	else if (code == LV_EVENT_CANCEL)
	{
		DBG_println("Preset save cancelled");
		lv_obj_add_flag(win_presetSaveDialog, LV_OBJ_FLAG_HIDDEN);
		const char* txtPtr = lv_textarea_get_text(ta);
		lv_textarea_set_text(ta, "");
	}
}
// ---------------------------------------------------------------------------------
/**
 * Print the memory usage 
 * @param param
 */
static void memory_monitor()
{
	lv_mem_monitor_t mon;
	lv_mem_monitor(&mon);
	DBG_printf("total: %6ld used: %6ld (%3d %%), frag: %3d %%, biggest free: %6d\n", (int32_t)mon.total_size, (int32_t)mon.total_size - mon.free_size,
			mon.used_pct,
			mon.frag_pct,
			(int32_t)mon.free_biggest_size);
}

void gui_updateFlag(preset_paramFlag_t flag, uint8_t value)
{
	switch(flag)
	{
		case PRESET_FLAG_BYPASS:
			lv_obj_set_state(btn_main_bypass, LV_STATE_CHECKED, (bool)(value & 0x01));
			break;		
		case PRESET_FLAG_INSELECT:
			lv_roller_set_selected(rl_inType, value & 0x03, LV_ANIM_OFF);
			break;
		case PRESET_FLAG_COMP_EN:
			lv_obj_set_state(btn_main_comp, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_comp, btn_comp, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			break;
		case PRESET_FLAG_WAH_EN:
			lv_obj_set_state(btn_main_wah, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_wah, btn_wah, NULL, NULL, CHANGE_BTN_LABEL_NONE);		
			value &= 0x01;
			break;
		case PRESET_FLAG_WAH_MODEL:
			lv_dropdown_set_selected(dd_wahModel, value & 0x03);
			break;			
		case PRESET_FLAG_BOOST_EN:
			lv_obj_set_state(btn_main_boost, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_boost, btn_boost, NULL, NULL, CHANGE_BTN_LABEL_NONE);		
			break;
		case PRESET_FLAG_OCTAVE_EN:
			lv_obj_set_state(btn_main_oct, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_oct, btn_octave, NULL, NULL, CHANGE_BTN_LABEL_NONE);	
			value &= 0x01;
			break;
		case PRESET_FLAG_AMPMODEL_NO:
			lv_dropdown_set_selected(dd_main_ampModel, value & 0x0F);
			lv_dropdown_set_selected(dd_ampModel, value & 0x0F);
			break;
		case PRESET_FLAG_DOUBLER_EN:
			lv_obj_set_state(btn_main_doubler, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_doubler, btn_doubler, NULL, NULL, CHANGE_BTN_LABEL_NONE);			
			break;
		case PRESET_FLAG_CABIR_NO:
			// 0-9 : IRs, 10=Off ->
			if (value == lv_dropdown_get_option_count(dd_main_cabModel)-1) value = 0;
			else value++; 
			lv_dropdown_set_selected(dd_main_cabModel, value & 0x0F);
			lv_dropdown_set_selected(dd_cabModel, value & 0x0F);
			break;
		case PRESET_FLAG_DELAY_EN:
			lv_obj_set_state(btn_main_delay, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_delay, btn_delay, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			break;
		case PRESET_FLAG_REVBTYPE:
			value &= 0x03; 
			lv_dropdown_set_selected(dd_reverb_type, value);
			lv_tabview_set_active(tabview_reverb, value, LV_ANIM_OFF);
			if (value == REVERB_SPRING)
			{
				lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_CHECKED, false);
				lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_CHECKED, false);	
				lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_DISABLED, true);
				lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_DISABLED, true);
			}	
			else
			{
				lv_obj_set_state(btn_reverbPLSC_freeze, LV_STATE_DISABLED, false);
				lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_DISABLED, false);
			}
			break;
		case PRESET_FLAG_REVB_EN:
			lv_obj_set_state(btn_main_reverb, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_reverb, btn_reverb, labelTxt_ON, labelTxt_OFF, CHANGE_BTN_LABEL_DST);
			break;
		case PRESET_FLAG_REVB_FREEZE_EN:
			lv_obj_set_state(btn_main_reverb_freeze, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_reverb_freeze, btn_reverbPLSC_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);	
			break; 
		case PRESET_FLAG_DELAY_FREEZE_EN:
			lv_obj_set_state(btn_main_delay_freeze, LV_STATE_CHECKED, (bool)(value & 0x01));
			set_linked_btn(btn_main_delay_freeze, btn_delay_freeze, NULL, NULL, CHANGE_BTN_LABEL_NONE);	
			break;
		case PRESET_FLAG_POT1_ASSIGN:
			lv_roller_set_selected(rl_pot1Assign, value & 0x3F, LV_ANIM_OFF);
			break;
		case PRESET_FLAG_POT2_ASSIGN:
			lv_roller_set_selected(rl_pot2Assign, value & 0x3F, LV_ANIM_OFF);
			break;
		case PRESET_FLAG_POT3_ASSIGN:
			lv_roller_set_selected(rl_pot3Assign, value & 0x3F, LV_ANIM_OFF);
			break;
		case PRESET_FLAG_EXP_ASSIGN:
			lv_roller_set_selected(rl_ExpAssign, value & 0x3F, LV_ANIM_OFF);
			break;
		case PRESET_FLAG_FT1_ASSIGN:
			lv_roller_set_selected(rl_ft1Assign, value & 0x0F, LV_ANIM_OFF);
			break;	
		case PRESET_FLAG_FT2_ASSIGN:
			lv_roller_set_selected(rl_ft2Assign, value & 0x0F, LV_ANIM_OFF);
			break;			
		case PRESET_FLAG_DRYSIG:
			lv_obj_set_state(btn_main_dry, LV_STATE_CHECKED, (bool)(value & 0x01));
			break;
		case PRESET_FLAG_IDX:
			lv_label_set_text_fmt(label_presetNo, "%d", (value & 0x7F) + 1 );
			break;
		case PRESET_FLAG_LAST:
		default:

			break;	
	}
}

void gui_updateParam(preset_param_t paramNo, uint8_t paramValue)
{
	if(sliders[paramNo] == NULL) return;
	lv_slider_set_value(sliders[paramNo], paramValue, LV_ANIM_OFF);
	slider_update_valueLabel(sliders[paramNo], paramValue);
}

void gui_populatePreset(preset_t *presetData)
{
	uint16_t i;
	const char* txtPtr = teensy.getPresetName();
	lv_label_set_text(label_presetName, txtPtr);
	lv_textarea_set_placeholder_text(kbd_txtArea, txtPtr);
	lv_textarea_set_text(kbd_txtArea, "");
	lv_label_set_text_fmt(label_presetNo, "%03d", teensy.getFlag(PRESET_FLAG_IDX));
	lv_label_set_text_fmt(label_main_footer, "TGX4 V%s WWW.HEXEFX.COM | %s %s %s", teensy.hw_getVersion(), 
							teensy.hw_getMCU(), teensy.hw_getSDready(), teensy.hw_getPSRAM());

	for (i=0; i<PRESET_FLAG_LAST; i++)
	{
		gui_updateFlag((preset_paramFlag_t)i, teensy.getFlag((preset_paramFlag_t)i));
	}
	
	for (i=0; i<PRESET_PARAM_LAST; i++)
	{
		gui_updateParam((preset_param_t)i, teensy.getParam((preset_param_t)i));
	}
}