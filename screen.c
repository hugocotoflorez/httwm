#include "term.h"
#include "termstuff.h"
#include <stdio.h>

void
print_borders(Term *term)
{
#define TPI term->position.i
#define TPJ term->position.j
#define TSC term->size.ws_col
#define TSR term->size.ws_row
#define BS  BORDER_SIZE
#define BDV BORDER_DOUBLE_VERTICAL

    term_apply_color(BORDER_FG, FG);
    term_apply_color(BORDER_BG, BG);
    // top border
    term_cursor_position(TPI - BS, TPJ - (BS + BDV));
    for (int i = 0; i < TSC + BS * 2 * (1 + BDV); i++)
        putchar(BORDER_CHAR);
    // lateral borders
    for (int i = 0; i < TSR; i++)
    {
        term_cursor_position(TPI + i, TPJ - (BS + BDV));
        putchar(BORDER_CHAR);
        if (BDV)
            putchar(BORDER_CHAR);
        term_cursor_position(TPI + i, TPJ + TSC);
        putchar(BORDER_CHAR);
        if (BDV)
            putchar(BORDER_CHAR);
    }
    // bottom border
    term_cursor_position(TPI + TSR, TPJ - (BS + BDV));
    for (int i = 0; i < TSC + BS * 2 * (1 + BDV); i++)
        putchar(BORDER_CHAR);
    term_cursor_restore_saved_position();

#undef TPI
#undef TPJ
#undef TSC
#undef TSR
#undef BS
#undef BDV
}

/* Clear contents in terminal screen space (quite like clear) */
void
screen_blank(Term *term)
{
    term_cursor_save_current_possition();
    term_apply_color(BG_COLOR, BG);

    for (int i = 0; i < term->size.ws_row; i++)
    {
        term_cursor_position(term->position.i + i, term->position.j);
        for (int j = 0; j < term->size.ws_col; j++)
            putchar(' ');
    }
}

/* Print all term content and its border */
void
screen_print_term(Term *term)
{
    term_cursor_save_current_possition();
    //print_borders(term);
    //screen_blank(term);
    term_cursor_restore_saved_position();
}
