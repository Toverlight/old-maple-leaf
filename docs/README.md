# MapleLeaf template 说明

一个给自己复用的 VS Code C++ 起手模板，默认工作流是 clangd + CMake Presets + MSVC。

## 特点

- 项目名默认跟随文件夹名，复制模板后改目录名即可。
- 默认使用 CMake Presets 管理 Debug 和 Release。
- clangd 直接读取 build/debug 下的 compile_commands.json。
- VS Code 任务内置 Debug/Release 的 configure、build、run。
- MSVC 环境通过 scripts/Invoke-MsvcCommand.ps1 自动探测，不写死本机 VS 路径。

## 复制后需要改什么

通常只需要改目录名，然后开始写 src 下的代码。

如果后面要接第三方库，优先在 CMakeLists.txt 里继续补 target_include_directories、find_package、target_link_libraries。

## 快速复制模板

仓库里带了一个 PowerShell 脚本：[scripts/New-CppProjectFromTemplate.ps1](scripts/New-CppProjectFromTemplate.ps1)。它会把模板中有用的文件复制到当前目录或指定目录，但不会带上 git 元文件和构建产物。

复制完成后，脚本还会把新项目里的 README 标题从模板名初始化为目标目录名。

默认复制这些内容：

- .clangd
- .gitignore
- .vscode
- CMakeLists.txt
- CMakePresets.json
- README.md
- dep
- docs
- scripts/Invoke-MsvcCommand.ps1
- src

不会复制这些内容：

- .git
- build
- .cache
- 仅供模板仓库自身使用的其他临时文件

推荐用法：

1. 把 [scripts](scripts) 目录加入 Windows 用户 PATH，或者单独把 [scripts/New-CppProjectFromTemplate.ps1](scripts/New-CppProjectFromTemplate.ps1) 放到你自己的命令目录里。
2. 新建一个空文件夹并进入它。
3. 运行 New-CppProjectFromTemplate.ps1。

常用参数：

- 复制到当前目录：New-CppProjectFromTemplate.ps1
- 复制到指定目录：New-CppProjectFromTemplate.ps1 -Destination D:/Code/MyNewProject
- 目标目录非空时允许覆盖：New-CppProjectFromTemplate.ps1 -Force
- 如果脚本不和模板仓库放在一起，可以手动指定模板根目录：New-CppProjectFromTemplate.ps1 -TemplateRoot D:/CodeFamily/VSCode/C++/CppProj_Template

## 常用入口

- 构建调试版：CMake Build Debug
- 运行调试版：Run Debug
- 构建并运行调试版：CMake Build and Run Debug
- 仅清理调试版：CMake Clean Debug
- 构建发布版：CMake Build Release
- 运行发布版：Run Release
- 构建并运行发布版：CMake Build and Run Release
- 仅清理发布版：CMake Clean Release
- 清理全部构建输出：CMake Clean All

## 任务怎么执行

- 菜单入口：Terminal -> Run Task
- 命令面板：Tasks: Run Task
- 调试入口：左侧 Run and Debug 面板，选择 Launch Debug Build 或 Launch Release Build

VS Code 原生没有特别强的任务专用侧边栏；如果想集中查看和鼠标执行任务，比较适合额外装一个 Task Explorer 扩展。

## 快捷键建议

这套模板里的任务和调试配置都适合绑定快捷键，但要区分两件事：

- 用户级快捷键本身是 VS Code 用户级设置，不是工作区隔离设置。
- 如果你不想影响别的 VS Code 使用习惯，最好给这套 C++ 工作流单独建一个 Profile，再在那个 Profile 里配置快捷键。
- 如果某些组合键已经被 Windows、输入法或显卡驱动之类的系统级热键抢占，VS Code 不能覆盖它们。

仓库里带了一个可直接参考的快捷键样例文件：[docs/vscode-user-keybindings.jsonc](docs/vscode-user-keybindings.jsonc)。

## 调试怎么用

- 左侧打开 Run and Debug。
- 选择 Launch Debug Build，可以在启动前自动执行 CMake Build Debug。
- 选择 Launch Release Build，可以在启动前自动执行 CMake Build Release。
- 开始调试、停止调试、重启调试都可以绑定快捷键。

如果是第一次用调试快捷键，先在 Run and Debug 面板里手动选一次目标配置，后面启动快捷键就会沿用当前选择。

如果断点没有命中，而程序一启动就直接跑完，先检查是不是旧的构建缓存混进了别的编译器。这个模板的调试配置使用的是 MSVC 调试器，因此应该配套使用 cl 编译；如果 build/debug 之前是用 MinGW g++ 配出来的，就需要先执行 CMake Clean Debug 或 CMake Clean All，再重新执行 CMake Configure Debug 和 CMake Build Debug。

## 目录说明

- src: 源码目录
- dep: 预留给手动放置的依赖
- scripts: 辅助脚本
- build/debug: Debug 构建输出
- build/release: Release 构建输出
