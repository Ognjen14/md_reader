; Registers MDReader as a handler for markdown files, for the current user only.
;
; This adds MDReader to the "Open with" list and gives it a document type; it
; deliberately does not make itself the default handler. Since Windows 10 the
; default for an extension is protected by a hash tied to the user's own choice,
; and any installer that writes it directly is either ignored or resets the
; association to "unknown". Offering the entry and letting the user pick is the
; only approach that actually works.

WriteRegStr HKCU "Software\Classes\MDReader.Document" "" "Markdown Document"
WriteRegStr HKCU "Software\Classes\MDReader.Document\DefaultIcon" "" "$INSTDIR\appMDReader.exe,0"
WriteRegStr HKCU "Software\Classes\MDReader.Document\shell\open\command" "" '"$INSTDIR\appMDReader.exe" "%1"'

WriteRegStr HKCU "Software\Classes\.md\OpenWithProgids" "MDReader.Document" ""
WriteRegStr HKCU "Software\Classes\.markdown\OpenWithProgids" "MDReader.Document" ""

; Also list the application itself, so it appears under "Open with" for any
; text-like file, not only the two extensions above.
WriteRegStr HKCU "Software\Classes\Applications\appMDReader.exe" "FriendlyAppName" "MDReader"
WriteRegStr HKCU "Software\Classes\Applications\appMDReader.exe\shell\open\command" "" '"$INSTDIR\appMDReader.exe" "%1"'
WriteRegStr HKCU "Software\Classes\Applications\appMDReader.exe\SupportedTypes" ".md" ""
WriteRegStr HKCU "Software\Classes\Applications\appMDReader.exe\SupportedTypes" ".markdown" ""
WriteRegStr HKCU "Software\Classes\Applications\appMDReader.exe\SupportedTypes" ".txt" ""

; Tell the shell the association table changed, so icons refresh without a
; sign-out.
System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
