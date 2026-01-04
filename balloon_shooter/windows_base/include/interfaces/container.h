#pragma once

#include <memory>
#include <shared_mutex>

namespace wb
{
    /*******************************************************************************************************************
     * LockedRef
     * A simple wrapper for a reference to an object that is locked for exclusive access.
     * This is useful for thread-safe access to shared data.
    /******************************************************************************************************************/

    template<typename T>
    class LockedRef 
    {
    public:
        LockedRef(std::unique_lock<std::shared_mutex>&& lock, T& ref) : 
            lock_(std::move(lock)), ref_(ref) 
        {
        }

        T& operator()() 
        {
            return ref_;
        }

    private:
        std::unique_lock<std::shared_mutex> lock_;
        T& ref_;
    };

    /*******************************************************************************************************************
     * IContainer
     * An interface for a container that can hold objects of type T.
    /******************************************************************************************************************/

    class IContainer
    {
    public:
        virtual ~IContainer() = default;
        
        virtual void Clear() = 0;
        virtual size_t GetSize() const = 0;
        virtual bool Has(const size_t &index) const = 0;
    };

    /*******************************************************************************************************************
     * IStaticContainer
     * An interface for a static container that can hold objects of type T.
     * It provides methods to create the container, get and set elements by index,
     * The size of the container is fixed at creation time.
    /******************************************************************************************************************/

    template <typename T>
    class IStaticContainer : public IContainer
    {
    public:
        virtual ~IStaticContainer() = default;

        virtual void Create(size_t size) = 0;

        virtual T &Get(size_t index) = 0;
        virtual T *PtrGet(size_t index) = 0;
        virtual LockedRef<T> ThreadSafeGet(size_t index) = 0;
        virtual void Set(size_t index, std::unique_ptr<T> data) = 0;
        virtual std::unique_ptr<T> Release(size_t index) = 0;
    };

    /*******************************************************************************************************************
     * IDynamicContainer
     * An interface for a dynamic container that can hold objects of type T.
     * It provides methods to get elements by index, add new elements,
     * erase elements by index, and thread-safe access to elements.
     * The size of the container can change dynamically.
    /******************************************************************************************************************/

    class IOptionalValue;

    template <typename T>
    class IDynamicContainer : public IContainer
    {
    public:
        virtual ~IDynamicContainer() = default;

        virtual T &Get(const IOptionalValue &index) = 0;
        virtual T *PtrGet(const IOptionalValue &index) = 0;
        virtual LockedRef<T> ThreadSafeGet(const IOptionalValue &index) = 0;

        virtual std::unique_ptr<IOptionalValue> Add(std::unique_ptr<T> data) = 0;
        virtual std::unique_ptr<T> Erase(const IOptionalValue &index) = 0;
    };

    /*******************************************************************************************************************
     * Container interfaces
    /******************************************************************************************************************/

    class IEntity;
    using IEntityContainer = IDynamicContainer<IEntity>;

    class IComponent;
    using IComponentContainer = IDynamicContainer<IComponent>;

    class ISystem;
    using ISystemContainer = IStaticContainer<ISystem>;

    class IAsset;
    using IAssetContainer = IStaticContainer<IAsset>;

    class ISceneFacade;
    using ISceneContainer = IStaticContainer<ISceneFacade>;

    class IMonitor;
    using IMonitorContainer = IStaticContainer<IMonitor>;

    class IWindowFacade;
    using IWindowContainer = IStaticContainer<IWindowFacade>;

    class ISharedFacade;
    using ISharedContainer = IStaticContainer<ISharedFacade>;

} // namespace wb