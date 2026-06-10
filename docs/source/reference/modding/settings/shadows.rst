Shadows Settings
################

.. omw-setting::
   :title: enable shadows
   :type: boolean
   :range: true, false
   :default: false
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable or disable shadow rendering using shadow mapping.
   More realistic but may reduce performance.
   Forces shaders usage like :ref:`force shaders`.

.. omw-setting::
   :title: number of shadow maps
   :type: int
   :range: 1 to 8
   :default: 3
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Number of shadow maps used.
   More maps improve shadow quality but may reduce performance or cause texture conflicts.

.. omw-setting::
   :title: shadow mapping method
   :type: string
   :range: legacy vdsm | stable csm
   :default: legacy vdsm
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Selects the shadow mapping implementation.
   ``legacy vdsm`` uses the original view-dependent shadow maps.
   ``stable csm`` uses snapped orthographic cascades intended to reduce shimmering.

.. omw-setting::
   :title: maximum shadow map distance
   :type: float32
   :range: full 32-bit float range
   :default: 8192
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Maximum distance shadows cover from the camera.
   Set ≤ 0 to disable distance limit.

.. omw-setting::
   :title: shadow fade start
   :type: float32
   :range: [0, 1]
   :default: 0.9
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Fraction of maximum shadow distance at which shadows start fading.
   No effect if distance limit disabled.

.. omw-setting::
   :title: enable debug hud
   :type: boolean
   :range: true, false
   :default: false

   Show debug HUD visualizing shadow map contents.
   Recommended for developers or advanced users.

.. omw-setting::
   :title: enable debug overlay
   :type: boolean
   :range: true, false
   :default: false

   Show debug overlay showing shadow map coverage areas.
   Recommended for advanced debugging.

.. omw-setting::
   :title: compute scene bounds
   :type: string
   :range: primitives | bounds | none
   :default: bounds
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Method to compute shadow map coverage:
   - `primitives`: better shadows, higher CPU cost
   - `bounds`: better performance, lower quality
   - `none`: disables computation

.. omw-setting::
   :title: shadow map resolution
   :type: int
   :range: dependent on GPU/driver
   :default: 1024
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Size of shadow maps.
   Higher values improve quality but increase GPU load.
   Powers of two may perform better on some hardware.

.. omw-setting::
   :title: actor shadows
   :type: boolean
   :range: true, false
   :default: false

   Enable shadows cast by NPCs and creatures.
   May reduce performance.

.. omw-setting::
   :title: player shadows
   :type: boolean
   :range: true, false
   :default: false
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable shadows cast by the player character.
   May reduce performance.

.. omw-setting::
   :title: terrain shadows
   :type: boolean
   :range: true, false
   :default: true
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable shadows cast by terrain.
   May reduce performance.

.. omw-setting::
   :title: object shadows
   :type: boolean
   :range: true, false
   :default: true
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable shadows cast by regular placed world objects.
   Paged exterior static geometry is controlled separately by ``static object shadows``.
   May reduce performance.

.. omw-setting::
   :title: static object shadows
   :type: boolean
   :range: true, false
   :default: false
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable shadows cast by paged static exterior geometry, such as rocks, buildings, and merged chunks.
   This can be expensive in dense cities.

.. omw-setting::
   :title: small object shadows
   :type: boolean
   :range: true, false
   :default: false
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Reserved control for very small object casters.
   Contact ambient occlusion is usually cheaper for grounding small clutter.

.. omw-setting::
   :title: shadow caster min radius
   :type: float32
   :range: >= 0
   :default: 32

   Reserved minimum caster radius for future per-object shadow filtering.

.. omw-setting::
   :title: shadow caster max distance
   :type: float32
   :range: >= 0
   :default: 8192

   Reserved maximum caster distance for future per-object shadow filtering.

.. omw-setting::
   :title: shadow small feature culling
   :type: boolean
   :range: true, false
   :default: true

   Enables small feature culling on shadow cameras.
   Can improve dense-scene shadow performance, but may remove very small casters from shadow maps.

.. omw-setting::
   :title: shadow small feature culling pixel size
   :type: float32
   :range: >= 0
   :default: 4.0

   Pixel threshold used by shadow-camera small feature culling.

.. omw-setting::
   :title: enable shadow cascade stats
   :type: boolean
   :range: true, false
   :default: false

   Logs per-cascade shadow cull timing and cascade counts for profiling.

