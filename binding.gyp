{
    "targets": [
        {
            "target_name": "bitimage",
            "dependencies": [
                "node_modules/freetype2/gyp/libfreetype.gyp:libfreetype"
            ],
            "sources": [
                'src/bitimage/bitchar.cpp',
                'src/bitimage/bitimage.cpp',
                'src/bitimage/bitimage_fonts.cpp',
                'src/bitimage/bitimage_js.cpp' ]
        }
    ]
}
