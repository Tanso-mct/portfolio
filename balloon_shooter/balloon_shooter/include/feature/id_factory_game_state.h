#pragma once

namespace balloon_shooter
{
    class GameStateIDFactory
    {
    public:
        GameStateIDFactory() = default;
        ~GameStateIDFactory() = default;

        static size_t Create();
    };

} // namespace balloon_shooter