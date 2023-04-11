# MFGeometryLibrary
Library for creating some geometrical objects and their physics representation for bulletPhysics.
Base geometries can be found in the dir [MFBaseGeometries](https://github.com/etkmichi/MFGeometryLibrary/tree/main/MFGeometryLibrary/MFBaseGeometries)

The MFNoiseGeometry can be used to create more complex surfaces. The related physics object is not convex and may use a lot of process time. A better implementation would be to map noise values onto a box. The box is a convex physics shape and will result in less physics glitches. The mapped noise points can be scaled for better performance.

The compund geometry classes can be used to stick some geometries together (including the physical representations).

Supported OS:
  * Linux

Additional libraries:<br>
  * [MFBasics](https://github.com/etkmichi/MFBasics)
  * [MFGeometryLibrary](https://github.com/etkmichi/MFGeometryLibrary)
  * stb
  * glfw
  * vulkan
  * boost
    * boost_system
    * boost_filesystem
    * boost_atomic
  * X11

Additional applications:
  * glslangValidator
