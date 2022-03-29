## Assignment 3
### CWL: chsteven
### Name: Steven Chen
### Student number: 82507526

<<<<<<< HEAD
Basic features:
Changed `SUPER APPROXIMATE` to AABB collision detection.

Advance features:
Turn eggs into ellipses and have more precise collision detection.

`detailed_check` in `physics_system.cpp`
In summary, it checks the distance between two entities and compare that with their radii. The reason for a radii check is because eggs are elliptical (more close to a circle), so it will be more accurate to check the circular bounds instead of the rectangular bounds. Since `detailed_check()` is checked after AABB passed, we can ignore the part of the circular bound that is outside of the rectangular bound.

Disintegrate effect of chicken dying

`disintegrate` in `world_system_cpp`
In summary, instead of having chicken turn red, point downward and free fall, chicken now disappears upon dying and create particles at chicken meshes positions. The particles jump up a little and then followed by a fall (like Mario dying).
