/* vim:set cin ft=c sw=4 sts=4 ts=8 et ai cino=Ls\:0t0(0 : -*- mode:c;fill-column:80;tab-width:8;c-basic-offset:4;indent-tabs-mode:nil;c-file-style:"k&r" -*-*/

#include "curses.h"
#include "hack.h"
#include "wincurs.h"
#include "cursinit.h"
#include "patchlevel.h"

#include <ctype.h>

/* Initialization and startup functions for curses interface */

/* Private declarations */

#define NETHACK_CURSES      1
#define SLASHEM_CURSES      2
#define UNNETHACK_CURSES    3
#define SPORKHACK_CURSES    4
#define GRUNTHACK_CURSES    5
#define DNETHACK_CURSES     6

static void set_window_position(int *, int *, int *, int *, int,
                                int *, int *, int *, int *, int,
                                int, int);

/* array to save initial terminal colors for later restoration */

typedef struct nhrgb_type {
    short r;
    short g;
    short b;
} nhrgb;

nhrgb orig_yellow;
nhrgb orig_white;
nhrgb orig_darkgray;
nhrgb orig_hired;
nhrgb orig_higreen;
nhrgb orig_hiyellow;
nhrgb orig_hiblue;
nhrgb orig_himagenta;
nhrgb orig_hicyan;
nhrgb orig_hiwhite;

/* Banners used for an optional ASCII splash screen */

#define NETHACK_SPLASH_A \
" _   _        _    _    _               _    "

#define NETHACK_SPLASH_B \
"| \\ | |      | |  | |  | |             | |   "

#define NETHACK_SPLASH_C \
"|  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"

#define NETHACK_SPLASH_D \
"| . ` | / _ \\| __||  __  | / _` | / __|| |/ /"

#define NETHACK_SPLASH_E \
"| |\\  ||  __/| |_ | |  | || (_| || (__ |   < "

#define NETHACK_SPLASH_F \
"|_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"

#define SLASHEM_SPLASH_A \
" _____  _              _     _  ______  __  __ "

#define SLASHEM_SPLASH_B \
" / ____|| |            | |   ( )|  ____||  \\/  |"

#define SLASHEM_SPLASH_C \
"| (___  | |  __ _  ___ | |__  \\|| |__   | \\  / |"

#define SLASHEM_SPLASH_D \
" \\___ \\ | | / _` |/ __|| '_ \\   |  __|  | |\\/| |"

#define SLASHEM_SPLASH_E \
" ____) || || (_| |\\__ \\| | | |  | |____ | |  | |"

#define SLASHEM_SPLASH_F \
"|_____/ |_| \\__,_||___/|_| |_|  |______||_|  |_|"

#define UNNETHACK_SPLASH_A \
" _    _         _   _        _    _    _               _"

#define UNNETHACK_SPLASH_B \
"| |  | |       | \\ | |      | |  | |  | |             | |"

#define UNNETHACK_SPLASH_C \
"| |  | | _ __  |  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"

#define UNNETHACK_SPLASH_D \
"| |  | || '_ \\ | . ` | / _ \\| __||  __  | / _` | / __|| |/ /"

#define UNNETHACK_SPLASH_E \
"| |__| || | | || |\\  ||  __/| |_ | |  | || (_| || (__ |   <"

#define UNNETHACK_SPLASH_F \
" \\____/ |_| |_||_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"

#define SPORKHACK_SPLASH_A \
"  _____                      _     _    _               _    "
#define SPORKHACK_SPLASH_B \
" / ____|                    | |   | |  | |             | |   "
#define SPORKHACK_SPLASH_C \
"| (___   _ __    ___   _ __ | | __| |__| |  __ _   ___ | | __"
#define SPORKHACK_SPLASH_D \
" \\___ \\ | '_ \\  / _ \\ | '__|| |/ /|  __  | / _` | / __|| |/ /"
#define SPORKHACK_SPLASH_E \
" ____) || |_) || (_) || |   |   < | |  | || (_| || (__ |   < "
#define SPORKHACK_SPLASH_F \
"|_____/ | .__/  \\___/ |_|   |_|\\_\\|_|  |_| \\__,_| \\___||_|\\_\\"
#define SPORKHACK_SPLASH_G \
"        | |                                                  "
#define SPORKHACK_SPLASH_H \
"        |_|                                                 "

