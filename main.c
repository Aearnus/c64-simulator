#include <X11/Xlib.h>
#include <unistd.h>
#include <math.h>

#define WINDOWWIDTH 1366
#define WINDOWHEIGHT 768

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

#define setForeground(r,g,b) (XSetForeground(display,graphics_context,colorFromRGB(colormap,display,r,g,b).pixel))
#define fillRectangle(x,y,w,h) XFillRectangle(display,window,graphics_context,x,y,w,h)
void draw(Display* display, Window window, GC graphics_context, Colormap colormap) {
    setForeground(164, 163, 255); //light outer purple and text color
    fillRectangle(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    
    setForeground(65, 64, 228); //dark inner purple
    fillRectangle(50, 50, WINDOWWIDTH - 100, WINDOWHEIGHT - 100);
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
