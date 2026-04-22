#include "features/os/window.h"
#include "utils/api.h"

class HWND__;

namespace WNE
{
    class WNE_API WindowNT : public Window
    {
    public:
        bool setup(int width, int height, WindowType type);

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