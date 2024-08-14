#include <libdragon.h>

#define DSP_WID 320
#define DSP_HEI 240

static sprite_t *banana_spr;
static rdpq_font_t *font;

static float spin, spin_amount;

int main(void)
{
	spin_amount = 2.4f;

	debug_init_isviewer();
	debug_init_usblog();
	display_init(RESOLUTION_320x240, DEPTH_32_BPP, 3, GAMMA_NONE,
		     ANTIALIAS_RESAMPLE);
	rdpq_init();
	dfs_init(DFS_DEFAULT_LOCATION);
	asset_init_compression(2);
	joypad_init();

	font = rdpq_font_load("rom:/font.font64");
	rdpq_font_style(font, 0,
			&(rdpq_fontstyle_t){ .color = RGBA32(0, 0, 0, 0xFF) });
	rdpq_text_register_font(1, font);

	banana_spr = sprite_load("rom:/banana.ci4.sprite");

	while (1) {
		/*************
		 * RENDERING *
		 *************/
		rdpq_attach(display_get(), NULL);
		rdpq_clear(color_from_packed32(0xFFFFFFFF));

		/* banana */
		rdpq_set_mode_standard();
		rdpq_mode_dithering(DITHER_BAYER_NONE);
		rdpq_mode_filter(FILTER_BILINEAR);
		rdpq_sprite_blit(banana_spr, DSP_WID >> 1, DSP_HEI >> 1,
				 &(const rdpq_blitparms_t){
					 .theta = spin,
					 .cx = banana_spr->width >> 1,
					 .cy = banana_spr->height >> 1,
					 .scale_x = 2,
					 .scale_y = 2 });

		/* text */
		char buf[128];
		sprintf(buf, "Spin Speed (Press A): %.3f\n", spin_amount);
		rdpq_text_print(NULL, 1, 32, 240 - 32, buf);

		rdpq_detach_show();

		/************
		 * UPDATING *
		 ************/
		joypad_poll();
		spin_amount += (spin_amount * 0.01f) *
			       joypad_get_buttons_pressed(JOYPAD_PORT_1).a;
		spin += spin_amount * display_get_delta_time();
	}
}
