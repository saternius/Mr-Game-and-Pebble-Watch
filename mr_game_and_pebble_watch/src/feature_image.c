/*

   Demonstrate how to display a two color, black and white bitmap
   image with no transparency.

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

 */
#include <stdlib.h>
#include <time.h>
#include "pebble.h"
#define ACCEL_STEP_MS 30
  
static Window *window;
Layer *window_layer;

static BitmapLayer *image_layer;
static Layer *enterFrame;
static BitmapLayer *p1_fighter;
static BitmapLayer *p2_fighter;
static BitmapLayer *referee;
static BitmapLayer *p1_flag_layer;
static BitmapLayer *p2_flag_layer;
static BitmapLayer *blinker;
TextLayer *text_layer;

GBitmap *image;
GBitmap *level_counter;
GBitmap *empty;
GBitmap *p1_stance;
GBitmap *p1_kick;
GBitmap *p1_flag;
GBitmap *p2_flag;
GBitmap *p2_stance;
GBitmap *p2_kick;
GBitmap *flag3;
GBitmap *flag2;
GBitmap *flag1;
static GBitmap *p1_dodge;
static GBitmap *p1_owned;
//GBitmap *p1_hits;
static GBitmap *p2_dodge;
static GBitmap *p2_owned;
static GBitmap *p2_hits;
static GBitmap *p1_owned_two;
static GBitmap *p2_owned_two;

/*
static GBitmap *nine;
static GBitmap *eight;
static GBitmap *seven;
static GBitmap *six;
static GBitmap *five;
static GBitmap *four;
static GBitmap *three;
static GBitmap *two;
static GBitmap *one;
*/

static GBitmap *temp_sign;
static GBitmap *levels;

static GBitmap *p1_hit;
GBitmap *sign_one;
GBitmap *sign_two;
GBitmap *blink;


const int taunt_length=40;
int player_health = 105;
int player2_health=105;
bool menu = false;
int sign_timer=40;
int taunt_timer = 40;
bool sign = true;
bool kick = false;
static AppTimer *timer;
bool count_down=false;

int flags = 3;
int temp_timer = 0;
bool begin = true;
bool hammertime = false;

int p1_smack = 0;
int p2_smack = 0;

//AI variables
int level = 1;
int reaction_time = 60;
int base_reaction_time = 50;
int luck = 20;
int failures = 5;


bool fail = false;
bool finish = false;
int finish_timer = 50;
bool flash = true;

bool p2_knock = false;
bool p1_knock = false;
bool initialize =true;
int weird = 10;
int weird2=-14;
int weird3 =35;
bool pre_smack = false;


static void clear_a_bit(){
  gbitmap_destroy(level_counter);
  gbitmap_destroy(empty);
  gbitmap_destroy(p1_stance);
  gbitmap_destroy(p1_kick);
  gbitmap_destroy(p1_flag);
  gbitmap_destroy(p2_flag);
  gbitmap_destroy(p2_kick);
  gbitmap_destroy(p2_stance);
  gbitmap_destroy(flag3);
  gbitmap_destroy(flag2);
  gbitmap_destroy(flag1);
  gbitmap_destroy(p1_dodge);
  gbitmap_destroy(p1_owned);
  gbitmap_destroy(p2_dodge);
  gbitmap_destroy(p2_hits);
  gbitmap_destroy(p2_owned);
  gbitmap_destroy(p2_owned_two);
  gbitmap_destroy(p1_owned_two);
  gbitmap_destroy(levels);
  gbitmap_destroy(p1_hit);
}




void middle_click_handler(ClickRecognizerRef recognizer, void *context) {
  /*
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "%u Bottles", p2_smack);
  text_layer_set_text(text_layer, body_text);
  */
  if(!menu){
    gbitmap_destroy(image);
    image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    bitmap_layer_set_bitmap(image_layer, image);
    bitmap_layer_set_bitmap(referee, level_counter); 
   // bitmap_layer_set_bitmap(referee, empty); 
    menu=true;
  }
}

