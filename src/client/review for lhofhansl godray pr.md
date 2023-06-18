I like the godrays. They look very good, especially when they are tinted / affected by light direction. Some things:

- I think the downsampled godrays looked a lot better. The current non-downsampled version still looks grainy, even with filtering. 

- Would it be possible to extract the light direction code from the dynamic shadows code, so that tinting the godrays works even if dynamic shadows are unavailable (Android) or disabled? It doesn't make sense to me that volumetric lighting partially depends on dynamic shadows.

- I'd prefer this API:
    ```lua
    -- reasons for set_lighting:
    --   - volumetric light is a light thing, not a sky thing
    --   - consistency: dynamic shadows and automatic exposure are also in set_lighting
    player:set_lighting({
       -- reasons for a separate table:
        --   - maybe we want to expose more parameters of the algorithm in the future
        --   - consistency: dynamic shadows and automatic exposure also have separate tables
        volumetric_light = {
            strength = 0.5,
        },
    })
    ```

I've tested the initial version of this PR on Android btw (with OpenGL ES 2). It worked nicely, but decreased the framerate from 60 to 40 FPS.
