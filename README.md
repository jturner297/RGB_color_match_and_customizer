# RGB Color Match + Customizer

<img src="assets/RGBgameThumbnailA.jpg" width="500" />

A two-in-one embedded color game built on STM32

## Project Overview
**RGB Color Match + Customizer** is an embedded game project using the **STM32L476RG microcontroller**. It features two distinct games
1. RGB Color Match - A puzzle game where players have to manually adjust RGB values to match another LED's color
2. RGB Color Customizer - A sandbox game where players can freely mix RGB values to create/customize colors

# RGB Color Match

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
