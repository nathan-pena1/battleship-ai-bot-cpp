#include "battleship_ui.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "combat.h"
#include "game_manager.h"
#include "map.h"
#include "player.h"
#include "raylib.h"
#include "ships.h"

namespace {

constexpr int kNameMaxChars = 24;

enum class Phase {
    NameEntry,
    FleetPlacement,
    Battle,
    GameOver,
};

struct UiTheme {
    Color bgDeep{6, 14, 22, 255};
    Color bgHorizon{12, 42, 58, 255};
    Color bgSky{18, 28, 42, 255};
    Color panel{32, 38, 46, 250};
    Color panelInner{48, 56, 68, 255};
    Color panelBorder{72, 84, 102, 255};
    Color brass{200, 168, 110, 255};
    Color accent{72, 220, 160, 255};
    Color radar{60, 255, 140, 255};
    Color text{235, 238, 242, 255};
    Color textMuted{140, 152, 168, 255};
    Color water{22, 56, 82, 255};
    Color waterHi{32, 78, 108, 255};
    Color foam{90, 140, 168, 60};
    Color shipHull{55, 62, 74, 255};
    Color shipHi{78, 88, 102, 255};
    Color hit{230, 70, 70, 255};
    Color miss{190, 205, 218, 200};
    Color ok{62, 200, 140, 255};
    Color warn{240, 190, 80, 255};
    Color hullDark{38, 44, 58, 255};
    Color hullLight{92, 108, 128, 255};
    Color deck{70, 82, 98, 255};
    Color rust{140, 72, 48, 255};
    Color muzzle{255, 200, 80, 255};
};

struct Toast {
    std::string msg;
    float ttl = 0.f;
    Color color{255, 255, 255, 255};
};

float Hash01(int x, int y) {
    uint32_t n = static_cast<uint32_t>(x * 374761393 + y * 668265263);
    n = (n ^ (n >> 13)) * 1274126177;
    return static_cast<float>(n & 0xffff) / 65535.f;
}

void DrawStars(int w, int h) {
    for (int i = 0; i < 120; i++) {
        int sx = static_cast<int>(Hash01(i, 0) * static_cast<float>(w));
        int sy = static_cast<int>(Hash01(0, i) * static_cast<float>(h * 55) / 100.f);
        float tw = Hash01(i, i);
        unsigned char a = static_cast<unsigned char>(40 + tw * 80);
        DrawPixel(sx, sy, {220, 230, 255, a});
    }
}

void DrawBackgroundScene(const UiTheme &t, int w, int h) {
    DrawRectangleGradientV(0, 0, w, h / 2, t.bgSky, t.bgHorizon);
    DrawRectangleGradientV(0, h / 2, w, h / 2, t.bgHorizon, t.bgDeep);
    DrawStars(w, h);
    float horizonY = static_cast<float>(h) * 0.42f;
    DrawRectangleGradientV(0, static_cast<int>(horizonY), w, h - static_cast<int>(horizonY),
                           Fade({20, 70, 90, 40}, 0.35f), BLANK);
    for (int i = 0; i < 5; i++) {
        float y = horizonY + 20.f + static_cast<float>(i) * 18.f;
        float a = 0.08f - static_cast<float>(i) * 0.012f;
        DrawLine(0, static_cast<int>(y), w, static_cast<int>(y), Fade(t.foam, a));
    }
}

void DrawPanelBevel(Rectangle r, const UiTheme &t, float round) {
    DrawRectangleRounded(
        {r.x + 3, r.y + 3, r.width, r.height}, round, 12, Fade(BLACK, 0.45f));
    DrawRectangleRounded(r, round, 12, t.panel);
    DrawRectangleRounded(
        {r.x + 2, r.y + 2, r.width - 4, r.height - 4}, round * 0.85f, 10,
        Fade(t.panelInner, 0.12f));
    DrawRectangleRoundedLines(r, round, 12, t.panelBorder);
    DrawRectangleRoundedLines(
        {r.x + 1, r.y + 1, r.width - 2, r.height - 2}, round * 0.9f, 10,
        Fade(t.brass, 0.35f));
}

void DrawRivets(Rectangle r, const Color &c) {
    float inset = 10.f;
    Vector2 pts[4] = {{r.x + inset, r.y + inset},
                      {r.x + r.width - inset, r.y + inset},
                      {r.x + inset, r.y + r.height - inset},
                      {r.x + r.width - inset, r.y + r.height - inset}};
    for (Vector2 p : pts) {
        DrawCircleV(p, 3.f, Fade(c, 0.5f));
        DrawCircleV(p, 1.5f, Fade(WHITE, 0.15f));
    }
}

bool ScreenToCell(Vector2 mouse, Rectangle board, float cellSize, int &row, int &col) {
    if (!CheckCollisionPointRec(mouse, board))
        return false;
    float dx = mouse.x - board.x;
    float dy = mouse.y - board.y;
    col = static_cast<int>(dx / cellSize);
    row = static_cast<int>(dy / cellSize);
    return row >= 0 && row < gridSize && col >= 0 && col < gridSize;
}

Rectangle BoardInner(Rectangle outer, float labelMargin) {
    return {outer.x + labelMargin, outer.y + labelMargin,
            outer.width - 2.f * labelMargin, outer.height - 2.f * labelMargin};
}

void DrawTacticalCorners(Rectangle inner, float len, float thick, Color col) {
    float x0 = inner.x, y0 = inner.y, x1 = inner.x + inner.width, y1 = inner.y + inner.height;
    DrawLineEx({x0, y0}, {x0 + len, y0}, thick, col);
    DrawLineEx({x0, y0}, {x0, y0 + len}, thick, col);
    DrawLineEx({x1, y0}, {x1 - len, y0}, thick, col);
    DrawLineEx({x1, y0}, {x1, y0 + len}, thick, col);
    DrawLineEx({x0, y1}, {x0 + len, y1}, thick, col);
    DrawLineEx({x0, y1}, {x0, y1 - len}, thick, col);
    DrawLineEx({x1, y1}, {x1 - len, y1}, thick, col);
    DrawLineEx({x1, y1}, {x1, y1 - len}, thick, col);
}

void DrawBattleHudBar(int w, const UiTheme &t, const char *commander, int allyShips, int hostileShips,
                      bool weaponsLive, bool enemySalvo) {
    Rectangle bar{24.f, 96.f, static_cast<float>(w) - 48.f, 36.f};
    DrawRectangleRounded(bar, 0.35f, 10, Fade(BLACK, 0.55f));
    DrawRectangleRoundedLines(bar, 0.35f, 10, Fade(t.brass, 0.5f));
    char buf[160];
    snprintf(buf, sizeof(buf), "CMDR %-20s", commander);
    DrawText(buf, 40, 104, 15, t.brass);
    snprintf(buf, sizeof(buf), "TASK FORCE  %i  HULLS", allyShips);
    DrawText(buf, 280, 104, 15, t.text);
    snprintf(buf, sizeof(buf), "BANDIT TRACK  %i", hostileShips);
    DrawText(buf, 500, 104, 15, t.radar);
    const char *st = enemySalvo ? "INCOMING SALVO" : (weaponsLive ? "FIRE CONTROL: LIVE" : "STANDBY");
    Color sc = enemySalvo ? t.warn : (weaponsLive ? t.accent : t.textMuted);
    int sw = MeasureText(st, 15);
    DrawText(st, w - sw - 40, 104, 15, sc);
    float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 5.f);
    Color led = weaponsLive && !enemySalvo ? Fade(t.accent, 0.4f + 0.5f * pulse) : Fade(t.hit, 0.35f);
    DrawCircleV({static_cast<float>(w) - sw - 54.f, 111.f}, 5.f, led);
    DrawCircleV({static_cast<float>(w) - sw - 54.f, 111.f}, 2.f, Fade(WHITE, 0.5f));
}

