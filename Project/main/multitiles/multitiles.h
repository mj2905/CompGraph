#pragma once
#include "icg_helper.h"
#include "../framebuffer.h"
#include "../terrain/terrain.h"
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
    MultiTiles(unsigned int offset_x, unsigned int offset_y) : x(offset_x+0.5), y(offset_y+0.5), x_visible(offset_x+0.5), y_visible(offset_y+0.5),
    framebuffers_positions({BL_TILE, BR_TILE, TL_TILE, TR_TILE, NV_LR_B, NV_LR_T, NV_BT_R, NV_BT_L}){ //we assign values to the array (initial order)

    }

    void Init(size_t width, size_t height) {

        assert(INCREMENT <= 0.5);

        for(size_t i = 0; i < framebuffers.size(); ++i) {
            framebuffers[i].Init(size_tile, size_tile, true);
        }

        terrain.Init(width, height);

        perlin.Init();

        for(int i = 0; i < 4; ++i) {
            drawPerlin(framebuffers[i], x + (-0.5+ (i%2)), y + (-0.5 + i/2)); //we draw in the visible framebuffers, with values (-0.5, -0.5), (0.5, -0.5), (-0.5, 0.5), (0.5, 0.5)
        }
        terrain.changeTexture(getTexturesVisible()); //we give it to the terrain, so that it can be rendered
        terrain.Moved(x_visible, y_visible);
    }

    void Draw(
              const mat4 &model = IDENTITY_MATRIX,
              const mat4 &view = IDENTITY_MATRIX,
              const mat4 &projection = IDENTITY_MATRIX) {
        terrain.Draw(x_visible, y_visible, model, view, projection);
    }


    void incrementX(float increment) {
      assert(increment <= 0.5);

        //if we are moving a bit to the right from the center, we compute and store the right not visible tiles
        if(x_visible <= x and x_visible + increment > x) {
            drawRightTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
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

            terrain.changeTexture(getTexturesVisible());


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

        x_visible += increment;
        terrain.Moved(x_visible, y_visible);
    }


    void decrementX(float increment) {
      assert(increment <= 0.5);

        //if we are moving a bit to the left from the center, we compute and store the left not visible tiles
        if(x_visible >= x and x_visible - increment < x) {
            drawLeftTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
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

            terrain.changeTexture(getTexturesVisible());

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

        x_visible -= increment;
        terrain.Moved(x_visible, y_visible);
    }


    void incrementY(float increment) {
      assert(increment <= 0.5);

        //if we are moving a bit to the top from the center, we compute and store the top not visible tiles
        if(y_visible <= y and y_visible + increment > y) {
            drawUpTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
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

            terrain.changeTexture(getTexturesVisible());

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

        y_visible += increment;
        terrain.Moved(x_visible, y_visible);
    }

    Terrain* getTerrain() {
      return &terrain;
    }

    void decrementY(float increment) {
      assert(increment <= 0.5);

        //if we are moving a bit to the bottom from the center, we compute and store the bottom not visible tiles
        if(y_visible >= y and y_visible - increment < y) {
            drawDownTiles();
        }
        //if we are already in the boundaries of the visible tiles, we rotate tiles so that we have a continuity
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

            terrain.changeTexture(getTexturesVisible());

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

        y_visible -= increment;
        terrain.Moved(x_visible, y_visible);
    }

    void Cleanup() {
        terrain.Cleanup();
        perlin.Cleanup();
        for(auto& framebuffer : framebuffers) {
            framebuffer.Cleanup();
        }
    }

private:
        const float INCREMENT = 0.001; // the increment step, must be <= 0.5
        float x, y;
        float x_visible, y_visible;
        Terrain terrain;
        PerlinNoise perlin;
        size_t size_tile = 512;
        array<FrameBuffer, 8> framebuffers;
        array<size_t, 8> framebuffers_positions; //array of indices of framebuffers

        //method used to give to the terrain the framebuffers we want to display
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
