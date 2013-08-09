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
	
	/*aug_log("%d/%d, %d/%d\n", *col, cols, *row, rows);*/
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

int aug_plugin_init(struct aug_plugin *plugin, const struct aug_api *api) {
	AUG_API_INIT(plugin, api);

	aug_log("init\n");

	aug_callbacks_init(&g_callbacks);
	g_callbacks.cell_update = cell_update;
	g_callbacks.post_scroll = post_scroll;
	g_callbacks.user = NULL;
	aug_callbacks(&g_callbacks, NULL);

	return 0;
}

void aug_plugin_free() {}

