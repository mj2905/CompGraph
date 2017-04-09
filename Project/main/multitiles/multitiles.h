#pragma once
#include "icg_helper.h"
#include "../framebuffer.h"
#include "../grid/grid.h"
#include "../perlin_noise/perlin.h"

#include <array>

#define BL_TILE 0 //visible, bottom left tile
#define BR_TILE 1 //visible, bottom right tile
#define TL_TILE 2 //visible, top left tile
#define TR_TILE 3 //visible, top right tile
#define NV_LR_B 4 //not visible, left or right, bottom tile
#define NV_LR_T 5 //not visible, left or right, top tile
#define NV_BT_R 6 //not visible, bottom or top, right tile
#define NV_BT_L 7 //not visible, bottom or top, left tile

/* the values are as follows
 *
 *   7 6
 * 5 2 3 5
 * 4 0 1 4
 *   7 6
 *
 * */


using namespace glm;

class MultiTiles {
public:
    MultiTiles(unsigned int x, unsigned int y) : x(x+0.5), y(y+0.5), x_visible(x+0.5), y_visible(y+0.5),
    framebuffers_positions({BL_TILE, BR_TILE, TL_TILE, TR_TILE, NV_LR_B, NV_LR_T, NV_BT_R, NV_BT_L}){ //we assign values to the array (initial order)

    }

    void Init() {

        assert(INCREMENT <= 0.5);

        for(size_t i = 0; i < framebuffers.size(); ++i) {
            framebuffers[i].Init(size_tile, size_tile, true);
        }

        grid.Init();
        perlin.Init();

        for(int i = 0; i < 4; ++i) {
            drawPerlin(framebuffers[i], x + (-0.5+ (i%2)), y + (-0.5 + i/2)); //we draw in the visible framebuffers, with values (-0.5, -0.5), (0.5, -0.5), (-0.5, 0.5), (0.5, 0.5)
        }
        grid.changeTexture(getTexturesVisible()); //we give it to the grid, so that it can be rendered
    }

    void Draw(const mat4 &model = IDENTITY_MATRIX,
              const mat4 &view = IDENTITY_MATRIX,
              const mat4 &projection = IDENTITY_MATRIX) {
        grid.Draw(x_visible, y_visible, model, view, projection);
        //perlin.Draw(x_visible, y_visible);
    }

    void incrementX() {
        //if we are moving a bit to the right from the center, we compute and store the right not visible tiles
        if(x_visible <= x and x_visible + INCREMENT > x) {
            drawRightTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
        if(x_visible + INCREMENT >= x + 0.5) {

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


            //we compute the corners if we need them, otherwise there would be some problems of bad computed textures.
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

        x_visible += INCREMENT;
    }

    void decrementX() {
        //if we are moving a bit to the left from the center, we compute and store the left not visible tiles
        if(x_visible >= x and x_visible - INCREMENT < x) {
            drawLeftTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
        if(x_visible - INCREMENT <= x - 0.5) {

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

            //we compute the corners if we need them, otherwise there would be some problems of bad computed textures.
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

        x_visible -= INCREMENT;
    }

    void incrementY() {
        //if we are moving a bit to the top from the center, we compute and store the top not visible tiles
        if(y_visible <= y and y_visible + INCREMENT > y) {
            drawUpTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
        if(y_visible + INCREMENT >= y + 0.5) {

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

            //we compute the corners if we need them, otherwise there would be some problems of bad computed textures.
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

        y_visible += INCREMENT;
    }

    void decrementY() {
        //if we are moving a bit to the bottom from the center, we compute and store the bottom not visible tiles
        if(y_visible >= y and y_visible - INCREMENT < y) {
            drawDownTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
        if(y_visible - INCREMENT <= y - 0.5) {

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

            //we compute the corners if we need them, otherwise there would be some problems of bad computed textures.
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

        y_visible -= INCREMENT;
    }

    void Cleanup() {
        grid.Cleanup();
        perlin.Cleanup();
        for(auto& framebuffer : framebuffers) {
            framebuffer.Cleanup();
        }
    }

private:
        const float INCREMENT = 0.005; // the increment step, must be <= 0.5
        float x, y;
        float x_visible, y_visible;
        Grid grid;
        PerlinNoise perlin;
        int size_tile = 512;
        array<FrameBuffer, 8> framebuffers;
        array<size_t, 8> framebuffers_positions; //array of indices of framebuffers

        //method used to give to the grid the framebuffers we want to display
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
