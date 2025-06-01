<h1>KPC:TNC DLL HOOK</h1>

>An ongoing project of mine for the game: [Kiss: Psycho Circus: The Nightmare Child](https://en.wikipedia.org/wiki/Kiss:_Psycho_Circus:_The_Nightmare_Child).

This was originally started as a way to read memory addresses and create a stats window with player velocity, position x,y,z, global timer, for the purpose of speedrunning, but it has 
transpired into being a full KPC mod project.

<h2>Planned Features:</h2>

- [x] Read game process memory
- [x] Inject DLL
- [x] Hook DLL
- [ ] Inject code before D3D7 endframe function
- [ ] Implement functional GUI via [ImGui](https://github.com/ocornut/imgui) or any other libs
- [ ] Show player stats
- [ ] Change player stats
- [ ] Change level
- [ ] Editable GUI 

<h2>How to use:</h2>

<ol>
  <li>Open solution (.sln file)</li>
  <li><strong>Set both main.cpp and dllmain.cpp to x86</strong> (to the left of the "Local Windows Debugger" option) </li>
  <li>Go to Project -> Properties -> Configuration Properties -> General -> Configuration Type</li>
  <li>Set it to Dynamic Library (.dll)</li>
  <li>Build it (ctrl+b)</li>
  <li>Go back to Configuration Type</li>
  <li>Set it to Application (.exe)</li>
  <li>Build + run</li>
  <li>Application will look for the game (client.exe) </li>
  <li>Open the game (KPC:TNC) and the dll will automatically inject</li>
</ol>

<h2>Thanks to:</h2>

  [Montrii for the D3D7 hook, which my project is based on](https://github.com/Montrii/DirectX7Hook)

  [Low Level Game Dev for making a tutorial on how to read memory of a process in Windows api](https://www.youtube.com/@lowlevelgamedev9330)
  >I struggle with win32 api, I only know POSIX


