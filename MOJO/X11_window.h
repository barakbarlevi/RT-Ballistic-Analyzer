/*=============================================================================
Basic X11 implementation for an illustrative text box. Its background color
and presented text will alter if and when a signal is recevied by a condition
variable.
=============================================================================*/

#pragma once
#include <X11/Xlib.h>

unsigned long returnRGB(int r, int g, int b) {return b + (g<<8) + (r<<16);}

// The X window which contains the text.
struct {
    int width;
    int height;
    char * text;
    int text_len;
    char * text2;
    int text_len2;
    Display * display;
    int screen;
    Window root;
    Window window;
    GC gc;
    XFontStruct * font;
    unsigned long black_pixel;    
    unsigned long white_pixel;
    unsigned long red =returnRGB(255,0,0);
    unsigned long green =returnRGB(0,255,0);
    unsigned long blue =returnRGB(0,0,255);
}
text_box;


// Connect to the display, set up the basic variables.
static void x_connect ()
{
    text_box.display = XOpenDisplay (NULL);
    if (! text_box.display) {
        std::cout << "text box display failed: " << std::strerror(errno) << std::endl;
        //fprintf (stderr, "Could not open display.\n");
        exit (1);
    }
    text_box.screen = DefaultScreen (text_box.display);
    text_box.root = RootWindow (text_box.display, text_box.screen);
    text_box.black_pixel = BlackPixel (text_box.display, text_box.screen);
    text_box.white_pixel = WhitePixel (text_box.display, text_box.screen);
}


// Create the window.
static void create_window ()
{
    text_box.width = 200;
    text_box.height = 200;
    text_box.window = 
    XCreateSimpleWindow(text_box.display,
                            text_box.root,
                            1, /* x */
                            1, /* y */
                            text_box.width,
                            text_box.height,
                            0, /* border width */
                            text_box.black_pixel, /* border pixel */
                            text_box.green  /* background */);
    XSelectInput(text_box.display, text_box.window, ExposureMask);
    XMapWindow(text_box.display, text_box.window);   
}


// Set up the GC (Graphics Context).
static void set_up_gc ()
{
    text_box.screen = DefaultScreen(text_box.display);
    text_box.gc = XCreateGC(text_box.display, text_box.window, 0, 0);
    XSetBackground(text_box.display, text_box.gc, text_box.white_pixel); 
    XSetForeground(text_box.display, text_box.gc, text_box.black_pixel); 
}


// Set up the text font.
static void set_up_font ()
{
    const char * fontname = "-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*";
    text_box.font = XLoadQueryFont (text_box.display, fontname);

    // If the font could not be loaded, revert to the "fixed" font.
    if (! text_box.font) {
        fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
        text_box.font = XLoadQueryFont (text_box.display, "fixed");
    }
    XSetFont (text_box.display, text_box.gc, text_box.font->fid);
}


// Draw the window.
static void draw_screen (SyncObject *syncObject)
{
    sleep(1);
    //printf("entered draw_screen\n");
    int x;
    int y;
    int direction;
    int ascent;
    int descent;
    XCharStruct overall;

    // Centre the text in the middle of the box.
    XTextExtents (text_box.font, text_box.text, text_box.text_len,
                & direction, & ascent, & descent, & overall);
    x = (text_box.width - overall.width) / 2;
    y = text_box.height / 2 + (ascent - descent) / 2;

    XSetWindowBackground(text_box.display, text_box.window, text_box.green); 
    XClearWindow (text_box.display, text_box.window);
    printf("Right before XDrawString for green\n");
    XDrawString (text_box.display, text_box.window, text_box.gc,
                x, y, text_box.text, text_box.text_len);
    XSync(text_box.display, false);
    
    //printf("Here before wait line\n");
    if(!syncObject->condition_boolean_color_)
    {
        pthread_cond_wait(&syncObject->condition_variable_color_, &syncObject->condition_lock_color_);
        //printf("Here after wait line for color\n");
        XSetWindowBackground(text_box.display, text_box.window, text_box.red); 
        XClearWindow(text_box.display, text_box.window);
        printf("Right before XDrawString for red\n");
        XDrawString (text_box.display, text_box.window, text_box.gc,
                x, y, text_box.text2, text_box.text_len2);
        XSync(text_box.display, false);
        syncObject->condition_boolean_color_ = true;
        pthread_mutex_unlock(&syncObject->condition_lock_color_);
    }
    
    pthread_cond_wait(&syncObject->condition_variable_finished_, &syncObject->condition_lock_finished_);
    pthread_mutex_unlock(&syncObject->condition_lock_finished_);
    //printf("exiting draw_screen\n");
    
    XFreeGC(text_box.display, text_box.gc);
    XDestroyWindow(text_box.display, text_box.window);
    XCloseDisplay(text_box.display);
    exit(0);
}


void *windowWork(void *arg) 
{
    text_box.text = (char*)"Pure ballistic";
    text_box.text_len = strlen (text_box.text);
    text_box.text2 = (char*)"Not pure ballistic";
    text_box.text_len2 = strlen (text_box.text2);

    SyncObject *syncObject = (SyncObject *) arg;

    x_connect ();
    create_window ();
    set_up_gc ();
    set_up_font ();
    
    // There's no event loop here. The window only changes when a notification is recevied.
    //event_loop();
    draw_screen(syncObject);

    return 0;
}