void DrawRadarSweep(Vector2 center, float radius, const UiTheme &t) {
    float deg = static_cast<float>(fmod(GetTime() * 38.0, 360.0));
    float rad = deg * DEG2RAD;
    Vector2 arm{center.x + cosf(rad) * radius, center.y + sinf(rad) * radius};
    DrawLineEx(center, arm, 3.f, Fade(t.radar, 0.35f));
    DrawLineEx(center, arm, 1.f, Fade(WHITE, 0.55f));
    DrawCircleSector(center, radius * 0.92f, deg - 21.f, deg + 1.f, 24, Fade(t.radar, 0.06f));
}

void DrawTargetingReticle(Rectangle cell, const UiTheme &t) {
    float cx = cell.x + cell.width / 2.f;
    float cy = cell.y + cell.height / 2.f;
    float s = std::min(cell.width, cell.height) * 0.42f;
    float tick = 5.f;
    DrawLineEx({cx - s, cy}, {cx - s + tick, cy}, 2.f, Fade(t.radar, 0.85f));
    DrawLineEx({cx + s - tick, cy}, {cx + s, cy}, 2.f, Fade(t.radar, 0.85f));
    DrawLineEx({cx, cy - s}, {cx, cy - s + tick}, 2.f, Fade(t.radar, 0.85f));
    DrawLineEx({cx, cy + s - tick}, {cx, cy + s}, 2.f, Fade(t.radar, 0.85f));
    DrawCircleLinesV({cx, cy}, s * 0.55f, Fade(t.radar, 0.5f));
}

