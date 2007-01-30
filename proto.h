#ifndef _PROTO_H_
#define _PROTO_H_

#include "egobootypedef.h"

//typedef struct glmatrix { float v[16]; } GLMATRIX;	//was v[4][4], changed for OGL compatibility
//typedef struct glvector { float x,y,z; } GLVECTOR;
/*GLVECTOR vsub(GLVECTOR A, GLVECTOR B);
GLVECTOR Normalize(GLVECTOR vec);
GLVECTOR CrossProduct(GLVECTOR A, GLVECTOR B);
float DotProduct(GLVECTOR A, GLVECTOR B);*/
char *os_cvrt_filename(char *name, char ch);
void make_directory(char *dirname);
void remove_directory(char *dirname);
void delete_file(char *filename);
void copy_file(char *source, char *dest);
void delete_directory(char *dirname);
void copy_directory(char *dirname, char *todirname);
void empty_import_directory(char* home);
void insert_space(int position);
void copy_one_line(int write);
int load_one_line(int read);
int load_parsed_line(int read);
void surround_space(int position);
void parse_null_terminate_comments();
int get_indentation();
void fix_operators();
int starts_with_capital_letter();
unsigned int get_high_bits();
int tell_code(int read);
void add_code(unsigned int highbits);
void parse_line_by_line();
unsigned int jump_goto(int index);
void parse_jumps(int ainumber);
void log_code(int ainumber, char* savename);
int ai_goto_colon(int read);
void get_code(int read);
void load_ai_codes(char* loadname);
int load_ai_script(char *loadname);
void reset_ai_script();
int what_action(char cTmp);
int get_level(unsigned char x, unsigned char y, unsigned int fan, unsigned char waterwalk);
void release_all_textures();
void load_one_icon(char *szLoadName);
void prime_titleimage();
void prime_icons();
void release_all_icons();
void release_all_titleimages();
void release_all_models();
void reset_sounds();
void release_grfx( void );
void release_map();
void release_module( void );
void close_session();
void general_error(int a, int b, char *szerrortext);
void play_sound_pvf(int index, int pan, int volume, int frequency);
void play_sound_skip(int index, int skip);
void play_sound_pvf_looped(int index, int pan, int volume, int frequency);
void stop_sound(int index);
int load_one_wave(char *szFileName);
void make_newloadname(char *modname, char *appendname, char *newloadname);
void load_global_waves(char *modname);
void export_one_character(int character, int owner, int number);
void export_all_local_players(void);
void quit_module(void);
void quit_game(void);
void goto_colon(FILE* fileread);
unsigned char goto_colon_yesno(FILE* fileread);
char get_first_letter(FILE* fileread);
void reset_tags();
int read_tag(FILE *fileread);
void read_all_tags(char *szFilename);
int tag_value(char *string);
void read_controls(char *szFilename);
unsigned char control_key_is_pressed(unsigned char control);
unsigned char control_mouse_is_pressed(unsigned char control);
unsigned char control_joya_is_pressed(unsigned char control);
unsigned char control_joyb_is_pressed(unsigned char control);
void free_all_enchants();
void undo_idsz(int idsz);
int get_idsz(FILE* fileread);
void load_one_enchant_type(char* szLoadName, unsigned short profile);
unsigned short get_free_enchant();
void unset_enchant_value(unsigned short enchantindex, unsigned char valueindex);
void remove_enchant_value(unsigned short enchantindex, unsigned char valueindex);
int get_free_message(void);
void display_message(int message, unsigned short character);
void remove_enchant(unsigned short enchantindex);
unsigned short enchant_value_filled(unsigned short enchantindex, unsigned char valueindex);
void set_enchant_value(unsigned short enchantindex, unsigned char valueindex,
    unsigned short enchanttype);
void getadd(int min, int value, int max, int* valuetoadd);
void fgetadd(float min, float value, float max, float* valuetoadd);
void add_enchant_value(unsigned short enchantindex, unsigned char valueindex,
    unsigned short enchanttype);
unsigned short spawn_enchant(unsigned short owner, unsigned short target,
    unsigned short spawner, unsigned short enchantindex, unsigned short modeloptional);
