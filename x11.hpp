namespace X11Lib {
extern "C" {
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#include <X11/keysym.h>
#include <stdio.h>
}

#include <sstream>
#include <string>
#include <cstdlib>


static Display *lookup_display = NULL;
static std::stringstream output;
static int selectedIndex = 0;

#define SPACE 65
#define BACKSPACE 22

// Ignored Symbols
#define CTRL 37
#define SHIFT 50
#define CAPS_LOCK 66
#define TAB 23
#define INSERT 118
#define HOME 110
#define PRIOR 115
#define NEXT 117
#define ESCAPE 9 // Raylib uses this to exit btw

int getRandomNumber(int high) {
    return rand() % high;
}

static bool should_key_be_ignored(int code) {
    return CTRL == code || SHIFT == code || CAPS_LOCK == code || TAB == code ||
           INSERT == code || HOME == code || PRIOR == code || NEXT == code;
}

static void writeKey(int keycode, char* keyname) {
    if (!should_key_be_ignored(keycode)) {
        switch (keycode) {
        case SPACE:
            output << "[_]";
	    break;
        case BACKSPACE:
            output << "[" << keyname <<"]";
	    break;
        default:
            output << keyname;
        }
    }
}


static void event_callback(XPointer priv, XRecordInterceptData *hook) {
  if (hook->category == XRecordFromServer) {
    const unsigned char *data = hook->data;
    int event_type = data[0] & 0x7F; // low 7 bits = event type
    int keycode = data[1];

    if (event_type == KeyPress || event_type == KeyRelease) {
      KeySym keysym = XKeycodeToKeysym(lookup_display, keycode, 0);
      char *keyname = XKeysymToString(keysym);

      if (event_type == KeyPress) {
        /*if (keyname)
          printf("KeyPress: %s -> %d\n", keyname, keycode);
        else
          printf("KeyPress: [keycode %d]\n", keycode);
        */
        writeKey(keycode, keyname);
        selectedIndex = getRandomNumber(14);
      } else {
        /*
        if (keyname)
          printf("KeyRelease: %s\n", keyname);
        else
          printf("KeyRelease: [keycode %d]\n", keycode);
        */
      }
      fflush(stdout);
    }
  }

  XRecordFreeData(hook);
}


} // namespace X11Lib
