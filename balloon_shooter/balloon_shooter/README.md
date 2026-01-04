# WindowsBaseLibrary's application template

## Required string replacement
This template needs to replace these string. Project name is the full project name, but window and scene are dose not necessary put 'window' or 'scene' at the front of it.

- `$project_name$`
  - Replace Project's name with snake_case

- `$WindowName$`
  - Replace window name with camelCase

- `$window_name$`
  - Replace window name with snake_case

- `$SceneName$`
  - Replace scene name with CamelCase

- `$scene_name$`
  - Replace scene name with snake_case

## Required folder name replacement
After changing the folder name, it is necessary to exclude the files in the already added folder from Visual Studio and add them again in the appropriate filter.
- [widnow_folder_header](include/window_$window_name$), [window_folder_source](src/window$window_name$)
  - Replace $window_name$ to the window name which dose not have 'window' at the front

- [scene_folder_header](include/scene_$scene_name$), [scene_folder_source](src/scene_$scene_name$)
  - Replace $scene_name$ to the scene name which dose not have 'scene' at the front

## Required Visual Studio's filter name replacement
- `window_$window_name$` filters in the source and header filter
  - Replace $window_name$ to the window name which dose not have 'window' at the front

- `scene_$scene_name$` filters in the source and header filter
  - Replace $scene_name$ to the scene name which dose not have 'scene' at the front