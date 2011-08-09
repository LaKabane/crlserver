/*
 * Copyright (c) 2011 Tristan Le Guern <leguern@medu.se>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/queue.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <curses.h>
#include <err.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <unistd.h>

#include "crlserver.h"
#include "db.h"
#include "init.h"
#include "log.h"
#include "pathnames.h"
#include "session.h"

struct session session;

int
create_playground(const char *player_name) {
	char playground[MAXPATHLEN] = CRLSERVER_PLAYGROUND;

	if (player_name == NULL)
		return -1;

	(void)strncat(playground, player_name, 1);
	(void)mkdir(playground, 0700);
	(void)strlcat(playground, "/", sizeof playground);
	(void)strlcat(playground, player_name, sizeof playground);
	(void)mkdir(playground, 0700);

	if (access(playground, F_OK) == -1)
		return -1;

	session.home = strdup(playground);
	if (session.home == NULL)
		fclean_up("Memory error");

	return 0;
}

void
init(void) {
	db_init();

	initscr();
	if (has_colors() == TRUE)
		start_color();
	curs_set(0); /* Remove the cursor */

	start_window();

	if ((LINES < DROWS) || (COLS < DCOLS))
		fclean_up("must be displayed on 24 x 80 screen (or larger)");

	session.logged = 0;
}

void
start_window(void) {
	(void)cbreak();
	(void)noecho();
	(void)nonl();
	(void)keypad(stdscr, TRUE);
}

__inline void
end_window() {
	(void)endwin();
}