.. omw-setting::
   :title: enable indoor shadows
   :type: boolean
   :range: true, false
   :default: false
   :location: :bdg-success:`Launcher > Settings > Visuals > Shadows`

   Enable shadows indoors.
   Only actors cast shadows indoors without full ceiling shadows.
   Can cause shadows appearing through objects.

.. omw-setting::
   :title: polygon offset factor
   :type: float32
   :range: full 32-bit float range, sensibly >1.0
   :default: 1.1

   Polygon offset factor for shadow map rendering.
   Reduces shadow flicker but may increase Peter Panning.

.. omw-setting::
   :title: polygon offset units
   :type: float32
   :range: full 32-bit float range, sensibly 1 to 10
   :default: 4.0

   Polygon offset units for shadow map rendering.
   Works with offset factor to reduce artifacts.

.. omw-setting::
   :title: normal offset distance
   :type: float32
   :range: full 32-bit float range, sensibly 0 to 2
   :default: 1.0

   Distance along surface normal to project shadow coordinates.
   Reduces flicker with less Peter Panning than polygon offset.

.. omw-setting::
   :title: use front face culling
   :type: boolean
   :range: true, false
   :default: false

   Exclude front faces from shadow maps for performance.
   May increase Peter Panning artifacts.

.. omw-setting::
   :title: split point uniform logarithmic ratio
   :type: float32
   :range: [0, 1]
   :default: 0.5

   Controls balance between logarithmic and uniform split points for shadow splits.
   Adjust when using large view distances or distant terrain.

.. omw-setting::
   :title: stable csm cascades
   :type: int
   :range: 1 to 8
   :default: 2

   Number of cascades used by the stable CSM shadow path.

.. omw-setting::
   :title: stable csm resolution
   :type: int
   :range: dependent on GPU/driver
   :default: 1024

   Shadow map resolution used by each stable CSM cascade.

.. omw-setting::
   :title: stable csm distance
   :type: float32
   :range: > 0
   :default: 6144

   Maximum distance covered by stable CSM shadows.

.. omw-setting::
   :title: stable csm split lambda
   :type: float32
   :range: [0, 1]
   :default: 0.65

   Blend between uniform and logarithmic cascade split placement.

.. omw-setting::
   :title: stable csm texel snapping
   :type: boolean
   :range: true, false
   :default: true

   Snaps stable CSM cascade centers to shadow texel increments to reduce shimmering.

.. omw-setting::
   :title: stable csm update interval ms
   :type: int
   :range: 16 to 1000
   :default: 100

   Minimum time in milliseconds between stable CSM shadow map refreshes after a cascade needs updating.
   Higher values reduce shadow-map churn, but can make shadows lag slightly while moving.
   ``100`` updates at most 10 times per second.

.. omw-setting::
   :title: stable csm sun update angle threshold
   :type: float32
   :range: >= 0
   :default: 1.0

   Sun angle change, in degrees, required before stable CSM refreshes for sun movement.
   Higher values reduce thin-shadow flicker from tiny sun motion.

.. omw-setting::
   :title: enable ambient occlusion
   :type: boolean
   :range: true, false
   :default: false

   Enables the built-in contact ambient occlusion post-process.

.. omw-setting::
   :title: ao method
   :type: string
   :range: contact | none
   :default: none

   Selects the ambient occlusion method.

.. omw-setting::
   :title: ao radius
   :type: float32
   :range: >= 0
   :default: 90

   Default contact ambient occlusion sample radius in game units.

.. omw-setting::
   :title: ao strength
   :type: float32
   :range: >= 0
   :default: 0.65

   Default contact ambient occlusion strength.

.. omw-setting::
   :title: ao sample count
   :type: int
   :range: 1 to 32
   :default: 8

   Default number of contact ambient occlusion samples.

.. omw-setting::
   :title: ao fade distance
   :type: float32
   :range: >= 0
   :default: 4096

   Distance where contact ambient occlusion fades out.

.. omw-setting::
   :title: split point bias
   :type: float32
   :range: full C++ float range
   :default: 0.0

   Bias parameter used in shadow split computation.
   Non-zero values can cause unusual behavior.

.. omw-setting::
   :title: minimum lispsm near far ratio
   :type: float32
   :range: > 0
   :default: 0.25

   Minimum near/far ratio for Light Space Perspective Shadow Map.
   Controls distribution of shadow detail near and far from the camera.
