extern void update_autocomplete(unsigned char c);
extern int handle_autocomplete_key(unsigned char k, unsigned char sh);
extern void insert_autocomplete(void);
extern void check_autocomplete_start(void);
extern void show_autocomplete_popup(void);

extern int autocomplete_pos;
extern int autocomplete_show;
extern int autocomplete_selected;
extern int autocomplete_count;