#define GRUNTHACK_SPLASH_A \
" ______                      _    _    _               _    "
#define GRUNTHACK_SPLASH_B \
"/  ____)                    | |  | |  | |             | |   "
#define GRUNTHACK_SPLASH_C \
"| / ___  _ __  _   _  _ __  | |_ | |__| |  __ _   ___ | |  _"
#define GRUNTHACK_SPLASH_D \
"| | L  \\| '__)| | | || '_ \\ | __)|  __  | / _` | / __)| |/ /"
#define GRUNTHACK_SPLASH_E \
"| l__) || |   | |_| || | | || |_ | |  | || (_| || (__ |   < "
#define GRUNTHACK_SPLASH_F \
"\\______/|_|   \\___,_||_| |_| \\__)|_|  |_| \\__,_| \\___)|_|\\_\\"

#define DNETHACK_SPLASH_A \
"     _  _   _        _    _    _               _    "
#define DNETHACK_SPLASH_B \
"    | || \\ | |      | |  | |  | |             | |   "
#define DNETHACK_SPLASH_C \
"  __| ||  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"
#define DNETHACK_SPLASH_D \
" / _` || . ` | / _ \\| __||  __  | / _` | / __|| |/ /"
#define DNETHACK_SPLASH_E \
"| (_| || |\\  ||  __/| |_ | |  | || (_| || (__ |   < "
#define DNETHACK_SPLASH_F \
" \\__,_||_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"


/* win* is size and placement of window to change, x/y/w/h is baseline which can
   decrease depending on alignment of win* in orientation.
   Negative minh/minw: as much as possible, but at least as much as specified. */
static void
set_window_position(int *winx, int *winy, int *winw, int *winh, int orientation,
                    int *x, int *y, int *w, int *h, int border_space,
                    int minh, int minw)
{
    *winw = *w;
    *winh = *h;

    /* Set window height/width */
    if (orientation == ALIGN_TOP || orientation == ALIGN_BOTTOM) {
        if (minh < 0) {
            *winh = (*h - ROWNO - border_space);
            if (-minh > *winh)
                *winh = -minh;
        } else
            *winh = minh;
        *h -= (*winh + border_space);
    } else {
        if (minw < 0) {
            *winw = (*w - COLNO - border_space);
            if (-minw > *winw)
                *winw = -minw;
        } else
            *winw = minw;
        *w -= (*winw + border_space);
    }

    *winx = *w + border_space + *x;
    *winy = *h + border_space + *y;

    /* Set window position */
    if (orientation != ALIGN_RIGHT) {
        *winx = *x;
        if (orientation == ALIGN_LEFT)
            *x += *winw + border_space;
    }
    if (orientation != ALIGN_BOTTOM) {
        *winy = *y;
        if (orientation == ALIGN_TOP)
            *y += *winh + border_space;
    }
}

/* Create the "main" nonvolitile windows used by nethack */

