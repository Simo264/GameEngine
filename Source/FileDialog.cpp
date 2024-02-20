#include "FileDialog.hh"

#include <Windows.h>
#include <commdlg.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Path FileDialog::OpenFileDialog(const char* filter)
{
  OPENFILENAMEA ofn = { 0 };
  TCHAR szFile[260] = { 0 };
  ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
  ofn.lStructSize = sizeof(OPENFILENAMEA);
  ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetOpenFileNameA(&ofn) == TRUE)
  {
    return Path(ofn.lpstrFile);
  }

  return Path();
}

Path FileDialog::SaveFileDialog(const char* filter)
{
  OPENFILENAMEA ofn = { 0 };
  TCHAR szFile[260] = { 0 };
  ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
  ofn.lStructSize = sizeof(OPENFILENAMEA);
  ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetSaveFileNameA(&ofn) == TRUE)
  {
    return Path(ofn.lpstrFile);
  }

  return Path();
}

