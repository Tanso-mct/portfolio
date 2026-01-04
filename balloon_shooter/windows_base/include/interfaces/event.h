#pragma once

#include <memory>

namespace wb
{
    template <typename KEY>
    class IEventTable
    {
    public:
        virtual ~IEventTable() = default;

        virtual void Clear() = 0;
        virtual bool Has(KEY key) const = 0;
        virtual size_t GetSize() const = 0;
    };

    template <typename KEY, typename EVENT>
    class IEventInstanceTable : public IEventTable<KEY>
    {
    public:
        virtual ~IEventInstanceTable() = default;

        virtual void Add(KEY key, std::unique_ptr<EVENT> event) = 0;
        virtual std::unique_ptr<EVENT> Remove(KEY key) = 0;
        virtual std::unique_ptr<EVENT>& Get(KEY key) = 0;
        virtual void Set(KEY key, std::unique_ptr<EVENT> event) = 0;
    };

    template <typename KEY, typename EVENT, typename... ARGS>
    class IEventFuncTable : public IEventTable<KEY>
    {
    public:
        virtual ~IEventFuncTable() = default;

        virtual void Add(KEY key, void (EVENT::*func)(ARGS...)) = 0;
        virtual void (EVENT::*Remove(KEY key))(ARGS...) = 0;
        virtual void (EVENT::*Get(KEY key))(ARGS...) = 0;
        virtual void Set(KEY key, void (EVENT::*func)(ARGS...)) = 0;
    };

    template <typename EVENT_KEY, typename EVENT, typename FUNC_KEY, typename... ARGS>
    class IEventInvoker
    {
    public:
        virtual ~IEventInvoker() = default;
        
        virtual void SetInstanceTable(std::unique_ptr<IEventInstanceTable<EVENT_KEY, EVENT>> instanceTable) = 0;
        virtual IEventInstanceTable<EVENT_KEY, EVENT> &GetInstanceTable() = 0;

        virtual void SetFuncTable(std::unique_ptr<IEventFuncTable<FUNC_KEY, EVENT, ARGS...>> funcTable) = 0;
        virtual IEventFuncTable<FUNC_KEY, EVENT, ARGS...> &GetFuncTable() = 0;

        virtual void Invoke(EVENT_KEY eventKey, FUNC_KEY funcKey, ARGS... args) = 0;
    };

} // namespace wb