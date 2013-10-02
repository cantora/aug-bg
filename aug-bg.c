#include <stdint.h>

#include "aug_plugin.h"
#include "aug_api.h"

const char aug_plugin_name[] = "aug-bg";

#define g_boo_h 15
#define g_boo_w 16
uint8_t g_boo_map[g_boo_h][g_boo_w] = {
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },

	{ 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
	{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0 },
	{ 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0 },

	{ 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0 },

	{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },
	{ 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 }
};


static uint32_t g_cmd_ch = 0x62; /* b */
static int g_on = 1;
static int g_rows = 0;
static int g_cols = 0;

void cell_update(
	int rows, int cols, int *row, int *col, wchar_t *wch, 
	attr_t *attr, int *color_pair, aug_action *action, void *user
);

AUG_GLOBAL_API_OBJECTS;

struct aug_plugin_cb g_callbacks;

int column_min(int cols) {
	return cols-1-g_boo_w*2;
}

int row_min() {
	return 1;
}

int row_max() {
	return (g_boo_h+1);
}

void post_scroll(int rows, int cols, int direction, aug_action *action, void *user) {
	int rmax, cmin;

	(void)(direction);
	(void)(user);
	(void)(action);
	
	if(!g_on)
		return;

	rmax = row_max();
	if(direction == 0)
		return;
	else if(direction < 0)
		rmax += -direction;
	/* else it got scrolled off top edge */

	if(rmax > rows)
		rmax = rows;

	cmin = column_min(cols);

	if(rmax >= 0 && cmin > 10)
		aug_primary_term_damage(cmin, cols-1, 0, rmax);
}

void cell_update(int rows, int cols, int *row, int *col, wchar_t *wch, 
		attr_t *attr, int *color_pair, aug_action *action, void *user) {
	int col_min, row_min, r, c;
	(void)(rows);
	(void)(color_pair);
	(void)(wch);
	(void)(action);
	(void)(user);
	
	/*aug_log("cell_update: g_on=%d, %d/%d, %d/%d\n", g_on, *col, cols, *row, rows);*/
	if(!g_on)
		return;

	col_min = column_min(cols);
	row_min = 1;
	if(col_min > 10 
			&& *col >= col_min && *col < (cols-1)
			&& *row >= row_min && *row < row_max() ) {
		r = *row - 1;
		c = *col - col_min;
		if(g_boo_map[r][c/2] != 0) {
			*attr = *attr | A_REVERSE;
		}
	}
}

static void on_cmd_key(uint32_t ch, void *user) {
	int rmax, cmin;

	(void)(ch);
	(void)(user);

	/*aug_log("on_cmd_key: ch=0x%02x, g_on=%d\n", ch, g_on);*/
	/* do nothing if we havent been notified of the 
	 * screen size yet */
	if(g_rows < 1 || g_cols < 1) {
		aug_log("havent gotten dims yet, do nothing\n");
		return;
	}

	g_on = !g_on;
	rmax = row_max();

	if(rmax > g_rows)
		rmax = g_rows;

	cmin = column_min(g_cols);

	if(rmax > 0)
		aug_primary_term_damage(cmin, g_cols-1, 0, rmax);

}

void primary_term_dims_change(int rows, int cols, void *user) {
	(void)(user);

	g_rows = rows;
	g_cols = cols;
}

int aug_plugin_init(struct aug_plugin *plugin, const struct aug_api *api) {
	AUG_API_INIT(plugin, api);

	aug_log("init\n");

	aug_callbacks_init(&g_callbacks);
	g_callbacks.cell_update = cell_update;
	g_callbacks.post_scroll = post_scroll;
	g_callbacks.primary_term_dims_change = primary_term_dims_change;
	g_callbacks.user = NULL;
	aug_callbacks(&g_callbacks, NULL);

	if(aug_key_bind(g_cmd_ch, on_cmd_key, NULL) != 0) {
		aug_log("expected to be able to bind to extension '%s'. abort...");
		return -1;
	}
	return 0;
}

void aug_plugin_free() {
	aug_key_unbind(g_cmd_ch);
}

