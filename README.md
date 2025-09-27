# RGB Color Match + Mixer (Work in Progress)

<img src="assets/RGBgameThumbnailA.jpg" width="500" />

An **embedded game platform** built on the STM32L476RG microcontroller that uses RGB LEDs and pushbuttons to deliver interactive gameplay. 

Currently supports two games: **Color Match** and **Color Mixer**, with room to grow into a full suite of RGB-based mini-games.

## General Platform Mechanics + Features
These features are shared across both **Color Match** and **Color Mixer** modes:  

- **Visual Feedback:** LED animation is used to provide real-time visual cues.
  - **Start screen animations:** Immediately reflects the selected mode, so users always know which mode is active before starting.  
  - **Brightness Bars:** Visual representation of RGB intensity levels.
- **Interactive Buttons:** Start the game or adjust RGB channels during gameplay
- **Mode Selection via User Button:** The special onboard button allows users to switch between Color Match and Color Mixer on the start screen. 

<sub><strong>General Mechanics Demo</strong></sub>

<a href="https://www.youtube.com/watch?v=B4YANqUfPBE">
  <img src="assets/RGBmechanicsTHUMBNAIL.png" width="400" />
</a>  

---

## RGB Color Match
A puzzle game where players have to manually adjust RGB values to match another LED's color

<img src="assets/CM_START_SCREEN2.gif" width="500" />

### Features
🎮 **Puzzle Solving Gameplay** 
- Players have to configure each three RGB channels to match a target color.
- Each new color provides a fresh puzzle to solve with no set rounds or limits.  

💡 **Hardware RNG**  
- Randomly selects from a pool of preset colors using onboard hardware RNG.
- Ensures each playthrough is unique and replayable.

🔄 **Accesibilty Options**  
- **Guided Mode:** Alerts players when an RGB channel is correctly matched.  
- **Skip Option:** Players can skip a color if stuck and continue the game without penalty.  

<sub><strong>Video Demo</strong></sub>

<a href="https://www.youtube.com/watch?v=oqsAb8M-xsQ">
  <img src="assets/colorMatchDemoImage.png" width="400" />
</a>  

---

## RGB Color Customizer
A sandbox game where players can freely mix RGB values to create/customize colors
 
<img src="assets/CC_START_SCREEN1.gif" width="500" />

### Features
🎮 **Sandbox Gameplay** 
- Endless creative mode where players can freely mix and experiment with RGB values.  
- No target color or win condition. Just open-ended exploration of color blending.  

🎨 **Fine-tuned brightness tweaking**  
- Adjust RGB channels with precise control for subtle variations.  
- Perfect for experimenting with gradients, shades, and unique color combinations.  

<sub><strong>Video Demo</strong></sub>

<a href="https://www.youtube.com/watch?v=AMXlK-1DzHM">
  <img src="assets/colorMixDemoImage.png" width="400" />
</a>  

---

## Block Diagram
<img src="assets/RGBgameBD.drawio (2)-1.png" alt="Game Thumbnail" width="1000"/> 