void
curses_create_main_windows()
{
    int min_message_height = 1;
    int message_orientation = 0;
    int status_orientation = 0;
    int border_space = 0;
    int hspace = term_cols - 80;
    boolean borders = FALSE;

    switch (iflags.wc2_windowborders) {
    case 1:                     /* On */
        borders = TRUE;
        break;
    case 2:                     /* Off */
        borders = FALSE;
        break;
    case 3:                     /* Auto */
        if ((term_cols > 81) && (term_rows > 25)) {
            borders = TRUE;
        }
        break;
    default:
        borders = FALSE;
    }


    if (borders) {
        border_space = 2;
        hspace -= border_space;
    }

    if ((term_cols - border_space) < COLNO) {
        min_message_height++;
    }

    /* Determine status window orientation */
    if (!iflags.wc_align_status || (iflags.wc_align_status == ALIGN_TOP)
        || (iflags.wc_align_status == ALIGN_BOTTOM)) {
        if (!iflags.wc_align_status) {
            iflags.wc_align_status = ALIGN_BOTTOM;
        }
        status_orientation = iflags.wc_align_status;
    } else {                    /* left or right alignment */

        /* Max space for player name and title horizontally */
        if ((hspace >= 26) && (term_rows >= 24)) {
            status_orientation = iflags.wc_align_status;
            hspace -= (26 + border_space);
        } else {
            status_orientation = ALIGN_BOTTOM;
        }
    }

    /* Determine message window orientation */
    if (!iflags.wc_align_message || (iflags.wc_align_message == ALIGN_TOP)
        || (iflags.wc_align_message == ALIGN_BOTTOM)) {
        if (!iflags.wc_align_message) {
            iflags.wc_align_message = ALIGN_TOP;
        }
        message_orientation = iflags.wc_align_message;
    } else {                    /* left or right alignment */

        if ((hspace - border_space) >= 25) {    /* Arbitrary */
            message_orientation = iflags.wc_align_message;
        } else {
            message_orientation = ALIGN_TOP;
        }
    }

    /* Figure out window positions and placements. Status and message area can be aligned
       based on configuration. The priority alignment-wise is: status > msgarea > game.
       Define everything as taking as much space as possible and shrink/move based on
       alignment positions. */
    int message_x = 0;
    int message_y = 0;
    int status_x = 0;
    int status_y = 0;
    int inv_x = 0;
    int inv_y = 0;
    int map_x = 0;
    int map_y = 0;

    int message_height = 0;
    int message_width = 0;
    int status_height = 0;
    int status_width = 0;
    int inv_height = 0;
    int inv_width = 0;
    int map_height = (term_rows - border_space);
    int map_width = (term_cols - border_space);

    boolean status_vertical = FALSE;
    boolean msg_vertical = FALSE;
    if (status_orientation == ALIGN_LEFT ||
        status_orientation == ALIGN_RIGHT)
        status_vertical = TRUE;
    if (message_orientation == ALIGN_LEFT ||
        message_orientation == ALIGN_RIGHT)
        msg_vertical = TRUE;

    int statusheight = 3;
    if (iflags.classic_status)
        statusheight = 2;

    /* Vertical windows have priority. Otherwise, priotity is:
       status > inv > msg */
    if (status_vertical)
        set_window_position(&status_x, &status_y, &status_width, &status_height,
                            status_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, statusheight, 26);

    if (flags.perm_invent) {
        /* Take up all width unless msgbar is also vertical. */
        int width = -25;
        if (msg_vertical)
            width = 25;

        set_window_position(&inv_x, &inv_y, &inv_width, &inv_height,
                            ALIGN_RIGHT, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, width);
    }

    if (msg_vertical)
        set_window_position(&message_x, &message_y, &message_width, &message_height,
                            message_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, -25);

    /* Now draw horizontal windows */
    if (!status_vertical)
        set_window_position(&status_x, &status_y, &status_width, &status_height,
                            status_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, statusheight, 26);

    if (!msg_vertical)
        set_window_position(&message_x, &message_y, &message_width, &message_height,
                            message_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, -25);

    if (map_width > COLNO)
        map_width = COLNO;

    if (map_height > ROWNO)
        map_height = ROWNO;

    if (curses_get_nhwin(STATUS_WIN)) {
        curses_del_nhwin(STATUS_WIN);
        curses_del_nhwin(MESSAGE_WIN);
        curses_del_nhwin(MAP_WIN);
        curses_del_nhwin(INV_WIN);

        clear();
    }

    curses_add_nhwin(STATUS_WIN, status_height, status_width, status_y,
                     status_x, status_orientation, borders);

    curses_add_nhwin(MESSAGE_WIN, message_height, message_width, message_y,
                     message_x, message_orientation, borders);

    if (flags.perm_invent)
        curses_add_nhwin(INV_WIN, inv_height, inv_width, inv_y, inv_x,
                         ALIGN_RIGHT, borders);

    curses_add_nhwin(MAP_WIN, map_height, map_width, map_y, map_x, 0, borders);

    refresh();

    curses_refresh_nethack_windows();

    if (iflags.window_inited) {
        curses_update_stats();
        if (flags.perm_invent)
            curses_update_inventory();
    } else {
        iflags.window_inited = TRUE;
    }
}