void DrawShipSegment(Rectangle cell, const Cell (&grid)[gridSize][gridSize], int r, int c,
                     const UiTheme &t, bool damaged, float animT) {
    const Cell &cl = grid[r][c];
    const Ship *sp = cl.getShip();
    if (!sp)
        return;

    auto same = [&](int dr, int dc) -> bool {
        int nr = r + dr, nc = c + dc;
        if (nr < 0 || nr >= gridSize || nc < 0 || nc >= gridSize)
            return false;
        return grid[nr][nc].containsShip() && grid[nr][nc].getShip() == sp;
    };
    const bool nL = same(0, -1), nR = same(0, 1), nU = same(-1, 0), nD = same(1, 0);
    const bool horiz = nL || nR;
    const bool vert  = nU || nD;

    // pad must match the 'pad' value in DrawPlayerOcean / DrawEnemyRadar (2.f).
    // Extending by pad on a connected side closes the 4px gap between adjacent cells.
    const float pad   = 2.f;
    const float capIn = 5.f;   // inset for bow/stern end-caps
    const float sideF = 0.185f; // how much of cell height to inset for ship sides (horiz)

    float hLeft, hRight, hTop, hBottom;

    if (horiz && !vert) {
        hLeft   = nL ? cell.x - pad              : cell.x + capIn;
        hRight  = nR ? cell.x + cell.width + pad : cell.x + cell.width - capIn;
        hTop    = cell.y + cell.height * sideF;
        hBottom = cell.y + cell.height * (1.f - sideF);
    } else if (vert && !horiz) {
        hLeft   = cell.x + cell.width  * sideF;
        hRight  = cell.x + cell.width  * (1.f - sideF);
        hTop    = nU ? cell.y - pad               : cell.y + capIn;
        hBottom = nD ? cell.y + cell.height + pad : cell.y + cell.height - capIn;
    } else {
        // single-cell ship or unusual junction
        hLeft   = cell.x + capIn;
        hRight  = cell.x + cell.width  - capIn;
        hTop    = cell.y + capIn;
        hBottom = cell.y + cell.height - capIn;
    }

    if (hRight - hLeft < 2.f || hBottom - hTop < 2.f)
        return;

    Rectangle hull{hLeft, hTop, hRight - hLeft, hBottom - hTop};
    float cx = hull.x + hull.width  / 2.f;
    float cy = hull.y + hull.height / 2.f;

    Color colTop = damaged ? ColorBrightness(t.rust, 0.25f) : t.hullLight;
    Color colBot = damaged ? ColorBrightness(t.hit, -0.2f)  : t.hullDark;

    // Base hull body
    DrawRectangleRec(hull, colBot);

    // Highlight band and deck stripe — orientation aware so both look correct
    if (vert && !horiz) {
        // Vertical ship: highlight the LEFT side (light from left)
        Rectangle hiStrip{hull.x, hull.y, hull.width * 0.44f, hull.height};
        DrawRectangleRec(hiStrip, Fade(colTop, damaged ? 0.68f : 0.90f));
        // Deck stripe runs along the length (vertical)
        float deckX = hull.x + hull.width * 0.22f;
        DrawLineEx({deckX, hull.y + 2.f}, {deckX, hull.y + hull.height - 2.f}, 1.2f,
                   Fade(t.deck, 0.75f));
    } else {
        // Horizontal or single: highlight the TOP side
        Rectangle hiStrip{hull.x, hull.y, hull.width, hull.height * 0.46f};
        DrawRectangleRec(hiStrip, Fade(colTop, damaged ? 0.68f : 0.90f));
        // Deck stripe runs along the length (horizontal)
        float deckY = hull.y + hull.height * 0.22f;
        DrawLineEx({hull.x + 2.f, deckY}, {hull.x + hull.width - 2.f, deckY}, 1.2f,
                   Fade(t.deck, 0.75f));
    }

    // Thin outline last so it sits on top of strips
    DrawRectangleLinesEx(hull, 1.f, Fade(WHITE, damaged ? 0.22f : 0.10f));

    // Per-role details (only on specific segments to avoid repetition)
    bool isMidHoriz = (horiz && !vert) && nL && nR;
    bool isMidVert  = (vert && !horiz) && nU && nD;
    bool isBow      = (horiz && !vert) && !nR;   // rightmost = bow (horizontal)
    bool isBowV     = (vert  && !horiz) && !nD;  // bottommost = bow (vertical)

    if (isMidHoriz) {
        // Conning tower centered on this cell's hull segment
        float tw = hull.width * 0.44f, th = hull.height * 0.56f;
        DrawRectangleRec({cx - tw / 2.f, hull.y + 1.f, tw, th}, Fade(t.hullDark, 0.95f));
        DrawRectangleLinesEx({cx - tw / 2.f, hull.y + 1.f, tw, th}, 1.f, Fade(t.shipHi, 0.5f));
        for (int i = -1; i <= 1; i++) {
            float px = cx + static_cast<float>(i) * hull.width * 0.18f;
            DrawCircleV({px, hull.y + hull.height * 0.70f}, 2.f, Fade({38,46,60,255}, 0.9f));
            DrawCircleV({px, hull.y + hull.height * 0.68f}, 0.8f, Fade(WHITE, 0.22f));
        }
    } else if (isMidVert) {
        // Conning tower centered on the narrow hull — wide enough to span it
        float tw = hull.width * 0.72f, th = hull.height * 0.40f;
        float blockX = cx - tw / 2.f;
        float blockY = cy - th / 2.f;
        DrawRectangleRec({blockX, blockY, tw, th}, Fade(t.hullDark, 0.95f));
        DrawRectangleLinesEx({blockX, blockY, tw, th}, 1.f, Fade(t.shipHi, 0.5f));
        // Portholes stacked vertically
        for (int i = -1; i <= 1; i++) {
            float py = cy + static_cast<float>(i) * hull.height * 0.14f;
            DrawCircleV({blockX + tw * 0.72f, py}, 2.f, Fade({38,46,60,255}, 0.9f));
            DrawCircleV({blockX + tw * 0.70f, py - 0.5f}, 0.8f, Fade(WHITE, 0.22f));
        }
    }

    if (isBow) {
        // Bow pointed nose (horizontal — points right)
        float tipX = hull.x + hull.width - 1.f;
        float midY = hull.y + hull.height / 2.f;
        // CCW winding: top-tip → mid-nose → bottom-tip
        DrawTriangle({tipX, hull.y + hull.height * 0.85f},
                     {tipX + 7.f, midY},
                     {tipX, hull.y + hull.height * 0.15f},
                     Fade(colBot, 0.55f));
        DrawLineEx({tipX, hull.y + hull.height * 0.15f}, {tipX + 7.f, midY}, 1.5f, Fade(colTop, 0.5f));
        DrawLineEx({tipX + 7.f, midY}, {tipX, hull.y + hull.height * 0.85f}, 1.5f, Fade(colTop, 0.5f));
    }
    if (isBowV) {
        // Bow pointed nose (vertical — points downward)
        float tipY = hull.y + hull.height - 1.f;
        float midX = hull.x + hull.width / 2.f;
        // CCW winding when Y increases downward: right → nose-below → left
        DrawTriangle({hull.x + hull.width * 0.85f, tipY},
                     {hull.x + hull.width * 0.15f, tipY},
                     {midX, tipY + 7.f},
                     Fade(colBot, 0.55f));
        DrawLineEx({hull.x + hull.width * 0.15f, tipY}, {midX, tipY + 7.f}, 1.5f, Fade(colTop, 0.5f));
        DrawLineEx({midX, tipY + 7.f}, {hull.x + hull.width * 0.85f, tipY}, 1.5f, Fade(colTop, 0.5f));
    }

    // Stern gun nub (opposite end from bow)
    if ((horiz && !vert) && !nL) {
        DrawCircleV({hull.x + 4.f, cy}, 3.f, Fade(t.hullDark, 0.7f));
        DrawLineEx({hull.x + 4.f, cy}, {hull.x - 4.f, cy}, 2.5f, Fade(t.hullLight, 0.55f));
    }
    if ((vert && !horiz) && !nU) {
        DrawCircleV({cx, hull.y + 4.f}, 3.f, Fade(t.hullDark, 0.7f));
        DrawLineEx({cx, hull.y + 4.f}, {cx, hull.y - 4.f}, 2.5f, Fade(t.hullLight, 0.55f));
    }

    if (damaged) {
        float flick  = 0.5f + 0.5f * sinf(animT * 14.f);
        float fireSz = std::min(hull.width, hull.height) * 0.38f;
        DrawCircleV({cx, cy}, fireSz, Fade(t.muzzle, 0.22f * flick));
        DrawCircleV({cx, cy}, fireSz * 0.45f, Fade(t.hit, 0.35f * flick));
        float wave = static_cast<float>(fmod(animT * 2.4, 1.0)) * fireSz * 2.8f;
        if (wave > 1.f)
            DrawRing({cx, cy}, wave, wave + 2.f, 0, 360, 18, Fade(t.hit, 0.4f));
        float xcx = cell.x + cell.width  / 2.f;
        float xcy = cell.y + cell.height / 2.f;
        DrawLineEx({xcx - 8, xcy - 8}, {xcx + 8, xcy + 8}, 2.5f, Fade(WHITE, 0.92f));
        DrawLineEx({xcx - 8, xcy + 8}, {xcx + 8, xcy - 8}, 2.5f, Fade(WHITE, 0.92f));
    }
}

void DrawWaterCellBase(Rectangle cell, Color base, float shimmer) {
    Color adj = ColorBrightness(base, shimmer);
    DrawRectangleRounded(cell, 0.22f, 6, adj);
    DrawRectangleRoundedLines(cell, 0.22f, 6, Fade(WHITE, 0.05f + fabsf(shimmer) * 0.08f));
}

void DrawGridLabels(Rectangle inner, float cellSize, const UiTheme &t, int fontSize) {
    for (int i = 0; i < gridSize; i++) {
        std::string s = std::to_string(i);
        float tx = inner.x + i * cellSize + cellSize / 2.f -
                   MeasureText(s.c_str(), fontSize) / 2.f;
        DrawText(s.c_str(), static_cast<int>(tx), static_cast<int>(inner.y - 22), fontSize,
                 t.brass);
        float ty = inner.y + i * cellSize + cellSize / 2.f - fontSize / 2.f;
        DrawText(s.c_str(), static_cast<int>(inner.x - 24), static_cast<int>(ty), fontSize,
                 t.brass);
    }
}

