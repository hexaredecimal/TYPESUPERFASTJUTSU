#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <string>
#include <vector>


#include "raylib.hpp"
#include "x11.hpp"

constexpr int WIDTH = 200;
constexpr int HEIGHT = 200;
constexpr float SCALE = 0.1;



int main(void) {
  srand(time(NULL));
  X11Lib::Display *ctrl_display, *data_display;
  X11Lib::XRecordRange *range;
  X11Lib::XRecordClientSpec clients;
  X11Lib::XRecordContext ctx;

  // Open two independent display connections
  ctrl_display = X11Lib::XOpenDisplay(NULL);
  if (!ctrl_display) {
    fprintf(stderr, "Cannot open control display\n");
    return 1;
  }

  data_display = X11Lib::XOpenDisplay(NULL);
  if (!data_display) {
    fprintf(stderr, "Cannot open data display\n");
    XCloseDisplay(ctrl_display);
    return 1;
  }

  X11Lib::lookup_display = X11Lib::XOpenDisplay(NULL);
  if (!X11Lib::lookup_display) {
    fprintf(stderr, "Cannot open lookup display for key translation\n");
    return 1;
  }

  // Check if RECORD extension is available
  int major, minor;
  if (!XRecordQueryVersion(ctrl_display, &major, &minor)) {
    fprintf(stderr, "XRecord extension not available\n");
    XCloseDisplay(ctrl_display);
    XCloseDisplay(data_display);
    return 1;
  }

  // Record all clients
  clients = XRecordAllClients;

  // Capture KeyPress + KeyRelease
  range = X11Lib::XRecordAllocRange();
  if (!range) {
    fprintf(stderr, "Failed to allocate XRecordRange\n");
    return 1;
  }
  range->device_events.first = KeyPress;
  range->device_events.last = KeyRelease;

  ctx = XRecordCreateContext(ctrl_display, 0, &clients, 1, &range, 1);
  if (!ctx) {
    fprintf(stderr, "Failed to create XRecord context\n");
    return 1;
  }

  // Flush control display so context is created
  XSync(ctrl_display, False);

  printf("Listening for global key events...\n");
  fflush(stdout);

  // Enable context on the data display
  if (!XRecordEnableContextAsync(data_display, ctx, X11Lib::event_callback,
                                 NULL)) {
    fprintf(stderr, "Failed to enable XRecord context\n");
    return 1;
  }

  Raylib::InitWindow(WIDTH, HEIGHT, "Hello, from raylib");
  Raylib::SetTargetFPS(60);

  std::vector<Raylib::Texture2D> images;
  auto images_path = "./images/";
  for (int i = 1; i <= 14; ++i) {
    std::stringstream pathStream;
    pathStream << images_path << "frame" << i << ".png";
    std::string path = pathStream.str();
    images.push_back(Raylib::LoadTexture(path.c_str()));
  }

  Raylib::Texture2D firstTexture = images[X11Lib::selectedIndex];

  float frameWidth = static_cast<float>(firstTexture.width);
  float frameHeight = static_cast<float>(firstTexture.height);


  Raylib::Rectangle sourceRec = {
      .x = 0.0f, .y = 0.0f, .width = frameWidth, .height = frameHeight};


  Raylib::Rectangle destRec = {.x = WIDTH / 2,
                               .y = HEIGHT / 2,
                               .width = frameWidth / 5.0f,
                               .height = frameHeight / 5.0f};

  Raylib::Vector2 origin = {.x = frameWidth * SCALE, .y = frameHeight * SCALE};

  int fontSize = 18;

  int boxStartX = 10;
  int boxStartY = HEIGHT - (HEIGHT * 0.20);;
  int boxHeight = fontSize + 10;
  int boxWidth = WIDTH - 2 * boxStartX;

  // Event processing loop
  while (!Raylib::WindowShouldClose()) {
    XRecordProcessReplies(data_display);

    auto output = X11Lib::output.str();
    int textWidth = Raylib::MeasureText(output.c_str(), fontSize);
    while (textWidth > boxWidth - 10 && !output.empty()) { // 5px padding each side
        output.erase(0, 1); // drop leftmost character
        textWidth = Raylib::MeasureText(output.c_str(), fontSize);
    }

    Raylib::BeginDrawing();
    Raylib::ClearBackground(Raylib::Color{.r = 18, .g = 18, .b = 18, .a = 255});

    firstTexture = images[X11Lib::selectedIndex];
    Raylib::DrawTexturePro(firstTexture, sourceRec, destRec, origin, 0,
                           Raylib::WHITE);

    Raylib::DrawText("[", boxStartX, boxStartY, fontSize, Raylib::WHITE);
    Raylib::DrawText("]", boxStartX + boxWidth - Raylib::MeasureText("]", fontSize), boxStartY, fontSize, Raylib::WHITE);

    int textX = boxStartX + boxWidth - textWidth - Raylib::MeasureText("]", fontSize) - 5; // 5px padding
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
