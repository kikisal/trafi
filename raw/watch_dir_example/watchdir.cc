#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <cassert>
#include <stdint.h>

#include <trafi_server.h>

// watch for files.
void watch();

int main() {

  trafi::trafi_server trfs;

  trfc.set_configuration("config.trf");
  

  trfs.run();

  // watch();

  return 0;
}

void watch() {

    char* path = "C:\\Users\\ki\\Desktop\\TRF-gs0";
    printf("watching %s for changes...\n", path);



    HANDLE file = CreateFile(path,
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
      NULL);
    assert(file != INVALID_HANDLE_VALUE);
    OVERLAPPED overlapped;
    overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

    uint8_t change_buf[1024];
    BOOL success = ReadDirectoryChangesW(
      file, change_buf, 1024, TRUE,
      FILE_NOTIFY_CHANGE_FILE_NAME  |
      FILE_NOTIFY_CHANGE_DIR_NAME   |
      FILE_NOTIFY_CHANGE_LAST_WRITE,
      NULL, &overlapped, NULL);

    while (true) {
      DWORD result = WaitForSingleObject(overlapped.hEvent, 0);

      if (result == WAIT_OBJECT_0) {
        DWORD bytes_transferred;
        GetOverlappedResult(file, &overlapped, &bytes_transferred, FALSE);

        FILE_NOTIFY_INFORMATION *event = (FILE_NOTIFY_INFORMATION*)change_buf;

        for (;;) {
          DWORD name_len = event->FileNameLength / sizeof(wchar_t);

          switch (event->Action) {
            case FILE_ACTION_ADDED: {
              wprintf(L"       Added: %.*s\n", name_len, event->FileName);
            } break;

            case FILE_ACTION_REMOVED: {
              wprintf(L"     Removed: %.*s\n", name_len, event->FileName);
            } break;

            case FILE_ACTION_MODIFIED: {
                wprintf(L"    Modified: %.*s\n", name_len, event->FileName);
            } break;

            case FILE_ACTION_RENAMED_OLD_NAME: {
              wprintf(L"Renamed from: %.*s\n", name_len, event->FileName);
            } break;

            case FILE_ACTION_RENAMED_NEW_NAME: {
              wprintf(L"          to: %.*s\n", name_len, event->FileName);
            } break;

            default: {
              printf("Unknown action!\n");
            } break;
          } // switch(event->Action)

          // Are there more events to handle?
          if (event->NextEntryOffset) {
            *((uint8_t**)&event) += event->NextEntryOffset; // PRETTY PRETTY NEAT WHEN YOU UNDERSTAND IT.
          } else {
            break;
          }
        } // for(;;)

        // Queue the next event
        BOOL success = ReadDirectoryChangesW(
              file, change_buf, 1024, TRUE,
              FILE_NOTIFY_CHANGE_FILE_NAME  |
              FILE_NOTIFY_CHANGE_DIR_NAME   |
              FILE_NOTIFY_CHANGE_LAST_WRITE,
              NULL, &overlapped, NULL);
      }
      // Do other loop stuff here...
    } // while(true)

} // void watch()
