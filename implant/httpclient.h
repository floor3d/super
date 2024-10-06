#include <windows.h>
#include <winhttp.h>

// make an HTTP request and return a heap allocated buffer with the result
// the caller is responsble for freeing
LPBYTE HTTPRequest(LPCWSTR pwszVerb,
                   LPCWSTR pwszServer,
                   LPCWSTR pwszPath,
                   int nServerPort,
                   LPCWSTR pwszUserAgent,
                   LPBYTE data,
                   SIZE_T stData,
                   LPDWORD stOut,
                   BOOL bTLS,
                   LPCWSTR pwszHeaders, DWORD dwHeadersLength);

