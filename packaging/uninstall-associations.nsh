; Removes what install-associations.nsh registered.
;
; Note what is *not* removed: HKCU\Software\MDReader, where QSettings keeps the
; theme, window geometry, recent files and pane sizes. Reinstalling and finding
; all of that gone is worse than leaving a few registry values behind.

DeleteRegValue HKCU "Software\Classes\.md\OpenWithProgids" "MDReader.Document"
DeleteRegValue HKCU "Software\Classes\.markdown\OpenWithProgids" "MDReader.Document"

DeleteRegKey HKCU "Software\Classes\MDReader.Document"
DeleteRegKey HKCU "Software\Classes\Applications\appMDReader.exe"

System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