void DrawPlayerOcean(const Cell (&grid)[gridSize][gridSize], Rectangle inner, float cellSize,
                     const UiTheme &t, int hoverR, int hoverC, bool showHover, float animT) {
    const float pad = 2.f;
    for (int r = 0; r < gridSize; r++) {
        for (int c = 0; c < gridSize; c++) {
            Rectangle cell{inner.x + c * cellSize + pad, inner.y + r * cellSize + pad,
                           cellSize - 2.f * pad, cellSize - 2.f * pad};
            Color deep = ColorBrightness(t.water, -0.05f * static_cast<float>((r + c) % 3));
            Color base = ((r + c) % 2 == 0) ? deep : t.waterHi;
            if (showHover && r == hoverR && c == hoverC)
                base = ColorBrightness(Fade(t.accent, 0.55f), 0.15f);
            float shimmer =
                0.07f * sinf(animT * 2.4f + static_cast<float>(r) * 0.7f + static_cast<float>(c) * 0.9f);
            DrawWaterCellBase(cell, base, shimmer);

            const Cell &cl = grid[r][c];
            if (cl.containsShip() && !cl.beenAttacked()) {
                DrawShipSegment(cell, grid, r, c, t, false, animT);
            } else if (cl.containsShip() && cl.beenAttacked()) {
                DrawShipSegment(cell, grid, r, c, t, true, animT);
            } else if (!cl.containsShip() && cl.beenAttacked()) {
                float cx = cell.x + cell.width / 2.f;
                float cy = cell.y + cell.height / 2.f;
                float rip = 3.f + 4.f * sinf(animT * 5.f + static_cast<float>(r + c));
                DrawRing({cx, cy}, rip, rip + 2.f, 0, 360, 16, Fade(t.miss, 0.35f));
                DrawCircleV({cx, cy}, 4.5f, Fade(t.miss, 0.85f));
                DrawCircleV({cx, cy - 1.f}, 1.5f, Fade(WHITE, 0.4f));
            }
        }
    }
}

void DrawEnemyRadar(const Cell (&grid)[gridSize][gridSize], Rectangle inner, float cellSize,
                    const UiTheme &t, int hoverR, int hoverC, bool showHover, float animT) {
    const float pad = 2.f;
    Vector2 center{inner.x + inner.width / 2.f, inner.y + inner.height / 2.f};
    float maxR = std::sqrt(inner.width * inner.width + inner.height * inner.height) / 2.f;
    DrawRing(center, maxR * 0.25f, maxR * 0.26f, 0, 360, 72, Fade(t.radar, 0.07f));
    DrawRing(center, maxR * 0.42f, maxR * 0.43f, 0, 360, 72, Fade(t.radar, 0.055f));
    DrawRing(center, maxR * 0.6f, maxR * 0.61f, 0, 360, 72, Fade(t.radar, 0.04f));
    DrawLineEx({center.x - maxR, center.y}, {center.x + maxR, center.y}, 1.f, Fade(t.radar, 0.05f));
    DrawLineEx({center.x, center.y - maxR}, {center.x, center.y + maxR}, 1.f, Fade(t.radar, 0.05f));

    for (int r = 0; r < gridSize; r++) {
        for (int c = 0; c < gridSize; c++) {
            Rectangle cell{inner.x + c * cellSize + pad, inner.y + r * cellSize + pad,
                           cellSize - 2.f * pad, cellSize - 2.f * pad};
            Color base = ((r + c) % 2 == 0) ? Fade(t.water, 0.5f) : Fade(t.waterHi, 0.45f);
            if (showHover && r == hoverR && c == hoverC)
                base = Fade(t.radar, 0.2f);
            float pulse =
                0.05f * sinf(animT * 3.f + static_cast<float>(r * 3 + c * 5));
            DrawWaterCellBase(cell, base, pulse);
            DrawRectangleRoundedLines(cell, 0.22f, 6, Fade(t.radar, 0.14f));
        }
    }

    DrawRadarSweep(center, maxR * 0.95f, t);

    for (int r = 0; r < gridSize; r++) {
        for (int c = 0; c < gridSize; c++) {
            Rectangle cell{inner.x + c * cellSize + pad, inner.y + r * cellSize + pad,
                           cellSize - 2.f * pad, cellSize - 2.f * pad};
            const Cell &cl = grid[r][c];
            if (showHover && r == hoverR && c == hoverC && !cl.beenAttacked())
                DrawTargetingReticle(cell, t);
            if (!cl.beenAttacked())
                continue;
            if (cl.containsShip()) {
                // Confirmed hit — flat destruction marker, no hull model so orientation stays hidden
                float hcx = cell.x + cell.width  / 2.f;
                float hcy = cell.y + cell.height / 2.f;
                float flick = 0.5f + 0.5f * sinf(animT * 11.f + static_cast<float>(r * 7 + c * 3));
                DrawRectangleRounded(cell, 0.2f, 6, {20, 6, 6, 235});
                DrawCircleV({hcx, hcy}, cell.width * 0.36f, Fade(t.muzzle, 0.16f * flick));
                DrawCircleV({hcx, hcy}, cell.width * 0.18f, Fade(t.hit,   0.20f * flick));
                DrawLineEx({hcx - 9, hcy - 9}, {hcx + 9, hcy + 9}, 2.8f, Fade(WHITE, 0.92f));
                DrawLineEx({hcx - 9, hcy + 9}, {hcx + 9, hcy - 9}, 2.8f, Fade(WHITE, 0.92f));
                DrawRectangleRoundedLines(cell, 0.2f, 6, Fade(t.hit, 0.72f));
            } else {
                float cx = cell.x + cell.width / 2.f;
                float cy = cell.y + cell.height / 2.f;
                DrawCircleV({cx, cy}, 6.f, Fade(t.radar, 0.12f));
                DrawRing({cx, cy}, 5.f, 8.f, 0, 360, 18, Fade(t.miss, 0.75f));
                DrawCircleV({cx, cy}, 2.f, Fade(WHITE, 0.5f));
            }
        }
    }
}

