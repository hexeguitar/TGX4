#ifndef _GUI_H_
#define _GUI_H_

#include "TGX4.h"

void gui_init(void);
void gui_process();
void gui_updateFlag(preset_paramFlag_t flag, uint8_t value);
void gui_updateParam(preset_param_t paramNo, uint8_t paramValue);
void gui_populatePreset(preset_t* presetData);
void gui_show_popup(const char* msg, uint32_t timeout_ms);

#endif // _GUI_H_
