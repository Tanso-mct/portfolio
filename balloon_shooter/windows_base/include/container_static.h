#pragma once

#include "windows_base/include/interfaces/container.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

#include <vector>
#include <memory>
#include <shared_mutex>

namespace wb
{
    template <typename T>
    class StaticContainer : public IStaticContainer<T>
    {
    private:
        std::vector<std::unique_ptr<T>> datas_;
        mutable std::shared_mutex mutex_;

    public:
        /***************************************************************************************************************
         * Constructor / Destructor
         * It has unique_ptr so it cannot be copied or assigned.
        /**************************************************************************************************************/

        StaticContainer() = default;
        ~StaticContainer() override = default;

        StaticContainer(const StaticContainer&) = delete;
        StaticContainer& operator=(const StaticContainer&) = delete;

        /***************************************************************************************************************
         * IContainer implementation
        /**************************************************************************************************************/

        void Clear() override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            datas_.clear();
        }

        size_t GetSize() const override
        {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return datas_.size();
        }

        bool Has(const size_t &index) const override
        {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return index < datas_.size() && datas_[index] != nullptr;
        }

        /***************************************************************************************************************
         * IStaticContainer implementation
        /**************************************************************************************************************/

        void Create(size_t size) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (!datas_.empty())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Datas already exist. Clear the container before creating a new one."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            datas_.resize(size);
        }

        T &Get(size_t index) override
        {
            if (index >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            T* dataPtr = datas_[index].get();
            if (dataPtr == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid data at index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *datas_[index];
        }

        T *PtrGet(size_t index) override
        {
            if (index >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return datas_[index].get();
        }

        LockedRef<T> ThreadSafeGet(size_t index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            if (datas_[index] == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid data at index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return LockedRef<T>{std::move(lock), *datas_[index]};
        }

        void Set(size_t index, std::unique_ptr<T> data) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            datas_[index] = std::move(data);
        }

        std::unique_ptr<T> Release(size_t index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index)}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            std::unique_ptr<T> releasedData = std::move(datas_[index]);
            datas_[index] = nullptr; // Set the pointer to nullptr after releasing

            return releasedData; // Return the released data
        }
    };

} // namespace wb