void right_click_handler(ClickRecognizerRef recognizer, void *context) {
  //dodge
  if(hammertime || count_down){
    if(count_down){
      pre_smack=true;
    }
    weird2=-28;
    weird=-5;
    weird3=30;
    layer_remove_from_parent(bitmap_layer_get_layer(p1_fighter))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(p1_fighter));
    layer_remove_from_parent(bitmap_layer_get_layer(referee))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(referee));
    
    layer_remove_from_parent(bitmap_layer_get_layer(p1_flag_layer))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(p1_flag_layer));
    layer_remove_from_parent(bitmap_layer_get_layer(p2_flag_layer))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(p2_flag_layer));

    p1_knock=true;
    bitmap_layer_set_bitmap(p1_fighter,p1_hit);
    GRect move_pos = (GRect) { .origin = { -26, 10 }, .size = { 180, 180 } };
    layer_set_frame(bitmap_layer_get_layer(p1_fighter), move_pos);
    bitmap_layer_set_compositing_mode(p1_fighter, GCompOpClear);
    if(p1_smack>=p2_smack){
      player2_health-=p1_smack*5;
      if(p2_knock){
        bitmap_layer_set_bitmap(p2_fighter, p2_owned);
        blink = p2_owned;
      }else{
        blink = p2_owned_two;
        bitmap_layer_set_bitmap(p2_fighter, p2_owned_two);
      }
      bitmap_layer_set_compositing_mode(p2_fighter, GCompOpClear);
      finish = true;
      hammertime=false;
      blinker = p2_fighter;
    }else{
        player_health-=15;
    }
  }
}

void left_click_handler(ClickRecognizerRef recognizer, void *context) {
  //smack
  if(hammertime){
    hammertime=false;
    finish = true;
    bitmap_layer_set_bitmap(p1_fighter,p1_dodge);
    GRect move_pos = (GRect) { .origin = { 0, weird3+15 }, .size = { 150, 150 } };
    layer_set_frame(bitmap_layer_get_layer(p1_fighter), move_pos);
    if(p1_smack<p2_smack){
      blink = p2_flag;
      blinker = p2_fighter;
    }else{
      blink = p1_dodge;
      blinker = p1_fighter;
    }
  }
}


void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, middle_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, right_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, left_click_handler);
}


static void update_callback(Layer *me, GContext *ctx) {
  if(menu){
   graphics_context_set_fill_color(ctx, GColorBlack);
   graphics_fill_rect(ctx, GRect(15+(105-player_health), 155, player_health, 10),0, GCornerNone);
   graphics_fill_rect(ctx, GRect(15+(105-player2_health), 4, player2_health, 10),0, GCornerNone);
   //bitmap_layer_set_bitmap(referee, level_counter);
   bitmap_layer_set_alignment(referee, GAlignCenter);
  }
}



void ai_smack(){
  layer_remove_from_parent(bitmap_layer_get_layer(p2_fighter))	;
  layer_add_child(window_layer, bitmap_layer_get_layer(p2_fighter));
  layer_remove_from_parent(bitmap_layer_get_layer(referee))	;
  layer_add_child(window_layer, bitmap_layer_get_layer(referee));
    layer_remove_from_parent(bitmap_layer_get_layer(p1_flag_layer))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(p1_flag_layer));
    layer_remove_from_parent(bitmap_layer_get_layer(p2_flag_layer))	;
    layer_add_child(window_layer, bitmap_layer_get_layer(p2_flag_layer));
  
  p2_knock=true;
  bitmap_layer_set_bitmap(p2_fighter,p2_hits);
  GRect move_pos = (GRect) { .origin = { -15, -15 }, .size = { 150, 150 } };
  layer_set_frame(bitmap_layer_get_layer(p2_fighter), move_pos);
  bitmap_layer_set_compositing_mode(p2_fighter, GCompOpClear);
  if(!fail){
    player_health-=p2_smack*5;
    if(p1_knock){
      bitmap_layer_set_bitmap(p1_fighter, p1_owned);
      blink = p1_owned;
    }else{
      blink = p1_owned_two;
      bitmap_layer_set_bitmap(p1_fighter, p1_owned_two);
    }
    bitmap_layer_set_compositing_mode(p1_fighter, GCompOpClear);
    finish = true;
    blinker = p1_fighter; 
  }else{
    player2_health-=15;
  }
}

