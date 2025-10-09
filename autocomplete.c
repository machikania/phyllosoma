#include <string.h>
#include "pico/stdlib.h"
#include "./editor.h"
#include "./keyinput.h"
#include "./compiler.h"
#include "./autocomplete.h"
#include "./interface/usbkeyboard.h"
#include "./interface/graphlib.h"
#include "./interface/videodriver.h"
#include "./interface/ff.h"

// Autocompletion system variables
static unsigned char autocomplete_buffer[32]; // Buffer for current word being typed
// Exported variables
int autocomplete_pos = 0; // Position in autocomplete buffer
int autocomplete_show = 0; // Whether to show autocomplete popup
int autocomplete_selected = 0; // Currently selected item in popup
int autocomplete_count = 0; // Number of matching items

#define AUTOCOMPLETE_MAX_ITEMS 8
static int autocomplete_matches[AUTOCOMPLETE_MAX_ITEMS]; // Indices into g_reserved_words

// Function to check if character is part of a word (same as delimitercheck)
#define is_word_char(x) (x>='@' && x<='Z' || x>='a' && x<='z' || x=='_' || x>='0' && x<='9')

// Editor facilities
extern int insertchar(_tbuf *bp,unsigned int ix,unsigned char c,int undo);
extern void gabagecollect2(void);
extern void cursor_right(void);
extern _tbuf *cursorbp;
extern unsigned short cursorix;
extern int edited, cx, cy, cx2, EDITWIDTHY;

// Function to find autocomplete matches using existing reserved words
static void find_autocomplete_matches(void) {
    int i;
    autocomplete_count = 0;
    
    if (autocomplete_pos == 0) return;
    
    // Convert buffer to uppercase for comparison
    unsigned char upper_buffer[32];
    for (i = 0; i < autocomplete_pos; i++) {
        if (autocomplete_buffer[i] >= 'a' && autocomplete_buffer[i] <= 'z') {
            upper_buffer[i] = autocomplete_buffer[i] - 32;
        } else {
            upper_buffer[i] = autocomplete_buffer[i];
        }
    }
    upper_buffer[autocomplete_pos] = 0;
    
    // Find matching keywords from g_reserved_words
    for (i = 0; i < 189 && autocomplete_count < AUTOCOMPLETE_MAX_ITEMS; i++) {
        const char* keyword = g_reserved_words[i];
        int j;
        
        // Check if keyword starts with typed text
        for (j = 0; j < autocomplete_pos; j++) {
            if (keyword[j] != upper_buffer[j]) break;
        }
        if (j == autocomplete_pos) {
            autocomplete_matches[autocomplete_count++] = i;
        }
    }
    
    autocomplete_selected = 0;
}

// Function to update autocomplete buffer
void update_autocomplete(unsigned char c) {
    if (is_word_char(c)) {
        if (autocomplete_pos < 31) {
            autocomplete_buffer[autocomplete_pos++] = c;
            autocomplete_buffer[autocomplete_pos] = 0;
            find_autocomplete_matches();
            if (autocomplete_count > 0) {
                autocomplete_show = 1;
            }
        }
    } else {
        autocomplete_show = 0;
        autocomplete_pos = 0;
    }
}

// Function to handle autocomplete navigation
int handle_autocomplete_key(unsigned char k, unsigned char sh) {
    if (!autocomplete_show) return 0;
    
    switch (k) {
        case VK_UP:
        case VK_NUMPAD8:
            if (autocomplete_selected > 0) {
                autocomplete_selected--;
            }
            return 1;
            
        case VK_DOWN:
        case VK_NUMPAD2:
            if (autocomplete_selected < autocomplete_count - 1) {
                autocomplete_selected++;
            }
            return 1;
            
        case VK_RETURN:
        case VK_SEPARATOR:
        case VK_TAB:
            insert_autocomplete();
            return 1;
            
        case VK_ESCAPE:
            autocomplete_show = 0;
            autocomplete_pos = 0;
            return 1;
            
        default:
            return 0;
    }
}

