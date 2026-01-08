#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <random>
#include <string>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

std::string generateNickname() {
  static std::vector<std::string> consonants = {
      "b", "c", "d", "f",  "g",  "k",  "l",  "m",  "n",  "r",  "s",
      "t", "v", "z", "br", "kr", "st", "th", "dr", "vr", "zl", "ch"};

  static std::vector<std::string> vowels = {"a", "e",  "i",  "o",
                                            "u", "ae", "io", "ou"};

  static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<int> c(0, consonants.size() - 1);
  std::uniform_int_distribution<int> v(0, vowels.size() - 1);
  std::uniform_int_distribution<int> len(2, 4);

  std::string name;
  int syllables = len(rng);

  for (int i = 0; i < syllables; ++i) {
    name += consonants[c(rng)];
    name += vowels[v(rng)];
  }

  name[0] = std::toupper(name[0]);
  return name;
}

SDL_Texture *renderText(SDL_Renderer *r, TTF_Font *font,
                        const std::string &text, SDL_Color col) {
  if (!font)
    return nullptr;

  SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text.c_str(), col);
  if (!surf)
    return nullptr;

  SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
  SDL_FreeSurface(surf);
  return tex;
}

bool pointInRect(int x, int y, const SDL_Rect &r) {
  return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("SDL_Init error: %s", SDL_GetError());
    return 1;
  }

  if (TTF_Init() != 0) {
    SDL_Log("TTF_Init warning: %s", TTF_GetError());
  }

  SDL_Window *window = SDL_CreateWindow(
      "Nickname Generator (ESC to quit)", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    SDL_Log("Window error: %s", SDL_GetError());
    return 1;
  }

  SDL_ShowWindow(window);

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    SDL_Log("Renderer error: %s", SDL_GetError());
    return 1;
  }

  TTF_Font *font = nullptr;

  const char *fontPaths[] = {
      "./Roboto-Regular.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/TTF/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/freefont/FreeSans.ttf", nullptr};

  for (int i = 0; fontPaths[i]; ++i) {
    font = TTF_OpenFont(fontPaths[i], 32);
    if (font) {
      SDL_Log("Loaded font: %s", fontPaths[i]);
      break;
    }
  }

  if (!font) {
    SDL_Log("INFO: Font load failed — running without text");
  }

  SDL_Color white = {255, 255, 255, 255};

  SDL_Rect button = {WIDTH / 2 - 100, HEIGHT - 120, 200, 60};
  std::string nickname = "PRESS GENERATE";

  bool running = true;
  SDL_Event e;

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        running = false;

      if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }

      if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mx = e.button.x;
        int my = e.button.y;

        if (pointInRect(mx, my, button)) {
          nickname = generateNickname();
        } else {
          SDL_SetClipboardText(nickname.c_str());
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &button);

    if (font) {
      SDL_Texture *btnText = renderText(renderer, font, "GENERATE", white);
      if (btnText) {
        int w, h;
        SDL_QueryTexture(btnText, nullptr, nullptr, &w, &h);
        SDL_Rect dst = {button.x + (button.w - w) / 2,
                        button.y + (button.h - h) / 2, w, h};
        SDL_RenderCopy(renderer, btnText, nullptr, &dst);
        SDL_DestroyTexture(btnText);
      }

      SDL_Texture *nickText = renderText(renderer, font, nickname, white);
      if (nickText) {
        int w, h;
        SDL_QueryTexture(nickText, nullptr, nullptr, &w, &h);
        SDL_Rect dst = {WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, w, h};
        SDL_RenderCopy(renderer, nickText, nullptr, &dst);
        SDL_DestroyTexture(nickText);
      }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  if (font)
    TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