void ai_dodge(){
  finish = true;
  bitmap_layer_set_bitmap(p2_fighter,p2_dodge);
  GRect move_pos = (GRect) { .origin = { 0, -50 }, .size = { 150, 150 } };
  layer_set_frame(bitmap_layer_get_layer(p2_fighter), move_pos);
  if(!fail){
    blink = p1_flag;
    blinker = p1_fighter;
  }else{
    blink = p2_dodge;
    blinker = p2_fighter;
  }
}
static void start_stage(){
  /*
    if(initialize){
        initialize=false;
        gbitmap_destroy(level_counter);
        level_counter = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_HIT_BLACK);
    }*/
        pre_smack=false;
        begin=true;
        taunt_timer=taunt_length;
        p1_knock=false;
        p2_knock=false;
        finish_timer=50;
        temp_timer =  (rand() % 20)+20;
        reaction_time=base_reaction_time+(rand()%luck);
        bitmap_layer_set_bitmap(referee, empty);
        bitmap_layer_set_bitmap(p1_fighter, p1_stance);
        bitmap_layer_set_bitmap(p2_fighter,p2_stance);
        GRect powah = (GRect) { .origin = { weird, weird3 }, .size = { 150, 150 } };
        GRect powah2 = (GRect) { .origin = { 10, -35 }, .size = { 150, 150 } };
        GRect flags_pos = (GRect) { .origin = { -30, 0 }, .size = { 150, 150 } };
        GRect p1_flag_pos = (GRect) { .origin = { -45, 32 }, .size = { 150, 150 } };
        GRect p2_flag_pos = (GRect) { .origin = { -45, -32 }, .size = { 150, 150 } };
        layer_set_frame(bitmap_layer_get_layer(p1_fighter), powah);
        layer_set_frame(bitmap_layer_get_layer(p2_fighter), powah2);
        layer_set_frame(bitmap_layer_get_layer(referee),flags_pos);
        layer_set_frame(bitmap_layer_get_layer(p1_flag_layer),p1_flag_pos);
        layer_set_frame(bitmap_layer_get_layer(p2_flag_layer),p2_flag_pos);  
        bitmap_layer_set_bitmap(p1_flag_layer, empty); 
        bitmap_layer_set_bitmap(p2_flag_layer, empty); 
        flags=3;
        p1_smack=0;
        bitmap_layer_set_compositing_mode(p1_fighter, GCompOpAssign);
        bitmap_layer_set_compositing_mode(p2_fighter, GCompOpAssign);
  
}
static void timer_callback(void *data) {
  /*
  player_health--;
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "%u Bottles", player_health);
  text_layer_set_text(text_layer, body_text);
  */
  if(menu){
    if(sign){
      sign_timer--;
      if(sign_timer<1){
        sign=false;
        //build the fighting position
         start_stage();
      } 
    }else if(taunt_timer>0){
      taunt_timer--;
      if(taunt_timer%10==0){
        if(!kick){//make em kick
           bitmap_layer_set_bitmap(p1_fighter, p1_kick);
           bitmap_layer_set_bitmap(p2_fighter,p2_kick);
        }else{//make em stand
           bitmap_layer_set_bitmap(p1_fighter, p1_stance);
           bitmap_layer_set_bitmap(p2_fighter,p2_stance);
        }
        if(taunt_timer==0){
          count_down=true;
        }
        kick=!kick;
      }
    }else if(count_down){
      if(begin){
        begin = false;
        bitmap_layer_set_bitmap(referee,flag3);
      }
      temp_timer--;
      if(temp_timer<1){
        temp_timer = (rand()%20)+20;
        flags--;
        if(flags==2){
          bitmap_layer_set_bitmap(referee,flag2);
        }else if(flags==1){
          bitmap_layer_set_bitmap(referee,flag1);
        }else if(flags==0){
          bitmap_layer_set_bitmap(referee,empty);
          count_down=false;
          hammertime=true;
          begin=true;
        }
      }
    }else if(hammertime){
      if(begin){
        begin=false;
        
        if(!pre_smack){
           p1_smack = (rand()%9)+1;
           bitmap_layer_set_bitmap(p1_fighter, p1_flag);
           GRect powah = (GRect) { .origin = { weird2, weird3 }, .size = { 150, 150 } };
           layer_set_frame(bitmap_layer_get_layer(p1_fighter), powah);
        }

        
        bitmap_layer_set_bitmap(p2_fighter, p2_flag);
        GRect powah2 = (GRect) { .origin = { -14, -35 }, .size = { 150, 150 } };
        layer_set_frame(bitmap_layer_get_layer(p2_fighter), powah2);
        p2_smack = (rand()%9)+1;
        uint32_t numbers[9] = {RESOURCE_ID_IMAGE_ONE, RESOURCE_ID_IMAGE_TWO, RESOURCE_ID_IMAGE_THREE, RESOURCE_ID_IMAGE_FOUR, RESOURCE_ID_IMAGE_FIVE, RESOURCE_ID_IMAGE_SIX, RESOURCE_ID_IMAGE_SEVEN, RESOURCE_ID_IMAGE_EIGHT, RESOURCE_ID_IMAGE_NINE};
        gbitmap_destroy(sign_one);
        gbitmap_destroy(sign_two);
        sign_one = gbitmap_create_with_resource(numbers[p1_smack-1]);
        sign_two = gbitmap_create_with_resource(numbers[p2_smack-1]);
        bitmap_layer_set_bitmap(p1_flag_layer,sign_one);
        bitmap_layer_set_bitmap(p2_flag_layer,sign_two);
        layer_remove_from_parent(bitmap_layer_get_layer(referee))	;
        layer_add_child(window_layer, bitmap_layer_get_layer(referee));
      }
      
      reaction_time--;
      if(reaction_time<0){
        fail = (rand()%failures) == 2;
        if(p2_smack>=p1_smack){
          if(fail){
            hammertime=false;
            ai_dodge();
          }else{
            hammertime=false;
            ai_smack();
          }
        }else{
          if(fail){
            ai_smack();
          }else{
            hammertime=false;
            ai_dodge();
          }
        }
      }
      
    }else if(finish){
      finish_timer--;
      if(blinker!=NULL){
        if(finish_timer%5==0){
          if(flash){
            bitmap_layer_set_bitmap(blinker, empty);
          }else{
            bitmap_layer_set_bitmap(blinker, blink);
          }
          flash=!flash;
        }
      }
      if(finish_timer<1){
        finish=false;
        if(player_health<1){
          // clear_a_bit();
           gbitmap_destroy(image);
           image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YOU_LOOSE);
           bitmap_layer_set_bitmap(image_layer, image);
        }else{
        
        if(player2_health<1){
          if(level<5){
            sign=true;
            sign_timer = 40;
            level++;
            player_health=105;
            player2_health=105;
            //set level difficulty
            if(level==2){
               reaction_time = 30;
               base_reaction_time = 20;
               luck = 10;
               failures = 10;
            }else if(level==3){
               reaction_time = 25;
               base_reaction_time = 16;
               luck = 8;
               failures = 20;
            }else if(level==4){
               reaction_time = 10;
               base_reaction_time = 10;
               luck = 5;
               failures = 40;
            }else if(level==5){
               reaction_time = 4;
               base_reaction_time = 5;
               luck = 5;
               failures = 80;
            }
            
          }else{
             gbitmap_destroy(image);
           // clear_a_bit();
             image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YOU_WON);
             bitmap_layer_set_bitmap(image_layer, image);
          }
        }else{
          start_stage();
        }
        }
      }
    }
    
  }
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}