void load_action_names(char* loadname);
void get_name(FILE* fileread, char *szName);
void read_setup(char* filename);
void log_madused(char *savename);
void make_lightdirectionlookup();
float light_for_normal(int rotation, int normal, float lx, float ly, float lz, float ambi);
void make_lighttable(float lx, float ly, float lz, float ambi);
void make_lighttospek(void);
int vertexconnected(int modelindex, int vertex);
void get_madtransvertices(int modelindex);
int rip_md2_header(void);
void fix_md2_normals(unsigned short modelindex);
void rip_md2_commands(unsigned short modelindex);
int rip_md2_frame_name(int frame);
void rip_md2_frames(unsigned short modelindex);
int load_one_md2(char* szLoadname, unsigned short modelindex);
void load_all_music_sounds(char *modname);
void create_ig_trackmaster();
void load_ig_length(char *filename);
void check_ig_passage();
void check_ig_music();
void load_all_music_tracks(char *modname);
void load_music_sequence(char *modname);
void load_all_music_loops(char *modname);
void start_music_track(int track);
void play_next_track();
void stop_music();
void change_music_track(int track);
void play_next_music_sound();

//void dump_matrix(GLMATRIX a);
/*inline GLMATRIX IdentityMatrix();
inline GLMATRIX ZeroMatrix(void);  // initializes matrix to zero
inline GLMATRIX MatrixMult(const GLMATRIX a, const GLMATRIX b);  
GLMATRIX Translate(const float dx, const float dy, const float dz);
GLMATRIX RotateX(const float rads);
GLMATRIX RotateY(const float rads);
GLMATRIX RotateZ(const float rads);
GLMATRIX ScaleXYZ(const float sizex, const float sizey, const float sizez);
GLMATRIX ScaleXYZRotateXYZTranslate(const float sizex, const float sizey, const float sizez,
   unsigned short turnz, unsigned short turnx, unsigned short turny,
   float tx, float ty, float tz);
GLMATRIX FourPoints(float orix, float oriy, float oriz,
                     float widx, float widy, float widz,
                     float forx, float fory, float forz,
                     float upx,  float upy,  float upz,
                     float scale);
inline GLMATRIX ViewMatrix(const GLVECTOR from,      // camera location
                            const GLVECTOR at,        // camera look-at target
                            const GLVECTOR world_up,  // world�s up, usually 0, 0, 1
                            const float roll);          // clockwise roll around
                                                       //    viewing direction, 
                                                       //    in radians
inline GLMATRIX ProjectionMatrix(const float near_plane,     // distance to near clipping plane
                                  const float far_plane,      // distance to far clipping plane
                                  const float fov);            // field of view angle, in radians
*/
int open_passage(int passage);
int break_passage(int passage, unsigned short starttile, unsigned short frames,
    unsigned short become, unsigned char meshfxor);
void flash_passage(int passage, unsigned char color);
unsigned char find_tile_in_passage(int passage, int tiletype);
unsigned short who_is_blocking_passage(int passage);
unsigned short who_is_blocking_passage_ID(int passage, unsigned int idsz);
int close_passage(int passage);
void clear_passages();
void add_shop_passage(int owner, int passage);
void add_passage(int tlx, int tly, int brx, int bry, unsigned char open, unsigned char mask);
void flash_character_height(int character, unsigned char valuelow, signed short low,
    unsigned char valuehigh, signed short high);
void flash_character(int character, unsigned char value);
void flash_select();
void add_to_dolist(int cnt);
void order_dolist(void);
void make_dolist(void);
void keep_weapons_with_holders();
void make_enviro(void);
void make_prtlist(void);
void make_turntosin(void);
void make_one_character_matrix(unsigned short cnt);
void free_one_particle_no_sound(int particle);
void play_particle_sound(int particle, signed char sound);
void free_one_particle(int particle);
void free_one_character(int character);
void free_inventory(int character);
void attach_particle_to_character(int particle, int character, int grip);
void make_one_weapon_matrix(unsigned short cnt);
void make_character_matrices();
int get_free_particle(int force);
int get_free_character();
unsigned char find_target_in_block(int x, int y, float chrx, float chry, unsigned short facing, 
  unsigned char onlyfriends, unsigned char anyone, unsigned char team,
  unsigned short donttarget, unsigned short oldtarget);
