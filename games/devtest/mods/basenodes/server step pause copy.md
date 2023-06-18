### Summary

In recent 5.9.0-dev versions of Minetest, I've observed that the server step sometimes "pauses", i.e. `minetest.register_globalstep` Lua callbacks stop running for a while.

This happens both with the "Host Server" checkbox enabled and without.

### Steps to reproduce

1. Create a new Devtest world with the "flat" mapgen. Disable decorations and the like.

2. Put this code into a mod:

    ```lua
    local last_time

    minetest.register_globalstep(function(dtime)
        local now_time = minetest.get_us_time()
        print(string.format("[step] dtime(MT)   = %f", dtime))
        if last_time then
            local dtime_real = (now_time - last_time) / 1000000
            print(string.format("       dtime(real) = %f", dtime_real))
            if dtime_real > 0.5 then
                local c = string.char(27)
                print("       " .. c .. "[31;1;4mHuh, there's been a gap of more than half a second!" .. c .. "[0m")
            end
        end
        last_time = now_time
    end)
    ```

3. Join your world. Grant yourself the "fast" privilege and run in one direction for some time

### What I expected

A steady stream of "[step] ..." messages appears in the console.

The message "Huh, there's been a gap of more than half a second!" only appears very rarely. If it appears, the `dtime` reported by Minetest roughly matches the actual `dtime`.

This is not only what I expect, but also what I observe if I revert commit 322c4a5b2b8343c3942555c6a337e765ef84c3c4.

### What I observed

A stream of "[step] ..." messages appears in the console, but it often stops for a few seconds.

The message "Huh, there's been a gap of more than half a second!" appears very often. When it appears, the `dtime` reported by Minetest is much lower than the actual `dtime`.

```
[step] dtime(MT)   = 0.001954
       dtime(real) = 0.002533
[step] dtime(MT)   = 0.002484
       dtime(real) = 0.002486
[step] dtime(MT)   = 0.002494
       dtime(real) = 0.001580
[step] dtime(MT)   = 0.001580
       dtime(real) = 0.001044
[step] dtime(MT)   = 0.001028
       dtime(real) = 0.000619
[step] dtime(MT)   = 0.001002
       dtime(real) = 2.220502
       Huh, there's been a gap of more than half a second!
[step] dtime(MT)   = 0.001682
       dtime(real) = 0.002542
[step] dtime(MT)   = 0.002491
       dtime(real) = 0.002499
[step] dtime(MT)   = 0.002505
       dtime(real) = 0.001260
[step] dtime(MT)   = 0.001261
       dtime(real) = 0.001036
[step] dtime(MT)   = 0.001022
       dtime(real) = 0.000249
```
