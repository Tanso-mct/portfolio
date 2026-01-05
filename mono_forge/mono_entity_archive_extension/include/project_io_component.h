#pragma once

#include <string>
#include <string_view>

#include "ecs/include/component.h"
#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the component
class MONO_ENTITY_ARCHIVE_EXT_DLL ProjectIOComponentHandle : public ecs::ComponentHandle<ProjectIOComponentHandle> {};

// The component class
class MONO_ENTITY_ARCHIVE_EXT_DLL ProjectIOComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    ProjectIOComponent();
    virtual ~ProjectIOComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;
    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get the project path
    std::wstring_view GetProjectPath() const { return project_path_; }

    // Set the project path
    void SetProjectPath(std::wstring_view project_path) { project_path_ = project_path; }

    // Check if an import is requested
    bool IsImportRequested() const { return import_requested_; }

    // Set whether an import is requested
    void SetImportRequested(bool import_requested) { import_requested_ = import_requested; }

    // Check if an export is requested
    bool IsExportRequested() const { return export_requested_; }

    // Set whether an export is requested
    void SetExportRequested(bool export_requested) { export_requested_ = export_requested; }

private:
    // The project path. Always this path is to the vcxproj file.
    std::wstring project_path_ = L"";

    // Whether an import is requested
    bool import_requested_ = false;

    // Whether an export is requested
    bool export_requested_ = false;

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_entity_archive_extension