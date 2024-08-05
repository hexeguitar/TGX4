#include "display.h"
#include "debug_log.h"


static LGFX tft;

const uint32_t draw_buf_size = (DISPLAY_HOR_RES * DISPLAY_VER_RES / 10) * (LV_COLOR_DEPTH / 8);
static uint8_t bufA[draw_buf_size];
static uint8_t bufB[draw_buf_size];

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void touch_cb(lv_indev_t *indev, lv_indev_data_t *data);

lv_disp_t *lcd;
lv_indev_t *indev;

void display_init()
{
	// LGFX driver config
	tft.begin();
	tft.setRotation(1);
	tft.setBrightness(255);
	tft.fillScreen(TFT_BLACK);
	lv_init();
	lv_tick_set_cb([]()
				   { return (uint32_t)(esp_timer_get_time() / 1000ULL); });

	lcd = lv_display_create(DISPLAY_HOR_RES, DISPLAY_VER_RES);
	if (lcd == NULL)
	{
		DBG_println("Display create failed!");
		lv_free(lcd);
		return;
	}
	lv_display_set_flush_cb(lcd, flush_cb);
	lv_display_set_buffers(lcd, bufA, bufB, sizeof(bufA), LV_DISPLAY_RENDER_MODE_PARTIAL);
	
	indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, touch_cb);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	uint32_t w = lv_area_get_width(area);
	uint32_t h = lv_area_get_height(area);
	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);
	tft.writePixels((lgfx::rgb565_t *)&px_map[0], w * h);
	tft.endWrite();

	lv_display_flush_ready(disp);
}

static void touch_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
	uint16_t x, y, count;

	if (count = tft.getTouch(&x, &y))
	{
		data->state = LV_INDEV_STATE_PR;
		data->point.x = x;
		data->point.y = y;
	}
	else	data->state = LV_INDEV_STATE_REL;
}
