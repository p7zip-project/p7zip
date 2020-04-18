
	7zG
	===



1) building
-----------

  make test_7zG : build bin/7zG and make some tests


2) installing 7zG (tested on Ubuntu 8.04)
-----------------

  copy the method used for 7z !

  - find where 7z is installed
    type 7z

  - copy the 7z shell
    cp /usr/bin/7z /usr/bin/7zG

  - edit /usr/bin/7zG to replace 7z with 7zG

  - chmod 555  /usr/bin/7zG

  - copy bin/7zG according to what is written in /usr/bin/7zG
    cp bin/7zG /usr/lib/p7zip/7zG
    chmod 555 /usr/lib/p7zip/7zG

  - To have access to the Lang files, copy Lang
    cp -r GUI/Lang /usr/lib/p7zip/Lang
    find /usr/lib/p7zip/Lang -type d -exec chmod 555 {} \;
    find /usr/lib/p7zip/Lang -type f -exec chmod 444 {} \;


  - To have access to the help, copy help
    cp -r GUI/help /usr/lib/p7zip/help
    find /usr/lib/p7zip/help -type d -exec chmod 555 {} \;
    find /usr/lib/p7zip/help -type f -exec chmod 444 {} \;

  - copy p7zipForFilemanager
    cp GUI/p7zipForFilemanager  /usr/bin/p7zipForFilemanager
    chmod 555 /usr/bin/p7zipForFilemanager

  - copy the 7-zip icon 
    cp GUI/p7zip_16_ok.png /usr/share/icons/hicolor/16x16/apps/p7zip.png
    chmod 444 /usr/share/icons/hicolor/16x16/apps/p7zip.png
  
3) installing the context menu for KDE   (dolphin)  (tested on Ubuntu 8.04)
--------------------------------------------------
  only for the user :
    mkdir -p ~/.kde/share/apps/d3lphin/servicemenus/
    cp GUI/kde/*.desktop ~/.kde/share/apps/d3lphin/servicemenus/


  for all:
    cp GUI/kde/*.desktop /usr/share/apps/d3lphin/servicemenus/


4) installing the context menu for KDE   (konqueror)  (tested on Ubuntu 8.04)
----------------------------------------------------
  only for the user :
    mkdir -p ~/.kde/share/apps/konqueror/servicemenus/
    cp GUI/kde/*.desktop ~/.kde/share/apps/konqueror/servicemenus/

  for all:
     cp  GUI/kde/*.desktop  /usr/share/apps/konqueror/servicemenus/


5) installing the context menu for gnome   (nautilus)
-----------------------------------------------------
  don't know how to add a context menu in nautilus :(

