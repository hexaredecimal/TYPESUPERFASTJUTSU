
#include <unordered_map>

#include <sstream>
#include <string>
#include <bits/stdc++.h>

namespace X11Lib {
extern "C" {
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#include <X11/keysym.h>
}

static Display *lookup_display = NULL;
static std::stringstream output;
static int selectedIndex = 0;
static int maxIndex = 0;
static std::unordered_map<int, std::string> keymap;
static bool uppercase = false;


#define PADD(str) " " str " "

#define SPACE 65
#define BACKSPACE 22

// Ignored Symbols
#define CTRL_L 37
#define CTRL_R 105
#define SHIFT_L 50
#define SHIFT_R 62
#define CAPS_LOCK 66
#define TAB 23
#define INSERT 118
#define HOME 110
#define PRIOR 115
#define NEXT 117
#define PERIOD 60
#define COMMA 59
#define SLASH 61
#define EQUAL 21
#define MINUS 20
#define BRACKETL 34
#define BRACKETR 35
#define SEMICOLON 47
#define BACKSLASH 51
#define APOSTROPHE 48
#define GRAVE 49
#define SUPER_L 133
#define SUPER_R 134
#define ALT_L 64
#define ALT_R 108
#define RETURN 36
#define ESCAPE 9 // Raylib uses this to exit btw

int getRandomNumber(int high) { return rand() % high; }

static void initializeKeyMap() {
  keymap[CTRL_L] = PADD("CTRL");
  keymap[CTRL_R] = PADD("CTRL");
  keymap[SHIFT_L] = PADD("SHIFT");
  keymap[SHIFT_R] = PADD("SHIFT");
  keymap[CAPS_LOCK] = PADD("CL");
  keymap[BACKSPACE] = PADD("[bs]");
  keymap[SUPER_L] = PADD("SUPER");
  keymap[SUPER_R] = PADD("SUPER");
  keymap[ALT_L] = PADD("ALT");
  keymap[ALT_R] = PADD("ALT");
  keymap[RETURN] = PADD("[ENTER]");
  
  keymap[TAB] = "    ";
  keymap[INSERT] = "";
  keymap[HOME] = "";
  keymap[PRIOR] = "";
  keymap[NEXT] = "";
  keymap[PERIOD] = ".";
  keymap[COMMA] = ",";
  keymap[SLASH] = "/";
  keymap[EQUAL] = "=";
  keymap[MINUS] = "-";
  keymap[ESCAPE] = "ESC";
  keymap[SPACE] = " ";
  keymap[BRACKETL] = "[";
  keymap[BRACKETR] = "]";
  keymap[SEMICOLON] = ");";
  keymap[BACKSLASH] = "/";
  keymap[APOSTROPHE] = "'";
  keymap[GRAVE] = "`";
}

static void writeKey(int keycode, char *keyname) {
  if (keymap.find(keycode) != keymap.end()) {
    if (keycode == CAPS_LOCK) {
      uppercase = !uppercase;
    }

    auto key = keymap[keycode];
    if (uppercase) {
      transform(key.begin(), key.end(), key.begin(),
              ::toupper);
    }
    output << key;
  } else {
    output << keyname;
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
        /*
        if (keyname)
          printf("KeyPress: %s -> %d\n", keyname, keycode);
        else
          printf("KeyPress: [keycode %d]\n", keycode); */
        writeKey(keycode, keyname);
        selectedIndex = getRandomNumber(maxIndex);
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
