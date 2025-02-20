#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_TEXT_LENGTH 2000
#define MAX_COL_LEN 80
#define MAX_ROW_LEN 25

#define FE_INPUT_MODE 1
#define FE_MUTE_MODE 2

void clearScreen(SDL_Renderer * renderer);
void mkTxtSurface(TTF_Font * font, char lines[MAX_ROW_LEN][MAX_COL_LEN], int lineCount, SDL_Color * textColor, SDL_Texture ** textTexture, SDL_Renderer * renderer);
void putchar_window(char lines[MAX_ROW_LEN][MAX_COL_LEN], int *lineCount, const char *c);

void fe_boot_m(char lines[MAX_ROW_LEN][MAX_COL_LEN], int *lineCount, int *running) {
    putchar_window(lines, lineCount, "BOOT: Running bootstrap...");
    sleep(1);
    *running = 0;
}

int main(int argc, char* argv[]) {
    /* SDL init */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Не удалось инициализировать SDL: %s\n", SDL_GetError());
        return 1;
    }

    /* TTF init */
    if (TTF_Init() == -1) {
        printf("Не удалось инициализировать TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("fe boot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        printf("Не удалось создать окно: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Не удалось создать рендерер: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/fonts/ProggyCleanRu.ttf", 15);
    if (!font) {
        printf("Не удалось загрузить шрифт: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Color textColor = {255, 255, 255};
    char lines[MAX_ROW_LEN][MAX_COL_LEN] = {0};
    SDL_Texture* txt_lines[MAX_ROW_LEN] = {0};
    int lines_w[MAX_ROW_LEN] = {0};
    int lineCount = 0;

    int running = 1;
    int isDelPressed = 0;
    int feCurrentMode = FE_MUTE_MODE;
    SDL_Event event;

    putchar_window(lines, &lineCount, "BOOT: Booting Files Engine...");
    lineCount++;

    uint32_t lastDelPressTime = SDL_GetTicks();
    putchar_window(lines, &lineCount, "<Press DEL to run settings mode>");
    lineCount++;

    while (running) {
        if (!isDelPressed && SDL_GetTicks() - lastDelPressTime > 1000) {
            fe_boot_m(lines, &lineCount, &running);
            // running = 0;
            // fe_main();
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (lines[lineCount][strlen(lines[lineCount])-1] != 0x01 && strlen(lines[lineCount])>0) {
                        lines[lineCount][strlen(lines[lineCount])-1] = 0;
                    }
                }

                else if (feCurrentMode == FE_INPUT_MODE && event.key.keysym.sym == SDLK_RETURN) {
                    if (lineCount < MAX_ROW_LEN - 1) {
                        lineCount++;
                    }
                }

                else if (!isDelPressed && event.key.keysym.sym == SDLK_DELETE) {
                    isDelPressed = 1;

                    putchar_window(lines, &lineCount, "BOOT: Settings mode is on.");
                    lineCount++;
                    putchar_window(lines, &lineCount, "fe bootstrap> \x01");

                    feCurrentMode = FE_INPUT_MODE;
                }
            }

            else if (feCurrentMode == FE_INPUT_MODE && event.type == SDL_TEXTINPUT) {
                putchar_window(lines, &lineCount, event.text.text);
            }
        }

        clearScreen(renderer);
        mkTxtSurface(font, lines, lineCount, &textColor, txt_lines, renderer);


        for (unsigned int i=0; i<MAX_ROW_LEN; ++i) {
            if (txt_lines[i]==0) { continue; }
            int textHeight;
            SDL_QueryTexture(txt_lines[i], NULL, NULL, &lines_w[i], &textHeight);
            SDL_Rect renderQuad = { 0, i*17, lines_w[i], textHeight };
            SDL_RenderCopy(renderer, txt_lines[i], NULL, &renderQuad);
        }

        /* render */
        SDL_RenderPresent(renderer);
    }

    /* free resources */
    for (unsigned int i=0; i<MAX_ROW_LEN; ++i) {
        SDL_DestroyTexture(txt_lines[i]);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}


void clearScreen(SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void mkTxtSurface(TTF_Font * font, char lines[MAX_ROW_LEN][MAX_COL_LEN], int lineCount, SDL_Color * textColor, SDL_Texture ** textTextures, SDL_Renderer * renderer) {
    for (int i = 0; i <= lineCount; i++) {
        // Проверка на пустую строку
        if (strlen(lines[i]) == 0) {
            textTextures[i] = NULL; // Установите текстуру в NULL, если строка пустая
            continue; // Переход к следующей строке
        }

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, lines[i], *textColor);
        if (!textSurface) {
            printf("Не удалось создать поверхность текста: %s\n", TTF_GetError());
            textTextures[i] = NULL; // Установите текстуру в NULL в случае ошибки
            continue; // Переход к следующей строке
        }

        if (textTextures[i]) {
            SDL_DestroyTexture(textTextures[i]);
        }

        textTextures[i] = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }
}




void putchar_window(char lines[MAX_ROW_LEN][MAX_COL_LEN], int *lineCount, const char *c) {
    if (strlen(lines[*lineCount]) < MAX_COL_LEN - 1) {
        strncat(lines[*lineCount], c, MAX_COL_LEN - strlen(lines[*lineCount]) - 1);
    }
}
