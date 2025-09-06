#include <iostream>
#include <ostream>

#include <sstream>
#include <string>
#include <vector>

#include "raylib.hpp"
#include "x11.hpp"

#include "fs.hpp"

#include "ini.h"

typedef struct {
  char *character;
  char *imagePath;
  char *title;
  int width;
  int height;
} Config;

char *strip(char *str) {
  size_t len = strlen(str);
  if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
    str[len - 1] = '\0'; // remove trailing "
    return str + 1;      // skip leading "
  }
  return str; // no quotes
}

static int handler(void *user, const char *section, const char *name,
                   const char *value) {
  Config *pconfig = (Config *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("window", "width")) {
    pconfig->width = atoi(value);
  } else if (MATCH("window", "height")) {
    pconfig->height = atoi(value);
  } else if (MATCH("window", "title")) {
    pconfig->title = strip(strdup(value));
  } else if (MATCH("animation", "character")) {
    pconfig->character = strip(strdup(value));
  } else if (MATCH("animation", "imagesPath")) {
    pconfig->imagePath = strip(strdup(value));
  } else {
    std::cerr << "Invalid: Section: " << section << " field: " << name << "\n";
    return 0;
  }

  return 1;
}

int main(void) {
  Config config = {0};

  if (ini_parse("./config.ini", handler, &config) < 0) {
    std::cerr << "Can't load 'test.ini'" << std::endl;
    return 1;
  }

  srand(time(NULL));
  X11Lib::Display *ctrl_display, *data_display;
  X11Lib::XRecordRange *range;
  X11Lib::XRecordClientSpec clients;
  X11Lib::XRecordContext ctx;

  // Open two independent display connections
  ctrl_display = X11Lib::XOpenDisplay(NULL);
  if (!ctrl_display) {
    std::cerr << "Cannot open control display\n" << std::endl;
    return 1;
  }

  data_display = X11Lib::XOpenDisplay(NULL);
  if (!data_display) {
    std::cerr << "Cannot open data display\n" << std::endl;
    XCloseDisplay(ctrl_display);
    return 1;
  }

  X11Lib::lookup_display = X11Lib::XOpenDisplay(NULL);
  if (!X11Lib::lookup_display) {
    std::cerr << "Cannot open lookup display for key translation" << std::endl;
    return 1;
  }

  // Check if RECORD extension is available
  int major, minor;
  if (!XRecordQueryVersion(ctrl_display, &major, &minor)) {
    std::cerr << "XRecord extension not available" << std::endl;
    XCloseDisplay(ctrl_display);
    XCloseDisplay(data_display);
    return 1;
  }

  // Record all clients
  clients = XRecordAllClients;

  // Capture KeyPress + KeyRelease
  range = X11Lib::XRecordAllocRange();
  if (!range) {
    std::cerr << "Failed to allocate XRecordRange" << std::endl;
    return 1;
  }
  range->device_events.first = KeyPress;
  range->device_events.last = KeyRelease;

  ctx = XRecordCreateContext(ctrl_display, 0, &clients, 1, &range, 1);
  if (!ctx) {
    std::cerr << "Failed to create XRecord context" << std::endl;
    return 1;
  }

  // Flush control display so context is created
  XSync(ctrl_display, False);

  std::cout << "Listening for global key events...\n" << std::endl;
  fflush(stdout);

  // Enable context on the data display
  if (!XRecordEnableContextAsync(data_display, ctx, X11Lib::event_callback,
                                 NULL)) {
    std::cout << "Failed to enable XRecord context\n" << std::endl;
    return 1;
  }

  X11Lib::initializeKeyMap();

  const int WIDTH = config.width;
  const int HEIGHT = config.height;
  const float SCALE = 0.1;

  Raylib::InitWindow(WIDTH, HEIGHT, config.title);
  Raylib::SetTargetFPS(60);

  std::vector<Raylib::Texture2D> images;
  std::vector<std::string> imagePaths;

  char buffer[256] = "";
  std::stringstream ss;
  ss << config.imagePath << "/" << config.character << "/";

  std::string images_path(ss.str());
  if (!Fs::getImagesInPath(images_path, imagePaths, ".png"))
    return 1;

  X11Lib::maxIndex = imagePaths.size();
  for (auto imagePath : imagePaths) {
    std::cout << imagePath << std::endl;
    auto path = imagePath.c_str();
    images.push_back(Raylib::LoadTexture(path));
  }

  Raylib::Texture2D firstTexture = images[X11Lib::selectedIndex];

  float frameWidth = static_cast<float>(firstTexture.width);
  float frameHeight = static_cast<float>(firstTexture.height);

  Raylib::Rectangle sourceRec = {
      .x = 0.0f, .y = 0.0f, .width = frameWidth, .height = frameHeight};

  Raylib::Rectangle destRec = {.x = static_cast<float>(WIDTH) / 2.0f,
                               .y = static_cast<float>(HEIGHT) / 2.0f,
                               .width = frameWidth / 5.0f,
                               .height = frameHeight / 5.0f};

  Raylib::Vector2 origin = {.x = frameWidth * SCALE, .y = frameHeight * SCALE};

  int fontSize = 18;

  int boxStartX = 10;
  int boxStartY = HEIGHT - (HEIGHT * 0.20);
  int boxHeight = fontSize + 10;
  int boxWidth = WIDTH - 2 * boxStartX;

  // Event processing loop
  while (!Raylib::WindowShouldClose()) {
    XRecordProcessReplies(data_display);

    auto output = X11Lib::output.str();
    int textWidth = Raylib::MeasureText(output.c_str(), fontSize);
    while (textWidth > boxWidth - 10 &&
           !output.empty()) { // 5px padding each side
      output.erase(0, 1);     // drop leftmost character
      textWidth = Raylib::MeasureText(output.c_str(), fontSize);
    }

    Raylib::BeginDrawing();
    Raylib::ClearBackground(Raylib::Color{.r = 18, .g = 18, .b = 18, .a = 255});

    firstTexture = images[X11Lib::selectedIndex];
    Raylib::DrawTexturePro(firstTexture, sourceRec, destRec, origin, 0,
                           Raylib::WHITE);

    Raylib::DrawText("[", boxStartX, boxStartY, fontSize, Raylib::WHITE);
    Raylib::DrawText("]",
                     boxStartX + boxWidth - Raylib::MeasureText("]", fontSize),
                     boxStartY, fontSize, Raylib::WHITE);

    int textX = boxStartX + boxWidth - textWidth -
                Raylib::MeasureText("]", fontSize) - 5; // 5px padding
    DrawText(output.c_str(), textX, boxStartY, fontSize, Raylib::WHITE);

    Raylib::EndDrawing();
  }

  for (auto &texture : images) {
    Raylib::UnloadTexture(texture);
  }

  Raylib::CloseWindow();

  XRecordFreeContext(ctrl_display, ctx);
  XCloseDisplay(ctrl_display);
  XCloseDisplay(data_display);
  XCloseDisplay(X11Lib::lookup_display);
  return 0;
}
