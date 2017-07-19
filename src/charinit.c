/*
 * Windowport-agnostic character initialisation dialog
 * lifted from sporkhack's tty windowport by Tangles
 * July 2017.
 */
#include "hack.h"
#include "wintype.h"

struct player_selection_menu_option {
    int x, y;
    char accel;
    char* item;
    int* savein;
    int (*convfunction)();
    boolean (*validator)();
} player_selection_menu_options[] = {
    { 2, 4, 'a', "Archeologist", &flags.initrole, str2role, ok_role },
        { 2, 5, 'b', "Barbarian",    &flags.initrole, str2role, ok_role },
        { 2, 6, 'c', "Caveman",      &flags.initrole, str2role, ok_role },
        { 2, 7, 'h', "Healer",       &flags.initrole, str2role, ok_role },
        { 2, 8, 'k', "Knight",       &flags.initrole, str2role, ok_role },
        { 2, 9, 'm', "Monk",         &flags.initrole, str2role, ok_role },
        { 2,10, 'p', "Priest",       &flags.initrole, str2role, ok_role },
        { 2,11, 'R', "Ranger",       &flags.initrole, str2role, ok_role },
        { 2,12, 'r', "Rogue",        &flags.initrole, str2role, ok_role },
        { 2,13, 's', "Samurai",      &flags.initrole, str2role, ok_role },
        { 2,14, 't', "Tourist",      &flags.initrole, str2role, ok_role },
        { 2,15, 'v', "Valkyrie",     &flags.initrole, str2role, ok_role },
        { 2,16, 'w', "Wizard",       &flags.initrole, str2role, ok_role },

        {21, 4, 'H', "Human",        &flags.initrace, str2race, ok_race },
        {21, 5, 'D', "Dwarf",        &flags.initrace, str2race, ok_race },
        {21, 6, 'E', "Elf",          &flags.initrace, str2race, ok_race },
        {21, 7, 'G', "Gnome",        &flags.initrace, str2race, ok_race },
        {21, 8, 'O', "Orc",          &flags.initrace, str2race, ok_race },

        {21,11, 'M', "Male",         &flags.initgend, str2gend, ok_gend },
        {21,12, 'F', "Female",       &flags.initgend, str2gend, ok_gend },

        {21,15, 'L', "Lawful",       &flags.initalign,str2align,ok_align},
        {21,16, 'N', "Neutral",      &flags.initalign,str2align,ok_align},
        {21,17, 'C', "Chaotic",      &flags.initalign,str2align,ok_align},

        { 2,19, 'q', "quit",         0,               0,        0},
        {13,19, '*', "reroll",       0,               0,        0},
        {26,19, '.', "play!",        0,               0,        0},
        {0,0,0,0,0,0} /* fencepost */
};

/* Traditionally, character selection has been taken care of by the winbdowport.
 * Now, the windowport can call this to do the char selection in the window of
 * its choice. Note that the Window must be able to accommodate the coordinates
 * defined above.
 */
int 
player_selection_menu(win,toggle_color)
    winid win;
    void (*toggle_color)(int,int);
{
    char obuf[QBUFSZ];
    int ch;
    boolean xallowed = TRUE;

    /* prevent an unnecessary prompt */
    rigid_role_checks();

    /* Grab ourselves a random character that's as consistent with
       the RC as possible */
    if (flags.initrole < 0)
        flags.initrole = pick_role(flags.initrace, flags.initgend,
                                   flags.initalign, PICK_RANDOM);
    if (flags.initrole < 0)
        flags.initrole = randrole();

    if (flags.initrace < 0 || !validrace(flags.initrole, flags.initrace))
        flags.initrace = pick_race(flags.initrole, flags.initgend,
                                   flags.initalign, PICK_RANDOM);
    if (flags.initrace < 0)
        flags.initrace = randrace(flags.initrole);

    if (flags.initgend < 0 || !validgend(flags.initrole, flags.initrace,
                                         flags.initgend))
        flags.initgend = pick_gend(flags.initrole, flags.initrace,
                                   flags.initalign, PICK_RANDOM);
    if (flags.initgend < 0)
        flags.initgend = randgend(flags.initrole, flags.initrace);

    if (flags.initalign < 0 || !validalign(flags.initrole, flags.initrace,
                                           flags.initalign))
        flags.initalign = pick_align(flags.initrole, flags.initrace,
                                     flags.initgend, PICK_RANDOM);
    if (flags.initalign < 0)
        flags.initalign = randalign(flags.initrole, flags.initrace);

    /* Fixed parts of the window */
    clear_nhwindow(win);

    Sprintf(obuf, "%s %s, welcome to SporkHack!", Hello((struct monst *)0), plname);
    curs(win, 1, 0);
    putstr(win, 0, obuf);

    curs(win, 6, 2);
    (*toggle_color)(CLR_WHITE,ON);
    putstr(win, 0, "Create your character:");
    (*toggle_color)(CLR_WHITE,OFF);
    curs(win, 6, 4);
    putstr(win, ATR_INVERSE, "Class");
    curs(win, 25, 4);
    putstr(win, ATR_INVERSE, "Race");
    curs(win, 25, 11);
    putstr(win, ATR_INVERSE, "Gender");
    curs(win, 25, 15);
    putstr(win, ATR_INVERSE, "Alignment");

    /* Changing parts of the window */
    do {
        struct player_selection_menu_option* p = player_selection_menu_options;
        int savestat, i;
        while (p->x) {
            int color = CLR_GRAY;
            curs(win, p->x, (p->y+1));
            Sprintf(obuf, "%c %c %s", p->accel,
                    p->savein && p->convfunction(p->item) == *(p->savein) ? '+' : '-',
                    p->item);
            /* We colour a crga in brown if it couldn't be changed to without
               having to change some other aspect of the character */
            if (p->savein) {
                savestat = *(p->savein);
                *(p->savein) = p->convfunction(p->item);
                if (!p->validator(flags.initrole,flags.initrace,
                                  flags.initgend,flags.initalign)) {
                    color = CLR_BROWN;
                    if (obuf[2] == '+') obuf[2] = '!';
                }
                *(p->savein) = savestat;
            }
            if (obuf[2] != '-' || p->accel == '.') color += BRIGHT;
            (*toggle_color)(color,ON); putstr(win, 0, obuf); (*toggle_color)(color,OFF);
            p++;
        }
        if (ok_role (flags.initrole,flags.initrace,flags.initgend,flags.initalign) &&
            ok_race (flags.initrole,flags.initrace,flags.initgend,flags.initalign) &&
            ok_gend (flags.initrole,flags.initrace,flags.initgend,flags.initalign) &&
            ok_align(flags.initrole,flags.initrace,flags.initgend,flags.initalign)) {
            xallowed = TRUE;
        } else { xallowed = FALSE; }

        curs(win, 26, 20); /* the . of . - play! */

        ch = readchar();
        if (ch == 'q' || ch == 'Q') return -1;
        if (ch == '*') {
            flags.initrole = randrole();
            flags.initrace = randrace(flags.initrole);
            flags.initgend = randgend(flags.initrole, flags.initrace);
            flags.initalign = randalign(flags.initrole, flags.initrace);
        }
        p = player_selection_menu_options;
        while (p->x) {
            if (p->accel == ch && p->savein) {
                *(p->savein) = p->convfunction(p->item);
                break;
            }
            p++;
        }
    } while (ch != '.' || !xallowed);

    /* Success! */
    display_nhwindow(win, FALSE);
    return 0;
}

