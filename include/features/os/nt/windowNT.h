#include "features/os/window.h"
#include "core/api.h"

class HWND__;

namespace wne
{
    class WNE_API WindowNT : public Window
    {
    public:
        uint16 mousePositionX = 0;
        uint16 mousePositionY = 0;

        bool setup(uint width, uint height, WindowType type);

        void update(float delta) override final;
        void render() override final;
        void updateWindowSize() override final;
        void close() override final;

        void setFocused(bool state);

    private:
        HWND__ *hWnd;

        void setProcessDPIAware();
        int getStyleForState(WindowType type);
        float getAdditionalWindowWidth();
        float getAdditionalWindowHeight();
    };
}