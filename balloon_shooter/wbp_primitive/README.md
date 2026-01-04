# WindowsBaseLibrary Plugin template

## Required string replacement
This template needs to replace these string. 
PLUGIN_NAME is not the project name, but rather the name without WindowsBaseLibrary_Plugin, etc. 
(Example : ProjectName = windows_base_plugin_temp -> PLUGIN_NAME = TEMP)

- `$project_name$`
  - Replace Project's name with snake_case
  
- `$PROJECTNAME$`
  - Replace Project's name with all capital letters.

- `$PLUGIN_NAME$`
  - Replace Plugin's name with SNAKE_CASE

## About preprocessor
Add the EXPORT macro on line 3 of [dll_config.h](include/dll_config.h) from Visual Studio to the project's preprocessor.