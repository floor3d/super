#include <stdlib.h>
#include <windows.h>
#include <winhttp.h>

#include "../httpclient.h"

//
// DWORD create_key_for_obfuscation() {
//   DWORD key = 32;
//   for(DWORD i = 0; i < 42; i++) {
//     key += i * 365;
//     key = (key % 1505);
//     key -= 24 ^ (4 * key);
//     key = (key % 1504);
//     key = key * key;
//     key -= 24 ^ (4 * key);
//     key += i * 3;
//     key = key * key;
//     key -= 24 ^ (4 * key);
//     key = (key % 1504);
//     key = key * key;
//     key += 420;
//     key += i * 3 * key * key;
//     key = (key % 2052);
//   }
//   return key;
// }
//
// // Function to swap two wide characters
// void swap(wchar_t* a, wchar_t* b) {
//     wchar_t temp = *a;
//     *a = *b;
//     *b = temp;
// }
//
// void deobfuscateString(const wchar_t* input, wchar_t* output, DWORD key) {
//     // Copy input to output
//     wcscpy(output, input);
//
//     // Swap adjacent characters
//     for (int i = 0; output[i] && output[i + 1]; i += 2) {
//         swap(&output[i], &output[i + 1]);
//     }
//
//     // XOR with key 42
//     while (*output) {
//         *output++ ^= key; // XOR with key 42
//     }
// }

LPBYTE HTTPRequest(LPCWSTR pwszVerb, LPCWSTR pwszServer, LPCWSTR pwszPath,
                   int nServerPort, LPCWSTR pwszUserAgent, LPBYTE data,
                   SIZE_T stData, LPDWORD stOut, BOOL bTLS, LPCWSTR pwszHeaders,
                   DWORD dwHeadersLength) {
  HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
  LPBYTE pBuffer = NULL;
  BOOL bResults = FALSE;
  DWORD dwTotalBytesRead = 0;
  // Use WinHTTP to initialize a session
  hSession = WinHttpOpen(pwszUserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                         WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (!hSession) {
    goto cleanup;
  }

  // Connect to the server
  hConnect = WinHttpConnect(hSession, pwszServer, nServerPort, 0);
  if (!hConnect) {
    goto cleanup;
  }

  // Create an HTTP request handle
  hRequest = WinHttpOpenRequest(
      hConnect, pwszVerb, pwszPath, NULL, WINHTTP_NO_REFERER,
      WINHTTP_DEFAULT_ACCEPT_TYPES, bTLS ? WINHTTP_FLAG_SECURE : 0);
  if (!hRequest) {
    goto cleanup;
  }

  // Add custom headers if provided. how to write the pwszheaders?
  if (pwszHeaders && dwHeadersLength > 0) {
    bResults = WinHttpAddRequestHeaders(hRequest, pwszHeaders, dwHeadersLength,
                                        WINHTTP_ADDREQ_FLAG_ADD);
    if (!bResults) {
      goto cleanup;
    }
  }

  // Send the request
  bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                data, (DWORD)stData, (DWORD)stData, 0);
  if (!bResults) {
    goto cleanup;
  }

  // Receive the response
  bResults = WinHttpReceiveResponse(hRequest, NULL);
  if (!bResults) {
    goto cleanup;
  }

  // Allocate memory for the response
  const DWORD CHUNK_SIZE = 4096;
  DWORD dwBytesRead = 0;
  pBuffer = (LPBYTE)malloc(CHUNK_SIZE);
  if (!pBuffer) {
    goto cleanup;
  }

  // Read the response data in chunks
  do {
    bResults = WinHttpReadData(hRequest, pBuffer + dwTotalBytesRead, CHUNK_SIZE,
                               &dwBytesRead);
    if (!bResults) {
      goto cleanup;
    }
    dwTotalBytesRead += dwBytesRead;

    // Resize the buffer if needed
    LPBYTE pTempBuffer =
        (LPBYTE)realloc(pBuffer, dwTotalBytesRead + CHUNK_SIZE);
    if (!pTempBuffer) {
      free(pBuffer);
      pBuffer = NULL;
      goto cleanup;
    }
    pBuffer = pTempBuffer;
  } while (dwBytesRead > 0);

  // Set the total number of bytes read
  *stOut = dwTotalBytesRead;

cleanup:
  // Clean up resources
  if (hRequest) {
    WinHttpCloseHandle(hRequest);
  }
  if (hConnect) {
    WinHttpCloseHandle(hConnect);
  }
  if (hSession) {
    WinHttpCloseHandle(hSession);
  }

  return pBuffer;
}
