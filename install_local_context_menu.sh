#! /bin/sh

tryInstall()
{
  prg=$1

  if [ -d ${HOME}/.kde/share/apps/${prg} ]
  then
    echo "Installing menus for ${prg} ..."
    mkdir -p  ${HOME}/.kde/share/apps/${prg}/servicemenus
    cp GUI/kde/*.desktop ${HOME}/.kde/share/apps/${prg}/servicemenus/
  fi
}

tryInstall d3lphin
tryInstall dolphin
tryInstall konqueror