bool PreviewCells(int len, int anchorR, int anchorC, char dir, int cellsR[5], int cellsC[5],
                  int &n) {
    n = 0;
    if (dir == 'h' || dir == 'H') {
        bool right = true;
        for (int i = 0; i < len; i++) {
            int cc = anchorC + i;
            if (anchorR < 0 || anchorR >= gridSize || cc < 0 || cc >= gridSize)
                right = false;
        }
        if (right) {
            for (int i = 0; i < len; i++) {
                cellsR[n] = anchorR;
                cellsC[n++] = anchorC + i;
            }
            return true;
        }
        for (int i = 0; i < len; i++) {
            int cc = anchorC - i;
            if (anchorR < 0 || anchorR >= gridSize || cc < 0 || cc >= gridSize)
                return false;
            cellsR[n] = anchorR;
            cellsC[n++] = cc;
        }
        return n == len;
    }
    bool down = true;
    for (int i = 0; i < len; i++) {
        int rr = anchorR + i;
        if (rr < 0 || rr >= gridSize || anchorC < 0 || anchorC >= gridSize)
            down = false;
    }
    if (down) {
        for (int i = 0; i < len; i++) {
            cellsR[n] = anchorR + i;
            cellsC[n++] = anchorC;
        }
        return true;
    }
    n = 0;
    for (int i = 0; i < len; i++) {
        int rr = anchorR - i;
        if (rr < 0 || rr >= gridSize || anchorC < 0 || anchorC >= gridSize)
            return false;
        cellsR[n] = rr;
        cellsC[n++] = anchorC;
    }
    return n == len;
}

void DrawPlacementPreview(const Cell (&grid)[gridSize][gridSize], Rectangle inner, float cellSize,
                          const UiTheme &t, int len, int anchorR, int anchorC, char dir,
                          bool valid) {
    int cellsR[5], cellsC[5], n = 0;
    if (!PreviewCells(len, anchorR, anchorC, dir, cellsR, cellsC, n) || n != len)
        return;
    Color fill = valid ? Fade(t.ok, 0.4f) : Fade(t.hit, 0.32f);
    Color line = valid ? t.ok : t.hit;
    const float pad = 2.f;
    for (int i = 0; i < n; i++) {
        int r = cellsR[i], c = cellsC[i];
        Rectangle cell{inner.x + c * cellSize + pad, inner.y + r * cellSize + pad,
                       cellSize - 2.f * pad, cellSize - 2.f * pad};
        DrawRectangleRounded(cell, 0.28f, 6, fill);
        DrawRectangleRoundedLines(cell, 0.28f, 6, line);
        if (grid[r][c].containsShip()) {
            DrawRectangleRounded(
                {cell.x + 1, cell.y + 1, cell.width - 2, cell.height - 2}, 0.25f, 4,
                Fade(t.hit, 0.45f));
        }
    }
}

int FirstUnplacedIndex(std::vector<Ship> &fleet) {
    for (size_t i = 0; i < fleet.size(); i++) {
        if (!fleet[i].beenPlaced())
            return static_cast<int>(i);
    }
    return -1;
}

void PushToast(Toast &t, const std::string &m, Color c, float duration = 2.2f) {
    t.msg = m;
    t.ttl = duration;
    t.color = c;
}

// Sharp ~530ms on / ~530ms off — no sine fade, no conflicting placeholder glyph.
bool NameCaretOn() {
    return (static_cast<int>(std::floor(GetTime() * 1.85)) & 1) == 0;
}

void DrawNameEntryField(const Rectangle &card, const std::string &nameInput, const UiTheme &t) {
    const int labelFs = 20;
    const int valueFs = 28;
    const float fieldX = card.x + 44.f;
    const float fieldY = card.y + 88.f;
    const float fieldW = card.width - 88.f;
    const float fieldH = 44.f;

    DrawText("COMMANDER CALLSIGN", static_cast<int>(fieldX), static_cast<int>(card.y + 34),
             labelFs, t.brass);

    Rectangle fieldBg{fieldX, fieldY, fieldW, fieldH};
    DrawRectangleRounded(fieldBg, 0.15f, 10, Fade(BLACK, 0.55f));
    DrawRectangleRoundedLines(fieldBg, 0.15f, 10, Fade(t.brass, 0.45f));

    const int textX = static_cast<int>(fieldX + 12.f);
    const int textY = static_cast<int>(fieldY + 8.f);

    if (nameInput.empty()) {
        DrawText("Type your name, then press ENTER", textX, textY + 2, 18, Fade(t.textMuted, 0.65f));
    } else {
        DrawText(nameInput.c_str(), textX, textY, valueFs, t.text);
    }

    const int textW = MeasureText(nameInput.c_str(), valueFs);
    const int caretX = textX + textW;
    const int caretTop = textY + 2;
    const int caretH = 24;

    float underlineY = fieldY + fieldH - 9.f;
    DrawLineEx({fieldX + 14.f, underlineY}, {fieldX + fieldW - 14.f, underlineY}, 1.5f,
               Fade(t.brass, 0.4f));

    if (NameCaretOn())
        DrawRectangle(caretX + 2, caretTop, 3, caretH, t.accent);

    DrawText("ENTER — deploy    ESC — abort", static_cast<int>(fieldX),
             static_cast<int>(card.y + 188), 17, t.textMuted);
}

void DrawTitleBanner(int w, const UiTheme &t) {
    const char *title = "BATTLESHIP";
    int titleSize = 40;
    int tw = MeasureText(title, titleSize);
    int tx = w / 2 - tw / 2;
    DrawText(title, tx + 3, 29, titleSize, Fade(BLACK, 0.55f));
    DrawText(title, tx + 1, 27, titleSize, Fade(t.brass, 0.35f));
    DrawText(title, tx, 26, titleSize, t.text);
    DrawText("NAVAL COMBAT SIMULATION", w / 2 - MeasureText("NAVAL COMBAT SIMULATION", 14) / 2, 72,
             14, t.textMuted);
}

void DrawVignette(int w, int h) {
    DrawRectangleGradientV(0, 0, w, 80, Fade(BLACK, 0.5f), BLANK);
    DrawRectangleGradientV(0, h - 100, w, 100, BLANK, Fade(BLACK, 0.45f));
}

} // namespace

