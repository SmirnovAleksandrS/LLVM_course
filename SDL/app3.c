#include "sim.h"

void app(void) {
  /* --- Константы компиляции (всё целое) --- */
  #define CELL 3                         /* коэрсинг: больше => меньше памяти на стеке */
  #define W   (SIM_X_SIZE / CELL)        /* ширина расчётной сетки */
  #define H   (SIM_Y_SIZE / CELL)        /* высота расчётной сетки */
  #define STEPS_PER_FRAME 4              /* «скорость времени» */
  #define SOURCES 4                      /* число шаров-источников */
  #define COOLING 1                      /* целочислительное охлаждение на шаг */
  /* alpha = 1/4 реализуем через сдвиг вправо на 2 бита: (lap >> 2) */

  /* --- Поля температуры (двойной буфер), всё на стеке --- */
  int U[2][H][W];
  int ping = 0, pong = 1;

  /* Инициализация нулями */
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
      U[ping][y][x] = 0;

  /* --- Источники (позиции, скорости, радиусы, температуры) --- */
  int sx[SOURCES], sy[SOURCES], svx[SOURCES], svy[SOURCES], sr[SOURCES], st[SOURCES];

  for (int i = 0; i < SOURCES; ++i) {
    sr[i] = 4 + (simRand() % 9);                 /* 4..12 */
    st[i] = 176 + (simRand() & 63);              /* 176..239 */

    int xmin = 1 + sr[i];
    int xmax = (W - 2) - sr[i];
    int ymin = 1 + sr[i];
    int ymax = (H - 2) - sr[i];
    if (xmax < xmin) { xmin = 1; xmax = W - 2; } /* на случай очень мелкой сетки */
    if (ymax < ymin) { ymin = 1; ymax = H - 2; }

    sx[i] = xmin + (simRand() % (xmax - xmin + 1));
    sy[i] = ymin + (simRand() % (ymax - ymin + 1));

    /* постоянная скорость: по каждой оси ±1 (без нулей) */
    svx[i] = (simRand() & 1) ? 1 : -1;
    svy[i] = (simRand() & 1) ? 1 : -1;
  }

  /* --- Главный цикл --- */
  while (1) {
    /* движение и отскоки (учитываем радиус) */
    for (int i = 0; i < SOURCES; ++i) {
      sx[i] += svx[i];
      sy[i] += svy[i];

      int xmin = 1 + sr[i];
      int xmax = (W - 2) - sr[i];
      int ymin = 1 + sr[i];
      int ymax = (H - 2) - sr[i];

      if (sx[i] <= xmin) { sx[i] = xmin; svx[i] = -svx[i]; }
      else if (sx[i] >= xmax) { sx[i] = xmax; svx[i] = -svx[i]; }

      if (sy[i] <= ymin) { sy[i] = ymin; svy[i] = -svy[i]; }
      else if (sy[i] >= ymax) { sy[i] = ymax; svy[i] = -svy[i]; }
    }

    /* несколько шагов явной схемы на кадр */
    for (int s = 0; s < STEPS_PER_FRAME; ++s) {
      int (*cur)[W]  = U[ping];
      int (*next)[W] = U[pong];

      /* подогрев: диски в текущих позициях */
      for (int i = 0; i < SOURCES; ++i) {
        int cx = sx[i], cy = sy[i], r = sr[i], r2 = r * r;
        int y0 = cy - r; if (y0 < 1) y0 = 1;
        int y1 = cy + r; if (y1 > H - 2) y1 = H - 2;
        int x0 = cx - r; if (x0 < 1) x0 = 1;
        int x1 = cx + r; if (x1 > W - 2) x1 = W - 2;

        for (int y = y0; y <= y1; ++y) {
          int dy = y - cy; int dy2 = dy * dy;
          for (int x = x0; x <= x1; ++x) {
            int dx = x - cx;
            if (dx * dx + dy2 <= r2) {
              int tt = st[i];
              if (cur[y][x] < tt) cur[y][x] = tt;
            }
          }
        }
      }

      /* теплопроводность: u_{n+1} = u + (lap >> 2) - COOLING; края = 0 */
      for (int y = 1; y < H - 1; ++y) {
        for (int x = 1; x < W - 1; ++x) {
          int u   = cur[y][x];
          int lap = cur[y][x-1] + cur[y][x+1] + cur[y-1][x] + cur[y+1][x] - 4 * u;
          int un  = u + (lap >> 2) - COOLING;  /* alpha = 1/4 */
          if (un < 0) un = 0;
          if (un > 255) un = 255;
          next[y][x] = un;
        }
      }
      for (int x = 0; x < W; ++x) { next[0][x] = 0; next[H-1][x] = 0; }
      for (int y = 0; y < H; ++y) { next[y][0] = 0; next[y][W-1] = 0; }

      int t = ping; ping = pong; pong = t;
    }

    /* отрисовка: температура 0..255 -> (r, g=r/2, b=255-r) */
    int (*draw)[W] = U[ping];
    for (int gy = 0; gy < H; ++gy) {
      for (int gx = 0; gx < W; ++gx) {
        int T = draw[gy][gx];
        if (T < 0) T = 0; if (T > 255) T = 255;
        int r = T;
        int g = T >> 1;
        int b = 255 - T;
        int color = (255 << 24) | (r << 16) | (g << 8) | b;

        int x0 = gx * CELL, y0 = gy * CELL;
        for (int py = 0; py < CELL; ++py) {
          int y = y0 + py; if (y >= SIM_Y_SIZE) break;
          for (int px = 0; px < CELL; ++px) {
            int x = x0 + px; if (x >= SIM_X_SIZE) break;
            simPutPixel(x, y, color);
          }
        }
      }
    }

    simFlush();
  }
}
