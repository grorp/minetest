#include "settings.h"
#include "client/renderingengine.h"

double calcImgsize(bool fixed_size, bool real_coordinates, v2u32 screensize,
        v2f formspec_size, v2f padding)
{
    const double gui_scaling = g_settings->getFloat("gui_scaling", 0.5f, 42.0f);
    const double screen_dpi = RenderingEngine::getDisplayDensity() * 96;

    // In fixed-size mode, inventory image size
    // is 0.53 inch multiplied by the gui_scaling
    // config parameter.  This magic size is chosen
    // to make the main menu (15.5 inventory images
    // wide, including border) just fit into the
    // default window (800 pixels wide) at 96 DPI
    // and default scaling (1.00).
    double fixed_imgsize = 0.5555 * screen_dpi * gui_scaling;

    if (fixed_size)
        return fixed_imgsize;

    // Variables for the maximum imgsize that can fit in the screen.
    double fitx_imgsize;
    double fity_imgsize;

    v2f padded_screensize(
        screensize.X * (1.0f - padding.X * 2.0f),
        screensize.Y * (1.0f - padding.Y * 2.0f)
    );

    if (real_coordinates) {
        fitx_imgsize = padded_screensize.X / formspec_size.X;
        fity_imgsize = padded_screensize.Y / formspec_size.Y;
    } else {
        // The maximum imgsize in the old coordinate system also needs to
        // factor in padding and spacing along with 0.1 inventory slot spare
        // and help text space, hence the magic numbers.
        fitx_imgsize = padded_screensize.X /
                ((5.0 / 4.0) * (0.5 + formspec_size.X));
        fity_imgsize = padded_screensize.Y /
                ((15.0 / 13.0) * (0.85 + formspec_size.Y));
    }

    s32 min_screen_dim = std::min(padded_screensize.X, padded_screensize.Y);

    double prefer_imgsize;
    if (g_settings->getBool("enable_touch")) {
        // The preferred imgsize should be larger to accommodate the
        // smaller screensize.
        prefer_imgsize = min_screen_dim / 10 * gui_scaling;
    } else {
        // Desktop computers have more space, so try to fit 15 coordinates.
        prefer_imgsize = min_screen_dim / 15 * gui_scaling;
    }

    // THIS IS IT
    prefer_imgsize = std::max(prefer_imgsize, fixed_imgsize);

    // Try to use the preferred imgsize, but if that's bigger than the maximum
    // size, use the maximum size.
    return std::min(prefer_imgsize, std::min(fitx_imgsize, fity_imgsize));
}
