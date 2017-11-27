# CIS565 GPU Final Project: Procedural Terrain Generation with Vulkan

__Team: Rudraksha Shah and Mauricio Mutai__

Project Overview
=================

Currently, many game studios and engines are transitioning or have already transitioned to using Vulkan. However, there aren’t many small teams developing directly with Vulkan. We want to show that even a small team of two can implement a significant project with Vulkan.

Vulkan’s graphics pipeline gives us access to the tessellation control and evaluation shaders. We want to take advantage of them to generate a procedurally generated terrain with varying level of detail. Our goal is to combine compute and tessellation shaders to optimize the terrain generation so that it is fast, while still generating aesthetically pleasing results. Further, the JCGT paper linked below suggests a modification to the deferred pipeline to further reduce shading costs. However, one flaw mentioned by the paper is that it has no current support for tessellation -- the paper does mention one possible solution, though. We wanted to implement this paper to compare the performance between a Forward, Deferred, and the paper’s pipelines.

With these goals in mind, we wish to implement a renderer in Vulkan that procedurally generates realistic-looking terrain making heavy use of tessellation in order to achieve LOD.


Goals
==========

-   Base goals:
    -   Procedurally generated terrain. The terrain is generated using dynamic levels of tessellation in order to achieve varying LOD.
    -   Forward and Deferred pipelines for comparison purposes.
    -   Implementation of this paper: http://jcgt.org/published/0002/02/04/
        -   We want to see if their modification provides any gains in a tessellation-heavy project.
    -   Integrate texture mapping to further test the paper -- a lot of the gains claimed by the paper come from saving on too-early texture reads
-   Stretch Goals (we plan on implementing some of these as “extra features”):
    -   Procedural rain/snow
    -   Water simulation
    -   Shadow mapping
    -   Simple terrain editing (raise/lower certain areas)
    -   Support for heightmaps


Project Timeline
============

-   11/20: Have basic forward pipeline with procedurally tessellated terrain and varying LOD
-   11/27: Add deferred pipeline, texture mapping
-   12/04: Implement paper’s pipeline, extra features
-   12/11: Extra features


Resources
==========

We'd like to thank the creators of the resources below for providing valuable insight for this project:

-   [The Visibility Buffer: A Cache-Friendly Approach to Deffered Shading](http://jcgt.org/published/0002/02/04/)
-   [Sascha Willems' example implementation of a deferred pipeline](https://github.com/SaschaWillems/Vulkan/blob/master/examples/deferred/deferred.cpp)
-   [Patricio Gonzalez Vivo's noise functions](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)
-   [Adrian Biagioli's page on Perlin noise](http://flafla2.github.io/2014/08/09/perlinnoise.html)
