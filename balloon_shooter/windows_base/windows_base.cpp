#include "windows_base/src/pch.h"
#include "windows_base/windows_base.h"

bool wb::WindowsBaseLibrary::isInitialized_ = false;

std::unique_ptr
<
    wb::IEventInvoker<HWND, wb::IWindowEvent, HWND, wb::ContainerStorage&, UINT, WPARAM, LPARAM>
> wb::WindowsBaseLibrary::windowEventInvoker_ = nullptr;

std::unique_ptr<wb::ContainerStorage> wb::WindowsBaseLibrary::containerStorage_ = nullptr;


inline void wb::WindowsBaseLibrary::Initialize(LibraryConfig &config)
{
    if (isInitialized_)
    {
        std::string err = CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"WindowsBaseLibrary is already initialized."}
        );

        ConsoleLogErr(err);
        ErrorNotify("WINDOWS_BASE", err);
        ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Create standard console when not in debug mode
    /******************************************************************************************************************/

#ifndef NDEBUG
    wb::CreateStandardConsole();
#endif

    /*******************************************************************************************************************
     * Create containers
    /******************************************************************************************************************/

    containerStorage_ = std::make_unique<wb::ContainerStorage>();

    {
        // Create window container
        std::unique_ptr<wb::IWindowContainer> windowCont = std::make_unique<wb::WindowContainer>();
        windowCont->Create(wb::gWindowRegistry.GetMaxID() + 1);
        containerStorage_->SetContainer<wb::IWindowContainer>(std::move(windowCont));

        // Create monitor container
        std::unique_ptr<wb::IMonitorContainer> monitorCont = std::make_unique<wb::MonitorContainer>();
        monitorCont->Create(wb::gMonitorRegistry.GetMaxID() + 1);
        containerStorage_->SetContainer<wb::IMonitorContainer>(std::move(monitorCont));

        // Create scene container
        std::unique_ptr<wb::ISceneContainer> sceneCont = std::make_unique<wb::SceneContainer>();
        sceneCont->Create(wb::gSceneFacadeRegistry.GetMaxID() + 1);
        containerStorage_->SetContainer<wb::ISceneContainer>(std::move(sceneCont));

        // Create shared container
        std::unique_ptr<wb::ISharedContainer> sharedCont = std::make_unique<wb::SharedContainer>();
        sharedCont->Create(wb::gSharedFacadeRegistry.GetMaxID() + 1);
        containerStorage_->SetContainer<wb::ISharedContainer>(std::move(sharedCont));

        // Create asset container
        std::unique_ptr<wb::IAssetContainer> assetCont = std::make_unique<wb::AssetContainer>();
        assetCont->Create(wb::gAssetRegistry.GetMaxID() + 1);
        containerStorage_->SetContainer<wb::IAssetContainer>(std::move(assetCont));
    }

    /*******************************************************************************************************************
     * Create window facades
    /******************************************************************************************************************/

    {
        wb::IWindowContainer &windowContainer = containerStorage_->GetContainer<wb::IWindowContainer>();
        for (const size_t &id : config.createWindowIDs_)
        {
            if (id > wb::gWindowRegistry.GetMaxID())
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Window ID is invalid: ", std::to_string(id)}
                );

                ConsoleLogErr(err);
                ErrorNotify("WINDOWS_BASE", err);
                ThrowRuntimeError(err);
            }

            // Get the factory for the window facade
            wb::IWindowFacadeFactory &factory = wb::gWindowRegistry.GetFacadeFactory(id);

            // Create the window facade using the factory
            std::unique_ptr<wb::IWindowFacade> facade = factory.Create();

            // Set the facade in the window container
            windowContainer.Set(id, std::move(facade));
        }
    }

    /*******************************************************************************************************************
     * Create monitors
    /******************************************************************************************************************/

    {
        wb::IWindowContainer &windowContainer = containerStorage_->GetContainer<wb::IWindowContainer>();
        wb::IMonitorContainer &monitorContainer = containerStorage_->GetContainer<wb::IMonitorContainer>();

        for (const size_t &id : config.createWindowIDs_)
        {
            // Get the window facade from the container
            wb::IWindowFacade &windowFacade = windowContainer.Get(id);

            for (const size_t &monitorID : windowFacade.GetMonitorIDs())
            {
                if (monitorID > wb::gMonitorRegistry.GetMaxID())
                {
                    std::string err = CreateErrorMessage
                    (
                        __FILE__, __LINE__, __FUNCTION__,
                        {"Monitor ID is invalid: ", std::to_string(monitorID)}
                    );

                    ConsoleLogErr(err);
                    ErrorNotify("WINDOWS_BASE", err);
                    ThrowRuntimeError(err);
                }

                // Get the factory id for the monitor
                const size_t &factoryID = wb::gMonitorRegistry.GetFactoryID(monitorID);

                // Get the factory for the monitor
                wb::IMonitorFactory &factory = wb::gMonitorFactoryRegistry.GetFactory(factoryID);

                // Create the monitor using the factory
                std::unique_ptr<wb::IMonitor> monitor = factory.Create();

                // Set the monitor in the monitor container
                monitorContainer.Set(monitorID, std::move(monitor));
            }
        }
    }

    /*******************************************************************************************************************
     * Create scene facades
    /******************************************************************************************************************/

    {
        wb::ISceneContainer &sceneContainer = containerStorage_->GetContainer<wb::ISceneContainer>();
        for (const size_t &id : config.createSceneIDs_)
        {
            if (id > wb::gSceneFacadeRegistry.GetMaxID())
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Scene facade ID is invalid: ", std::to_string(id)}
                );

                ConsoleLogErr(err);
                ErrorNotify("WINDOWS_BASE", err);
                ThrowRuntimeError(err);
            }

            // Get the factory for the scene facade
            wb::ISceneFacadeFactory &factory = wb::gSceneFacadeRegistry.GetFactory(id);

            // Create the scene facade using the factory
            std::unique_ptr<wb::ISceneFacade> facade = factory.Create();

            // Set the facade in the scene container
            sceneContainer.Set(id, std::move(facade));
        }
    }

    /*******************************************************************************************************************
     * Create all shared facades
    /******************************************************************************************************************/

    {
        wb::ISharedContainer &sharedContainer = containerStorage_->GetContainer<wb::ISharedContainer>();
        std::vector<size_t> keys = wb::gSharedFacadeRegistry.GetKeys();

        for (const size_t &id : keys)
        {
            // Get the factory for the shared facade
            wb::ISharedFacadeFactory &factory = wb::gSharedFacadeRegistry.GetFactory(id);

            // Create the shared facade using the factory
            std::unique_ptr<wb::ISharedFacade> facade = factory.Create();

            // Set the facade in the shared container
            sharedContainer.Set(id, std::move(facade));
        }
    }

    /*******************************************************************************************************************
     * Create window and show it
    /******************************************************************************************************************/

    {
        wb::IWindowContainer &windowContainer = containerStorage_->GetContainer<wb::IWindowContainer>();
        for (const size_t &id : config.createWindowIDs_)
        {
            // Get the window facade from the container
            wb::IWindowFacade &windowFacade = windowContainer.Get(id);

            // Create window class
            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.lpfnWndProc = config.windowProc_;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = windowFacade.GetName().data();

            // Create the window
            windowFacade.Create(wc);

            if (windowFacade.NeedsShowWhenCreated())
            {
                windowFacade.Show();
            }
            else
            {
                windowFacade.Hide();
            }
        }
    }

    /*******************************************************************************************************************
     * Create event invoker
    /******************************************************************************************************************/

    windowEventInvoker_ 
        = std::make_unique<wb::EventInvoker<HWND, wb::IWindowEvent, HWND, wb::ContainerStorage&, UINT, WPARAM, LPARAM>>();

    {
        std::unique_ptr<wb::IEventInstanceTable<HWND, wb::IWindowEvent>> instanceTable 
            = std::make_unique<wb::EventInstTable<HWND, wb::IWindowEvent>>();

        std::unique_ptr<wb::IEventFuncTable<HWND, wb::IWindowEvent, wb::ContainerStorage&, UINT, WPARAM, LPARAM>> funcTable 
            = std::make_unique<wb::EventFuncTable<HWND, wb::IWindowEvent, wb::ContainerStorage&, UINT, WPARAM, LPARAM>>();

        // Get the window facade container
        wb::IWindowContainer &windowContainer = containerStorage_->GetContainer<wb::IWindowContainer>();

        // Create window events and set them in the instance table
        for (const size_t &id : config.createWindowIDs_)
        {
            // Get the event factory for the window
            wb::IWindowEventFactory &eventFactory = wb::gWindowRegistry.GetEventFactory(id);

            // Create a window event using the factory
            std::unique_ptr<wb::IWindowEvent> windowEvent = eventFactory.Create();

            // Get the window facade from the container
            wb::IWindowFacade &windowFacade = windowContainer.Get(id);

            // Add the window event to the instance table
            instanceTable->Add(windowFacade.GetHandle(), std::move(windowEvent));

            // Add the event function to the function table
            funcTable->Add(windowFacade.GetHandle(), &wb::IWindowEvent::OnEvent);
        }

        windowEventInvoker_->SetInstanceTable(std::move(instanceTable));
        windowEventInvoker_->SetFuncTable(std::move(funcTable));
    }
    
    isInitialized_ = true;
}

inline void wb::WindowsBaseLibrary::Run()
{
    if (!isInitialized_)
    {
        std::string err = CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"WindowsBaseLibrary is not initialized."}
        );

        ConsoleLogErr(err);
        ErrorNotify("WINDOWS_BASE", err);
        ThrowRuntimeError(err);
    }

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

inline void wb::WindowsBaseLibrary::HandleWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (isInitialized_)
    {
        windowEventInvoker_->Invoke
        (
            hWnd, 
            hWnd, 
            *containerStorage_, 
            msg, 
            wParam, 
            lParam
        );
    }
}

void wb::WindowsBaseLibrary::Shutdown()
{
    
}