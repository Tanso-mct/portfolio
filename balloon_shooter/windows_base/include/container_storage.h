#pragma once

#include "windows_base/include/interfaces/container.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class ContainerStorage
    {
    private:
        std::unique_ptr<ISceneContainer> sceneContainer_ = nullptr;
        std::unique_ptr<IAssetContainer> assetContainer_ = nullptr;
        std::unique_ptr<IMonitorContainer> monitorContainer_ = nullptr;
        std::unique_ptr<IWindowContainer> windowContainer_ = nullptr;
        std::unique_ptr<ISharedContainer> sharedContainer_ = nullptr;

    public:
        ContainerStorage() = default;
        ~ContainerStorage() = default;

        /***************************************************************************************************************
         * Template methods for setting and getting containers
         * They throw an error for normal container types because they are used as specializations.
        /**************************************************************************************************************/

        template <typename T>
        void SetContainer(std::unique_ptr<T> container)
        {
            std::string err = CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Container type is not supported."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        template <typename T>
        T &GetContainer()
        {
            std::string err = CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Container type is not supported."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        /***************************************************************************************************************
         * ISceneContainer specialization
        /**************************************************************************************************************/

        template <>
        void SetContainer(std::unique_ptr<ISceneContainer> container)
        {
            sceneContainer_ = std::move(container);
        }

        template <>
        ISceneContainer &GetContainer()
        {
            if (!sceneContainer_)
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Scene container is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *sceneContainer_;
        }

        /***************************************************************************************************************
         * IAssetContainer specialization
        /**************************************************************************************************************/

        template <>
        void SetContainer(std::unique_ptr<IAssetContainer> container)
        {
            assetContainer_ = std::move(container);
        }

        template <>
        IAssetContainer &GetContainer()
        {
            if (!assetContainer_)
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Asset container is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *assetContainer_;
        }

        /***************************************************************************************************************
         * IMonitorContainer specialization
        /**************************************************************************************************************/

        template <>
        void SetContainer(std::unique_ptr<IMonitorContainer> container)
        {
            monitorContainer_ = std::move(container);
        }

        template <>
        IMonitorContainer &GetContainer()
        {
            if (!monitorContainer_)
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Monitor container is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *monitorContainer_;
        }

        /***************************************************************************************************************
         * IWindowContainer specialization
        /**************************************************************************************************************/

        template <>
        void SetContainer(std::unique_ptr<IWindowContainer> container)
        {
            windowContainer_ = std::move(container);
        }

        template <>
        IWindowContainer &GetContainer()
        {
            if (!windowContainer_)
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Window container is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *windowContainer_;
        }

        /***************************************************************************************************************
         * ISharedContainer specialization
        /**************************************************************************************************************/

        template <>
        void SetContainer(std::unique_ptr<ISharedContainer> container)
        {
            sharedContainer_ = std::move(container);
        }

        template <>
        ISharedContainer &GetContainer()
        {
            if (!sharedContainer_)
            {
                std::string err = CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Shared container is not set."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return *sharedContainer_;
        }
    

    };

} // namespace wb