{
    "targets": [
        {
            "target_name": "bitscreen",
            "dependencies": [
                "node_modules/freetype2/gyp/libfreetype.gyp:libfreetype"
            ],
            "sources": [
                'src/bitscreen/utf8iterator.cpp',
                'src/bitscreen/bitchar.cpp',
                'src/bitscreen/bitobject.cpp',
                'src/bitscreen/bitscreen.cpp',
                'src/bitscreen/bitfonts.cpp',
                'src/bitscreen/bitscreen_js.cpp' ]
        }
    ]
}