unsigned short find_target(float chrx, float chry, unsigned short facing, 
  unsigned short targetangle, unsigned char onlyfriends, unsigned char anyone,
  unsigned char team, unsigned short donttarget, unsigned short oldtarget);
void debug_message(char *text);
void reset_end_text();
void append_end_text(int message, unsigned short character);
unsigned short spawn_one_particle(float x, float y, float z,
  unsigned short facing, unsigned short model, unsigned short pip, 
  unsigned short characterattach, unsigned short grip, unsigned char team,
  unsigned short characterorigin, unsigned short multispawn, unsigned short oldtarget);
unsigned char __prthitawall(int particle);
void disaffirm_attached_particles(unsigned short character);
unsigned short number_of_attached_particles(unsigned short character);
void reaffirm_attached_particles(unsigned short character);
void do_enchant_spawn();
void move_particles(void);
void attach_particles();
void free_all_particles();
void free_all_characters();
void show_stat(unsigned short statindex);
void check_stats();
void check_screenshot();
BOOL dump_screenshot();
void add_stat(unsigned short character);
void move_to_top(unsigned short character);
void sort_stat();
void setup_particles();
unsigned short terp_dir(unsigned short majordir, unsigned short minordir);
unsigned short terp_dir_fast(unsigned short majordir, unsigned short minordir);
unsigned char __chrhitawall(int character);
void move_water(void);
void play_action(unsigned short character, unsigned short action, unsigned char actionready);
void set_frame(unsigned short character, unsigned short frame, unsigned char lip);
void reset_character_alpha(unsigned short character);
void reset_character_accel(unsigned short character);
void detach_character_from_mount(unsigned short character, unsigned char ignorekurse,
  unsigned char doshop);
void spawn_bump_particles(unsigned short character, unsigned short particle);
int generate_number(int numbase, int numrand);
void drop_money(unsigned short character, unsigned short money);
void call_for_help(unsigned short character);
void give_experience(int character, int amount, unsigned char xptype);
void give_team_experience(unsigned char team, int amount, unsigned char xptype);
void disenchant_character(unsigned short cnt);
void damage_character(unsigned short character, unsigned short direction,
    int damagebase, int damagerand, unsigned char damagetype, unsigned char team,
    unsigned short attacker, unsigned short effects);
void kill_character(unsigned short character, unsigned short killer);
void spawn_poof(unsigned short character, unsigned short profile);
void naming_names(int profile);
void read_naming(int profile, char *szLoadname);
void prime_names(void);
void tilt_characters_to_terrain();
int spawn_one_character(float x, float y, float z, int profile, unsigned char team,
  unsigned char skin, unsigned short facing, char *name, int override);
void respawn_character(unsigned short character);
unsigned short change_armor(unsigned short character, unsigned short skin);
void change_character(unsigned short cnt, unsigned short profile, unsigned char skin,
    unsigned char leavewhich);
unsigned short get_target_in_block(int x, int y, unsigned short character, char items,
    char friends, char enemies, char dead, char seeinvisible, unsigned int idsz,
    char excludeid);
unsigned short get_nearby_target(unsigned short character, char items,
    char friends, char enemies, char dead, unsigned int idsz);
unsigned char cost_mana(unsigned short character, int amount, unsigned short killer);
unsigned short find_distant_target(unsigned short character, int maxdistance);
void switch_team(int character, unsigned char team);
void get_nearest_in_block(int x, int y, unsigned short character, char items,
    char friends, char enemies, char dead, char seeinvisible, unsigned int idsz);
unsigned short get_nearest_target(unsigned short character, char items,
    char friends, char enemies, char dead, unsigned int idsz);
unsigned short get_wide_target(unsigned short character, char items,
    char friends, char enemies, char dead, unsigned int idsz, char excludeid);
void issue_clean(unsigned short character);
int restock_ammo(unsigned short character, unsigned int idsz);
void issue_order(unsigned short character, unsigned int order);
void issue_special_order(unsigned int order, unsigned int idsz);
void set_alerts(int character);
int module_reference_matches(char *szLoadName, unsigned int idsz);
void add_module_idsz(char *szLoadName, unsigned int idsz);
unsigned char run_function(unsigned int value, int character);
void set_operand(unsigned char variable);
void run_operand(unsigned int value, int character);
void let_character_think(int character);
void let_ai_think();
void attach_character_to_mount(unsigned short character, unsigned short mount,
    unsigned short grip);
