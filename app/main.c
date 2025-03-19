#include <stdint.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>
#define CLAY_IMPLEMENTATION
#include "../clay/clay.h"
#include "../clay/renderers/raylib/clay_renderer_raylib.c"

typedef struct _StreamingSite {
    const char *id;
    const char *name;
    const char *URL;
} StreamingSite;
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}
int main() {
    unsigned int f = FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT;
    Clay_Raylib_Initialize(800, 600, "Hello World Window", f);
    SetExitKey(KEY_Q);

    uint64_t arena_size = Clay_MinMemorySize();
    void *arena_ptr = malloc(arena_size);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(arena_size, arena_ptr);

    Clay_Initialize(
        arena,
        (Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() },
        (Clay_ErrorHandler) { HandleClayErrors }
    );
    Font fonts[1] = {
        LoadFontEx("/usr/share/fonts/open-sans/OpenSans-Regular.ttf", 48, 0, 1224)
    };

    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    StreamingSite video_streaming_sites[3] = {
        { "hianime", "HiAnime", "hianime.to" },
        { "9anime", "9anime", "9animetv.to" },
        { "bflix", "BFlix", "bflix.gg" },
    };
    Clay_TextElementConfig streaming_site_styling[3];

    while(!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) {
            GetScreenWidth(),
            GetScreenHeight(),
        });
        Clay_BeginLayout();
        CLAY({
            .id = CLAY_ID("AppContainer"),
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_CENTER,
                },
                .childGap = 16,
            },
            .backgroundColor = { 15, 18, 24, 255 },
        }) {
            CLAY_TEXT(
                CLAY_STRING("VidAiO"),
                CLAY_TEXT_CONFIG({ .fontSize = 48, .textColor = {255, 255, 255, 255} })
            );
            CLAY({
                .id = CLAY_ID("SearchBar"),
                .layout = {
                    .padding = CLAY_PADDING_ALL(6),
                    .sizing = { CLAY_SIZING_FIXED(400), CLAY_SIZING_FIXED(30) }
                },
                .backgroundColor = { 14, 16, 18, 255 },
            }) {
                CLAY_TEXT(
                    CLAY_STRING("Enter the video to search"),
                    CLAY_TEXT_CONFIG({
                        .fontSize = 20,
                        .textColor = {255, 255, 255, 255},
                        .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                    })
                );
            }
            CLAY({
                .id = CLAY_ID("SiteOptions"),
                .layout = {
                    .sizing = { CLAY_SIZING_FIXED(400), CLAY_SIZING_FIXED(30) },
                    .childGap = 20,
                },
            }) {
                for (uint8_t i = 0; i < 3; i++) {
                    streaming_site_styling[i] = (Clay_TextElementConfig){
                        .fontSize = 16,
                        .textColor = {230, 230, 230, 230},
                        .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                    };
                    CLAY({
                        .id = Clay__HashString(
                            (Clay_String) {
                                strlen(video_streaming_sites[i].id),
                                video_streaming_sites[i].id
                            },
                            0,
                            0
                        ),
                        .layout = {
                            .padding = CLAY_PADDING_ALL(6),
                        },
                        .backgroundColor = { 14, 16, 18, 255 },
                    }) {
                        Clay__OpenTextElement(
                            (Clay_String) {
                                strlen(video_streaming_sites[i].name),
                                video_streaming_sites[i].name
                            },
                            &streaming_site_styling[i]
                        );
                    }
                }
            }
        }
        Clay_RenderCommandArray render_commands = Clay_EndLayout();
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(render_commands, fonts);
        EndDrawing();
    }

    Clay_Raylib_Close();
}
