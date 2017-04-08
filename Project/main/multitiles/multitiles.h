#pragma once
#include "icg_helper.h"
#include "../framebuffer.h"
#include "../grid/grid.h"
#include "../perlin_noise/perlin.h"

#include <array>

#define BL_TILE 0 //visible, bottom left tile
#define BR_TILE 1
#define TL_TILE 2
#define TR_TILE 3
#define NV_LR_B 4 //not visible, left or right, bottom tile
#define NV_LR_T 5
#define NV_BT_R 6
#define NV_BT_L 7


using namespace glm;

class MultiTiles {
public:
    MultiTiles(Grid& grid, float x, float y, float increment) : grid(grid), x(x), y(y), x_visible(x), y_visible(y), increment(increment) {}

    void Init() {

        framebuffers_positions = {BL_TILE, BR_TILE, TL_TILE, TR_TILE, NV_LR_B, NV_LR_T, NV_BT_R, NV_BT_L};

        for(size_t i = 0; i < framebuffers.size(); ++i) {
            framebuffers[i].Init(size_tile, size_tile, false);
        }

        perlin.Init();

        for(int i = 0; i < 4; ++i) {
            framebuffers[i].ClearContent();
            framebuffers[i].Bind();
                perlin.Draw(x + (-0.5+ (i%2)), y + (-0.5 + i/2));
            framebuffers[i].Unbind();
        }
        grid.changeTexture(getTexturesVisible());
    }

    float incrementX() {
        if(x_visible <= x and x_visible + increment > x) {
            drawRightTiles();
        }

        if(x_visible + increment >= x + 0.5) {

            GLuint tmp_down = framebuffers_positions[NV_LR_B];
            framebuffers_positions[NV_LR_B] = framebuffers_positions[BL_TILE];
            framebuffers_positions[BL_TILE] = framebuffers_positions[BR_TILE];
            framebuffers_positions[BR_TILE] = tmp_down;

            GLuint tmp_up = framebuffers_positions[NV_LR_T];
            framebuffers_positions[NV_LR_T] = framebuffers_positions[TL_TILE];
            framebuffers_positions[TL_TILE] = framebuffers_positions[TR_TILE];
            framebuffers_positions[TR_TILE] = tmp_up;

            ++x;

            grid.changeTexture(getTexturesVisible());


            //Aim ? To compute the corner in cas we shift first a bit to the top, then we go to the right so that we change tiles, ans then we go up
            if(y_visible > y) {
                GLuint tmp = framebuffers_positions[NV_BT_L];
                framebuffers_positions[NV_BT_L] = framebuffers_positions[NV_BT_R];
                framebuffers_positions[NV_BT_R] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_BT_R]], x+0.5, y+1.5);
            }
            else if(y_visible < y) {
                GLuint tmp = framebuffers_positions[NV_BT_L];
                framebuffers_positions[NV_BT_L] = framebuffers_positions[NV_BT_R];
                framebuffers_positions[NV_BT_R] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_BT_R]], x+0.5, y-1.5);
            }
        }

        x_visible += increment;
        return x_visible;
    }

    float decrementX() {
        if(x_visible >= x and x_visible - increment < x) {
            drawLeftTiles();
        }

        if(x_visible - increment <= x - 0.5) {

            GLuint tmp_down = framebuffers_positions[NV_LR_B];
            framebuffers_positions[NV_LR_B] = framebuffers_positions[BR_TILE];
            framebuffers_positions[BR_TILE] = framebuffers_positions[BL_TILE];
            framebuffers_positions[BL_TILE] = tmp_down;

            GLuint tmp_up = framebuffers_positions[NV_LR_T];
            framebuffers_positions[NV_LR_T] = framebuffers_positions[TR_TILE];
            framebuffers_positions[TR_TILE] = framebuffers_positions[TL_TILE];
            framebuffers_positions[TL_TILE] = tmp_up;

            --x;

            grid.changeTexture(getTexturesVisible());

            if(y_visible > y) {
                GLuint tmp = framebuffers_positions[NV_BT_R];
                framebuffers_positions[NV_BT_R] = framebuffers_positions[NV_BT_L];
                framebuffers_positions[NV_BT_L] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_BT_L]], x-0.5, y+1.5);
            }
            else if(y_visible < y) {
                GLuint tmp = framebuffers_positions[NV_BT_R];
                framebuffers_positions[NV_BT_R] = framebuffers_positions[NV_BT_L];
                framebuffers_positions[NV_BT_L] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_BT_L]], x-0.5, y-1.5);
            }
        }

        x_visible -= increment;
        return x_visible;
    }

    float incrementY() {
        if(y_visible <= y and y_visible + increment > y) {
            drawUpTiles();
        }

        if(y_visible + increment >= y + 0.5) {

            GLuint tmp_right = framebuffers_positions[NV_BT_R];
            framebuffers_positions[NV_BT_R] = framebuffers_positions[BR_TILE];
            framebuffers_positions[BR_TILE] = framebuffers_positions[TR_TILE];
            framebuffers_positions[TR_TILE] = tmp_right;

            GLuint tmp_left = framebuffers_positions[NV_BT_L];
            framebuffers_positions[NV_BT_L] = framebuffers_positions[BL_TILE];
            framebuffers_positions[BL_TILE] = framebuffers_positions[TL_TILE];
            framebuffers_positions[TL_TILE] = tmp_left;

            ++y;

            grid.changeTexture(getTexturesVisible());

            if(x_visible > x) {
                GLuint tmp = framebuffers_positions[NV_LR_B];
                framebuffers_positions[NV_LR_B] = framebuffers_positions[NV_LR_T];
                framebuffers_positions[NV_LR_T] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_LR_T]], x+1.5, y+0.5);
            }
            else if(x_visible < x) {
                GLuint tmp = framebuffers_positions[NV_LR_B];
                framebuffers_positions[NV_LR_B] = framebuffers_positions[NV_LR_T];
                framebuffers_positions[NV_LR_T] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_LR_T]], x-1.5, y+0.5);
            }
        }

        y_visible += increment;
        return y_visible;
    }

    float decrementY() {
        if(y_visible >= y and y_visible - increment < y) {
            drawDownTiles();
        }

        if(y_visible - increment <= y - 0.5) {

            GLuint tmp_right = framebuffers_positions[NV_BT_R];
            framebuffers_positions[NV_BT_R] = framebuffers_positions[TR_TILE];
            framebuffers_positions[TR_TILE] = framebuffers_positions[BR_TILE];
            framebuffers_positions[BR_TILE] = tmp_right;

            GLuint tmp_left = framebuffers_positions[NV_BT_L];
            framebuffers_positions[NV_BT_L] = framebuffers_positions[TL_TILE];
            framebuffers_positions[TL_TILE] = framebuffers_positions[BL_TILE];
            framebuffers_positions[BL_TILE] = tmp_left;

            --y;

            grid.changeTexture(getTexturesVisible());

            if(x_visible > x) {
                GLuint tmp = framebuffers_positions[NV_LR_T];
                framebuffers_positions[NV_LR_T] = framebuffers_positions[NV_LR_B];
                framebuffers_positions[NV_LR_B] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_LR_B]], x+1.5, y-0.5);
            }
            else if(x_visible < x) {
                GLuint tmp = framebuffers_positions[NV_LR_T];
                framebuffers_positions[NV_LR_T] = framebuffers_positions[NV_LR_B];
                framebuffers_positions[NV_LR_B] = tmp;
                drawPerlin(framebuffers[framebuffers_positions[NV_LR_B]], x-1.5, y-0.5);
            }
        }

        y_visible -= increment;
        return y_visible;
    }

    void Cleanup() {
        perlin.Cleanup();
        for(auto& framebuffer : framebuffers) {
            framebuffer.Cleanup();
        }
    }