unsigned short stack_in_pack(unsigned short item, unsigned short character);
void add_item_to_character_pack(unsigned short item, unsigned short character);
unsigned short get_item_from_character_pack(unsigned short character, unsigned short grip, unsigned char ignorekurse);
void drop_keys(unsigned short character);
void drop_all_items(unsigned short character);
void character_grab_stuff(int chara, int grip, unsigned char people);
void character_swipe(unsigned short cnt, unsigned char grip);
void move_characters(void);
void make_textureoffset(void);
int add_player(unsigned short character, unsigned short player, unsigned char device);
void clear_messages();
void clear_select();
void add_select(unsigned short character);
void setup_characters(char *modname);
void setup_passage(char *modname);
void setup_alliances(char *modname);
void load_mesh_fans();
void make_fanstart();
void make_twist();
int load_mesh(char *modname);
void read_mouse();
void read_key();
void read_joystick();
void reset_press();
#define GP2X_UP 0
#define GP2X_DOWN 4
#define GP2X_LEFT 2
#define GP2X_RIGHT 6
#define GP2X_UPLEFT 1
#define GP2X_UPRIGHT 7
#define GP2X_DOWNLEFT 3
#define GP2X_DOWNRIGHT 5
#define GP2X_CLICK 18
#define GP2X_A 12
#define GP2X_B 13
#define GP2X_Y 14
#define GP2X_X 15
#define GP2X_R 10
#define GP2X_L 11
#define GP2X_START 8
#define GP2X_SELECT 9
#define GP2X_VOLM 17
#define GP2X_VOLP 16
int key_set(unsigned k);
int key_left(void);
int key_right(void);
int key_up(void);
int key_down(void);
int key_set_once(unsigned k);
void read_input();
void check_add(unsigned char key, char bigletter, char littleletter);
void create_szfpstext(int frames);
void camera_look_at(float x, float y);
void project_view();
void make_renderlist();
void make_camera_matrix();
void figure_out_what_to_draw();
void bound_camera();
void bound_camtrack();
void set_one_player_latch(unsigned short player);
void set_local_latches(void);
void adjust_camera_angle(int height);
void move_camera();
void make_onwhichfan(void);
void bump_characters(void);
int prt_is_over_water(int cnt);
void do_weather_spawn();
void animate_tiles();
void stat_return();
void pit_kill();
void reset_players();
void start_building_packet();
void start_reading_packet();
void add_packet_uc(unsigned char uc);
void add_packet_sc(signed char sc);
void add_packet_us(unsigned short us);
void add_packet_ss(signed short ss);
void add_packet_ui(unsigned int ui);
void add_packet_si(signed int si);
void add_packet_sz(char *string);
void read_packet_sz(char *text);
unsigned char read_packet_uc();
signed char read_packet_sc();
unsigned short read_packet_us();
signed short read_packet_ss();
unsigned int read_packet_ui();
signed int read_packet_si();
int still_reading_packet();
void send_packet_to_host();
void send_packet_to_all_players();
void send_packet_to_host_guaranteed();
void send_packet_to_all_players_guaranteed();
void send_packet_to_one_player_guaranteed(int player);
void input_net_message();
void copy_file_to_all_players(char *source, char *dest);
void copy_file_to_host(char *source, char *dest);
void copy_directory_to_host(char *dirname, char *todirname);
void copy_directory_to_all_players(char *dirname, char *todirname);
void say_hello();
void talk_to_host();
void talk_to_remotes();
int find_module(char *smallname);
void clear_orders();
unsigned short get_empty_order();
void listen_for_packets();
void unbuffer_player_latches();
void chug_orders();
void resize_characters();
void update_game();
void update_timers();
void load_basic_textures(char *modname);
unsigned short action_number();
unsigned short action_frame();
unsigned short test_frame_name(char letter);
void action_copy_correct(int object, unsigned short actiona, unsigned short actionb);
void get_walk_frame(int object, int lip, int action);
void get_framefx(int frame);
void make_framelip(int object, int action);
void get_actions(int object);
void read_pair(FILE* fileread);
void undo_pair(int base, int rand);
void ftruthf(FILE* filewrite, char* text, unsigned char truth);
void fdamagf(FILE* filewrite, char* text, unsigned char damagetype);
void factiof(FILE* filewrite, char* text, unsigned char action);
void fgendef(FILE* filewrite, char* text, unsigned char gender);
void fpairof(FILE* filewrite, char* text, int base, int rand);
void funderf(FILE* filewrite, char* text, char* usename);
void export_one_character_name(char *szSaveName, unsigned short character);
void export_one_character_profile(char *szSaveName, unsigned short character);
void export_one_character_skin(char *szSaveName, unsigned short character);
int load_one_character_profile(char *szLoadName);
int load_one_particle(char *szLoadName, int object, int pip);
void reset_particles(char* modname);
void make_mad_equally_lit(int model);
void get_message(FILE* fileread);
void load_all_messages(char *loadname, int object);
void check_copy(char* loadname, int object);
int load_one_object(int skin, char* tmploadname);
void load_all_objects(char *modname);
void load_bars(char* szBitmap);
void load_map(char* szModule, int sysmem);
void load_font(char* szBitmap, char* szSpacing, int sysmem);
void make_water();
void read_wawalite(char *modname);
void reset_teams();
void reset_messages();
void make_randie();
void load_module(char *smallname);
void render_prt();
void render_shadow(int character);
void render_bad_shadow(int character);
void render_refprt();
void render_fan(unsigned int fan);
void render_water_fan(unsigned int fan, unsigned char layer, unsigned char mode);
void render_enviromad(unsigned short character, unsigned char trans);
void render_texmad(unsigned short character, unsigned char trans);
void render_mad(unsigned short character, unsigned char trans);
void render_refmad(int tnc, unsigned char trans);
void light_characters();
void light_particles();
void set_fan_light(int fanx, int fany, unsigned short particle);
void do_dynalight();
void render_water();
void draw_scene_sadreflection();
void draw_scene_zreflection();
BOOL get_mesh_memory();
void draw_blip(unsigned char color, int x, int y);
void draw_one_icon(int icontype, int x, int y, unsigned char sparkle);
void draw_one_font(int fonttype, int x, int y);
void draw_map(int x, int y);
int draw_one_bar(int bartype, int x, int y, int ticks, int maxticks);
void draw_string(char const *szText, int x, int y);
int length_of_word(char *szText);
int draw_wrap_string(char *szText, int x, int y, int maxx);
int draw_status(unsigned short character, int x, int y);
void draw_text();
void flip_pages();
void draw_scene();
void send_rts_order(int x, int y, unsigned char order, unsigned char target);
void build_select(float tlx, float tly, float brx, float bry, unsigned char team);
unsigned short build_select_target(float tlx, float tly, float brx, float bry, unsigned char team);
void move_rtsxy();
void do_cursor_rts();
void draw_main();
void setup_network();
int load_one_title_image(int titleimage, char *szLoadName);
int get_module_data(int modnumber, char *szLoadName);
int get_module_summary(char *szLoadName);
void load_all_menu_images();
void load_menu_trim();
void draw_trimx(int x, int y, int length);
void draw_trimy(int x, int y, int length);
void draw_trim_box(int left, int top, int right, int bottom);
void draw_trim_box_opening(int left, int top, int right, int bottom, float amount);
void load_menu();
void draw_titleimage(int image, int x, int y, int tagsz);
void find_open_sessions();
void find_all_players();
int create_player(int host);
int join_session(int session);
void stop_players_from_joining();
int host_session();
void turn_on_service(int service);
void do_cursor();
void menu_service_select();
void menu_start_or_join();
void draw_module_tag(int module, int y, int tagsz);
int get_skin(char *filename);
void check_player_import_begin(void);
void check_player_import(int rootid, char *dirname);
void check_player_import_end(void);
void menu_pick_player(int module);
void menu_module_loading(int module);
void menu_choose_host();
void menu_choose_module();
void menu_boot_players();
void menu_end_text();
void menu_initial_text();
void fiddle_with_menu();
void release_menu_trim();
void release_menu();
void reset_timers();
void reset_camera();
int sdlinit(int argc, char **argv);
int glinit(int argc, char **argv);
void gltitle();
char *DirGetFirst(char *search);
char *DirGetNext(void);
void DirClose();
int ClockGetTick();
int DirGetAttrib(char *fromdir);


#endif //#ifndef _PROTO_H_

