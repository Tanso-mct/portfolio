#pragma once

#include "windows_base/include/interfaces/container.h"

#include "windows_base/include/optional_value.h"
#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

#include <vector>
#include <memory>
#include <shared_mutex>
#include <tuple>

namespace wb
{
    template <typename T>
    class DynamicContainer : public IDynamicContainer<T>
    {
    private:
        std::vector<std::pair<std::unique_ptr<T>, std::shared_ptr<size_t>>> datas_;
        mutable std::shared_mutex mutex_;

    public:
        /***************************************************************************************************************
         * Constructor / Destructor
         * It has unique_ptr so it cannot be copied or assigned.
        /**************************************************************************************************************/

        DynamicContainer() = default;
        ~DynamicContainer() = default;

        DynamicContainer(const DynamicContainer &) = delete;
        DynamicContainer &operator=(const DynamicContainer &) = delete;

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
            return index < datas_.size() && datas_[index].first != nullptr;
        }

        /***************************************************************************************************************
         * IDynamicContainer implementation
        /**************************************************************************************************************/

        T &Get(const IOptionalValue &index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index() >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            if (datas_[index()].first == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid data's index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            return *datas_[index()].first;
        }

        T *PtrGet(const IOptionalValue &index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index() >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            if (datas_[index()].first == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid data's index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            return datas_[index()].first.get();
        }

        LockedRef<T> ThreadSafeGet(const IOptionalValue &index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index() >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            if (datas_[index()].first == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid data's index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            return LockedRef<T>{std::move(lock), *datas_[index()].first};
        }

        std::unique_ptr<IOptionalValue> Add(std::unique_ptr<T> data) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            size_t index = datas_.size(); // new element's index
            datas_.emplace_back(std::make_pair(std::move(data), std::make_shared<size_t>(index)));

            std::unique_ptr<IOptionalValue> optionalValue = std::make_unique<OptionalValue>(datas_.back().second);
            return optionalValue;
        }

        std::unique_ptr<T> Erase(const IOptionalValue &index) override
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);

            if (index() >= datas_.size())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Invalid index : " + std::to_string(index())}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WB_CONTAINER", err);
                wb::ThrowRuntimeError(err);
            }

            // Get the data to be erased
            std::unique_ptr<T> erasedData = std::move(datas_[index()].first);

            // Keep this index
            size_t keepIndex = index();

            // Reset erase data's index
            datas_[keepIndex].second.reset();

            // Move the last element to the erased position
            if (keepIndex < datas_.size() - 1)
            {
                datas_[keepIndex].first = std::move(datas_.back().first);
                datas_[keepIndex].second = std::move(datas_.back().second);

                // Update the index
                *datas_[keepIndex].second = keepIndex; // Set new index
            }

            // Remove the last element
            datas_.pop_back();

            return erasedData; // Return the erased data
        }
    };
} // namespace wb