static void init() {
  srand(time(NULL));
  window = window_create();
  window_set_fullscreen(window, true);
  
  window_set_click_config_provider(window, config_provider);
  
  window_stack_push(window, true /* Animated */);

  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // This needs to be deinited on app exit which is when the event loop ends
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_SCREEN);
  level_counter = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LEVEL_ONE);
  empty = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EMPTY);
  p1_stance = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_STANCE);
  p1_kick = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_KICK);
  p1_flag = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_FLAG);
  p2_flag = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_FLAG);
  p2_stance = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_STANCE);
  p2_kick = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_KICK);
  flag3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAG_THREE);
  flag2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAG_TWO);
  flag1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FLAG_ONE);
  p1_dodge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_DODGE);
  p2_dodge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_DODGE);
 
  p1_owned = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_OWNED_BLACK);
  p2_owned = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_OWNED_BLACK);
  p2_hits = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_HIT_BLACK);

  p1_owned_two = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_OWNED_TWO_BLACK);
  p2_owned_two = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PTWO_OWNED_TWO_BLACK);

  p1_hit = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PONE_HIT_BLACK);
  
  
  // The bitmap layer holds the image for display
  image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
  
  //player one bitmapLayer
  p1_fighter = bitmap_layer_create(bounds);
 // bitmap_layer_set_compositing_mode(p1_fighter, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(p1_fighter));

  //player two bitmapLayer
  p2_fighter = bitmap_layer_create(bounds);
 // bitmap_layer_set_compositing_mode(p2_fighter, GCompOpClear);
  layer_add_child(window_layer,bitmap_layer_get_layer(p2_fighter));
  
  //referee bitmapLayer
  referee = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(referee));

  //p1/p2_flaglayer bitmapLayer
  p1_flag_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(p1_flag_layer));
  p2_flag_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(p2_flag_layer));
  
  
  
  //create the actionLayer
   enterFrame = layer_create(bounds);
   layer_set_update_proc(enterFrame, update_callback);
   layer_add_child(window_layer, enterFrame);
  
  
	text_layer = text_layer_create(GRect(0, 0, 144, 38));
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, "Hi, I'm a Pebble!");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	//layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
   timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  
}

static void deinit(){
  text_layer_destroy(text_layer);
  gbitmap_destroy(image);
  clear_a_bit();
  bitmap_layer_destroy(p1_fighter);
  bitmap_layer_destroy(p2_fighter);
  bitmap_layer_destroy(referee);
  bitmap_layer_destroy(image_layer);
  bitmap_layer_destroy(p1_flag_layer);
  bitmap_layer_destroy(p2_flag_layer);
  window_destroy(window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}
