#include "windows_base/src/pch.h"
#include "windows_base/include/optional_value.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

wb::OptionalValue::OptionalValue(std::shared_ptr<size_t> value) :
    value_(value)
{
}

const size_t &wb::OptionalValue::operator()() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (value_.expired())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"This OptionalValue already released"}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *value_.lock();
}

bool wb::OptionalValue::IsValid() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return !value_.expired();
}

std::unique_ptr<wb::IOptionalValue> wb::OptionalValue::Clone() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (value_.expired())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"This OptionalValue already released"}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return std::make_unique<wb::OptionalValue>(value_.lock());
}