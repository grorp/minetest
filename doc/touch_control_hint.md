Currently, short tap is hardcoded as "place" and long tap is hardcoded as "dig". This PR allows mods to swap the meaning of short and long taps, depending on the wielded item and the `pointed_thing` type. This is done by adding a new field to item definitions:

```lua
touch_control_hint = {
    -- Only affects Android clients.
    -- Defines the meaning of short and long taps with the item in hand.
    -- The fields in this table have two valid values:
    -- * "long_dig_short_place" (dig = long tap,  place = short tap)
    -- * "short_dig_long_place" (dig = short tap, place = long tap )
    -- The field to be used is selected according to the current
    -- `pointed_thing`.

    pointed_nothing = "long_dig_short_place",
    pointed_node    = "long_dig_short_place",
    pointed_object  = "short_dig_long_place",
},
```

Although this is only a very limited amount of customizability, it already solves two important problems:

1. Items that require holding the place button can be made usable. Fixes #8543.
    
    A prime example of this are bows and crossbows in MineClone 2. They are currently completely unusable on Android because they must be drawn by holding the place button. With this PR, all you need to do is to add the following code to their item definitions:

    ```lua
    touch_control_hint = {
        pointed_nothing = "short_dig_long_place",
        pointed_node    = "short_dig_long_place",
        pointed_object  = "short_dig_long_place",
    },
    ```

    And you have usable bows on Android!

2. It becomes possible to punch with short taps.

    Currently, you have to do a long tap to punch another player or an entity. This is very bad for combat, as you have to wait 500 ms each time you want to punch your opponent. During these 500 ms you can't change your look direction, so it's trivial for your opponent to dodge the punch. To make things even worse, you can't keep your finger on the screen to punch continously because of commit 94feb62b087d61d0c211645970a36e6752d1c732.

    With this PR, you can make punching with short taps possible by adding the following code to item definitions:

    ```lua
    touch_control_hint = {
        pointed_object = "short_dig_long_place",
    },
    ```

    This not only fixes the problems described above, but it is also the behavior that players know from popular games such as Minecraft and MultiCraft.

    Because of these advantages, **this PR makes punching with short taps the default**.

## Alternatives

These problems could also be solved by adding separate on-screen buttons for placing and digging. I plan to implement that as another option at some point and have already created a prototype: https://github.com/minetest/minetest/issues/13229#issuecomment-1738520569.

However, I think that touch controls without additional on-screen buttons can and should work as well, as they are easier to understand for beginners and can be quite comfortable if you aren't doing PvP.

## To do

This PR is a Ready for Review.

Please give feedback on the concept, the API, the implementation, etc.!

## How to test

Play a MineClone 2 world on your Android phone. Verify that you can now punch mobs with a short tap. Verify that the rest of the touch controls still work as usual and that bows are still unusable.

Now put the following code into a mod:

```lua
minetest.register_on_mods_loaded(function()
    for name in pairs(minetest.registered_items) do
        if name:find("bow") then
            minetest.override_item(name, {
                touch_control_hint = {
                    pointed_nothing = "short_dig_long_place",
                    pointed_node    = "short_dig_long_place",
                    pointed_object  = "short_dig_long_place",
                },
            })
        end
    end
end)
```

Rejoin your world and see that bows have become usable.
