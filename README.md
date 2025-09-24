# RGB Color Match + Mixer (Work in Progress)

<img src="assets/RGBgameThumbnailA.jpg" width="500" />

A modular embedded gaming platform built on STM32

## Project Overview
An **embedded game platform** built on the STM32L476RG microcontroller that uses RGB LEDs and pushbuttons to deliver interactive gameplay. Currently supports two games: Color Match and Color Mixer, with room to grow into a full suite of RGB-based mini-games.
1. RGB Color Match - A puzzle game where players have to manually adjust RGB values to match another LED's color
2. RGB Color Customizer - A sandbox game where players can freely mix RGB values to create/customize colors

## General Platform Mechanics

<table>
  <tr>
    <td>
      <a href="https://www.youtube.com/watch?v=J02tvOuEa3o">
        <img src="assets/RGBmechanicsTHUMBNAIL.png" width="400" />
      </a>
    </td>
  </tr>
  <tr>
    <td><sub><strong>Demo</strong></sub></td>
  </tr>
</table>

- **Visual Feedback:** LED animation is used to provide real-time visual cues.
  - **Start screen animations:** Immediately reflects the selected mode, so users always know which mode is active before starting.  
  - **Brightness Bars:** Visual representation of RGB intensity levels.
- **Interactive Buttons:** Start the game or adjust RGB channels during gameplay
- **Mode Selection via User Button:** The special onboard button allows users to switch between Color Match and Color Mixer on the start screen. 


## RGB Color Match

<img src="assets/CM_START_SCREEN2.gif" width="500" />

## Features
🎮 **Focused on Puzzle Solving Gameplay**
  - Players have to configure each three RGB channels to match another RGB

🔄 **Guided/Accesibilty Mode**
  - If the player is struggling to match the RGB values, they can toggle on guided mode which alerts a player when they have succesfully match a color value

💡 **Hardware RNG**
  - To keep the game frsh/interesting, the game uses RNG to randomly select between preset colors
  - So the sequence of colors is always unique


# RGB Color Customizer
<img src="assets/CC_START_SCREEN1.gif" width="500" />

## Demos
<table>
  <tr>
    <td>
  <a href="https://www.youtube.com/watch?v=BQ9yHDXTO0I">
    <img src="assets/extended demo thumbnail.jpg" width="300%" />
      </a>
    </td>
    <td>
    <a href="https://www.youtube.com/watch?v=gO6LgwVbm_k">
    <img src="assets/move_mode demo thumbnail.jpg"" width="300%" />
      </a>
    </td>
  </tr>
  <tr>
    <td><sub><strong>Gameplay</strong></sub></td>
    <td><sub><strong>Mode Switching</strong></sub></td>
  </tr>
</table>
