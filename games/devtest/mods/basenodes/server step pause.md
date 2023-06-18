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

4. A steady stream of "[step] ..." messages appears in the console.

    Sometimes, however, the stream stops and the message "Huh, there's been a gap of more than half a second!" appears.

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

    One might think that this is normal lag. But the strange thing is that Minetest reports a normal `dtime`, although the actual `dtime` is much longer than usual.

### What I expected

The message "Huh, there's been a gap of more than half a second!" only appears very rarely. If it appears, the `dtime` reported by Minetest roughly matches the actual `dtime`.

That's actually what I see if I revert 322c4a5b2b8343c3942555c6a337e765ef84c3c4. It looks like this:

```
[step] dtime(MT)   = 0.033303
       dtime(real) = 0.031875
[step] dtime(MT)   = 0.016643
       dtime(real) = 0.002276
[step] dtime(MT)   = 0.016673
       dtime(real) = 0.030174
[step] dtime(MT)   = 0.699977
       dtime(real) = 0.706843
       Huh, there's been a gap of more than half a second!
[step] dtime(MT)   = 0.050051
       dtime(real) = 0.031766
[step] dtime(MT)   = 0.016693
       dtime(real) = 0.031831
[step] dtime(MT)   = 0.016693
       dtime(real) = 0.000678
[step] dtime(MT)   = 0.016589
       dtime(real) = 0.030213
```

### What I observed

The message "Huh, there's been a gap of more than half a second!" appears very often. When it appears, the `dtime` reported by Minetest is much lower than the actual `dtime`.