/* Initialize curses colors to colors used by NetHack */

void
curses_init_nhcolors()
{
#ifdef TEXTCOLOR
    if (has_colors()) {
        use_default_colors();
        init_pair(1, COLOR_BLACK, -1);
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_YELLOW, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);
        init_pair(7, COLOR_CYAN, -1);
        init_pair(8, -1, -1);

        {
            int i;

            int clr_remap[16] = {
                COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
                COLOR_BLUE,
                COLOR_MAGENTA, COLOR_CYAN, -1, COLOR_WHITE,
                COLOR_RED + 8, COLOR_GREEN + 8, COLOR_YELLOW + 8,
                COLOR_BLUE + 8,
                COLOR_MAGENTA + 8, COLOR_CYAN + 8, COLOR_WHITE + 8
            };

            for (i = 0; i < (COLORS >= 16 ? 16 : 8); i++) {
                init_pair(17 + (i * 2) + 0, clr_remap[i], COLOR_RED);
                init_pair(17 + (i * 2) + 1, clr_remap[i], COLOR_BLUE);
            }

            boolean hicolor = FALSE;
            if (COLORS >= 16)
                hicolor = TRUE;

            /* Work around the crazy definitions above for more background colors... */
            for (i = 0; i < (COLORS >= 16 ? 16 : 8); i++) {
                init_pair((hicolor ? 49 : 9) + i, clr_remap[i], COLOR_GREEN);
                init_pair((hicolor ? 65 : 33) + i, clr_remap[i], COLOR_YELLOW);
                init_pair((hicolor ? 81 : 41) + i, clr_remap[i], COLOR_MAGENTA);
                init_pair((hicolor ? 97 : 49) + i, clr_remap[i], COLOR_CYAN);
                init_pair((hicolor ? 113 : 57) + i, clr_remap[i], COLOR_WHITE);
            }
        }


        if (COLORS >= 16) {
            init_pair(9, COLOR_WHITE, -1);
            init_pair(10, COLOR_RED + 8, -1);
            init_pair(11, COLOR_GREEN + 8, -1);
            init_pair(12, COLOR_YELLOW + 8, -1);
            init_pair(13, COLOR_BLUE + 8, -1);
            init_pair(14, COLOR_MAGENTA + 8, -1);
            init_pair(15, COLOR_CYAN + 8, -1);
            init_pair(16, COLOR_WHITE + 8, -1);
        }

        if (can_change_color()) {
            /* Preserve initial terminal colors */
            color_content(COLOR_YELLOW, &orig_yellow.r, &orig_yellow.g,
                          &orig_yellow.b);
            color_content(COLOR_WHITE, &orig_white.r, &orig_white.g,
                          &orig_white.b);

            /* Set colors to appear as NetHack expects */
            init_color(COLOR_YELLOW, 500, 300, 0);
            init_color(COLOR_WHITE, 600, 600, 600);
            if (COLORS >= 16) {
                /* Preserve initial terminal colors */
                color_content(COLOR_RED + 8, &orig_hired.r,
                              &orig_hired.g, &orig_hired.b);
                color_content(COLOR_GREEN + 8, &orig_higreen.r,
                              &orig_higreen.g, &orig_higreen.b);
                color_content(COLOR_YELLOW + 8, &orig_hiyellow.r,
                              &orig_hiyellow.g, &orig_hiyellow.b);
                color_content(COLOR_BLUE + 8, &orig_hiblue.r,
                              &orig_hiblue.g, &orig_hiblue.b);
                color_content(COLOR_MAGENTA + 8, &orig_himagenta.r,
                              &orig_himagenta.g, &orig_himagenta.b);
                color_content(COLOR_CYAN + 8, &orig_hicyan.r,
                              &orig_hicyan.g, &orig_hicyan.b);
                color_content(COLOR_WHITE + 8, &orig_hiwhite.r,
                              &orig_hiwhite.g, &orig_hiwhite.b);

                /* Set colors to appear as NetHack expects */
                init_color(COLOR_RED + 8, 1000, 500, 0);
                init_color(COLOR_GREEN + 8, 0, 1000, 0);
                init_color(COLOR_YELLOW + 8, 1000, 1000, 0);
                init_color(COLOR_BLUE + 8, 0, 0, 1000);
                init_color(COLOR_MAGENTA + 8, 1000, 0, 1000);
                init_color(COLOR_CYAN + 8, 0, 1000, 1000);
                init_color(COLOR_WHITE + 8, 1000, 1000, 1000);
# ifdef USE_DARKGRAY
                if (COLORS > 16) {
                    color_content(CURSES_DARK_GRAY, &orig_darkgray.r,
                                  &orig_darkgray.g, &orig_darkgray.b);
                    init_color(CURSES_DARK_GRAY, 300, 300, 300);
                    /* just override black colorpair entry here */
                    init_pair(1, CURSES_DARK_GRAY, -1);
                }
# endif
            } else {
                /* Set flag to use bold for bright colors */
            }
        }
    }
