#include "InputHandler.h"
#include <algorithm>

namespace Lengine {
   

 
    
    void InputHandler::handleInputs(
        ImGuiLayer& imguiLayer,
        EditorLayer& editorLayer)
    {

        bool imguiCapturesMouse = imguiLayer.wantsCaptureMouse();
        bool imguiCapturesKeyboard = imguiLayer.wantsCaptureKeyboard();

        bool viewportFocused = editorLayer.GetViewportPanel().IsViewportFocused();
        bool viewportHovered = editorLayer.GetViewportPanel().IsViewportHovered();

        const float dt = editorLayer.GetPerformancePanel().getDeltaTime();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // 1. Send all events to ImGui

                imguiLayer.processEvent(event);

        }
        // ---- MODE 2: EDITOR MODE ----
        if (camera.isFixed)
        {
            if (viewportHovered)
            {
                editorLayer.getHoveredGizmoAxis();
                editorLayer.config.editingMode = false;

                for (SDL_Keycode key : EditorKeys::All)
                {
                    editorLayer.HandleKeyboardShortcuts(key);
                }
                for ( Uint8 button : { SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, SDL_BUTTON_MIDDLE}) {
                    if (inputManager.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
                        editorLayer.selectHoveredEntity();
                        editorLayer.beginArrowDrag();   // ✅ ONLY ON PRESS
                    }

                    if (inputManager.isMouseButtonDown(SDL_BUTTON_LEFT)) {
                        editorLayer.HandleArrowDrag();  // ✅ ONLY MOVE
                    }

                    if (!inputManager.isMouseButtonDown(SDL_BUTTON_LEFT)) {
                        editorLayer.endArrowDrag();     // ✅ CLEAN EXIT
                    }

                }
                if (inputManager.getScrollY()) {
                    editorLayer.HandleMouseWheel(inputManager.getScrollY());
                    // reset each frame otherwise the scroll activates in next frame
                    inputManager.resetScroll();
                }
            }
            else {
                editorLayer.config.editingMode = true;
                
            }
        }

        if (!camera.isFixed)
        {
            
            if (camera.controlMode == CameraControlMode::first) {
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

                camera.update(dt, relativeMouseCoords);
            }


            else {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                int mx, my;
                SDL_GetRelativeMouseState(&mx, &my);
                glm::vec2 relativeMouseCoords = { mx,my };


                camera.update(dt, relativeMouseCoords);
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