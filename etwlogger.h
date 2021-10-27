/*
Simple ETW event provider for logging strings (doesn't require a manifest or metadata).
*/

#include "common.h"

#include <evntprov.h>
#include <stdlib.h>
#include <strsafe.h>
#include <wincrypt.h>

// Returns true if the provider name was used to set the GUID argument
bool ProviderNameToGuid(const wchar_t* name, GUID* prov_guid) {
  const BYTE preamble[] = { 0x48, 0x2C, 0x2D, 0xB2, 0xC3, 0x90, 0x47, 0xC8,
                            0x87, 0xF8, 0x1A, 0x15, 0xBF, 0xC1, 0x30, 0xFB };
  const size_t SHA1_LEN = 20;
  const size_t MAX_NAME_LEN = 127;
  BYTE pbHash[SHA1_LEN];
  DWORD hashLen = SHA1_LEN;
  wchar_t pbData[MAX_NAME_LEN + 1];
  size_t name_len;

  HCRYPTPROV hProv = NULL;
  HCRYPTHASH hHash = NULL;
  bool result = false;

  if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0)) goto exit;
  if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) goto exit;

  if (FAILED(StringCchLengthW(name, MAX_NAME_LEN, &name_len))) goto exit;
  StringCchCopyW(pbData, ARRAYSIZE(pbData), name);
  CharUpperBuffW(pbData, (DWORD)name_len);

  // Make big endian
  for(size_t i = 0; i < name_len; ++i) {
    pbData[i] = _byteswap_ushort(pbData[i]);
  }

  if (!CryptHashData(hHash, preamble, sizeof(preamble), 0)) goto exit;
  if (!CryptHashData(hHash, (BYTE*)pbData, DWORD(name_len * 2), 0)) goto exit;
  if (!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &hashLen, 0)) goto exit;
  pbHash[7] = ((pbHash[7] & 0x0F) | 0x50);

  prov_guid->Data1 = *(ULONG*)pbHash;
  prov_guid->Data2 = *(USHORT*)(pbHash + 4);
  prov_guid->Data3 = *(USHORT*)(pbHash + 6);
  CopyMemory(prov_guid->Data4, pbHash + 8, 8);
  result = true;

exit:
  if (hHash) CryptDestroyHash(hHash);
  if (hProv) CryptReleaseContext(hProv, 0);
  return result;
}

class EtwLogger {
  struct ProviderState {
    GUID      guid;
    REGHANDLE regHandle;
    BOOL      enabled;
    UCHAR     level;
  };

 public:
  EtwLogger(const wchar_t* provider_name) {
    if (!ProviderNameToGuid(provider_name, &state.guid)) return;

    // Fail silently on failed registration. Also done if no name to register.
    HRESULT hr = EventRegister(&state.guid, EtwCallback, &state, &state.regHandle);
    if (FAILED(hr) || !provider_name) return;

    // Set the provider name in the provider traits
    // See https://docs.microsoft.com/en-us/windows/win32/etw/provider-traits
    const size_t kBufferLen = 100;
    char pData[kBufferLen];

    size_t nameLen = 0;
    if (FAILED(StringCchLengthW(provider_name, kBufferLen, &nameLen))) return;
    USHORT trait_size = USHORT(sizeof(USHORT) + nameLen + 1);
    if (trait_size > kBufferLen) return;  // Fail silently
    *reinterpret_cast<USHORT*>(pData) = trait_size;
    if (WideCharToMultiByte(1252, 0, provider_name, -1, &pData[2], kBufferLen - 2, NULL, NULL)) {
      hr = EventSetInformation(state.regHandle, EventProviderSetTraits, pData, trait_size);
    }
  }

  ~EtwLogger() {
    if (state.regHandle) EventUnregister(state.regHandle);
  }

  void WriteEvent(const wchar_t* msg, UCHAR level = 0, ULONGLONG keyword = 0) {
    if (state.enabled && level <= state.level) {
      EventWriteString(state.regHandle, level, keyword, msg);
    }
  }

 private:
  static void NTAPI EtwCallback(const GUID* srcId,
                      ULONG providerState,
                      UCHAR level,
                      ULONGLONG matchAnyKeyword,
                      ULONGLONG allKeyword,
                      PEVENT_FILTER_DESCRIPTOR filter,
                      void* context) {
    ProviderState *provider = reinterpret_cast<ProviderState*>(context);
    switch (providerState) {
      case 0:  // Disabled
        provider->enabled = false;
        provider->level = level;
        break;
      case 1:  // Enabled
        // level and keywords have all bits set if not specified by the session
        provider->enabled = true;
        provider->level = level;
        break;
    }
  }

  ProviderState state = {0};
};
