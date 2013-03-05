#include <stdint.h>

#include "aug_plugin.h"

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

const struct aug_api *g_api;
struct aug_plugin *g_plugin;

struct aug_plugin_cb g_callbacks = {
	.input_char = NULL,
	.cell_update = cell_update,
	.cursor_move = NULL,
	.screen_dims_change = NULL
};

void cell_update(int rows, int cols, int *row, int *col, wchar_t *wch, 
		attr_t *attr, int *color_pair, aug_action *action, void *user) {
	int col_min, row_min, r, c;
	(void)(rows);
	(void)(color_pair);
	(void)(wch);
	(void)(action);
	(void)(user);
	
	/*(*g_api->log)(g_plugin, "%d/%d, %d/%d\n", *col, cols, *row, rows);*/
	col_min = cols-1-g_boo_w*2;
	row_min = 1;
	if(*col >= col_min && *col < (cols-1)
			&& *row >= row_min && *row < (g_boo_h+1)) {
		r = *row - 1;
		c = *col - col_min;
		if(g_boo_map[r][c/2] != 0) {
			*attr = *attr | A_REVERSE;
		}
	}
}

int aug_plugin_init(struct aug_plugin *plugin, const struct aug_api *api) {
	g_plugin = plugin;	
	g_api = api;

	(*g_api->log)(g_plugin, "init\n");

	g_callbacks.user = NULL;
	(*g_api->callbacks)(g_plugin, &g_callbacks, NULL);

	return 0;
}

void aug_plugin_free() {}

