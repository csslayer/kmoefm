find_package(Gettext REQUIRED)
file(GLOB POFILES "*.po")
gettext_create_translations(kmoefm.pot ALL ${POFILES})
