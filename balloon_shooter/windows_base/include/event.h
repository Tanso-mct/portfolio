#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/event.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

#include <unordered_map>
#include <memory>

namespace wb
{
    template <typename KEY, typename EVENT>
    class EventInstTable : public IEventInstanceTable<KEY, EVENT>
    {
    private:
        std::unordered_map<KEY, std::unique_ptr<EVENT>> instanceMap_;

    public:
        /***************************************************************************************************************
         * Constructor / Destructor
         * It has unique_ptr so it cannot be copied or assigned.
        /**************************************************************************************************************/

        EventInstTable() = default;
        ~EventInstTable() override = default;

        EventInstTable(const EventInstTable&) = delete;
        EventInstTable& operator=(const EventInstTable&) = delete;

        /***************************************************************************************************************
         * IEventTable implementation
        /**************************************************************************************************************/

        void Clear() override
        {
            instanceMap_.clear();
        }

        bool Has(KEY key) const override
        {
            return instanceMap_.find(key) != instanceMap_.end();
        }

        size_t GetSize() const override
        {
            return instanceMap_.size();
        }

        /***************************************************************************************************************
         * IEventInstanceTable implementation
        /**************************************************************************************************************/

        void Add(KEY key, std::unique_ptr<EVENT> event) override
        {
            if (Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key already exists in EventInstTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            instanceMap_[key] = std::move(event);
        }

        std::unique_ptr<EVENT> Remove(KEY key) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventInstTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            std::unique_ptr<EVENT> removedEvent = std::move(instanceMap_[key]);
            instanceMap_.erase(key);

            return removedEvent;
        }

        std::unique_ptr<EVENT>& Get(KEY key) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventInstTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return instanceMap_[key];
        }

        void Set(KEY key, std::unique_ptr<EVENT> event) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventInstTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            instanceMap_[key] = std::move(event);
        }

    };

    template <typename KEY, typename EVENT, typename... ARGS>
    class EventFuncTable : public IEventFuncTable<KEY, EVENT, ARGS...>
    {
    private:
        std::unordered_map<KEY, void (EVENT::*)(ARGS...)> funcMap_;

    public:
        EventFuncTable() = default;
        ~EventFuncTable() override = default;

        /***************************************************************************************************************
         * IEventTable implementation
        /**************************************************************************************************************/

        void Clear() override
        {
            funcMap_.clear();
        }

        bool Has(KEY key) const override
        {
            return funcMap_.find(key) != funcMap_.end();
        }

        size_t GetSize() const override
        {
            return funcMap_.size();
        }

        /***************************************************************************************************************
         * IEventFuncTable implementation
        /**************************************************************************************************************/

        void Add(KEY key, void (EVENT::*func)(ARGS...)) override
        {
            if (Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key already exists in EventFuncTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            funcMap_[key] = func;
        }

        void (EVENT::*Remove(KEY key))(ARGS...) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventFuncTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            void (EVENT::*removedFunc)(ARGS...) = funcMap_[key];
            funcMap_.erase(key);

            return removedFunc;
        }

        void (EVENT::*Get(KEY key))(ARGS...) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventFuncTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return funcMap_[key];
        }

        void Set(KEY key, void (EVENT::*func)(ARGS...)) override
        {
            if (!Has(key))
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Key does not exist in EventFuncTable."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            funcMap_[key] = func;
        }
    };

    template <typename EVENT_KEY, typename EVENT, typename FUNC_KEY, typename... ARGS>
    class EventInvoker : public IEventInvoker<EVENT_KEY, EVENT, FUNC_KEY, ARGS...>
    {
    private:
        std::unique_ptr<IEventInstanceTable<EVENT_KEY, EVENT>> instanceTable_ = nullptr;
        std::unique_ptr<IEventFuncTable<FUNC_KEY, EVENT, ARGS...>> funcTable_ = nullptr;

    public:
        /***************************************************************************************************************
         * Constructor / Destructor
         * It has unique_ptr so it cannot be copied or assigned.
        /**************************************************************************************************************/

        EventInvoker() = default;
        ~EventInvoker() override = default;

        EventInvoker(const EventInvoker&) = delete;
        EventInvoker& operator=(const EventInvoker&) = delete;

        /***************************************************************************************************************
         * IEventInvoker implementation
        /**************************************************************************************************************/

        void SetInstanceTable(std::unique_ptr<IEventInstanceTable<EVENT_KEY, EVENT>> instanceTable) override
        {
            instanceTable_ = std::move(instanceTable);
        }

        IEventInstanceTable<EVENT_KEY, EVENT> &GetInstanceTable() override
        {
            if (instanceTable_ == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Instance table is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *instanceTable_;
        }

        void SetFuncTable(std::unique_ptr<IEventFuncTable<FUNC_KEY, EVENT, ARGS...>> funcTable) override
        {
            funcTable_ = std::move(funcTable);
        }

        IEventFuncTable<FUNC_KEY, EVENT, ARGS...> &GetFuncTable() override
        {
            if (funcTable_ == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Function table is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *funcTable_;
        }

        void Invoke(EVENT_KEY eventKey, FUNC_KEY funcKey, ARGS... args) override
        {
            if (instanceTable_ == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Instance table is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            if (funcTable_ == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Function table is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            if (!instanceTable_->Has(eventKey)) return; // No instance for this event key
            if (!funcTable_->Has(funcKey)) return; // No function for this func key

            std::unique_ptr<EVENT> &eventInstance = instanceTable_->Get(eventKey);
            void (EVENT::*eventFunc)(ARGS...) = funcTable_->Get(funcKey);

            // Invoke the function on the event instance
            (eventInstance.get()->*eventFunc)(args...);
        }
        
    };



} // namespace wb