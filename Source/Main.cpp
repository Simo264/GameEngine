#include "Engine.hh"

int main()
{
  Engine engine;
  engine.Initialize();
  engine.Run();
  engine.ShutDown();
  return 0;
}

#if 0
  
  while (window.Loop())
  {

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
      | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    
    ImGui::Begin("DockSpace", 0, window_flags);
    ImGui::PopStyleVar(2);
    
    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
      ImGuiID dockspace_id = ImGui::GetID("DockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("Menu"))
      {
        ImGui::MenuItem("Item 1");
        ImGui::Separator();
        ImGui::MenuItem("Item 2");
        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::End();

    // Viewport panel
    // ---------------------------------
    ImGui::Begin("Viewport");
    {
      ImVec2 frameSize = ImGui::GetContentRegionAvail();
      sceneViewport.x = frameSize.x;
      sceneViewport.y = frameSize.y;

      ImGui::Image(
        (ImTextureID)framebuffer.GetFramebufferTexture(),
        frameSize,
        ImVec2(0, 1),
        ImVec2(1, 0));
    }
    ImGui::End();

    // Scene panel
    // ---------------------------------
    ImGui::Begin("Scene");                          
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();



    // Log panel
    // ---------------------------------
    ImGui::Begin("Log");
    ImGui::End();
    
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(window.GetContext());
    }
  }
#endif