private:
        const float increment;
        float x, y;
        float x_visible, y_visible;
        Grid& grid;
        PerlinNoise perlin;
        int size_tile = 256;
        array<FrameBuffer, 8> framebuffers;
        array<size_t, 8> framebuffers_positions;

        array<GLuint, 4> getTexturesVisible() {
            return {framebuffers[framebuffers_positions[BL_TILE]].getTextureId(),
                        framebuffers[framebuffers_positions[BR_TILE]].getTextureId(),
                        framebuffers[framebuffers_positions[TL_TILE]].getTextureId(),
                        framebuffers[framebuffers_positions[TR_TILE]].getTextureId()};
        }

        void drawRightTiles() {
            drawPerlin(framebuffers[framebuffers_positions[NV_LR_B]], x+1.5, y-0.5);
            drawPerlin(framebuffers[framebuffers_positions[NV_LR_T]], x+1.5, y+0.5);
        }

        void drawLeftTiles() {
            drawPerlin(framebuffers[framebuffers_positions[NV_LR_B]], x-1.5, y-0.5);
            drawPerlin(framebuffers[framebuffers_positions[NV_LR_T]], x-1.5, y+0.5);
        }

        void drawUpTiles() {
            drawPerlin(framebuffers[framebuffers_positions[NV_BT_R]], x+0.5, y+1.5);
            drawPerlin(framebuffers[framebuffers_positions[NV_BT_L]], x-0.5, y+1.5);
        }

        void drawDownTiles() {
            drawPerlin(framebuffers[framebuffers_positions[NV_BT_R]], x+0.5, y-1.5);
            drawPerlin(framebuffers[framebuffers_positions[NV_BT_L]], x-0.5, y-1.5);
        }


        void drawPerlin(FrameBuffer& f, float x, float y) {
            f.ClearContent();
            f.Bind();
                perlin.Draw(x, y);
            f.Unbind();
        }
};
