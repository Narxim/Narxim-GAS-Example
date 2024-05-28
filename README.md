# Narxim's GAS Example
### A bare-bones Gameplay Ability System (GAS) example project to help you get started

![GitHub Repo stars](https://img.shields.io/github/stars/Narxim/Narxim-GAS-Example?style=flat-square)
![GitHub last commit](https://img.shields.io/github/last-commit/Narxim/Narxim-GAS-Tutorial?style=flat-square)
![GitHub](https://img.shields.io/github/license/Narxim/Narxim-GAS-Example?style=flat-square)
![Unreal](https://img.shields.io/badge/Unreal_Engine_Versions-_(4.26+)_(5.0+)_Current_Version_(5.4)_-informational?style=flat-square)

![Preview Image](https://i.imgur.com/4XLuHTa.png)

## Features

### A fully-setup example project!
___

GAS has a lot of moving parts and can be quite daunting to wrap your head around at first glance. I am providing this example project as a basis for learning how to properly set up your project to utilize all of its components. There are obviously many ways to set up your project, but I believe this example provides a simple, versatile base to build upon.

---

<p>Note: The 5.4 update of this project has introduced significant changes in an attempt to simplify the setup and add some commonly used / requested features!</p>

___

#### Other Features include:
- Thoroughly commented C++ code
- Example Blueprints
- Health Attribute Set (Includes Damage / Healing)
- Stamina Attribute Set
- Stamina Regeneration example (Based on Stamina Regeneration Attribute)
- (**New**) "Ability System Initialization Data" structure for initializing the Ability System Component
- (**New**) Ability Trigger / Release bindings tied directly to Enhanced Input (Not Gameplay Tag Based... yuck!)
- (**New**) Commonly used example Ability Tasks (Wait Enhanced Input Event / On Tick Event)
- (**New**) Native Gameplay Tags example (NativeGameplayTags.h)
- (**New**) Health Regeneration block after damage (3 seconds - Damage Volume)
- (**Updated**) Example UI elements with Attribute event bindings (In Game UI / Nameplate)
- GA Jump example with Stamina cost
- Player Character and Non Player Character class examples
- Example external Gameplay Effect application (Damage / Healing Volumes)
---
Ability System Initialization Data can be found in the Character class Blueprints.

![Ability System Initialization Data](https://i.imgur.com/GJ5iSVy.png)
___

[**Dan's (Tranek) GAS Documentation**](https://github.com/tranek/GASDocumentation) covers all of these concepts and provides a great breakdown of GAS overall. This repo was made as a starting point for getting set up and should be used alongside Dan's documentation. My intent is for devs who may not be as familiar with C++ or are just starting with GAS to have access to a more easily digestible setup example. Cheers!

___

#### Check out the **[Unreal Source Discord](https://discord.gg/unreal-slackers)** if you have any questions!
