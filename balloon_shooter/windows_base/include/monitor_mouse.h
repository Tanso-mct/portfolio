#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/factory.h"
#include "windows_base/include/interfaces/monitor_mouse.h"

namespace wb
{
    class WINDOWS_BASE_API MouseInputTypeTable : public IInputTypeTable
    {
    private:
        std::unordered_map<UINT, UINT> inputTypeMap_;
        bool isCreated_ = false;

    public:
        MouseInputTypeTable() = default;
        virtual ~MouseInputTypeTable() override = default;

        /***************************************************************************************************************
         * IInputTypeTable implementation
        /**************************************************************************************************************/

        void Create() override;
        bool CheckIsReady() const override;

        UINT GetInputType(UINT msg) const override;
    };

    struct MouseCodeTableHash 
    {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& pair) const 
        {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    struct MouseCodetableEqual 
    {
        template <typename T1, typename T2>
        bool operator()(const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const 
        {
            return lhs == rhs;
        }
    };

    class WINDOWS_BASE_API MouseCodeTable : public IMouseCodeTable
    {
    private:
        std::unordered_map<std::pair<UINT, UINT>, MouseCode, MouseCodeTableHash, MouseCodetableEqual> mouseCodeMap_;
        bool isCreated_ = false;

    public:
        MouseCodeTable() = default;
        virtual ~MouseCodeTable() override = default;

        /***************************************************************************************************************
         * IMouseCodeTable implementation
        /**************************************************************************************************************/

        void Create() override;
        bool CheckIsReady() const override;

        MouseCode GetMouseCode(UINT msg, WPARAM wParam) const override;
    };

    class WINDOWS_BASE_API MouseContext : public IMouseContext
    {
    private:
        std::vector<UINT> buttonStates_;
        MouseCode previousButton_ = MouseCode::Null;
        std::chrono::high_resolution_clock::time_point previousButtonTime_;

        int scrollDelta_ = 0;
        int positionX_ = 0;
        int positionY_ = 0;
        int deltaPositionX_ = 0;
        int deltaPositionY_ = 0;

    public:
        MouseContext() = default;
        virtual ~MouseContext() override = default;

        /***************************************************************************************************************
         * IMouseContext implementation
        /**************************************************************************************************************/

        std::vector<UINT> &ButtonStates() override { return buttonStates_; }
        MouseCode &PreviousButton() override { return previousButton_; }
        std::chrono::high_resolution_clock::time_point &PreviousButtonTime() override { return previousButtonTime_; }

        int &ScrollDelta() override { return scrollDelta_; }
        int &PositionX() override { return positionX_; }
        int &PositionY() override { return positionY_; }
        int &DeltaPositionX() override { return deltaPositionX_; }
        int &DeltaPositionY() override { return deltaPositionY_; }
    };

    const WINDOWS_BASE_API size_t &DefaultMouseMonitorFactoryID();

    class WINDOWS_BASE_API DefaultMouseMonitor : public IMouseMonitor
    {
    private:
        std::unique_ptr<IInputTypeTable> inputTypeTable_ = nullptr;
        std::unique_ptr<IMouseCodeTable> mouseCodeTable_ = nullptr;
        std::unique_ptr<IMouseContext> mouseContext_ = nullptr;

    public:
        DefaultMouseMonitor() = default;
        virtual ~DefaultMouseMonitor() override = default;

        DefaultMouseMonitor(const DefaultMouseMonitor &) = delete;
        DefaultMouseMonitor &operator=(const DefaultMouseMonitor &) = delete;

        /***************************************************************************************************************
         * IFacade implementation
        /**************************************************************************************************************/

        void SetContext(std::unique_ptr<IContext> context) override;
        bool CheckIsReady() const override;

        /***************************************************************************************************************
         * IMonitor implementation
        /**************************************************************************************************************/

        const size_t &GetFactoryID() const override;

        void EditState(UINT msg, WPARAM wParam, LPARAM lParam) override;

        void UpdateState() override;
        void UpdateRawInputState(RAWINPUT* raw) override;

        /***************************************************************************************************************
         * IMouseMonitor implementation
        /**************************************************************************************************************/

        void SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable) override;
        void SetMouseCodeTable(std::unique_ptr<IMouseCodeTable> mouseCodeTable) override;

        bool GetButton(const MouseCode &mouseCode) override;
        bool GetButtonDown(const MouseCode &mouseCode) override;
        bool GetButtonUp(const MouseCode &mouseCode) override;
        bool GetButtonDoubleTap(const MouseCode &mouseCode, const double &threshold) override;

        const int &GetScrollDelta() override { return mouseContext_->ScrollDelta(); }
        const int &GetPositionX() override { return mouseContext_->PositionX(); }
        const int &GetPositionY() override { return mouseContext_->PositionY(); }
        const int &GetDeltaPositionX() override { return mouseContext_->DeltaPositionX(); }
        const int &GetDeltaPositionY() override { return mouseContext_->DeltaPositionY(); }
    };

    class WINDOWS_BASE_API DefaultMouseMonitorFactory : public IMonitorFactory
    {
    public:
        DefaultMouseMonitorFactory() = default;
        virtual ~DefaultMouseMonitorFactory() override = default;

        std::unique_ptr<IMonitor> Create() const override;
    };

} // namespace wb