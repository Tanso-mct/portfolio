#pragma once

namespace mono_input_monitor
{
    enum class InputType : unsigned int
    {
        None = 0, Down, Press, Up,
        Size
    };

} // namespace mono_input_monitor