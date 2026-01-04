#pragma once

namespace class_template
{

// Argument list holder
// It is useful for specifying multiple types as a single template parameter
template <typename... Ts>
struct ConstructArgList {};

// Argument list holder
// It is useful for specifying multiple types as a single template parameter
template <typename... Ts>
struct SetupArgList {};

} // namespace class_template