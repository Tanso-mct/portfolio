## Template for MonoForge Plugin
This is a template of mono forge plugin vs project.

## String that needs to be replaced
 - `$project_name$` to your plugin vs project name in snake_case
 - `$PROJECTNAME$` to your plugin vs project name in all upper case
 - `$API_NAME$` to your plugin vs project's api name in SNAKE_CASE

## Required Visual Studio configuration changes
Replace MONOFFORGEPLUGINTEMPLATE_EXPORTS pre-processor to `$PROJECTNAME$`_EXPORTS
