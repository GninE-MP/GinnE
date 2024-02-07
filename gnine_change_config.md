# Description
As you can see GninE is a full copy of 'MTA BLUE' project.
So when it wants to open the game it can't do it properly.
We need to change some configs that tells to windows that this is GninE wants to open GTA San Andreas.
To do this we need to change some files and configs that i will describe them on this article.
Note: These changes is only for developers because players can use the installer application which we will create for theme to install this data and its configs to make them launcher work properly!

# Changes
1. [win_install_data.bat]() : install data downloads some data files like .dll libraries or ... from 'MTA BLUE' servers. This will create a folder called `mta` in `Bin` directory. So we must copy some of (not all of them because some of them aren't flexable with `GninE` application) its files to `GninE` directory in `Bin` directory. Also there are some files that we must change there name, which you can see in the following box:
```cpp
xinput1_3_mta.dll -> xinput1_3_GninE.dll
XInput9_1_0_mta.dll -> XInput9_1_0_GninE.dll
```
Also We need to copy `mtasa.dll` file to `GninE` directory in `Bin` directory to `GninE` launcher be able to use `MTA` API.

2. [win_create_projects.bat]() : This will now create visual studio projects perfectly. So it seems like we don't have any other problems here anymore.

3. [GninEsa://_SCHEME_/]() : We also neet to install `GninEsa://` scheme on windows `HKEY_CLASSES_ROOT`. So for doing this we need to open `Registery Editor`. The easiest way to do this is to open the `Run` dialog box via `WIN + R` and enter `regedir` command. Find `HKEY_CLASSES_ROOT` in the left area of Registery Editor. `Double-Click` or `Double-Tap` `HKEY_CLASSES_ROOT` to expend the hive. Now create a `GninEsa.reg` file in desktop or wherever. Write the following config in `GninEsa.reg` :
```cs
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\GninEsa]
@="URL:GninE San Andreas Protocol"
"URL Protocol"=""

[HKEY_CLASSES_ROOT\GninEsa\DefaultIcon]
@="[GninE installed directory]\\GninE.exe" // this is the launcher path. for example : "D:\\projects\\GinnE\\Bin\\GninE.exe" (NOTE: Don't forget to delete this comment because registery editor can't parse comments!)

[HKEY_CLASSES_ROOT\GninEsa\shell]

[HKEY_CLASSES_ROOT\GninEsa\shell\open]

[HKEY_CLASSES_ROOT\GninEsa\shell\open\command]
@="\"[GninE installed directory]\\GninE.exe\"%1"
```
After that on the top left of the editor on its header where `File, Edit, View, ...` buttons located, click on `File` button and click on import. Then choose your `GninEsa.reg` config path and after that click on `Open`. `Registery Editor` automatically will install your `GninEsa://` scheme on windows.