# Narxim's GAS Example
### A bare-bones Gameplay Ability System (GAS) template to help you get started.

![GitHub Repo stars](https://img.shields.io/github/stars/Narxim/Narxim-GAS-Example?style=flat-square)
![GitHub last commit](https://img.shields.io/github/last-commit/Narxim/Narxim-GAS-Tutorial?style=flat-square)
![GitHub](https://img.shields.io/github/license/Narxim/Narxim-GAS-Example?style=flat-square)
![Unreal](https://img.shields.io/badge/Unreal_Engine_Version-4.26_4.27_5.0_5.1-informational?style=flat-square)

## Features

### Ability System Component on the PlayerState (Player Only)
___
<p>Having the Ability System Component on the PlayerState allows it's data to be persistent if you plan on deleting your player pawn during gameplay.
This is also helpful if you Possess different pawns and still need a reference to the main character's stats and ability states.
The main drawback of this setup is the need to increase the PlayerState's NetUpdateFrequency so that updates from the servers version arent too delayed, which usually isnt a problem at all.</P>

___

#### Other Features include:
- Thoroughly Commented code
- Health Attribute Set (With Damage / Healing)
- Stamina Attribute Set
- Stamina regen example (Based on Stamina Regeneration Attribute)
- Ability bindings
- Attribute modifiers for Gameplay Abilities
- GA Jump example with stamina cost
- Player Character and Non Player Character classes
- Generic NPC GA use example (Jump)
- Helper functions for use in Blueprint
- Blueprint exposed "Give Gameplay Ability" function

___

[**Dan's (Tranek) GAS Documentation**](https://github.com/tranek/GASDocumentation) covers all of these concepts and goes into much more detail on how GAS works as a whole. This repo was made as a starting point for getting set up and should be used alongside Dan's documentation. My intent is for devs who may not be as familiar with C++ or are just starting with GAS to have access to a more easily digestable setup example. 

___

#### Check out the **[Unreal Slackers Discord](https://discord.gg/unreal-slackers)** if you have any questions!
