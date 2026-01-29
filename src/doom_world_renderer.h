#ifndef DOOM_WORLD_RENDERER_H
#define DOOM_WORLD_RENDERER_H

#include <stdint.h>
#include <string.h>

/**
 * Doom World Renderer - Phase 2.4
 * 
 * Purpose:
 * Implements raycasting-based 3D world rendering from Doom Nano
 * Optimized for 128×64 OLED display with 1-bit framebuffer
 * 
 * Architecture:
 * - Extracts renderMap() raycasting logic from Doom Nano
 * - Scales to OLED-compatible resolution
 * - Uses gradient dithering for depth perception
 * - Outputs to standard framebuffer for blitting
 * 
 * Constraints:
 * - No dynamic allocation (all static)
 * - Synchronous rendering (no internal loops)
 * - Non-blocking <15ms per frame
 * - Level data passed from doom-nano level.h
 */

namespace DoomWorldRenderer {

/**
 * Initialize world renderer
 * Sets up buffers and state
 */
void initialize();

/**
 * Render 3D world view
 * Cast rays and draw walls based on raycasting algorithm
 * 
 * @param level: Doom Nano level data
 * @param player_x: Player X position
 * @param player_y: Player Y position
 * @param dir_x: Player direction vector X
 * @param dir_y: Player direction vector Y
 * @param plane_x: Camera plane vector X
 * @param plane_y: Camera plane vector Y
 * @param view_height: Vertical camera bobbing offset
 */
void renderWorld(
    const uint8_t* level,
    double player_x, double player_y,
    double dir_x, double dir_y,
    double plane_x, double plane_y,
    double view_height
);

/**
 * Get rendered world framebuffer
 * Returns pointer to 128×64 monochrome buffer
 */
uint8_t* getWorldFramebuffer();

/**
 * Get depth buffer (per-column depth values)
 * Used by sprite renderer for occlusion testing
 * Returns pointer to 128-byte depth buffer (one value per column)
 */
const uint8_t* getDepthBuffer();

/**
 * Check initialization status
 */
bool isInitialized();

}  // namespace DoomWorldRenderer

#endif // DOOM_WORLD_RENDERER_H