#endif
}


/* Allow player to pick character's role, race, gender, and alignment.
 * Use non-windowport version in charinit.c
 * Need to pass a function pointer to set colour, as windowport
 * interface does not support this. Note that we hardcode mapwin
 * here, as we are using that for the character dialog
 */
void
curses_charsel_toggle_color(c,oo)
{
    curses_toggle_color_attr(mapwin,c,NONE,oo);
}    

void
curses_choose_character()
{
    if (player_selection_menu(MAP_WIN,curses_charsel_toggle_color) < 0)
        curses_bail(0);
}

/* Initialize and display options appropriately */

void
curses_init_options()
{
    set_wc_option_mod_status(WC_ALIGN_MESSAGE | WC_ALIGN_STATUS | WC_COLOR |
                             WC_HILITE_PET | WC_POPUP_DIALOG, SET_IN_GAME);

    set_wc2_option_mod_status(WC2_GUICOLOR, SET_IN_GAME);

    /* Remove a few options that are irrelevant to this windowport */
    set_option_mod_status("DECgraphics", SET_IN_FILE);
    set_option_mod_status("eight_bit_tty", SET_IN_FILE);

    /* Add those that are */
    set_option_mod_status("classic_status", SET_IN_GAME);

    /* Make sure that DECgraphics is not set to true via the config
       file, as this will cause display issues.  We can't disable it in
       options.c in case the game is compiled with both tty and curses. */
    if (iflags.DECgraphics) {
        switch_graphics(CURS_GRAPHICS);
    }
#ifdef PDCURSES
    /* PDCurses for SDL, win32 and OS/2 has the ability to set the
       terminal size programatically.  If the user does not specify a
       size in the config file, we will set it to a nice big 110x32 to
       take advantage of some of the nice features of this windowport. */
    if (iflags.wc2_term_cols == 0) {
        iflags.wc2_term_cols = 110;
    }

    if (iflags.wc2_term_rows == 0) {
        iflags.wc2_term_rows = 32;
    }

    resize_term(iflags.wc2_term_rows, iflags.wc2_term_cols);
    getmaxyx(base_term, term_rows, term_cols);

    /* This is needed for an odd bug with PDCurses-SDL */
    switch_graphics(ASCII_GRAPHICS);
    if (iflags.IBMgraphics) {
        switch_graphics(IBM_GRAPHICS);
    } else if (iflags.cursesgraphics) {
        switch_graphics(CURS_GRAPHICS);
    } else {
        switch_graphics(ASCII_GRAPHICS);
    }
#endif /* PDCURSES */
    if (!iflags.wc2_windowborders) {
        iflags.wc2_windowborders = 3;   /* Set to auto if not specified */
    }

    if (!iflags.wc2_petattr) {
        iflags.wc2_petattr = A_REVERSE;
    } else {                    /* Pet attribute specified, so hilite_pet should be true */

        iflags.hilite_pet = TRUE;
    }

#ifdef NCURSES_MOUSE_VERSION
    if (iflags.wc_mouse_support) {
        mousemask(BUTTON1_CLICKED, NULL);
    }
#endif
}


