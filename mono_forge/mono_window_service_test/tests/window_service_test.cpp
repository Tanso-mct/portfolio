#include "mono_window_service_test/pch.h"

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"

#include "window_provider/include/win32_window.h"
#include "mono_window_service/include/window_service.h"
#include "mono_window_service/include/window_service_command_list.h"
#include "mono_window_service/include/window_service_view.h"

namespace mono_window_service_test
{

bool ImportWindowService(mono_service::ServiceRegistry& service_registry)
{
    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(service_registry);

    // Import window service in to registry
    std::unique_ptr<mono_window_service::WindowService> window_service 
        = std::make_unique<mono_window_service::WindowService>(0);

    // Create setup parameters
    mono_window_service::WindowService::SetupParam setup_param;

    // Import window service
    return service_importer->Import(
        std::move(window_service), mono_window_service::WindowServiceHandle::ID(), setup_param);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // namespace mono_window_service_test

TEST(WindowService, Import)
{
    /*******************************************************************************************************************
     * Import window service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import window service in to registry
    bool result = mono_window_service_test::ImportWindowService(*service_registry);
    ASSERT_TRUE(result);
}

TEST(WindowService, Execute)
{
    /*******************************************************************************************************************
     * Import window service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import window service in to registry
    bool result = mono_window_service_test::ImportWindowService(*service_registry);
    ASSERT_TRUE(result);

    // Get window service proxy from window service
    std::unique_ptr<mono_service::ServiceProxy> window_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get window service
        mono_service::Service* service 
            = &registry.Get(mono_window_service::WindowServiceHandle::ID());
        mono_window_service::WindowService& window_service
            = dynamic_cast<mono_window_service::WindowService&>(*service);

        // Create service proxy
        window_service_proxy = window_service.CreateServiceProxy();
    });

    /*******************************************************************************************************************
     * Create window
    /******************************************************************************************************************/

    const uint32_t WINDOW_WIDTH = 800;
    const uint32_t WINDOW_HEIGHT = 600;
    window_provider::WindowHandle window_handle = window_provider::WindowHandle();
    mono_service::ServiceProgress progress = 0;
    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> service_command_list
            = window_service_proxy->CreateCommandList();
        mono_window_service::WindowServiceCommandList& window_command_list
            = dynamic_cast<mono_window_service::WindowServiceCommandList&>(*service_command_list);

        // Prepare create description
        std::unique_ptr<window_provider::Win32Window::CreateDesc> create_desc
            = std::make_unique<window_provider::Win32Window::CreateDesc>();
        create_desc->wc = 
        { 
            sizeof(create_desc->wc), CS_CLASSDC, mono_window_service_test::WndProc, 0L, 0L,
            ::GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr,
            L"Win32WindowTestClass", nullptr 
        };
        create_desc->title = L"Win32 Window Test";
        create_desc->width = WINDOW_WIDTH;
        create_desc->height = WINDOW_HEIGHT;

        // Create window 
        window_command_list.CreateWindowByService(window_handle, std::move(create_desc));

        // Submit command list to window service
        progress = window_service_proxy->SubmitCommandList(std::move(service_command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_window_service::WindowServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Show window
    /******************************************************************************************************************/

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> service_command_list
            = window_service_proxy->CreateCommandList();
        mono_window_service::WindowServiceCommandList& window_command_list
            = dynamic_cast<mono_window_service::WindowServiceCommandList&>(*service_command_list);

        // Show window 
        window_command_list.ShowWindowByService(window_handle);

        // Submit command list to window service
        progress = window_service_proxy->SubmitCommandList(std::move(service_command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_window_service::WindowServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Verify window
    /******************************************************************************************************************/

    {
        // Create view
        std::unique_ptr<mono_service::ServiceView> service_view
            = window_service_proxy->CreateView();
        mono_window_service::WindowServiceView& window_view
            = dynamic_cast<mono_window_service::WindowServiceView&>(*service_view);

        // Verify window dimensions
        uint32_t width = window_view.GetWindowWidth(window_handle);
        uint32_t height = window_view.GetWindowHeight(window_handle);
        ASSERT_EQ(width, WINDOW_WIDTH);
        ASSERT_EQ(height, WINDOW_HEIGHT);
    }

    /*******************************************************************************************************************
     * Destroy window
    /******************************************************************************************************************/

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> service_command_list
            = window_service_proxy->CreateCommandList();
        mono_window_service::WindowServiceCommandList& window_command_list
            = dynamic_cast<mono_window_service::WindowServiceCommandList&>(*service_command_list);

        // Destroy window 
        window_command_list.DestroyWindowByService(window_handle);

        // Submit command list to window service
        progress = window_service_proxy->SubmitCommandList(std::move(service_command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_window_service::WindowServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    // Cleanup service registry
    service_registry.reset();
}