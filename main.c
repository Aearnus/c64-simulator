#include <X11/Xlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define WINDOWWIDTH 1366
#define WIDTHMARGIN 50
#define WINDOWHEIGHT 768
#define HEIGHTMARGIN 50
//this is the original size, spare resolution changes
//6 colored pixels, two on the right for spacing
#define CHARWIDTH 8
//7 colored pixels, one on the bottom for spacing
#define CHARHEIGHT 8
#define CHARSPERROW ((int)(WINDOWWIDTH/CHARWIDTH))
#define CHARSPERCOLUMN ((int)(WINDOWHEIGHT/CHARHEIGHT))

//helper functions
#define to255(x) floor(x*(255/65535))
#define to65535(x) floor(x*(65535/255))
XColor colorFromRGB(Colormap colormap, Display* display, int red, int green, int blue) {
    XColor color;
    color.red = to65535(red);
    color.green = to65535(green);
    color.blue = to65535(blue);
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, colormap, &color);
    return color;
}

char text[CHARSPERCOLUMN][CHARSPERROW];
void _writeFlowingText(Display* display, Window window, GC graphics_context) {
    int i; //iterate through the text array
    for (i = 0; i < CHARSPERCOLUMN; i++) {
        XDrawString(display, window, graphics_context,
                    WIDTHMARGIN + 10, HEIGHTMARGIN + CHARHEIGHT + (i * CHARHEIGHT),
                    text[i], strlen(text[i]));
    }
}

#define writeFlowingText() _writeFlowingText(display,window,graphics_context)
#define setForeground(r,g,b) (XSetForeground(display,graphics_context,colorFromRGB(colormap,display,r,g,b).pixel))
#define fillRectangle(x,y,w,h) XFillRectangle(display,window,graphics_context,x,y,w,h)
void draw(Display* display, Window window, GC graphics_context, Colormap colormap) {
    setForeground(164, 163, 255); //light outer purple and text color
    fillRectangle(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    
    setForeground(65, 64, 228); //dark inner purple
    fillRectangle(WIDTHMARGIN, HEIGHTMARGIN, WINDOWWIDTH - 2 * WIDTHMARGIN, WINDOWHEIGHT - 2 * HEIGHTMARGIN);

    setForeground(164, 163, 255); //light purple text
    strcpy(text[0], "*** COMMODORE 64 ***"); //need to use strcpy instead of 
    strcpy(text[1], "65335 BYTES FREE"); //just indexing because... strings? i guess?
    writeFlowingText();
}

int main() {
    Display* display;
    if ((display = XOpenDisplay(NULL)) == NULL) {
        _exit(-1);
    }

    //make the colormap
    Colormap colormap;
    colormap = DefaultColormap(display, DefaultScreen(display));
    
    //get values of white and black
    //white and black are the only colors guarenteed to exist
    int white_color = WhitePixel(display, DefaultScreen(display));
    int black_color = BlackPixel(display, DefaultScreen(display));
    
    Window window;
    window = XCreateSimpleWindow(display, //the display
                                 DefaultRootWindow(display), //default desktop
                                 0, 0, //x y coords
                                 WINDOWWIDTH, WINDOWHEIGHT, //width height
                                 0, //border width
                                 black_color, black_color); //border colors

    //get MapNotify events (structural changes to a window)
    XSelectInput(display, window, ExposureMask);

    //map the window to the screen
    XMapWindow(display, window);
    
    //create a graphics context, allows stateful graphics in X
    GC graphics_context;
    graphics_context = XCreateGC(display, window, 0, NULL);

    //set the foreground color before drawing
    XSetForeground(display, graphics_context, white_color);
    
    //main event loop
    XEvent e;
    for (;;) {
        XNextEvent(display, &e);
        //if the event states the window was Exposed and needs to be redrawn
        if (e.type == Expose) {
            draw(display, window, graphics_context, colormap);
        }
    }
    
    //send the draw requests to the server
    XFlush(display);

    return 0;
}
