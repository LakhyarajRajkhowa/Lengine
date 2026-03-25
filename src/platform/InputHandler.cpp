#include "InputHandler.h"
#include <algorithm>

namespace Lengine {
   
 
    void InputHandler::handleInputs(
        ImGuiLayer& imguiLayer,
        EditorLayer& editorLayer,
        const float deltaTime
    )
    {

        bool imguiCapturesMouse = imguiLayer.wantsCaptureMouse();
        bool imguiCapturesKeyboard = imguiLayer.wantsCaptureKeyboard();

        bool viewportFocused = editorLayer.GetViewportPanel().IsViewportFocused();
        bool viewportHovered = editorLayer.GetViewportPanel().IsViewportHovered();

        const float dt = deltaTime;


        // ---- MODE 2: EDITOR MODE ----
        if (editorCamera.isFixed)
        {
            if (viewportHovered)
            {

                //for (SDL_Keycode key : EditorKeys::All)
                //{
                //    editorLayer.manipulator.HandleKeyboardShortcuts(key);
                //}

                //if (inputManager.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
                //    editorLayer.manipulator.SelectHoveredEntity();
                //}
            }
            else {
                
            }
        }

        if (!editorCamera.isFixed)
        {
            
            if (editorCamera.controlMode == CameraControlMode::first) {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                int mx, my;
                SDL_GetRelativeMouseState(&mx, &my);
                glm::vec2 relativeMouseCoords = { mx,my };

                // this is for bounding the cursor inside the viewport and recentering the cursor        

                ImVec2 pos = editorLayer.GetViewportPanel().GetViewportPos();
                ImVec2 size = editorLayer.GetViewportPanel().GetViewportSize();

                int clampedX = std::clamp(mx, (int)pos.x, (int)(pos.x + size.x - 1));
                int clampedY = std::clamp(my, (int)pos.y, (int)(pos.y + size.y - 1));

                if (mx != clampedX || my != clampedY)
                    SDL_WarpMouseInWindow(window.getWindow(), clampedX, clampedY);

                editorCamera.Update(dt, relativeMouseCoords);
            }


            else {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                int mx, my;
                SDL_GetRelativeMouseState(&mx, &my);
                glm::vec2 relativeMouseCoords = { mx,my };


                editorCamera.Update(dt, relativeMouseCoords);
                // reset each frame otherwise the scroll activates in next frame
                inputManager.resetScroll();

            }   
        
        }
        else
        {
            // Enter editor mode
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(SDL_ENABLE);
        }
        
    }
}