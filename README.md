
# FIEA Portfolio, Fall 2025

![Project](link)

Hi there! I'm Christina. 

This repository contains my programming portfolio for FIEA's application process. You'll find a selection of code samples from various projects I've worked on over the years. Below are some technical details.

## Raytracer
C++ (using GLM for math). Sample framework for parsing scenes, loading models, and writing to an image was provided; everything else is my own.

A raytracer built as the final project for a computer graphics course. Only basic raytracing, primitive intersection testing, and anti-aliasing was required; extra credit was awarded for other additions.
- Also implemented reflection, refraction, Fresnel effect (using Schlick's approximation), depth of field.

![](raytracer\build\Release\Cat.png)
> A scene rendered with the raytracer.

## Shadowmaps
C++ (using GLM for math, OpenGL, and GLSL shaders). Sample framework for loading a scene with a camera and empty shader code was provided; everything else is my own.

A live demo of a scene with shadowmapped lighting for a computer graphics course; implements a trackball-like control scheme for panning around, and drawing from a shadow map texture. 
- Also implemented perceptually-correct soft shadows.

![](shadowmaps\shadowmaps.mp4)
> The demo running - follow the shadows!

## Pathfinding Simulation
C# in Unity. Everything is my own.

A simulation built for a computer games course showing multiple agents ("NPCs") dynamically pathfinding a navmesh using classic A* search.  
- The navmesh is a three-dimensional grid of discrete nodes constructed automatically from the level.
- The heuristic used for the algorithm is octile distance between nodes.
- The ”open set” of nodes during pathfinding is optimized by using an abridged priority queue backed by a min-heap.

![](pathfinding\pathfinding.mp4)
![](pathfinding\data.png)
> Fun observations about the simulation's performance.

## Colt Express

C# in Unity. Everything is my own.

An event-based messaging API I built for [Colt Express](https://github.com/cpilip/comp-361-colt-express). My team designed, modelled, and implemented a game based on the board game [Colt Express](https://en.wikipedia.org/wiki/Colt_Express) (including the Horses & Stagecoach Expansion) for a yearlong course focused on software development via board games.
- Colt Express used a client-server architecture, where the player client acted as a glorified user interface; most of the game logic and state existed on the server. 

- When a player ended their turn, the API would serialize the player's actions as JSON and send them to the server, where the server would update its internal state and use the API to send out relevant visual updates to all other players.
- Since I was in charge of a lot of the user interface, I designed the API to be extremely easy to use and extensible - only one function needed to be called to send almost any data through the API, and the API handler relied only needed an event "name" to pass to the appropriate listener. A custom listener could then be implemented for any game event or UI element.
- Unfortunately, the lobby service and server are down, making the game no longer playable.

![](colt-express\userinterface.png)
![](colt-express\deliverable.png)
> A mockup of our UI and the messaging system output during our final demo.

# Professional Work

Most of my work has been in the real-time geospatial data space. I can't provide professional code samples, but here's what I've worked on.

I created initial proof-of-concepts to integrate USD (for use with NVIDIA Omniverse's platform and Unreal Engine 5) with Presagis's [VELOCITY 5D](https://www.youtube.com/watch?v=IMuxpx0cOGk) at CAE. Think something along the lines of [third party connectors](https://docs.omniverse.nvidia.com/connect/latest/3rd-party-connectors.html). This was a C++ based role. 

At the moment, I'm a part-time software developer on a research project using GIS‐enabled historiography to reconstruct and analyze historical travel routes to identify where Attila the Hun (yes, that [Attila the Hun](https://en.wikipedia.org/wiki/Attila)) might have been buried. Think something similar to this [paper](https://doi.org/10.1111/tgis.13056). The future goal is to present the findings as an interactive webmap and real-time simulation.

I also helped several students understand the basics of C++ as an undergraduate teaching assistant at McGill University.

# Final Comments


Thank you for viewing my portfolio. If you have further questions, please feel free to contact me!

For other (less technical) projects, here are some games I've helped make over the years.
- On [All Rat Remains](https://ethearian.itch.io/all-rat-remains) for McGameJam 2023, I wrote a maze generator for the rat to navigate between puzzles.
- On [re/start](https://ethearian.itch.io/restart) for McGameJam 2025, I was the narrative writer and level designer. 
- On [The Last Braincells](https://ethearian.artstation.com/projects/r9P1e5) for Ubisoft's Game Lab Competition, I was a level designer.