// Function to insert selected autocomplete item
void insert_autocomplete(void) {
    if (autocomplete_count == 0 || autocomplete_selected >= autocomplete_count) return;
    
    const char* selected = g_reserved_words[autocomplete_matches[autocomplete_selected]];
    int i;
    
    // Insert remaining characters of the selected keyword
    for (i = autocomplete_pos; selected[i]; i++) {
        unsigned char c = selected[i];
        
        // Convert to lowercase if we're continuing a word that was typed in lowercase
        if (c >= 'A' && c <= 'Z' && autocomplete_pos > 0) {
            if (autocomplete_buffer[0] >= 'a' && autocomplete_buffer[0] <= 'z') {
                c = c + 32;
            }
        }
        
        int result = insertchar(cursorbp, cursorix, c, 0);
        if (result > 0) {
            gabagecollect2();
            result = insertchar(cursorbp, cursorix, c, 0);
        }
        if (result == 0) {
            cursor_right();
        } else {
            break;
        }
    }
    
    autocomplete_show = 0;
    autocomplete_pos = 0;
    edited = 1;
}

// Function to check if we should start autocomplete based on cursor position
void check_autocomplete_start(void) {
    _tbuf *bp = cursorbp;
    int ix = cursorix;
    int word_start = 0;
    
    // Look backwards to find start of current word
    while (ix > 0 || bp->prev != NULL) {
        if (ix == 0) {
            if (bp->prev == NULL) break;
            bp = bp->prev;
            ix = bp->n;
            continue;
        }
        ix--;
        
        if (!is_word_char(bp->Buf[ix])) {
            // Found non-word character, word starts after this
            ix++;
            if (ix >= bp->n) {
                bp = bp->next;
                ix = 0;
            }
            break;
        }
        word_start++;
        if (word_start >= 31) break; // Limit word length
    }
    
    // Extract the word into autocomplete buffer
    autocomplete_pos = 0;
    while (word_start > 0 && autocomplete_pos < 31) {
        if (ix >= bp->n) {
            if (bp->next == NULL) break;
            bp = bp->next;
            ix = 0;
            continue;
        }
        
        unsigned char c = bp->Buf[ix];
        if (!is_word_char(c)) break;
        
        autocomplete_buffer[autocomplete_pos++] = c;
        ix++;
        word_start--;
    }
    
    autocomplete_buffer[autocomplete_pos] = 0;
    
    if (autocomplete_pos > 0) {
        find_autocomplete_matches();
        if (autocomplete_count > 0) {
            autocomplete_show = 1;
        }
    }
}

// Function to show autocomplete popup
void show_autocomplete_popup(void) {
    int i, x, y;
    unsigned char *saved_vp;
    
    if (autocomplete_count == 0) return;
    
    // Calculate popup position
    x = cx;
    y = cy + 1;
    if (x + 15 > WIDTH_X) x = WIDTH_X - 15;
    if (y + autocomplete_count > EDITWIDTHY) y = cy - autocomplete_count;
    if (y < 0) y = 0;
    
    // Save current TVRAM area that will be overwritten
    saved_vp = TVRAM + y * WIDTH_X + x;
    
    // Draw popup
    for (i = 0; i < autocomplete_count && i < 6; i++) {
        unsigned char *vp = TVRAM + (y + i) * WIDTH_X + x;
        unsigned char *attr_vp = vp + attroffset;
        const char* keyword = g_reserved_words[autocomplete_matches[i]];
        int j;
        unsigned char color = (i == autocomplete_selected) ? COLOR_AREASELECTTEXT : COLOR_INV;
        
        // Draw keyword with background
        for (j = 0; j < 12 && keyword[j]; j++) {
            vp[j] = keyword[j];
            attr_vp[j] = color;
        }
        // Fill remaining space
        for (; j < 12; j++) {
            vp[j] = ' ';
            attr_vp[j] = color;
        }
    }
}