/* Display an ASCII splash screen if the splash_screen option is set */

void
curses_display_splash_window()
{
    int x_start;
    int y_start;
    int which_variant = NETHACK_CURSES; /* Default to NetHack */
    curses_get_window_xy(MAP_WIN, &x_start, &y_start);

    if ((term_cols < 70) || (term_rows < 20)) {
        iflags.wc_splash_screen = FALSE;        /* No room for s.s. */
    }
#ifdef DEF_GAME_NAME
    if (strcmp(DEF_GAME_NAME, "SlashEM") == 0) {
        which_variant = SLASHEM_CURSES;
    }
#endif

#ifdef GAME_SHORT_NAME
    if (strcmp(GAME_SHORT_NAME, "UNH") == 0) {
        which_variant = UNNETHACK_CURSES;
    }
#endif

    if (strncmp("SporkHack", COPYRIGHT_BANNER_A, 9) == 0) {
        which_variant = SPORKHACK_CURSES;
    }

    if (strncmp("GruntHack", COPYRIGHT_BANNER_A, 9) == 0) {
        which_variant = GRUNTHACK_CURSES;
    }

    if (strncmp("dNethack", COPYRIGHT_BANNER_A, 8) == 0) {
        which_variant = DNETHACK_CURSES;
    }


    curses_toggle_color_attr(stdscr, CLR_WHITE, A_NORMAL, ON);
    if (iflags.wc_splash_screen) {
        switch (which_variant) {
        case NETHACK_CURSES:
            mvaddstr(y_start, x_start, NETHACK_SPLASH_A);
            mvaddstr(y_start + 1, x_start, NETHACK_SPLASH_B);
            mvaddstr(y_start + 2, x_start, NETHACK_SPLASH_C);
            mvaddstr(y_start + 3, x_start, NETHACK_SPLASH_D);
            mvaddstr(y_start + 4, x_start, NETHACK_SPLASH_E);
            mvaddstr(y_start + 5, x_start, NETHACK_SPLASH_F);
            y_start += 7;
            break;
        case SLASHEM_CURSES:
            mvaddstr(y_start, x_start, SLASHEM_SPLASH_A);
            mvaddstr(y_start + 1, x_start, SLASHEM_SPLASH_B);
            mvaddstr(y_start + 2, x_start, SLASHEM_SPLASH_C);
            mvaddstr(y_start + 3, x_start, SLASHEM_SPLASH_D);
            mvaddstr(y_start + 4, x_start, SLASHEM_SPLASH_E);
            mvaddstr(y_start + 5, x_start, SLASHEM_SPLASH_F);
            y_start += 7;
            break;
        case UNNETHACK_CURSES:
            mvaddstr(y_start, x_start, UNNETHACK_SPLASH_A);
            mvaddstr(y_start + 1, x_start, UNNETHACK_SPLASH_B);
            mvaddstr(y_start + 2, x_start, UNNETHACK_SPLASH_C);
            mvaddstr(y_start + 3, x_start, UNNETHACK_SPLASH_D);
            mvaddstr(y_start + 4, x_start, UNNETHACK_SPLASH_E);
            mvaddstr(y_start + 5, x_start, UNNETHACK_SPLASH_F);
            y_start += 7;
            break;
        case SPORKHACK_CURSES:
            mvaddstr(y_start, x_start, SPORKHACK_SPLASH_A);
            mvaddstr(y_start + 1, x_start, SPORKHACK_SPLASH_B);
            mvaddstr(y_start + 2, x_start, SPORKHACK_SPLASH_C);
            mvaddstr(y_start + 3, x_start, SPORKHACK_SPLASH_D);
            mvaddstr(y_start + 4, x_start, SPORKHACK_SPLASH_E);
            mvaddstr(y_start + 5, x_start, SPORKHACK_SPLASH_F);
            mvaddstr(y_start + 6, x_start, SPORKHACK_SPLASH_G);
            mvaddstr(y_start + 7, x_start, SPORKHACK_SPLASH_H);
            y_start += 9;
            break;
        case GRUNTHACK_CURSES:
            mvaddstr(y_start, x_start, GRUNTHACK_SPLASH_A);
            mvaddstr(y_start + 1, x_start, GRUNTHACK_SPLASH_B);
            mvaddstr(y_start + 2, x_start, GRUNTHACK_SPLASH_C);
            mvaddstr(y_start + 3, x_start, GRUNTHACK_SPLASH_D);
            mvaddstr(y_start + 4, x_start, GRUNTHACK_SPLASH_E);
            mvaddstr(y_start + 5, x_start, GRUNTHACK_SPLASH_F);
            y_start += 7;
            break;
        case DNETHACK_CURSES:
            mvaddstr(y_start, x_start, DNETHACK_SPLASH_A);
            mvaddstr(y_start + 1, x_start, DNETHACK_SPLASH_B);
            mvaddstr(y_start + 2, x_start, DNETHACK_SPLASH_C);
            mvaddstr(y_start + 3, x_start, DNETHACK_SPLASH_D);
            mvaddstr(y_start + 4, x_start, DNETHACK_SPLASH_E);
            mvaddstr(y_start + 5, x_start, DNETHACK_SPLASH_F);
            y_start += 7;
            break;
        default:
            impossible("which_variant number %d out of range", which_variant);
        }
    }

    curses_toggle_color_attr(stdscr, CLR_WHITE, A_NORMAL, OFF);

#ifdef COPYRIGHT_BANNER_A
    mvaddstr(y_start, x_start, COPYRIGHT_BANNER_A);
    y_start++;
#endif

#ifdef COPYRIGHT_BANNER_B
    mvaddstr(y_start, x_start, COPYRIGHT_BANNER_B);
    y_start++;
#endif

#ifdef COPYRIGHT_BANNER_C
    mvaddstr(y_start, x_start, COPYRIGHT_BANNER_C);
    y_start++;
#endif

#ifdef COPYRIGHT_BANNER_D       /* Just in case */
    mvaddstr(y_start, x_start, COPYRIGHT_BANNER_D);
    y_start++;
#endif
    curses_toggle_color_attr(stdscr, MORECOLOR, NONE, ON);
    mvaddstr(y_start, x_start, "  Press a key.  >>");
    curses_toggle_color_attr(stdscr, MORECOLOR, NONE, OFF);
    refresh();
    getch();
}


