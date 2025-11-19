#pragma once

namespace window
{
    static const int width = 1200;
    static const int height = 1200;
    static const char *title = "Classroom";
}

namespace room
{
    // Room dimensions optimized for proper bench spacing:
    // - 5 benches × 2m depth = 10m
    // - 4 gaps × 3.5m spacing = 14m (LARGE spacing to prevent overlap)
    // - Back margin: 2.5m (for backrest clearance)
    // - Front space: 4m (increased for teacher movement)
    // - Total width: 2.5 + 14 + 1 + 4 = 21.5m
    static const float length = 26.0f; // 32m length for proper side-to-side layout
    static const float width = 21.5f;  // Increased from 19m to 21.5m for 4m front space
    static const float height = 7.0f;
}

namespace furniture
{
    static const int rows = 5; // 5 rows of benches
    static const int cols = 5; // 5 columns of benches (5x5 = 25 benches total)
    static const float deskScale = 0.5f;
    static const int fans = 6;    // 6 fans total (2x3 grid)
    static const int fanRows = 2; // 2 rows of fans
    static const int fanCols = 3; // 3 columns of fans
    static const float fanScale = 1.0f;
}
