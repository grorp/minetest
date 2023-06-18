I like this PR. Removing the gradients on buttons makes the mainmenu (and also the in-game GUIs) look fresher and less dusty in my eyes.

My concerns:

- As a side effect, this PR also changes the color of mainmenu tabs. I think the old color is better, as it has a higher contrast to the background.

- > The arrows in scrollbars are a little low contrast. ~ Desour
   
   ☝️

- This PR makes groups of buttons which are very close to each other harder to recognize as separate buttons, for example here:
   
   ![screenshot of a group of buttons which are very close to each other](https://github.com/minetest/minetest/assets/82708541/78a7675e-3d48-4398-a227-de9d27fb0a55)

   The gradients helped in that regard.