int run_raylib_battleship_ui() {
    const int winW = 1280;
    const int winH = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(winW, winH, "Battleship");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    UiTheme theme;
    Phase phase = Phase::NameEntry;
    std::string nameInput;
    std::string savedName;
    std::unique_ptr<User> user;
    std::unique_ptr<Bot> enemy;
    GameManager game{};

    char placeDir = 'H';
    int selectedShipIndex = 0;
    Toast toast{};

    float enemyDelay = 0.f;
    bool waitingEnemy = false;
    std::string endHeadline;
    std::string endSub;

    int bsCount = 0;
    const int bsDelay = 28;
    const int bsInterval = 4;

    while (!WindowShouldClose()) {
        const int w = GetScreenWidth();
        const int h = GetScreenHeight();
        const float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        if (IsKeyPressed(KEY_ESCAPE))
            break;

        if (toast.ttl > 0.f)
            toast.ttl -= dt;
        if (waitingEnemy) {
            enemyDelay -= dt;
            if (enemyDelay <= 0.f && user && enemy) {
                waitingEnemy = false;
                Cell(&pgrid)[gridSize][gridSize] = user->getGrid();
                bool botHit = enemy->takeTurn(pgrid, *user);
                PushToast(toast, botHit ? "Enemy scored a hit on your fleet!" : "Enemy shot missed.",
                          botHit ? theme.hit : theme.text);
                if (game.winner(*user, *enemy)) {
                    if (user->getNumShips() > enemy->getNumShips()) {
                        endHeadline = "Victory";
                        endSub = "You sank the enemy fleet.";
                    } else {
                        endHeadline = "Defeat";
                        endSub = "Your fleet has been destroyed.";
                    }
                    game.endGame();
                    phase = Phase::GameOver;
                }
            }
        }

        if (phase == Phase::NameEntry) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 126 &&
                    static_cast<int>(nameInput.size()) < kNameMaxChars)
                    nameInput += static_cast<char>(key);
                key = GetCharPressed();
            }
            if (IsKeyDown(KEY_BACKSPACE)) {
                bsCount++;
                if (bsCount == 1) {
                    if (!nameInput.empty())
                        nameInput.pop_back();
                } else if (bsCount >= bsDelay) {
                    if ((bsCount - bsDelay) % bsInterval == 0 && !nameInput.empty())
                        nameInput.pop_back();
                }
            } else
                bsCount = 0;

            if (IsKeyPressed(KEY_ENTER) && !nameInput.empty()) {
                savedName = nameInput;
                user = std::make_unique<User>(User::createPlayer(savedName));
                user->setFleet(Player::defaultFleet());
                enemy.reset();
                game = GameManager{};
                placeDir = 'H';
                selectedShipIndex = std::max(0, FirstUnplacedIndex(user->getFleet()));
                phase = Phase::FleetPlacement;
                PushToast(toast, "Place your ships. Click a ship, then the grid. [R] rotate.",
                          theme.accent, 3.5f);
            }
        } else if (phase == Phase::FleetPlacement && user) {
            if (IsKeyPressed(KEY_R))
                placeDir = (placeDir == 'H' || placeDir == 'h') ? 'V' : 'H';

            std::vector<Ship> &fleet = user->getFleet();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float sidebarW = 280.f;
                float margin = 40.f;
                float boardSize = std::min((w - sidebarW - margin * 3) * 0.95f, h - 200.f);
                boardSize = std::max(320.f, boardSize);
                float labelM = 28.f;
                Rectangle boardOuter{margin + sidebarW, 100.f, boardSize + 2 * labelM,
                                     boardSize + 2 * labelM};
                Rectangle inner = BoardInner(boardOuter, labelM);
                float cellSize = inner.width / gridSize;

                float listX = margin;
                float listY = 120.f;
                for (size_t i = 0; i < fleet.size(); i++) {
                    Rectangle row{listX, listY + static_cast<float>(i) * 52.f, sidebarW - 20, 46.f};
                    if (CheckCollisionPointRec(mouse, row) && !fleet[i].beenPlaced()) {
                        selectedShipIndex = static_cast<int>(i);
                    }
                }

                int hr = -1, hc = -1;
                if (ScreenToCell(mouse, inner, cellSize, hr, hc) && selectedShipIndex >= 0 &&
                    selectedShipIndex < static_cast<int>(fleet.size()) &&
                    !fleet[static_cast<size_t>(selectedShipIndex)].beenPlaced()) {
                    Ship &cur = fleet[static_cast<size_t>(selectedShipIndex)];
                    Cell(&grid)[gridSize][gridSize] = user->getGrid();
                    if (validPlacement(cur, grid, hr, hc, placeDir)) {
                        cur.place();
                        placeShip(cur, grid, hr, hc, placeDir);
                        int next = FirstUnplacedIndex(fleet);
                        if (next >= 0)
                            selectedShipIndex = next;
                        if (FirstUnplacedIndex(fleet) < 0) {
                            enemy = std::make_unique<Bot>();
                            enemy->setFleet(Bot::defaultFleet());
                            enemy->createGrid();
                            game = GameManager{};
                            phase = Phase::Battle;
                            PushToast(toast, "All ships ready. Hunt the enemy grid on the right.",
                                      theme.ok, 3.f);
                        }
                    } else {
                        PushToast(toast, "Invalid placement — try another cell or press [R].",
                                  theme.warn);
                    }
                }
            }
        } else if (phase == Phase::Battle && user && enemy && !waitingEnemy &&
                   !game.isOver()) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float margin = 36.f;
                float gap = 40.f;
                float maxBoard = (w - margin * 2 - gap) / 2.f;
                float boardSize = std::min(maxBoard, h - 240.f);
                boardSize = std::max(280.f, boardSize);
                float labelM = 28.f;
                float top = 142.f;
                Rectangle rightOuter{margin + boardSize + 2 * labelM + gap, top,
                                     boardSize + 2 * labelM, boardSize + 2 * labelM};
                Rectangle enemyInner = BoardInner(rightOuter, labelM);
                float cellSize = enemyInner.width / gridSize;
                Cell(&egrid)[gridSize][gridSize] = enemy->getGrid();

                int tr = -1, tc = -1;
                if (ScreenToCell(mouse, enemyInner, cellSize, tr, tc) &&
                    user->validAttack(egrid, tr, tc)) {
                    bool hit = user->attackCoordinate(egrid, *enemy, tr, tc);
                    PushToast(toast, hit ? "Direct hit!" : "Splash — missed.",
                              hit ? theme.hit : theme.text);
                    if (game.winner(*user, *enemy)) {
                        if (user->getNumShips() > enemy->getNumShips()) {
                            endHeadline = "Victory";
                            endSub = "You sank the enemy fleet.";
                        } else {
                            endHeadline = "Defeat";
                            endSub = "Your fleet has been destroyed.";
                        }
                        game.endGame();
                        phase = Phase::GameOver;
                    } else {
                        waitingEnemy = true;
                        enemyDelay = 0.55f;
                    }
                }
            }
        } else if (phase == Phase::GameOver) {
            Rectangle cardRect{w / 2.f - 260.f, h / 2.f - 120.f, 520.f, 260.f};
            const float pAgainW = 220.f;
            const float pAgainH = 50.f;
            Rectangle playAgain{w / 2.f - pAgainW / 2.f, cardRect.y + 170.f, pAgainW, pAgainH};
            if (IsKeyPressed(KEY_ENTER) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                 CheckCollisionPointRec(mouse, playAgain))) {
                user = std::make_unique<User>(User::createPlayer(savedName));
                user->setFleet(Player::defaultFleet());
                enemy.reset();
                game = GameManager{};
                placeDir = 'H';
                selectedShipIndex = std::max(0, FirstUnplacedIndex(user->getFleet()));
                phase = Phase::FleetPlacement;
                waitingEnemy = false;
                PushToast(toast, "New fleet — place your ships.", theme.accent, 2.5f);
            }
        }

        BeginDrawing();
        DrawBackgroundScene(theme, w, h);
        DrawTitleBanner(w, theme);

        if (phase == Phase::NameEntry) {
            Rectangle card{w / 2.f - 340.f, h / 2.f - 130.f, 680.f, 260.f};
            DrawPanelBevel(card, theme, 0.12f);
            DrawRivets(card, theme.brass);
            DrawNameEntryField(card, nameInput, theme);
        } else if (phase == Phase::FleetPlacement && user) {
            float sidebarW = 280.f;
            float margin = 40.f;
            float boardSize = std::min((w - sidebarW - margin * 3) * 0.95f, h - 200.f);
            boardSize = std::max(320.f, boardSize);
            float labelM = 28.f;
            Rectangle boardOuter{margin + sidebarW, 100.f, boardSize + 2 * labelM,
                                 boardSize + 2 * labelM};
            DrawPanelBevel(boardOuter, theme, 0.1f);
            DrawRivets(boardOuter, theme.brass);
            Rectangle inner = BoardInner(boardOuter, labelM);
            float cellSize = inner.width / gridSize;
            DrawGridLabels(inner, cellSize, theme, 16);

            Cell(&grid)[gridSize][gridSize] = user->getGrid();
            int hr = -1, hc = -1;
            ScreenToCell(mouse, inner, cellSize, hr, hc);
            DrawPlayerOcean(grid, inner, cellSize, theme, hr, hc, true, static_cast<float>(GetTime()));

            std::vector<Ship> &fleet = user->getFleet();
            if (selectedShipIndex >= 0 && selectedShipIndex < static_cast<int>(fleet.size()) &&
                !fleet[static_cast<size_t>(selectedShipIndex)].beenPlaced() && hr >= 0) {
                Ship &sel = fleet[static_cast<size_t>(selectedShipIndex)];
                bool okPlace = validPlacement(sel, grid, hr, hc, placeDir);
                DrawPlacementPreview(grid, inner, cellSize, theme, sel.getHealth(), hr, hc, placeDir,
                                     okPlace);
            }

            Rectangle sidePanel{margin, 100.f, sidebarW - 10, 420.f};
            DrawPanelBevel(sidePanel, theme, 0.12f);
            DrawRivets(sidePanel, theme.brass);
            DrawText("FLEET STATUS", static_cast<int>(sidePanel.x + 20),
                     static_cast<int>(sidePanel.y + 14), 18, theme.brass);
            const char *orient =
                (placeDir == 'H' || placeDir == 'h') ? "Bow: horizontal  [R] flip"
                                                     : "Bow: vertical  [R] flip";
            DrawText(orient, static_cast<int>(sidePanel.x + 20), static_cast<int>(sidePanel.y + 40),
                     15, theme.textMuted);

            float listY = sidePanel.y + 72.f;
            for (size_t i = 0; i < fleet.size(); i++) {
                Rectangle row{sidePanel.x + 14, listY + static_cast<float>(i) * 52.f,
                              sidePanel.width - 28, 46.f};
                bool placed = fleet[i].beenPlaced();
                bool sel = (static_cast<int>(i) == selectedShipIndex) && !placed;
                Color bg = sel ? Fade(theme.accent, 0.22f) : Fade(BLACK, 0.25f);
                if (placed)
                    bg = Fade(theme.ok, 0.12f);
                DrawRectangleRounded(row, 0.2f, 8, bg);
                DrawRectangleRoundedLines(row, 0.2f, 8,
                                          sel ? theme.brass : Fade(theme.panelBorder, 0.9f));
                std::string line =
                    fleet[i].getName() + "  [" + std::to_string(fleet[i].getHealth()) + "]";
                if (placed)
                    line += "  SECURED";
                Color tc = placed ? theme.textMuted : theme.text;
                DrawText(line.c_str(), static_cast<int>(row.x + 12), static_cast<int>(row.y + 12),
                         17, tc);
            }
            DrawText("DEPLOYMENT GRID", static_cast<int>(boardOuter.x + 16),
                     static_cast<int>(boardOuter.y + 8), 15, theme.brass);
        } else if (phase == Phase::Battle && user && enemy) {
            const float animT = static_cast<float>(GetTime());
            float margin = 36.f;
            float gap = 40.f;
            float maxBoard = (w - margin * 2 - gap) / 2.f;
            float boardSize = std::min(maxBoard, h - 240.f);
            boardSize = std::max(280.f, boardSize);
            float labelM = 28.f;
            float top = 142.f;
            Rectangle leftOuter{margin, top, boardSize + 2 * labelM, boardSize + 2 * labelM};
            Rectangle rightOuter{margin + boardSize + 2 * labelM + gap, top,
                                 boardSize + 2 * labelM, boardSize + 2 * labelM};
            DrawPanelBevel(leftOuter, theme, 0.1f);
            DrawPanelBevel(rightOuter, theme, 0.1f);
            DrawRivets(leftOuter, theme.brass);
            DrawRivets(rightOuter, theme.brass);

            Rectangle leftInner = BoardInner(leftOuter, labelM);
            Rectangle rightInner = BoardInner(rightOuter, labelM);
            float cellSize = leftInner.width / gridSize;
            DrawGridLabels(leftInner, cellSize, theme, 15);
            DrawGridLabels(rightInner, cellSize, theme, 15);

            Cell(&pgrid)[gridSize][gridSize] = user->getGrid();
            Cell(&egrid)[gridSize][gridSize] = enemy->getGrid();

            int lr = -1, lc = -1;
            if (!waitingEnemy)
                ScreenToCell(mouse, rightInner, cellSize, lr, lc);

            DrawPlayerOcean(pgrid, leftInner, cellSize, theme, -1, -1, false, animT);
            DrawEnemyRadar(egrid, rightInner, cellSize, theme, lr, lc, !waitingEnemy, animT);

            DrawTacticalCorners(leftInner, 16.f, 2.5f, Fade(theme.brass, 0.85f));
            DrawTacticalCorners(rightInner, 16.f, 2.5f, Fade(theme.radar, 0.75f));

            BeginScissorMode(static_cast<int>(rightInner.x), static_cast<int>(rightInner.y),
                             static_cast<int>(rightInner.width), static_cast<int>(rightInner.height));
            for (int y = 0; y < static_cast<int>(rightInner.height); y += 4)
                DrawLineEx({rightInner.x, rightInner.y + static_cast<float>(y)},
                           {rightInner.x + rightInner.width, rightInner.y + static_cast<float>(y)},
                           1.f, Fade(BLACK, 0.035f));
            EndScissorMode();

            const char *cmd = savedName.empty() ? "UNKNOWN" : savedName.c_str();
            DrawBattleHudBar(w, theme, cmd, user->getNumShips(), enemy->getNumShips(),
                             !waitingEnemy, waitingEnemy);

            if (waitingEnemy) {
                // Dim overlay
                DrawRectangle(0, 0, w, h, Fade(BLACK, 0.68f));
                // Flash range 0.72–1.0 so text is always well above 50% visible
                float flash = 0.72f + 0.28f * sinf(animT * 4.5f);
                // Subtle pulsing ring
                float rad = 108.f + 22.f * sinf(animT * 5.f);
                DrawCircleLinesV({static_cast<float>(w) / 2.f, static_cast<float>(h) / 2.f},
                                 rad, Fade(theme.hit, 0.10f + 0.06f * flash));
                // Card — fully opaque background so nothing bleeds through
                const float cw = 490.f, ch = 96.f;
                Rectangle card{w / 2.f - cw / 2.f, h / 2.f - ch / 2.f, cw, ch};
                DrawRectangleRounded(card, 0.16f, 10, {8, 10, 18, 255});
                DrawRectangleRoundedLines(card, 0.16f, 10, theme.warn);
                // Left accent bar (solid, not fading)
                DrawRectangleRec({card.x, card.y + 10.f, 4.f, ch - 20.f}, theme.warn);
                // Headline — shadow for crispness then coloured text
                const char *wt  = "ENEMY SALVO — TRACKING";
                const int   wts = 24;
                int wtw = MeasureText(wt, wts);
                DrawText(wt, w / 2 - wtw / 2 + 1, static_cast<int>(card.y + 16) + 1, wts, BLACK);
                DrawText(wt, w / 2 - wtw / 2,     static_cast<int>(card.y + 16),     wts,
                         Fade(theme.warn, flash));
                // Sub-line — always full opacity
                const char *sub = "Brace for impact — computing enemy solution";
                DrawText(sub, w / 2 - MeasureText(sub, 15) / 2,
                         static_cast<int>(card.y + 58), 15, theme.textMuted);
            }

            DrawText("LMB — designate target", margin, h - 48, 15, theme.textMuted);
        } else if (phase == Phase::GameOver) {
            DrawRectangle(0, 0, w, h, Fade(BLACK, 0.72f));
            Rectangle card{w / 2.f - 260.f, h / 2.f - 120.f, 520.f, 260.f};
            DrawPanelBevel(card, theme, 0.14f);
            DrawRivets(card, theme.brass);
            int hs = 38;
            int hw = MeasureText(endHeadline.c_str(), hs);
            Color hc = (endHeadline == "Victory") ? theme.ok : theme.hit;
            DrawText(endHeadline.c_str(), w / 2 - hw / 2, static_cast<int>(card.y + 36), hs, hc);
            int sw = MeasureText(endSub.c_str(), 19);
            DrawText(endSub.c_str(), w / 2 - sw / 2, static_cast<int>(card.y + 88), 19, theme.text);
            float bw = 220.f;
            float bh = 50.f;
            float bx = w / 2.f - bw / 2.f;
            float by = card.y + 170.f;
            Rectangle again{bx, by, bw, bh};
            bool hot = CheckCollisionPointRec(mouse, again);
            DrawRectangleRounded(again, 0.22f, 10,
                                 hot ? Fade(theme.brass, 0.35f) : Fade(theme.brass, 0.15f));
            DrawRectangleRoundedLines(again, 0.22f, 10, theme.brass);
            const char *bt = "RE-DEPLOY FLEET";
            int tb = MeasureText(bt, 18);
            DrawText(bt, static_cast<int>(bx + bw / 2 - tb / 2), static_cast<int>(by + 15), 18,
                     theme.text);
            const char *h2 = "ENTER or click";
            DrawText(h2, w / 2 - MeasureText(h2, 15) / 2, static_cast<int>(card.y + 228), 15,
                     theme.textMuted);
        }

        if (toast.ttl > 0.f && !toast.msg.empty()) {
            // Fade out in the last 0.5s
            float fadeA = std::min(1.f, toast.ttl / 0.5f);
            const float barH = 50.f;
            Rectangle bar{30.f, static_cast<float>(h) - 70.f, static_cast<float>(w) - 60.f, barH};
            DrawRectangleRounded(bar, 0.25f, 10, Fade({10, 14, 20, 240}, fadeA));
            DrawRectangleRoundedLines(bar, 0.25f, 10, Fade(theme.panelBorder, 0.7f * fadeA));
            // Colored left accent bar
            Rectangle accent{bar.x, bar.y + 8.f, 4.f, barH - 16.f};
            DrawRectangleRec(accent, Fade(toast.color, fadeA));
            // Message text — large and clearly readable
            int msgFs = 20;
            int msgX  = static_cast<int>(bar.x + 18.f);
            int msgY  = static_cast<int>(bar.y + barH / 2.f - msgFs / 2.f);
            // Shadow pass for contrast against any background
            DrawText(toast.msg.c_str(), msgX + 1, msgY + 1, msgFs, Fade(BLACK, 0.6f * fadeA));
            DrawText(toast.msg.c_str(), msgX, msgY, msgFs, Fade(toast.color, fadeA));
            // Timer bar along the bottom edge
            float progress = toast.ttl / 2.2f;
            float timerW   = (bar.width - 4.f) * std::min(1.f, progress);
            DrawRectangle(static_cast<int>(bar.x + 2), static_cast<int>(bar.y + barH - 5.f),
                          static_cast<int>(timerW), 3, Fade(toast.color, 0.35f * fadeA));
        }

        DrawText("ESC — quit", w - 118, h - 28, 15, theme.textMuted);
        DrawVignette(w, h);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