/* Resore colors and cursor state before exiting */

void
curses_cleanup()
{
#ifdef TEXTCOLOR
    if (has_colors() && can_change_color()) {
        init_color(COLOR_YELLOW, orig_yellow.r, orig_yellow.g, orig_yellow.b);
        init_color(COLOR_WHITE, orig_white.r, orig_white.g, orig_white.b);

        if (COLORS >= 16) {
            init_color(COLOR_RED + 8, orig_hired.r, orig_hired.g, orig_hired.b);
            init_color(COLOR_GREEN + 8, orig_higreen.r, orig_higreen.g,
                       orig_higreen.b);
            init_color(COLOR_YELLOW + 8, orig_hiyellow.r,
                       orig_hiyellow.g, orig_hiyellow.b);
            init_color(COLOR_BLUE + 8, orig_hiblue.r, orig_hiblue.g,
                       orig_hiblue.b);
            init_color(COLOR_MAGENTA + 8, orig_himagenta.r,
                       orig_himagenta.g, orig_himagenta.b);
            init_color(COLOR_CYAN + 8, orig_hicyan.r, orig_hicyan.g,
                       orig_hicyan.b);
            init_color(COLOR_WHITE + 8, orig_hiwhite.r, orig_hiwhite.g,
                       orig_hiwhite.b);
# ifdef USE_DARKGRAY
            if (COLORS > 16) {
                init_color(CURSES_DARK_GRAY, orig_darkgray.r,
                           orig_darkgray.g, orig_darkgray.b);
            }
# endif
        }
    }
#endif
}
