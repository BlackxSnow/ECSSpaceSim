#pragma once

namespace CCX {
    enum class ConsoleColour {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Default
    };

    void SetConsoleColour(ConsoleColour foreground, ConsoleColour background);
}
